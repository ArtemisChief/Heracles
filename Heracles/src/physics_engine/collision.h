#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"

namespace heracles {

	class arbiter {
	public:
		void changeState(body& body1, body& body2, vec2& power, vec2& point);//修改碰撞后物体状态
		double calDistance(body& body1, vec2& power, vec2& point);//计算质点到力方向的距离
		double* calSpeed(body& body1, vec2& power);//计算速度在力方向的大小
	};

	class arbiter_key {
	public:

	};

}
