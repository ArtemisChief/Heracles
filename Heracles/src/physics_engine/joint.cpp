#include "joint.h"

namespace heracles {

	joint::joint(const body::ptr a, const body::ptr b) : id_a_(0), id_b_(0), a_(a), b_(b) {
	}

	unsigned int* joint::get_id_a() { return &id_a_; }

	unsigned int* joint::get_id_b() { return &id_b_; }

	body::ptr joint::get_a() const {
		return a_.lock();
	}

	void joint::set_a(const body::ptr a) {
		a_ = a;
	}

	body::ptr joint::get_b() const {
		return b_.lock();
	}

	void joint::set_b(const body::ptr b) {
		b_ = b;
	}

	revolute_joint::revolute_joint(const body::ptr a, const body::ptr b, const vec2 &anchor)
		: joint(a, b), anchor_(anchor) {
		local_anchor_a_ = a->get_rotation().transpose() * (anchor_ - a->get_world_position() + a->get_centroid());
		local_anchor_b_ = b->get_rotation().transpose() * (anchor_ - b->get_world_position() + b->get_centroid());
	}

	void revolute_joint::pre_step(const float dt) {
		static const float k_bias_factor = 0.2;
		auto a = a_.lock();
		auto b = b_.lock();
		ra_ = a->get_rotation() * local_anchor_a_;
		rb_ = b->get_rotation() * local_anchor_b_;
		auto k = (a->get_inv_mass() + b->get_inv_mass()) * mat22::i +
			a->get_inv_inertia() * mat22(ra_.y*ra_.y, -ra_.y*ra_.x, -ra_.y*ra_.x, ra_.x*ra_.x) +
			b->get_inv_inertia() * mat22(rb_.y*rb_.y, -rb_.y*rb_.x, -rb_.y*rb_.x, rb_.x*rb_.x);
		mass_ = k.inv();
		bias_ = -k_bias_factor / dt * (b->get_world_position() + b->get_centroid()
									 + rb_ - a->get_world_position() + a->get_centroid() - ra_);
		a->update_impulse(-p_, ra_);
		b->update_impulse(p_, rb_);
	}

	void revolute_joint::update_impulse() {
		auto a = a_.lock();
		auto b = b_.lock();
		const auto dv = (b->get_velocity() + cross(b->get_angular_velocity(), rb_)) -
			(a->get_velocity() + cross(a->get_angular_velocity(), ra_));
		const auto p = mass_ * (-1.0 * dv + bias_);
		a->update_impulse(-p, ra_);
		b->update_impulse(p, rb_);
		p_ += p;
	}

	const vec2 &revolute_joint::anchor() const { return anchor_; }

	vec2 revolute_joint::world_anchor_a() const {
		const auto a = a_.lock();
		return a->get_world_position() + (a->get_rotation() * local_anchor_a_ + a->get_centroid());
	}

	vec2 revolute_joint::world_anchor_b() const {
		const auto b = b_.lock();
		return b->get_world_position() + (b->get_rotation() * local_anchor_b_ + b->get_centroid());
	}

	const vec2& revolute_joint::get_local_anchor_a() const {
		return local_anchor_a_;
	}

	const vec2& revolute_joint::get_local_anchor_b() const {
		return local_anchor_b_;
	}



}