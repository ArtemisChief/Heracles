#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"
#include <mutex>
#include "collision.h"
#include <unordered_map>

namespace heracles {

	class world {
	public:
		using body_list = std::vector<body::ptr>;
		using arbiter_list = std::unordered_map<uint32_t, arbiter::ptr>;

		world(const vec2& g);
		~world() = default;

		void clear();
		void step(const float dt);	//世界处理一次运算

		void add(const body::ptr body);
		const vec2& get_g() const;
		const body_list& get_bodies() const;
		const arbiter_list& get_arbiters() const;

		static rigid_body::ptr create_ground(const float width, const float height,
											 const vec2& world_position);

		static rigid_body::ptr create_triangle(
			const float mass, const float base, const float height,
			const vec2& world_position);

		static rigid_body::ptr create_rectangle(
			const float mass, const float width, const float height,
			const vec2& world_position);

		static arbiter::ptr create_arbiter(body::ptr a, body::ptr b,
								  const vec2 &normal,
								  const arbiter::contact_list &contacts = arbiter::contact_list());

	private:
		vec2 g_;	//重力加速度
		body_list bodies_;
		arbiter_list arbiters_;
		size_t iterations_{ 10 };
	};

}
