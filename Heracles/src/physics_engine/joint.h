#pragma once
#include <memory>
#include "../util/math.h"
#include "body.h"

namespace heracles {

	//����
	class joint {
	public:
		using ptr = std::shared_ptr<joint>;

		joint(const body::ptr a, const body::ptr b);
		virtual ~joint() = default;

		virtual void pre_step(float dt) = 0;
		virtual void update_impulse() = 0;

		unsigned int* get_id_a();
		unsigned int* get_id_b();
		body::ptr get_a() const;
		void set_a(const body::ptr a);
		body::ptr get_b() const;
		void set_b(const body::ptr b);

	protected:
		DISALLOW_COPY_AND_ASSIGN(joint)

		unsigned int id_a_;
		unsigned int id_b_;
		std::weak_ptr<body> a_, b_; // �������������������

	};

	//��ת����
	class revolute_joint : public joint {
	public:
		using ptr = std::shared_ptr<revolute_joint>;

		revolute_joint(const body::ptr a, const body::ptr b, const vec2& anchor);

		void pre_step(const float dt) override;
		void update_impulse() override;

		const vec2& anchor() const;
		vec2 world_anchor_a() const;
		vec2 world_anchor_b() const;

		const vec2& get_anchor();
		const vec2& get_local_anchor_a();
		const vec2& get_local_anchor_b();

	protected:
		DISALLOW_COPY_AND_ASSIGN(revolute_joint)

		vec2 anchor_; // �̶�λ�ã��������꣩
		vec2 local_anchor_a_; // ����a�������
		vec2 local_anchor_b_; // ����b�������

		vec2 ra_; // a��ת�Ƕ�����
		vec2 rb_; // b��ת�Ƕ�����
		mat22 mass_; // ������
		vec2 p_; // ����
		vec2 bias_; // ����
	};

}
