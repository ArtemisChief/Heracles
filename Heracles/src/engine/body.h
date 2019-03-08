#pragma once
#include <vector>
#include "../util/math.h"

namespace heracles {

	class body {
	protected:
		DISALLOW_COPY_AND_ASSIGN(body);

		uint16_t id_;							// 刚体ID
		float mass_;							// 质量
		float inv_mass_;						// 质量倒数
		float inertia_;							// 转动惯量
		float inv_inertia_;						// 转动惯量倒数
		vec2  centroid_			{ 0, 0 };		// 质心
		vec2  world_position_	{ 0, 0 };		// 世界坐标位置
		mat22 rotation_			{ mat22::i };	// 旋转
		vec2  velocity_			{ 0, 0 };		// 速度
		float angular_velocity_	{ 0 };			// 角速度
		vec2  force_			{ 0, 0 };		// 受力
		float torque_			{ 0 };			// 扭矩
		float friction_			{ 1 };			// 摩擦力

	public:
		using vertex_list = std::vector<vec2>;
		using ptr = std::shared_ptr < body >;

		body(uint16_t id, float mass);
		virtual ~body() = default;

		bool can_collide(const body& other) const;					// 是否可以发生碰撞（如刚体和地面返回false）
		void update_impulse(const vec2& impulse, const vec2& r);	// 更新动量
		void update_force(const vec2& g, float dt);					// 更新力

		uint16_t get_id() const;

		float get_mass() const;
		float get_inv_mass() const;
		float get_inertia() const;
		float get_inv_inertia() const;
		const vec2& get_centroid() const;
		const vec2& get_world_position() const;
		const mat22& get_rotation() const;
		const vec2& get_velocity() const;
		float get_angular_velocity() const;
		const vec2& get_force() const;
		float get_torque() const;
		float get_friction() const;

		void set_mass(const float mass);
		void set_inertia(const float inertia);
		void set_centroid(const vec2& centroid);
		void set_world_position(const vec2& world_position);
		void set_rotation(const mat22& rotation);
		void set_velocity(const vec2& velocity);
		void set_angular_velocity(const float angular_velocity);
		void set_force(const vec2& force);
		void set_torque(const float torque);
		void set_friction(const float friction);
	};

	class polygon_body : public body {
	protected:
		DISALLOW_COPY_AND_ASSIGN(polygon_body);
		vertex_list vertices_;

	public:
		using ptr = std::shared_ptr < polygon_body >;

		polygon_body(uint16_t id, float mass, const vertex_list vertices);

		size_t count() const;
		vec2 operator[](size_t idx) const;
		vec2 edge(size_t idx) const;

		/* 分离轴定理（SAT）
		 * 检测两凸多边形是否相交
		 * 输入:	idx		产生最大间隙的轴
		 *		other	另一个凸多边形刚体对象
		 *		
		 * 输出:	float 类型的数值，最大的间隙值，非负则表示不相交
		 */
		float min_separating_axis(size_t& idx, const polygon_body& other) const;
	};
}