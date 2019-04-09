#include "world.h"
#include <iostream>

namespace heracles {

	world::world(const vec2& g) :g_(g) {}

	void world::clear() { bodies_.clear(); }

	void world::step(const float dt) {
		for (auto& body : bodies_)
			body->update_force(g_, dt);
	}

	void world::add(const body::ptr body) { bodies_.push_back(body); }

	const world::body_list& world::get_bodies() const { return bodies_; }

	const vec2& world::get_g() const { return g_; }

	rigid_body::ptr world::create_rigid_body(const std::string type, const float mass, const mat22* scale,
														 const vec2& world_position) const {
		auto vertices = &body::template_map[type];
		auto body = std::make_shared<rigid_body>(body::type_map[type], mass, vertices, scale);
		body->set_world_position(world_position);
		return body;
	}

	rigid_body::ptr world::create_point(const vec2& world_position) const {
		const auto s = new mat22{ 0.0017f, 0.0f, 0.0f,  0.0017f };
		return create_rigid_body("point", inf, s, world_position);
	}

	rigid_body::ptr world::create_line(const float length, const vec2& world_position) const {
		const auto scale = new mat22{ length / 2.0f, 0.0f, 0.0f,  0.0017f };
		return create_rigid_body("line", inf, scale, world_position);
	}

	rigid_body
		::ptr world::create_triangle(const float mass, const float base, const float height, 
										   const vec2& world_position) const {
		const auto scale = new mat22{ base / 3.46f, 0.0f, 0.0f,  height / 3.0f };
		return create_rigid_body("triangle", mass, scale, world_position);
	}

	rigid_body::ptr world::create_rectangle(const float mass, const float width, const float height, 
											const vec2& world_position) const {
		const auto scale = new mat22{ width / 4.0f, 0.0f, 0.0f,  height / 4.0f };
		return create_rigid_body("rectangle", mass, scale, world_position);
	}

}
