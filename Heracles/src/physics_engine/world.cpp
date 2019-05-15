#include "world.h"

namespace heracles {

	vec2 world::g;

	world::body_list world::bodies;

	world::joint_list world::joints;

	world::arbiter_list world::arbiters;

	std::atomic<bool> world::is_paused{ false };

	std::mutex world::mutex;

	size_t world::iterations{ 10 };

	void world::clear() {
		arbiters.clear();
		joints.clear();
		bodies.clear();
	}

	void world::step(const float dt) {
		// 碰撞检测
		for (size_t i = 0; i < bodies.size(); ++i) {
			for (auto j = i + 1; j < bodies.size(); ++j) {
				auto a = std::dynamic_pointer_cast<rigid_body>(bodies[i]);
				auto b = std::dynamic_pointer_cast<rigid_body>(bodies[j]);
				if (!a->can_collide(*b)) {
					continue;
				}
				uint32_t id;
				auto arbiter = arbiter::is_collide(a, b, id);
				auto iter = arbiters.find(id);
				if (arbiter == nullptr) {
					if (iter != arbiters.end()) {
						arbiters.erase(id);
					}
				}
				else if (iter == arbiters.end()) {
					arbiters[id] = arbiter;
				}
				else {
					auto &old_arbiter = iter->second;
					arbiter->update(*old_arbiter);
					arbiters[id] = arbiter;
				}
			}
		}
		for (auto &kv : arbiters) {
			kv.second->pre_step(dt);
		}

		for (auto &joint : joints) {
			joint->pre_step(dt);
		}

		// 更新动量
		for (size_t i = 0; i < iterations; ++i) {
			for (auto &kv : arbiters) {
				kv.second->update_impulse();
			}

			for (auto &joint : joints) {
				joint->update_impulse();
			}
		}

		// 更新外力
		for (auto& body : bodies)
			body->update_force(g, dt);
	}

	void world::lock() {
		mutex.lock();
	}

	void world::unlock() {
		mutex.unlock();
	}

	rigid_body::ptr world::check_point_in_poly(vec2& point)
	{
		if (!bodies.empty()) {
			// 找到距离point最近的body
			body::ptr body;
			auto min_distance = inf;

			for (auto &b : bodies) {
				const auto distance = (b->get_world_position() - point).magnitude();
				if (distance < min_distance) {
					min_distance = distance;
					body = b;
				}
			}

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

	void world::add(const body::ptr body) { bodies.push_back(body); }

	void world::add(const joint::ptr joint) { joints.push_back(joint); }

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