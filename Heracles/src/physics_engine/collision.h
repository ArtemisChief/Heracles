#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"
#include "world.h"

namespace heracles {

	class arbiter {
	protected:
		world* thisworld;
		const world::body_list& thisbodies = thisworld->get_bodies();
		arbiter_key* key = new arbiter_key();

		rigid_body::ptr thisbody1;
		rigid_body::ptr thisbody2;
		size_t idx;
		void solCollision();
		double dis_p2l(const vec2& vk, const vec2& v1, const vec2& v2);
	public:
		arbiter(world* thisworld);

		void testCollision();
	};

	class arbiter_key {
	public:
		void changeState(body& body1, body& body2, vec2& power, vec2& point);//修改碰撞后物体状态
	private:
		double calDistance(body& body1, vec2& power, vec2& point);//计算质点到力方向的距离
		double* calSpeed(body& body1, vec2& power);//计算速度在力方向的大小
	};

}
