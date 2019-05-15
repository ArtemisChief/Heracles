#include "level_manager.h"
#include "../physics_engine/world.h"
#include "graphic_renderer.h"
#include "user_interface.h"

namespace heracles {

	void level_manager::level_1() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		auto body = world::create_ground(10, 0.3, vec2(0, -2.25));
		graphic_renderer::bind_vertex_array(body);
		world::add(body);

		vec2 x(-2.3125f, -1.94f);
		for (auto i = 0; i < 15; ++i) {
			auto y = x;
			for (auto j = i; j < 15; ++j) {
				body = world::create_rectangle(10, 0.3, 0.3, y);
				graphic_renderer::bind_vertex_array(body);
				world::add(body);
				y += vec2(0.33f, 0.0f);
			}
			x += vec2(0.1575f, 0.31f);
		}

		world::unlock();
	}

	void level_manager::level_2() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		const auto ground = world::create_ground(10, 0.3, vec2(0, -2.25));
		ground->set_friction(0.4);
		graphic_renderer::bind_vertex_array(ground);
		world::add(ground);

		const auto mass = 10.0f;
		const auto y = 2.0f;

		auto last = ground;
		for (auto i = 0; i < 15; ++i) {
			auto box = world::create_rectangle(mass, 0.2, 0.075, vec2(0.15 + 0.35 * i, y));
			box->set_friction(0.4);
			graphic_renderer::bind_vertex_array(box);
			world::add(box);

			const auto joint = world::create_revolute_joint(last, box, vec2(0.35 * i, y));
			graphic_renderer::bind_vertex_array(joint);
			world::add(joint);
			last = box;
		}

		world::unlock();
	}

	void level_manager::level_3() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		const auto ground = world::create_ground(20, 0.3, vec2(5, -2.25));
		ground->set_friction(0.8);
		graphic_renderer::bind_vertex_array(ground);
		world::add(ground);

		const auto platform = world::create_ground(1, 0.15, vec2(-3.5, 0));
		platform->set_friction(0.2);
		graphic_renderer::bind_vertex_array(platform);
		world::add(platform);

		auto bullet = world::create_rectangle(20, 0.3, 0.3, vec2(-3.8, 0.3));
		bullet->set_friction(0.8);
		graphic_renderer::bind_vertex_array(bullet);
		world::add(bullet);

		for (auto i = 0; i < 20; ++i) {
			auto box = world::create_rectangle(20, 0.15, 2.5, vec2(-2.8 + 0.7 * i, -0.75));
			box->set_friction(0.8);
			graphic_renderer::bind_vertex_array(box);
			world::add(box);
		}

		world::unlock();
	}

	void level_manager::level_4() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		const auto ground = world::create_ground(10, 0.3, vec2(0, -2.25));
		ground->set_friction(0.95);
		graphic_renderer::bind_vertex_array(ground);
		world::add(ground);

		const auto platform = world::create_ground(0.5, 0.15, vec2(-2.8, -1.1));
		platform->set_friction(0.2);
		graphic_renderer::bind_vertex_array(platform);
		world::add(platform);

		auto bird = world::create_rectangle(10, 0.3, 0.3, vec2(-2.8, -0.8));
		bird->set_friction(0.95);
		graphic_renderer::bind_vertex_array(bird);
		world::add(bird);

		for (auto i = 0; i < 4; ++i) {
			auto box1 = world::create_rectangle(5, 0.25, 0.8, vec2(2.8, -1.7 + 0.93 * i));
			box1->set_friction(0.95);
			graphic_renderer::bind_vertex_array(box1);
			world::add(box1);

			auto box2 = world::create_rectangle(5, 0.25, 0.8, vec2(3.5, -1.7 + 0.93 * i));
			box2->set_friction(0.95);
			graphic_renderer::bind_vertex_array(box2);
			world::add(box2);

			auto box3 = world::create_rectangle(5, 0.1, 1.0, vec2(3.15, -1.235 + 0.93 * i));
			box3->set_friction(0.95);
			box3->set_rotation(pi / 2);
			graphic_renderer::bind_vertex_array(box3);
			world::add(box3);
		}

		auto top = world::create_triangle(5, 1.0, 0.6, vec2(3.15, 1.95));
		top->set_friction(0.95);
		graphic_renderer::bind_vertex_array(top);
		world::add(top);

		world::unlock();
	}

	void level_manager::level_5() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		const auto left = world::create_ground(0.05, 8, vec2(-4.3, 0));
		graphic_renderer::bind_vertex_array(left);
		world::add(left);

		const auto right = world::create_ground(0.05, 8, vec2(4.3, 0));
		graphic_renderer::bind_vertex_array(right);
		world::add(right);

		auto left_bevel = world::create_ground(0.05, 1.03, vec2(-1.1, 3.55));
		graphic_renderer::bind_vertex_array(left_bevel);
		left_bevel->set_rotation(pi / 6);
		left_bevel->set_friction(1.0);
		world::add(left_bevel);

		auto right_bevel = world::create_ground(0.05, 1.03, vec2(1.1, 3.55));
		graphic_renderer::bind_vertex_array(right_bevel);
		right_bevel->set_rotation(-pi / 6);
		right_bevel->set_friction(1.0);
		world::add(right_bevel);

		const auto left_top = world::create_ground(2.93, 0.05, vec2(-2.85, 4));
		graphic_renderer::bind_vertex_array(left_top);
		world::add(left_top);

		const auto right_top = world::create_ground(2.93, 0.05, vec2(2.85, 4));
		graphic_renderer::bind_vertex_array(right_top);
		world::add(right_top);

		auto body = world::create_ground(8.6, 0.05, vec2(0, -4));
		graphic_renderer::bind_vertex_array(body);
		world::add(body);

		vec2 x(-0.4f, 3.6f);
		for (auto i = 1; i < 9; ++i) {
			x -= vec2(0.4f, 0.8*sqrt(3) / 2);
			auto y = x;
			for (auto j = 0; j < i + 2; ++j) {
				if (i == 8) {
					body = world::create_ground(0.1, 1.88, y - vec2(0.0, 1.08));
					body->set_friction(1.0);
					graphic_renderer::bind_vertex_array(body);
					world::add(body);
				}

				body = world::create_tri_ground(0.32, 0.32*sqrt(3) / 2, y);
				body->set_friction(1.0);
				graphic_renderer::bind_vertex_array(body);
				world::add(body);

				y += vec2(0.8f, 0.0f);
			}
		}

		srand(time(nullptr));
		for (auto i = 1; i < 350; i++) {
			body = world::create_rectangle(100, 0.1, 0.1, vec2(static_cast<float>(rand() % 15) / 10 - 0.75, 3 + 0.1*i));
			body->set_friction(1.0);
			graphic_renderer::bind_vertex_array(body);
			world::add(body);
		}

		world::unlock();
	}

	void level_manager::level_6() {
		user_interface::target_body = nullptr;
		world::lock();
		world::clear();

		const auto ground = world::create_ground(10, 0.3, vec2(0, -2.25));
		graphic_renderer::bind_vertex_array(ground);
		world::add(ground);

		for (auto i = 0; i < 8; i++) {
			auto box = world::create_rectangle(1000, 0.5, 0.5, vec2(1.5 - 0.5*i, -1));
			box->set_friction(1.0);
			graphic_renderer::bind_vertex_array(box);
			world::add(box);

			const auto joint = world::create_revolute_joint(ground, box, vec2(1.5 - 0.5*i, 2));
			graphic_renderer::bind_vertex_array(joint);
			world::add(joint);
		}

		world::unlock();
	}

}
