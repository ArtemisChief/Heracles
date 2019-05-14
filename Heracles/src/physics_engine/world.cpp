#include "world.h"
#include <iostream>

namespace heracles {

	world::world(const vec2& g) :g_(g) {}

	void world::clear() {
		arbiters_.clear();
		joints_.clear();
		bodies_.clear();
	}

	void world::step(const float dt) {
		// 碰撞检测
		for (size_t i = 0; i < bodies_.size(); ++i) {
			for (size_t j = i + 1; j < bodies_.size(); ++j) {
				auto a = std::dynamic_pointer_cast<rigid_body>(bodies_[i]);
				auto b = std::dynamic_pointer_cast<rigid_body>(bodies_[j]);
				if (!a->can_collide(*b)) {
					continue;
				}
				uint32_t id;
				auto arbiter = arbiter::is_collide(a, b, id);
				auto iter = arbiters_.find(id);
				if (arbiter == nullptr) {
					if (iter != arbiters_.end()) {
						arbiters_.erase(id);
					}
				}
				else if (iter == arbiters_.end()) {
					arbiters_[id] = arbiter;
				}
				else {
					auto &old_arbiter = iter->second;
					arbiter->update(*old_arbiter);
					arbiters_[id] = arbiter;
				}
			}
		}
		for (auto &kv : arbiters_) {
			kv.second->pre_step(dt);
		}

		for (auto &joint : joints_) {
			joint->pre_step(dt);
		}

		// 更新动量
		for (size_t i = 0; i < iterations_; ++i) {
			for (auto &kv : arbiters_) {
				kv.second->update_impulse();
			}

			for (auto &joint : joints_) {
				joint->update_impulse();
			}
		}

		// 更新外力
		for (auto& body : bodies_)
			body->update_force(g_, dt);
	}

	void world::lock() {
		mutex_.lock();
	}

	void world::unlock() {
		mutex_.unlock();
	}

	rigid_body::ptr world::check_point_in_poly(vec2& point) {
		if (!bodies_.empty()) {
			// 找到距离point最近的body
			body::ptr body;
			auto min_distance = inf;

			for (auto &b : bodies_) {
				const auto distance = (b->get_world_position() - point).magnitude();
				if (distance < min_distance) {
					min_distance = distance;
					body = b;
				}
			}

			if (body->get_mass()==inf) 
				return nullptr;

			// 检测point是否在body内部
			auto result = false;
			const auto rb = std::dynamic_pointer_cast<rigid_body>(body);
			const int n = rb->get_vertices().size();
			for (auto i = 0, j = n - 1; i < n; j = i++) {
				const auto verti = (*rb)[i] + rb->get_world_position();
				const auto vertj = (*rb)[j] + rb->get_world_position();
				if (verti.y > point.y != vertj.y > point.y &&
					point.x < (vertj.x - verti.x) * (point.y - verti.y) / (vertj.y - verti.y) + verti.x)
					result = !result;
			}

			if (result)
				return rb;
		}

		return nullptr;
	}

	void world::add_impulse(rigid_body::ptr rigid_body, vec2& force) {
		rigid_body->update_impulse(force * rigid_body->get_mass() * 5.0f, vec2());
	}

	void world::add(const body::ptr body) { bodies_.push_back(body); }

	void world::add(const joint::ptr joint) { joints_.push_back(joint); }

	void world::del(const body::ptr body) { 
		std::vector<body::ptr>::iterator it;
		for (it=bodies_.begin(); it != bodies_.end(); it++)
		{
			if (body == *it)
			{
				bodies_.erase(it);
				break;
			}
		}
	}

	const world::body_list& world::get_bodies() const { return bodies_; }

	const world::joint_list& world::get_joints() const { return joints_; }

	const world::arbiter_list &world::get_arbiters() const { return arbiters_; }

	const vec2& world::get_g() const { return g_; }

	rigid_body::ptr world::create_ground(const float width, const float height, const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2(width / 2,  height / 2),
			vec2(-width / 2,  height / 2),
			vec2(-width / 2, -height / 2),
			vec2(width / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(inf, vertices);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_tri_ground(const float base, const float height, const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2(0,  height / 2),
			vec2(-base / 2, -height / 2),
			vec2(base / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(inf, vertices);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_triangle(const float mass, const float base, const float height,
										   const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2(0,  height / 2),
			vec2(-base / 2, -height / 2),
			vec2(base / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(mass, vertices);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_rectangle(const float mass, const float width, const float height,
											const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2(width / 2,  height / 2),
			vec2(-width / 2,  height / 2),
			vec2(-width / 2, -height / 2),
			vec2(width / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(mass, vertices);
		body->set_world_position(world_position);
		return body;
	}

	arbiter::ptr world::create_arbiter(body::ptr a, body::ptr b, const vec2 & normal, const arbiter::contact_list & contacts) {
		return std::make_shared<arbiter>(a, b, normal, contacts);
	}

	revolute_joint::ptr world::create_revolute_joint(body::ptr a, body::ptr b, const vec2& anchor) {
		return std::make_shared<revolute_joint>(a, b, anchor);
	}

}