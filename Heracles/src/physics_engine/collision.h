#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"

namespace heracles {

	// �Ӵ���
	struct contact {
		vec2 position; // λ��
		vec2 ra, rb;
		std::array<bool, 2> from_a;
		std::array<size_t, 2> indices;
		float separation;
		float pn{ 0 };
		float pt{ 0 };
		float bias{ 0 };
		float mass_normal{ 0 };
		float mass_tangent{ 0 };

		contact(const rigid_body &b, size_t idx);

		bool operator==(const contact &other) const;
		bool operator!=(const contact &other) const;
	};

	// ��ײ����
	class arbiter {
	public:
		using contact_list = std::vector<contact>;
		using ptr = std::shared_ptr<arbiter>;

		arbiter(body::ptr a, body::ptr b, const vec2 &normal, const contact_list &contacts);

		const contact_list& get_contacts() const;
		const vec2& get_normal() const;

		void pre_step(float dt);
		void update_impulse();
		void update(const arbiter &old_arbiter);

		void add_contact(const contact &contact);

		static ptr is_collide(rigid_body::ptr &pa, rigid_body::ptr &pb, uint32_t &id);

	private:
		std::weak_ptr<body> _a, _b; // ������ײ������������
		vec2 _normal; // ������
		contact_list _contacts; // �Ӵ����б�
	};

}
