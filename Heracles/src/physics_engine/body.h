#pragma once
#include <vector>
#include "../util/math.h"
#include <map>

namespace heracles {

	class body {
	protected:
		DISALLOW_COPY_AND_ASSIGN(body);

		unsigned int id_;						// ����ID
		float mass_;							// ����
		float inv_mass_;						// ��������
		float inertia_;							// ת������
		float inv_inertia_;						// ת����������
		vec2  centroid_			{ 0, 0 };		// ����
		vec2  world_position_	{ 0, 0 };		// ��������λ��
		mat22 rotation_			{ 1, 0, 0, 1 };	// ��ת
		vec2  velocity_			{ 0, 0 };		// �ٶ�
		float angular_velocity_	{ 0 };			// ���ٶ�
		vec2  force_			{ 0, 0 };		// ����
		float torque_			{ 0 };			// Ť��
		float friction_			{ 1 };			// Ħ����

	public:
		using vertex_list = std::vector<vec2>;
		using ptr = std::shared_ptr < body >;

		const static vertex_list point;
		const static vertex_list line;
		const static vertex_list triangle;
		const static vertex_list rectangle;
		static std::map<const std::string, unsigned int> type_map;
		static std::map<const std::string, vertex_list> template_map;

		body(uint16_t id, float mass);
		virtual ~body() = default;

		bool can_collide(const body& other) const;					// �Ƿ���Է�����ײ�������͵��淵��false��
		void update_impulse(const vec2& impulse, const vec2& r);	// ���¶���
		void update_force(const vec2& g, const float dt);			// ������

		unsigned int* get_id();
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
		void set_rotation(const float angle);
		void set_velocity(const vec2& velocity);
		void set_angular_velocity(const float angular_velocity);
		void set_force(const vec2& force);
		void set_torque(const float torque);
		void set_friction(const float friction);
	};

	class rigid_body : public body {
	protected:
		
		DISALLOW_COPY_AND_ASSIGN(rigid_body);
		const vertex_list* vertices_;
		const mat22* scale_;

	public:
		using ptr = std::shared_ptr < rigid_body >;

		rigid_body(const unsigned int id, const float mass, const vertex_list* vertices, const mat22* scale);

		size_t count() const;
		vec2 operator[](size_t idx) const;
		vec2 edge(size_t idx) const;

		vertex_list get_vertices() const;
		mat22 get_scale() const;

		/* �����ᶨ��SAT��
		 * �����͹������Ƿ��ཻ
		 * ����:	idx		��������϶����
		 *		other	��һ��͹����θ������
		 *		
		 * ���:	float ���͵���ֵ�����ļ�϶ֵ���Ǹ����ʾ���ཻ
		 */
		float min_separating_axis(size_t& idx, const rigid_body& other) const;
	};
}
