#pragma once
#include <memory>
#include "../util/math.h"
#include "body.h"

namespace heracles {

	//铰链
	class joint {
	public:
		using ptr = std::shared_ptr<joint>;

		joint(const body::ptr a, const body::ptr b);
		virtual ~joint() = default;

		virtual void pre_step(float dt) = 0;
		virtual void update_impulse() = 0;

		body::ptr get_a() const;
		void set_a(const body::ptr a);
		body::ptr get_b() const;
		void set_b(const body::ptr b);

	protected:
		DISALLOW_COPY_AND_ASSIGN(joint)

		std::weak_ptr<body> a_, b_; // 铰链相联结的两个刚体

	};

	//旋转铰链
	class revolute_joint : public joint {
	public:
		using ptr = std::shared_ptr<joint>;

		revolute_joint(const body::ptr a, const body::ptr b, const vec2& anchor);

		void pre_step(const float dt) override;
		void update_impulse() override;

		const vec2& anchor() const;
		vec2 world_anchor_a() const;
		vec2 world_anchor_b() const;

	protected:
		DISALLOW_COPY_AND_ASSIGN(revolute_joint)

		vec2 anchor_; // 固定位置（世界坐标）
		vec2 local_anchor_a_; // 刚体a相对坐标
		vec2 local_anchor_b_; // 刚体b相对坐标

		vec2 ra_; // a旋转角度向量
		vec2 rb_; // b旋转角度向量
		mat22 mass_; // 总质量
		vec2 p_; // 动量
		vec2 bias_; // 修正
	};

}
