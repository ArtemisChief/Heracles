#include "collision.h"
#include <algorithm>
#include "world.h"

namespace heracles {

	contact::contact(const rigid_body &b, const size_t idx) : separation(0) {
		indices = { {idx, idx} };
		fill(from_a.begin(), from_a.end(), false);
		position = b.get_world_position() + b[idx];
	}

	bool contact::operator==(const contact &other) const {
		if (from_a == other.from_a && indices == other.indices) {
			return true;
		}
		const decltype(from_a) from_a_swap = { {from_a[1], from_a[0]} };
		const decltype(indices) indices_swap = { {indices[1], indices[0]} };
		return from_a_swap == other.from_a && indices_swap == other.indices;
	}

	bool contact::operator!=(const contact &other) const {
		return !(*this == other);
	}

	arbiter::arbiter(const body::ptr a, const body::ptr b, const vec2 &normal, const contact_list &contacts)
		: a_(a), b_(b), normal_(normal), contacts_(contacts) {
	}

	const arbiter::contact_list &arbiter::get_contacts() const {
		return contacts_;
	}

	const vec2 &arbiter::get_normal() const {
		return normal_;
	}

	void arbiter::pre_step(const float dt) {
		static const float k_allowed_penetration = 0.001;
		const auto tangent = normal_.normal();
		const auto a = a_.lock();
		const auto b = b_.lock();
		for (auto &contact : contacts_) {
			const auto kn = a->get_inv_mass() + b->get_inv_mass() +
				dot(a->get_inv_inertia() * cross(cross(contact.ra, normal_), contact.ra) +
					b->get_inv_inertia() * cross(cross(contact.rb, normal_), contact.rb), normal_);
			const auto kt = a->get_inv_mass() + b->get_inv_mass() +
				dot(a->get_inv_inertia() * cross(cross(contact.ra, tangent), contact.ra) +
					b->get_inv_inertia() * cross(cross(contact.rb, tangent), contact.rb), tangent);

			contact.mass_normal = 1 / kn;
			contact.mass_tangent = 1 / kt;
			contact.bias = -k_bias_factor / dt * std::min(0.0f, contact.separation + k_allowed_penetration);
		}
	}

	void arbiter::update_impulse() {
		const auto tangent = normal_.normal();
		auto a = a_.lock();
		auto b = b_.lock();
		for (auto &contact : contacts_) {
			const auto dv = b->get_velocity() + cross(b->get_angular_velocity(), contact.rb) -
				(a->get_velocity() + cross(a->get_angular_velocity(), contact.ra));

			const auto vn = dot(dv, normal_);
			auto dpn = (-vn + contact.bias) * contact.mass_normal;
			dpn = std::max(contact.pn + dpn, 0.0f) - contact.pn;

			const auto friction = sqrt(a->get_friction() * b->get_friction());
			const auto vt = dot(dv, tangent);
			auto dpt = -vt * contact.mass_tangent;
			dpt = std::max(-friction * contact.pn, std::min(friction * contact.pn, contact.pt + dpt)) - contact.pt;

			const auto p = dpn * normal_ + dpt * tangent;
			a->update_impulse(-p, contact.ra);
			b->update_impulse(p, contact.rb);
			contact.pn += dpn;
			contact.pt += dpt;
		}
	}

	void arbiter::update(const arbiter &old_arbiter) {
		const auto &old_contacts = old_arbiter.contacts_;
		const auto tangent = normal_.normal();
		auto a = a_.lock();
		auto b = b_.lock();
		for (auto &new_contact : contacts_) {
			const auto old_contact = find(old_contacts.begin(), old_contacts.end(), new_contact);
			if (old_contact != old_contacts.end()) {
				new_contact.pn = old_contact->pn;
				new_contact.pt = old_contact->pt;

				const auto p = new_contact.pn * normal_ + new_contact.pt * tangent;
				a->update_impulse(-p, new_contact.ra);
				b->update_impulse(p, new_contact.rb);
			}
		}
	}

	float arbiter::k_bias_factor;

	void arbiter::add_contact(const contact &contact) { contacts_.push_back(contact); }

