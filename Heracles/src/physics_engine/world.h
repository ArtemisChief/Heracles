#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"
#include <mutex>
#include "collision.h"
#include <unordered_map>
#include "joint.h"
#include <atomic>

namespace heracles {

	class world {
	public:

		using body_list = std::vector<body::ptr>;
		using joint_list = std::vector<joint::ptr>;
		using arbiter_list = std::unordered_map<uint32_t, arbiter::ptr>;

		static vec2 g;	//重力加速度

		static body_list bodies;
		static joint_list joints;
		static arbiter_list arbiters;

		static std::atomic<bool> is_paused;

		static std::mutex mutex;

		static size_t iterations;

		static void clear();
		static void step(const float dt);	//世界处理一次运算

		static void lock();
		static void unlock();

		static rigid_body::ptr check_point_in_poly(vec2& point);
		static void add_impulse(rigid_body::ptr rigid_body, vec2& force);

		static void add(const body::ptr body);
		static void add(const joint::ptr joint);

		static rigid_body::ptr create_ground(const float width, const float height,
											 const vec2& world_position);

		static rigid_body::ptr create_tri_ground(const float base, const float height,
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

		static revolute_joint::ptr create_revolute_joint(body::ptr a, body::ptr b, const vec2 &anchor);
	};

}
