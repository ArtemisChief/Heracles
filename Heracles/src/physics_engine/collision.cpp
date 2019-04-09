#include "collision.h"
#include <vector>
#include <iostream>

namespace heracles {

	arbiter::arbiter(world* thisworld)
	{
		this->thisworld = thisworld;
	}

	void arbiter::testCollision()//暂时假设全为凸多边形
	{
		size_t size = thisbodies.size();
		for (size_t i = 0; i < size; ++i)
		{
			thisbody1 = thisbodies.at(i);
			for (size_t j = i + 1; j < size; ++j)
			{
				thisbody2 = thisbodies.at(j);
				if (thisbody1->min_separating_axis(idx, *thisbody2) <= 0)//确认碰撞
				{
					solCollision();
				}
			}
		}
	}

	void arbiter::solCollision()//暂时认为1的点进入2里面
	{
		size_t size = thisbody1->count();
		size_t size2 = thisbody2->count();
		std::vector<size_t> insideV;
		int j, k;
		bool l = false;

		vec2 v1, v2, vk;

		vec2 vp1 = thisbody1->get_world_position();
		vec2 vp2 = thisbody2->get_world_position();

		for (size_t i = 0; i < size; ++i)//判断有哪些点在里面
		{
			vk = thisbody1->get_vertices().at(i) + vp1;
			for (j = 0, k = size2 - 1; j < size2; k = j++)
			{
				v1 = thisbody2->get_vertices().at(j) + vp2;
				v2 = thisbody2->get_vertices().at(k) + vp2;
				if (((v1.y>vk.y) != (v2.y>vk.y)) &&
					(vk.x < (v2.x - v1.x) * (vk.y - v1.y) / (v2.y - v1.y) + v1.x))
					l = !l;
			}
			if (l) insideV.push_back(i);
		}

		size = insideV.size();//此时v1v2vk分别为当前过去坐标和碰撞点
		vec2 post_vp1 = thisbody1->getPost_position();
		v1.x = 0;
		v2.x = 0;
		v1.y = 0;
		v2.y = 0;

		body::vertex_list vertex1 = thisbody1->get_vertices();
		body::vertex_list vertex2 = thisbody1->getPostvertices();
		for (size_t i = 0; i < size; ++i)
		{
			v1.x += vertex1.at(i).x + vp1.x;
			v2.x += vertex2.at(i).x + post_vp1.x;
			v1.y += vertex1.at(i).y + vp1.y;
			v2.y += vertex2.at(i).y + post_vp1.y;
		}
		v1.x /= size;
		v1.y /= size;
		v2.x /= size;
		v2.y /= size;

		size = thisbody2->count();
		vertex1 = thisbody2->get_vertices();
		double dis = inf;
		size_t id;//碰撞的第几条边
		for (size_t i = 0; i < size; ++i)
		{
			double d = dis_p2l(v1, vertex1.at(i) + vp2, vertex1.at((i+1)%size) + vp2);
			if (d < dis)
			{
				dis = d;
				id = i;
			}
		}

		vk = l2l(v1, v2, vertex1.at(id) + vp2, vertex1.at((id + 1) % size) + vp2);//碰撞点;
		v1 = vertex1.at(id) + vp2;
		v2 = vertex1.at((id + 1) % size) + vp2;
		vec2 power = vec2(v2.y - v1.y, v1.x - v2.x);
		key->changeState(*thisbody1, *thisbody2, power, vk);
	}

	double arbiter::dis_p2l(const vec2& vk, const vec2& v1, const vec2& v2)//计算点到直线距离
	{
		double d1 = fabs(vk.x / (v1.x - v2.x) + vk.y / (v2.y - v1.y) + v2.x / (v2.x - v1.x) + v2.y / (v1.y - v2.y));
		double d2 = sqrt((1 / (v1.x - v2.x)) * (1 / (v1.x - v2.x)) + (1 / (v1.y - v2.y)) * (1 / (v1.y - v2.y)));
		return d1 / d2;
	}

	vec2 l2l(const vec2& v1, const vec2& v2, const vec2& v3, const vec2& v4)//计算直线交点
	{
		double a1, a2, b1, b2, c1, c2;
		a1 = v1.x - v2.x;
		b1 = v2.y - v1.y;
		c1 = v2.x / (v2.x - v1.x) + v2.y / (v1.y - v2.y);
		a2 = v3.x - v4.x;
		b2 = v4.y - v3.y;
		c2 = v4.x / (v4.x - v3.x) + v4.y / (v3.y - v4.y);

		double x = (c1*b2 - c2 * b1) / (a2*b1 - a1 * b2);
		double y = (a2*c1 - a1 * c2) / (a1*b2 - a2 * b1);
		return vec2(x, y);
	}


	void arbiter_key::changeState(body& body1, body& body2, vec2& power, vec2& point)
	{
		if (power.y < 0)
		{
			power.x = -power.x;
			power.y = -power.y;
		}//保证正方向向上

		double la = calDistance(body1, power, point);
		double lb = calDistance(body2, power, point);

		double* va = calSpeed(body1, power);
		double* vb = calSpeed(body2, power);

		double I = 2 * (va[1] + vb[1] + la * body1.get_angular_velocity() + lb * body2.get_angular_velocity())
			/ (body1.get_inv_mass() + body1.get_inv_inertia() + body2.get_inv_mass() + body2.get_inv_inertia());//冲量

		double va2 = I * body1.get_inv_mass() + va[1];
		double vb2 = -I * body2.get_inv_mass() + vb[1];

		double angle = atan((double)power.y / power.x);
		double vax = va2 * cos(angle) + va[0] * sin(angle);
		double vay = va2 * sin(angle) + va[0] * cos(angle);
		double vbx = vb2 * cos(angle) + vb[0] * sin(angle);
		double vby = vb2 * sin(angle) + vb[0] * cos(angle);

		double wa2 = I * la * body1.get_inv_inertia() + body1.get_angular_velocity();
		double wb2 = I * lb * body2.get_inv_inertia() + body2.get_angular_velocity();

		body1.set_velocity(vec2(vax,vay));
		body1.set_angular_velocity(wa2);
		body2.set_velocity(vec2(vbx, vby));
		body2.set_angular_velocity(wb2);
	}

	double arbiter_key::calDistance(body& body1, vec2& power, vec2& point)
	{
		double f1 = fabs((double)power.y * body1.get_world_position().x / power.x + point.y - (double)power.y * point.x / power.x);
		double f2 = sqrt(pow(power.y / power.x, 2) + 1);
		return f1 / f2;
	}

	double* arbiter_key::calSpeed(body& body1, vec2& power)
	{
		float f1 = (float)body1.get_velocity().y / body1.get_velocity().x - (float)power.y / power.x;
		float f2 = 1 + (float)body1.get_velocity().y * power.y / (body1.get_velocity().x * power.x);
		float angel = atan(f1 / f2);
		
		double vy = sqrt(body1.get_velocity().y * body1.get_velocity().y + body1.get_velocity().x * body1.get_velocity().x) * cos(angel);
		double vx = sqrt(body1.get_velocity().y * body1.get_velocity().y + body1.get_velocity().x * body1.get_velocity().x) * sin(angel);
		double v[2] = { vx, vy };
		return v;
	}
}