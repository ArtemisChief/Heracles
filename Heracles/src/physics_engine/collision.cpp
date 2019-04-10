#include "collision.h"
#include <iostream>
#include <algorithm>
#include "world.h"

namespace heracles {

	contact::contact(const rigid_body &b, size_t idx) {
		indices = { {idx, idx} };
		std::fill(from_a.begin(), from_a.end(), false);
		position = b.get_world_position() + b[idx];
	}

	bool contact::operator==(const contact &other) const {
		if (from_a == other.from_a && indices == other.indices) {
			return true;
		}
		decltype(from_a) from_a_swap = { {from_a[1], from_a[0]} };
		decltype(indices) indices_swap = { {indices[1], indices[0]} };
		return from_a_swap == other.from_a && indices_swap == other.indices;
	}

	bool contact::operator!=(const contact &other) const {
		return !(*this == other);
	}

	// ---------------------------------------------------
	// pair

	arbiter::arbiter(body::ptr a, body::ptr b, const vec2 &normal, const arbiter::contact_list &contacts)
		: _a(a), _b(b), _normal(normal), _contacts(contacts) {
	}

	const arbiter::contact_list &arbiter::get_contacts() const {
		return _contacts;
	}

	const vec2 &arbiter::get_normal() const {
		return _normal;
	}

	void arbiter::pre_step(float dt) {
		static const float kAllowedPenetration = 0.01;
		static const float kBiasFactor = 0.2; // ������ײϵ����1.0Ϊ��ȫ������ײ
		auto tangent = _normal.normal();
		auto a = _a.lock();
		auto b = _b.lock();
		for (auto &contact : _contacts) {
			auto kn = a->get_inv_mass() + b->get_inv_mass() +
				dot(a->get_inv_inertia() * cross(cross(contact.ra, _normal), contact.ra) +
					b->get_inv_inertia() * cross(cross(contact.rb, _normal), contact.rb), _normal);
			auto kt = a->get_inv_mass() + b->get_inv_mass() +
				dot(a->get_inv_inertia() * cross(cross(contact.ra, tangent), contact.ra) +
					b->get_inv_inertia() * cross(cross(contact.rb, tangent), contact.rb), tangent);

			contact.mass_normal = 1 / kn;
			contact.mass_tangent = 1 / kt;
			contact.bias = -kBiasFactor / dt * std::min(0.0f, contact.separation + kAllowedPenetration);
		}
	}

	void arbiter::update_impulse() {
		auto tangent = _normal.normal();
		auto a = _a.lock();
		auto b = _b.lock();
		for (auto &contact : _contacts) {
			vec2 dv = (b->get_velocity() + cross(b->get_angular_velocity(), contact.rb)) -
				(a->get_velocity() + cross(a->get_angular_velocity(), contact.ra));

			auto vn = dot(dv, _normal);
			auto dpn = (-vn + contact.bias) * contact.mass_normal;
			dpn = std::max(contact.pn + dpn, 0.0f) - contact.pn;

			float friction = std::sqrt(a->get_friction() * b->get_friction());
			auto vt = dot(dv, tangent);
			auto dpt = -vt * contact.mass_tangent;
			dpt = std::max(-friction * contact.pn, std::min(friction * contact.pn, contact.pt + dpt)) - contact.pt;

			auto p = dpn * _normal + dpt * tangent;
			a->update_impulse(-p, contact.ra);
			b->update_impulse(p, contact.rb);
			contact.pn += dpn;
			contact.pt += dpt;
		}
	}

	void arbiter::update(const arbiter &old_arbiter) {
		const auto &old_contacts = old_arbiter._contacts;
		auto tangent = _normal.normal();
		auto a = _a.lock();
		auto b = _b.lock();
		for (auto &new_contact : _contacts) {
			auto old_contact = std::find(old_contacts.begin(), old_contacts.end(), new_contact);
			if (old_contact != old_contacts.end()) {
				new_contact.pn = old_contact->pn;
				new_contact.pt = old_contact->pt;

				auto p = new_contact.pn * _normal + new_contact.pt * tangent;
				a->update_impulse(-p, new_contact.ra);
				b->update_impulse(p, new_contact.rb);
			}
		}
	}

	void arbiter::add_contact(const contact &contact) {
		_contacts.push_back(contact);
	}

	// ---------------------------------------------------
	// collision detection

		// �ҳ���С��϶
	static size_t incident_edge(const vec2 &N, const rigid_body &body) {
		size_t idx = SIZE_MAX;
		auto min_dot = inf;
		// ����B����ı�
		for (size_t i = 0; i < body.count(); ++i) {
			// ��ñ��ϵķ�����
			auto edge_normal = body.edge(i).normal();
			// ��÷�������SAT���ϵ�ͶӰ����
			auto _dot = dot(edge_normal, N);
			// �ҳ���СͶӰ������С��϶
			if (_dot < min_dot) {
				min_dot = _dot; // ��С��϶
				idx = i; // ��������
			}
		}
		return idx;
	}

	static size_t clip(arbiter::contact_list &contacts_out,
					   const arbiter::contact_list &contacts_in,
					   size_t idx, const vec2 &v0, const vec2 &v1) {
		size_t num_out = 0;
		// �õ�A����ı�v0_v1�ĵ�λ����
		auto N = (v1 - v0).normalized();
		// dist0 = v0_B0 X N
		auto dist0 = cross(contacts_in[0].position - v0, N);
		// dist1 = v0_B1 X N
		auto dist1 = cross(contacts_in[1].position - v0, N);
		if (dist0 <= 0) {
			// v0_B0 �� N ���߻�˳ʱ��
			contacts_out[num_out++] = contacts_in[0];
		}
		if (dist1 <= 0) {
			// v0_B1 �� N ���߻�˳ʱ��
			contacts_out[num_out++] = contacts_in[1];
		}
		if (dist0 * dist1 < 0) { // һ��һ��
			auto total_dist = dist0 - dist1;
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
		// ���ҽ���SATȫΪ�������ʾ�ཻ
		if (sa < sb) { // ��������
			std::swap(sa, sb);
			std::swap(ia, ib);
			std::swap(_pa, _pb);
		}
		auto &a = **_pa;
		auto &b = **_pb;
		// ���SAT����
		auto N = a.edge(ia).normal();
		// �����С��϶ʱ��B�����������
		auto idx = incident_edge(N, b);
		// �����С��϶ʱ��B�����յ�����
		auto next_idx = (idx + 1) % b.count();
		// �ؽ��б���ʱ��Ϊ�� idx-next_idx �ǰ������ص������е�
		// ������Ҫ�ж�B����������Ƿ���A�����ص�
		arbiter::contact_list contacts = { {b, idx},
										{b, next_idx} };
		auto clipped_contacts = contacts;
		// ����A����
		for (size_t i = 0; i < a.count(); ++i) {
			if (i == ia) { // ��SAT��ı�
				continue;
			}
			// ���
			auto v0 = a.get_world_position() + a[i];
			// �յ�
			auto v1 = a.get_world_position() + a[(i + 1) % a.count()];
			auto num = clip(clipped_contacts, contacts, i, v0, v1);
			if (num < 2) {
				id = MAKE_ID(*a.get_id(), *b.get_id());
				return nullptr;
			}
			assert(num == 2);
			contacts = clipped_contacts;
		}

		auto va = a.get_world_position() + a[ia];
		auto arbiter = world::create_arbiter(*_pa, *_pb, N);
		for (auto &contact : clipped_contacts) {
			auto sep = dot(contact.position - va, N);
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
