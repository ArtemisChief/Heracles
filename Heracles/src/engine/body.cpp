#include "body.h"

namespace heracles {

	body::body(const uint16_t id, const float mass) :id_(id) {
		set_mass(mass);
	}

	bool body::can_collide(const body& other) const {
		return !(mass_ == inf && other.mass_ == inf);
	}

	void body::update_impulse(const vec2& impulse, const vec2& r) {
		velocity_ += impulse * inv_mass_;
		angular_velocity_ += inv_inertia_ * cross(r, impulse);
	}

	void body::update_force(const vec2& g, float dt) {
		if (mass_ == inf)
			return;
		velocity_ += (g + force_ * inv_mass_) * dt;
		angular_velocity_ += (torque_*inv_inertia_)*dt;
		world_position_ += velocity_ * dt;
		rotation_ = mat22(angular_velocity_*dt)*rotation_;
	}

	vec2 body::local_to_world(const vec2& local_position) const {
		return world_position_ + local_position;
	}

	uint16_t body::get_id() const { return id_; }

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
	void body::set_velocity(const vec2& velocity) { velocity_ = velocity; }
	void body::set_angular_velocity(const float angular_velocity) { angular_velocity_ = angular_velocity; }
	void body::set_force(const vec2& force) { force_ = force; }
	void body::set_torque(const float torque) { torque_ = torque; }
	void body::set_friction(const float friction) { friction_ = friction; }


}