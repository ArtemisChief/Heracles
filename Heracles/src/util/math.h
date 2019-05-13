#pragma once
#include <cassert>
#include <cmath>
#include <array>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    const TypeName& operator=(const TypeName&) = delete;
#define MAKE_ID(a, b) (((a)<(b))?(((a)<<16)|(b)):(((b)<<16)|(a)))

namespace heracles {

	static const float pi = acos(-1.0f);
	static const float inf = std::numeric_limits<float>::max();

	struct vec2;
	struct mat22;

	inline vec2 operator+(const vec2& a, const vec2& b);
	inline void operator+=(vec2& a, const vec2& b);
	inline vec2 operator-(const vec2& a, const vec2& b);
	inline void operator-=(vec2& a, const vec2& b);
	inline vec2 operator*(const vec2& a, float b);
	inline vec2 operator*(float a, const vec2& b);
	inline void operator*=(vec2& a, float b);
	inline vec2 operator/(const vec2&a, float b);
	inline void operator/=(vec2& a, float b);
	inline float dot(const vec2& a, const vec2&b);
	inline float cross(const vec2& a, const vec2& b);
	inline vec2 cross(float a, const vec2& b);

	inline mat22 operator+(const mat22& a, const mat22& b);
	inline void operator+=(mat22& a, const mat22& b);
	inline mat22 operator+(const mat22& a, float b);
	inline mat22 operator+(float b, const mat22& a);
	inline void operator+=(mat22& a, float b);
	inline mat22 operator-(const mat22& a, const mat22& b);
	inline void operator-=(mat22& a, const mat22& b);
	inline mat22 operator-(const mat22& a, float b);
	inline mat22 operator-(float b, const mat22& a);
	inline void operator-=(mat22& a, float b);
	inline mat22 operator*(const mat22& a, float b);
	inline mat22 operator*(float a, const mat22& b);
	inline void operator*=(mat22& a, float b);
	inline vec2 operator*(const mat22& a, const vec2& b);
	inline mat22 operator*(const mat22& a, const mat22& b);
	inline void operator*=(mat22& a, const mat22& b);

	//--------------------------------------------------------------------

	struct vec2 {
		float x;
		float y;

		vec2() : x(0), y(0) {}
		vec2(const float x, const float y) :x(x), y(y) {}

		float operator[](size_t idx) {
			assert(idx < 2);
			return idx == 0 ? x : y;
		}

		const float &operator[](size_t idx) const {
			assert(idx < 2);
			return idx == 0 ? x : y;
		}

		vec2 operator-() const {
			return { -x, -y };
		}

		float magnitude() const {
			return sqrt(x * x + y * y);
		}

		vec2 normal() const {
			return vec2(y, -x).normalized();
		}

		vec2 normalized() const {
			return *this / magnitude();
		}
	};

	inline vec2 operator+(const vec2& a, const vec2& b) {
		return { a.x + b.x, a.y + b.y };
	}

	inline void operator+=(vec2& a, const vec2& b) {
		a = a + b;
	}

	inline vec2 operator-(const vec2& a, const vec2& b) {
		return { a.x - b.x, a.y - b.y };
	}

	inline void operator-=(vec2& a, const vec2& b) {
		a = a - b;
	}

	inline vec2 operator*(const vec2& a, const float b) {
		return { a.x * b, a.y * b };
	}

	inline vec2 operator*(const float a, const vec2& b) {
		return b * a;
	}

	inline void operator*=(vec2& a, const float b) {
		a = a * b;
	}

	inline vec2 operator/(const vec2& a, const float b) {
		return { a.x / b, a.y / b };
	}

	inline void operator/=(vec2& a, const float b) {
		a = a / b;
	}

	inline float dot(const vec2& a, const vec2& b) {
		return a.x * b.x + a.y * b.y;
	}

	inline float cross(const vec2& a, const vec2& b) {
		return a.x * b.y - a.y * b.x;
	}

	inline vec2 cross(const float a, const vec2& b) {
		return a * vec2(-b.y, b.x);
	}

	//--------------------------------------------------------------------

	struct mat22 {
	private:
		std::array<vec2, 2> mat_;

	public:
		static const mat22 i;
		mat22() :mat22(0, 0, 0, 0) {}
		mat22(const std::array<vec2, 2>& mat) :mat_(mat) {}
		mat22(const float a, const float b, const float c, const float d) :mat_{ { {a, b}, {c, d} } } {}
		mat22(const float theta) :mat_{ { {cos(theta), -sin(theta)}, {sin(theta), cos(theta)} } } {}

		mat22& operator=(const mat22& mat22) {
			if (this != &mat22) {
				this->mat_ = mat22.mat_;
			}
			return *this;
		}

		vec2 &operator[](const size_t idx) {
			return mat_[idx];
		}

		const vec2 &operator[](const size_t idx) const {
			return (*const_cast<mat22*>(this))[idx];
		}

		float det() const {
			return mat_[0][0] * mat_[1][1] - mat_[0][1] * mat_[1][0];
		}

		mat22 inv() const {
			return 1 / det() * mat22(mat_[1][1], -mat_[0][1], -mat_[1][0], mat_[0][0]);
		}

		mat22 transpose() const {
			return { mat_[0][0] ,mat_[1][0], mat_[0][1], mat_[1][1] };
		}
	};

	inline mat22 operator+(const mat22& a, const mat22& b) {
		return { a[0].x + b[0].x, a[0].y + b[0].y,
				a[1].x + b[1].x, a[1].y + b[1].y };
	}

	inline void operator+=(mat22& a, const mat22& b) {
		a = a + b;
	}

	inline mat22 operator+(const mat22& a, const float b) {
		return a + b * mat22::i;
	}

	inline mat22 operator+(const float a, const mat22& b) {
		return b + a;
	}

	inline void operator+=(mat22& a, const float b) {
		a = a + b;
	}

	inline mat22 operator-(const mat22& a, const mat22& b) {
		return { a[0].x - b[0].x, a[0].y - b[0].y,
				a[1].x - b[1].x, a[1].y - b[1].y };
	}

	inline void operator-=(mat22& a, const mat22& b) {
		a = a - b;
	}

	inline mat22 operator-(const mat22& a, const float b) {
		return a - b * mat22::i;
	}

	inline mat22 operator-(const float a, const mat22& b) {
		return a * mat22::i - b;
	}

	inline void operator-=(mat22& a, const float b) {
		a = a - b;
	}

	inline mat22 operator*(const mat22& a, const float b) {
		return { a[0].x * b, a[0].y * b,
				a[1].x * b, a[1].y * b };
	}

	inline mat22 operator*(const float a, const mat22& b) {
		return b * a;
	}

	inline void operator*=(mat22& a, const float b) {
		a = a * b;
	}

	inline vec2 operator*(const mat22& a, const vec2& b) {
		return { a[0][0] * b[0] + a[0][1] * b[1],
				a[1][0] * b[0] + a[1][1] * b[1] };
	}

	inline mat22 operator*(const mat22& a, const mat22& b) {
		return { a[0][0] * b[0][0] + a[0][1] * b[1][0],
				a[0][0] * b[0][1] + a[0][1] * b[1][1],
				a[1][0] * b[0][0] + a[1][1] * b[1][0],
				a[1][0] * b[0][1] + a[1][1] * b[1][1] };
	}

	inline void operator*=(mat22& a, const mat22& b) {
		a = a * b;
	}

}