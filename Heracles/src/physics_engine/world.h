#pragma once
#include "../util/math.h"
#include <vector>
#include "body.h"
#include <mutex>
#include <map>

namespace heracles {

	class world {
	public:

		using body_list = std::vector<body::ptr>;

		world(const vec2& g);
		~world() = default;

		static polygon_body::ptr create_box(
			const float mass, const float width,
			const float height, const vec2& world_position);

		void clear();
		void step(float dt);	//���紦��һ������

		void add(const body::ptr body);
		const vec2& get_g() const;
		const body_list& get_bodies() const;

	private:

		vec2 g_;	//�������ٶ�
		body_list bodies_;
	};

}
