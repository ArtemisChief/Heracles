#include "collision.h"
#include <iostream>

namespace heracles {
	void arbiter::changeState(body& body1, body& body2, vec2& power, vec2& point)
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

	double arbiter::calDistance(body& body1, vec2& power, vec2& point)
	{
		double f1 = (double)power.y * body1.get_world_position().x / power.x + point.y - (double)power.y * point.x / power.x;
		double f2 = sqrt(pow(power.y / power.x, 2) + 1);
		return f1 / f2;
	}

	double* arbiter::calSpeed(body& body1, vec2& power)
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