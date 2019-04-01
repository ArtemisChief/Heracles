#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"

namespace heracles {

	class arbiter {
	public:
		void changeState(body& body1, body& body2, vec2& power, vec2& point);//�޸���ײ������״̬
		double calDistance(body& body1, vec2& power, vec2& point);//�����ʵ㵽������ľ���
		double* calSpeed(body& body1, vec2& power);//�����ٶ���������Ĵ�С
	};

	class arbiter_key {
	public:

	};

}
