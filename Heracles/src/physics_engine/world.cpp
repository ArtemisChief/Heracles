#include "world.h"

namespace heracles {
	world::world(const vec2& g) :g_(g) {}

	polygon_body::ptr world::create_box(const float mass, const float width, const float height, const vec2& world_position) {
		body::vertex_list vertices = {
		vec2(-width / 2, -height / 2), vec2(0.0f, 0.0f), 	// Bottom Left
		vec2( width / 2, -height / 2), vec2(1.0f, 0.0f),	// Bottom Right
		vec2( width / 2,  height / 2), vec2(1.0f, 1.0f),	// Top Right
		vec2(-width / 2,  height / 2), vec2(0.0f, 1.0f),	// Top Left
		};
		auto body = std::make_shared<polygon_body>(0, mass, vertices);
		body->set_world_position(world_position);
		return body;
	}

	void world::clear() {
		bodies_.clear();
	}

	void world::step(float dt) {

	}
	void world::add(const body::ptr body) { bodies_.push_back(body); }

	const world::body_list& world::get_bodies() const { return bodies_; }

	const vec2& world::get_g() const { return g_; }

}