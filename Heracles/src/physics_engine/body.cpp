#include "body.h"
#include <iostream>

namespace heracles {

	const body::vertex_list body::point = {
		vec2( 0.0f,   2.0f),
		vec2(-1.73f, -1.0f),
		vec2( 1.73f, -1.0f)
	};

	const body::vertex_list body::line = {
		vec2(-1.0f, -1.0f),
		vec2( 1.0f, -1.0f),
		vec2( 1.0f,  1.0f),
		vec2(-1.0f,  1.0f)
	};

	const body::vertex_list body::triangle = {
		vec2( 0.0f,   2.0f),
		vec2(-1.73f, -1.0f),
		vec2( 1.73f, -1.0f)
	};

	const body::vertex_list body::rectangle = {
		vec2(-2.0f, -2.0f),
		vec2( 2.0f, -2.0f),
		vec2( 2.0f,  2.0f),
		vec2(-2.0f,  2.0f)
	};

	std::map<const std::string, unsigned int> body::type_map{
		std::pair<const std::string, unsigned int>("point", 0),
		std::pair<const std::string, unsigned int>("line", 0),
		std::pair<const std::string, unsigned int>("triangle", 0),
		std::pair<const std::string, unsigned int>("rectangle" ,0)
	};

	std::map<const std::string, body::vertex_list> body::template_map{
		std::pair<const std::string, vertex_list>("point", point),
		std::pair<const std::string, vertex_list>("line", line),
		std::pair<const std::string, vertex_list>("triangle", triangle),
		std::pair<const std::string, vertex_list>("rectangle", rectangle)
	};

	body::body(const uint16_t id, const float mass) :id_(id) { set_mass(mass); }

	bool body::can_collide(const body& other) const { return !(mass_ == inf && other.mass_ == inf); }

	void body::update_impulse(const vec2& impulse, const vec2& r) {
		velocity_ += impulse * inv_mass_;
		angular_velocity_ += inv_inertia_ * cross(r, impulse);
	}

	void body::update_force(const vec2& g, const float dt) {
		if (mass_ == inf)
			return;
		velocity_ += (g + force_ * inv_mass_) * dt;
		//angular_velocity_ += (torque_*inv_inertia_)*dt;
		world_position_ += velocity_ * dt;
		//rotation_ = mat22(angular_velocity_*dt)*rotation_;
	}

	unsigned int* body::get_id() { return &id_; }
	float body::get_mass() const { return mass_; }
	float body::get_inv_mass() const { return inv_mass_; }
	float body::get_inertia() const { return inertia_; }
	float body::get_inv_inertia() const { return inv_inertia_; }
	const vec2& body::get_centroid() const { return centroid_; }
	const vec2& body::get_world_position() const { return world_position_; }
	const mat22& body::get_rotation() const { return rotation_; }
	const vec2& body::get_velocity() const { return velocity_; }
	float body::get_angular_velocity() const { return angular_velocity_; }
	const vec2& body::get_force() const { return force_; }
	float body::get_torque() const { return torque_; }
	float body::get_friction() const { return friction_; }

	void body::set_mass(const float mass) { mass_ = mass; inv_mass_ = 1 / mass; }
	void body::set_inertia(const float inertia) { inertia_ = inertia; inv_inertia_ = 1 / inertia; }
	void body::set_centroid(const vec2& centroid) { centroid_ = centroid; }
	void body::set_world_position(const vec2& world_position) { world_position_ = world_position; }
	void body::set_rotation(const mat22& rotation) { rotation_ = rotation; }
	void body::set_rotation(const float angle) { set_rotation(mat22(angle)); }
	void body::set_velocity(const vec2& velocity) { velocity_ = velocity; }
	void body::set_angular_velocity(const float angular_velocity) { angular_velocity_ = angular_velocity; }
	void body::set_force(const vec2& force) { force_ = force; }
	void body::set_torque(const float torque) { torque_ = torque; }
	void body::set_friction(const float friction) { friction_ = friction; }

	// 用向量叉乘算多边形面积
	static float calculate_area(const std::vector<vec2> &vertices) {
		float area = 0;
		const auto size = vertices.size();

		for (size_t i = 0; i < size; ++i) {
			auto j = (i + 1) % size;
			area += cross(vertices[i], vertices[j]);
		}
		return area / 2.0f;
	}

	// 计算多边形质心
	static vec2 calculate_centroid(const std::vector<vec2> &vertices) {
		vec2 centroid;
		const auto size = vertices.size();

		for (size_t i = 0; i < size; ++i) {
			auto j = (i + 1) % size;
			centroid += (vertices[i] + vertices[j]) * cross(vertices[i], vertices[j]);
		}
		return centroid / 6.0f / calculate_area(vertices);
	}

	// 计算多边形转动惯量
	static float calculate_inertia(const float mass, const body::vertex_list &vertices) {
		float sum = 0, sum_area = 0;
		const auto size = vertices.size();
		for (size_t i = 0; i < size; ++i) {
			const auto p0 = vertices[i];
			const auto p1 = vertices[(i + 1) % size];
			const auto area = abs(cross(p0, p1));
			sum += area * (dot(p0, p0) + dot(p1, p1) + dot(p0, p1));
			sum_area += area;
		}
		return mass / 6 * sum / sum_area;
	}

	rigid_body::rigid_body(const unsigned int id, const float mass, const vertex_list* vertices, const mat22* scale)
					:body(id, mass), vertices_(vertices), scale_(scale) {
		set_centroid(calculate_centroid(*vertices_));
		set_inertia(calculate_inertia(mass, *vertices_));
	}

	size_t rigid_body::count() const { return vertices_->size(); }

	vec2 rigid_body::operator[](size_t idx) const { return rotation_ * ((*vertices_)[idx] - centroid_) + centroid_; }

	vec2 rigid_body::edge(size_t idx) const { return (*this)[(idx + 1) % vertices_->size()] - (*this)[idx]; }

	body::vertex_list rigid_body::get_vertices() const { return *vertices_; }

	mat22 rigid_body::get_scale() const { return *scale_; }

	float rigid_body::min_separating_axis(size_t &idx, const rigid_body &other) const {
		auto separation = -inf;
		for (size_t i = 0; i < vertices_->size(); ++i) {
			const auto va = world_position_ + rotation_ * (*this)[i]  ;
			const auto n = edge(i).normal();
			auto min_sep = inf;
			for (size_t j = 0; j < other.count(); ++j) {
				const auto vb = other.world_position_ + other.rotation_ * other[j];
				min_sep = std::min(min_sep, dot(vb - va, n));
			}
			if (min_sep > separation) {
				separation = min_sep;
				idx = i;
			}
		}
		return separation; // 非负表示不相交
	}

}
