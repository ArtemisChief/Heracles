#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"
#include "collision.h"
#include <mutex>

namespace heracles {

	class world {
	public:
		using body_list = std::vector<body::ptr>;

		world(const vec2& g);
		~world() = default;

		void clear();
		void step(const float dt);	//世界处理一次运算

		void add(const body::ptr body);
		const vec2& get_g() const;
		const body_list& get_bodies() const;

		rigid_body::ptr create_rigid_body(
			const std::string type,
			const float mass, const mat22* scale,
			const vec2& world_position) const;

		rigid_body::ptr create_point(const vec2& world_position) const;

		rigid_body::ptr create_line(const float length, const vec2& world_position) const;

		rigid_body::ptr create_triangle(
			const float mass, const float base, const float height,
			const vec2& world_position) const;

		rigid_body::ptr create_rectangle(
			const float mass, const float width, const float height,
			const vec2& world_position) const;

	private:
		vec2 g_;	//重力加速度
		body_list bodies_;
		arbiter* ar = new arbiter(this);
	};

}