	// 找出最小间隙
	static size_t incident_edge(const vec2 &n, const rigid_body &body) {
		auto idx = SIZE_MAX;
		auto min_dot = inf;
		// 遍历B物体的边
		for (size_t i = 0; i < body.count(); ++i) {
			// 获得边上的法向量
			const auto edge_normal = body.edge(i).normal();
			// 获得法向量在SAT轴上的投影长度
			const auto dot_ = dot(edge_normal, n);
			// 找出最小投影，即最小间隙
			if (dot_ < min_dot) {
				min_dot = dot_; // 最小间隙
				idx = i; // 返回索引
			}
		}
		return idx;
	}

	static size_t clip(arbiter::contact_list &contacts_out,
					   const arbiter::contact_list &contacts_in,
					   const size_t idx, const vec2 &v0, const vec2 &v1) {
		size_t num_out = 0;
		// 得到A物体的边v0_v1的单位向量
		const auto n = (v1 - v0).normalized();
		// dist0 = v0_B0 X N
		const auto dist0 = cross(contacts_in[0].position - v0, n);
		// dist1 = v0_B1 X N
		const auto dist1 = cross(contacts_in[1].position - v0, n);
		if (dist0 <= 0) {
			// v0_B0 与 N 共线或顺时针
			contacts_out[num_out++] = contacts_in[0];
		}
		if (dist1 <= 0) {
			// v0_B1 与 N 共线或顺时针
			contacts_out[num_out++] = contacts_in[1];
		}
		if (dist0 * dist1 < 0) { // 一正一负
			const auto total_dist = dist0 - dist1;
			auto v = (contacts_in[0].position * -dist1 + contacts_in[1].position * dist0) / total_dist;
			assert(!std::isnan(v.x) && !std::isnan(v.y));
			contacts_out[num_out].position = v;
			contacts_out[num_out].from_a[0] = true;
			contacts_out[num_out].indices[0] = idx;

			++num_out;
		}
		assert(num_out <= 2);
		return num_out;
	}

	arbiter::ptr arbiter::is_collide(rigid_body::ptr &pa, rigid_body::ptr &pb, uint32_t &id) {
		auto _pa = &pa;
		auto _pb = &pb;
		size_t ia, ib;
		float sa, sb;
		if ((sa = pa->min_separating_axis(ia, *pb)) >= 0) {
			id = MAKE_ID(*pa->get_id(), *pb->get_id());
			return nullptr;
		}
		if ((sb = pb->min_separating_axis(ib, *pa)) >= 0) {
			id = MAKE_ID(*pa->get_id(), *pb->get_id());
			return nullptr;
		}
		// 当且仅当SAT全为负，则表示相交
		if (sa < sb) { // 有序排列
			std::swap(sa, sb);
			std::swap(ia, ib);
			std::swap(_pa, _pb);
		}
		auto &a = **_pa;
		auto &b = **_pb;
		// 获得SAT的轴
		const auto n = a.edge(ia).normal();
		// 获得最小间隙时的B物体起点索引
		const auto idx = incident_edge(n, b);
		// 获得最小间隙时的B物体终点索引
		const auto next_idx = (idx + 1) % b.count();
		// 关节列表，暂时认为边 idx-next_idx 是包括在重叠部分中的
		// 下面需要判断B物体其余边是否与A物体重叠
		contact_list contacts = { {b, idx}, {b, next_idx} };
		auto clipped_contacts = contacts;
		// 遍历A物体
		for (size_t i = 0; i < a.count(); ++i) {
			if (i == ia) { // 非SAT轴的边
				continue;
			}
			// 起点
			const auto v0 = a.get_world_position() + a[i];
			// 终点
			const auto v1 = a.get_world_position() + a[(i + 1) % a.count()];
			auto num = clip(clipped_contacts, contacts, i, v0, v1);
			if (num < 2) {
				id = MAKE_ID(*a.get_id(), *b.get_id());
				return nullptr;
			}
			assert(num == 2);
			contacts = clipped_contacts;
		}

		const auto va = a.get_world_position() + a[ia];
		auto arbiter = world::create_arbiter(*_pa, *_pb, n);
		for (auto &contact : clipped_contacts) {
			const auto sep = dot(contact.position - va, n);
			if (sep <= 0) {
				contact.separation = sep;
				contact.ra = contact.position - a.get_world_position() + a.get_centroid();
				contact.rb = contact.position - b.get_world_position() + b.get_centroid();
				arbiter->add_contact(contact);
			}
		}
		id = MAKE_ID(*a.get_id(), *b.get_id());
		return arbiter;
	}

}