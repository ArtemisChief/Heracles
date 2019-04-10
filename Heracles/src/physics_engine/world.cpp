#include "world.h"
#include <iostream>

namespace heracles {

	world::world(const vec2& g) :g_(g) {}

	void world::clear() {
		arbiters_.clear();
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

		// 更新动量
		for (size_t i = 0; i < iterations_; ++i) {
			for (auto &kv : arbiters_) {
				kv.second->update_impulse();
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

	void world::add(const body::ptr body) { bodies_.push_back(body); }

	const world::body_list& world::get_bodies() const { return bodies_; }

	const world::arbiter_list &world::get_arbiters() const { return arbiters_; }

	const vec2& world::get_g() const { return g_; }

	rigid_body::ptr world::create_ground(const float width, const float height, const vec2& world_position)
	{
		rigid_body::vertex_list vertices = {
			vec2( width / 2,  height / 2),
			vec2(-width / 2,  height / 2),
			vec2(-width / 2, -height / 2),
			vec2( width / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(NULL, inf, vertices);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_triangle(const float mass, const float base, const float height,
										   const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2(		 0,  height / 2),
			vec2(-base / 2, -height / 2),
			vec2( base / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(NULL, mass, vertices);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_rectangle(const float mass, const float width, const float height,
											const vec2& world_position) {
		rigid_body::vertex_list vertices = {
			vec2( width / 2,  height / 2),
			vec2(-width / 2,  height / 2),
			vec2(-width / 2, -height / 2),
			vec2( width / 2, -height / 2)
		};
		auto body = std::make_shared<rigid_body>(NULL, mass, vertices);
		body->set_world_position(world_position);
		return body;
	}

	arbiter::ptr world::create_arbiter(body::ptr a, body::ptr b, const vec2 & normal, const arbiter::contact_list & contacts) {
		return std::make_shared<arbiter>(a, b, normal, contacts);
	}

}
