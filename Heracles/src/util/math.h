#pragma once

#include <cassert>
#include <cmath>
#include <array>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    const TypeName& operator=(const TypeName&) = delete;

namespace Heracles {

	static const float pi = acos(-1);
	static const float inf = std::numeric_limits<float>::max();

	struct Vec2;
	struct Mat22;

	inline Vec2 operator+(const Vec2 &a, const Vec2 &b);
	inline void operator+=(Vec2 &a, const Vec2 &b);
	inline Vec2 operator-(const Vec2 &a, const Vec2 &b);
	inline void operator-=(Vec2 &a, const Vec2 &b);
	inline Vec2 operator*(const Vec2 &a, float b);
	inline Vec2 operator*(float a, const Vec2 &b);
	inline void operator*=(Vec2 &a, float b);
	inline Vec2 operator/(const Vec2 &a, float b);
	inline void operator/=(Vec2 &a, float b);
	inline float dot(const Vec2 &a, const Vec2 &b);
	inline float cross(const Vec2 &a, const Vec2 &b);
	inline Vec2 cross(float a, const Vec2 &b);

	inline Mat22 operator+(const Mat22& a, const Mat22& b);
	inline void operator+=(Mat22& a, const Mat22& b);
	inline Mat22 operator+(const Mat22& a, float b);
	inline Mat22 operator+(float b, const Mat22& a);
	inline void operator+=(Mat22& a, float b);
	inline Mat22 operator-(const Mat22& a, const Mat22& b);
	inline void operator-=(Mat22& a, const Mat22& b);
	inline Mat22 operator-(const Mat22& a, float b);
	inline Mat22 operator-(float b, const Mat22& a);
	inline void operator-=(Mat22& a, float b);
	inline Mat22 operator*(const Mat22& a, float b);
	inline Mat22 operator*(float a, const Mat22& b);
	inline void operator*=(Mat22& a, float b);
	inline Vec2 operator*(const Vec2& a, const Mat22& b);
	inline void operator*=(Vec2& a, const Mat22& b);

	//--------------------------------------------------------------------

	struct Vec2 {
		float x;
		float y;

		Vec2() : x(0), y(0) {}
		Vec2(float x, float y) :x(x), y(y) {}

		float operator[](size_t idx) {
			assert(idx < 2);
			return idx == 0 ? x : y;
		}

		const float &operator[](size_t idx) const {
			return (*const_cast<Vec2*>(this))[idx];
		}

		Vec2 operator-() const {
			return { -x,-y };
		}

		float magnitude() const {
			return sqrt(x*x + y * y);
		}

		Vec2 normal() const {
			return Vec2(y, -x).normalized();
		}

		Vec2 normalized() const {
			return *this / magnitude();
		}
	};

	inline Vec2 operator+(const Vec2& a, const Vec2& b) {
		return { a.x + b.x, a.y + b.y };
	}

	inline void operator+=(Vec2& a, const Vec2& b) {
		a = a + b;
	}

	inline Vec2 operator-(const Vec2& a, const Vec2& b) {
		return { a.x - b.x, a.y - b.y };
	}

	inline void operator-=(Vec2& a, const Vec2& b) {
		a = a - b;
	}

	inline Vec2 operator*(const Vec2& a, float b) {
		return { a.x * b, a.y * b };
	}

	inline Vec2 operator*(float a, const Vec2& b) {
		return b * a;
	}

	inline void operator*=(Vec2& a, float b) {
		a = a * b;
	}

	inline Vec2 operator/(const Vec2& a, float b) {
		return { a.x / b, a.y / b };
	}

	inline void operator/=(Vec2& a, float b) {
		a = a / b;
	}

	inline float dot(const Vec2& a, const Vec2& b) {
		return a.x * b.x + a.y * b.y;
	}

	inline float cross(const Vec2& a, const Vec2& b) {
		return a.x * b.y - a.y * b.x;
	}

	inline Vec2 cross(float a, const Vec2& b) {
		return a * Vec2(-b.y, b.x);
	}

	//--------------------------------------------------------------------

	struct Mat22 {
	private:
		std::array<Vec2, 2> mat_;

	public:
		static const Mat22 I;
		Mat22() :Mat22(0, 0, 0, 0) {}
		Mat22(const std::array<Vec2, 2>& mat) :mat_(mat) {}
		Mat22(float a, float b, float c, float d) :mat_{ { {a, b}, {c, d} } } {}
		Mat22(float theta) :mat_{ {{cos(theta), -sin(theta)}, {sin(theta), cos(theta)}} } {}

		Vec2 &operator[](size_t idx) {
			return mat_[idx];
		}

		const Vec2 &operator[](size_t idx) const {
			return (*const_cast<Mat22*>(this))[idx];
		}

		float det() const {
			return mat_[0][0] * mat_[1][1] - mat_[0][1] * mat_[1][0];
		}

		Mat22 inv() const {
			return 1 / det() * Mat22(mat_[1][1], -mat_[0][1], -mat_[1][0], mat_[0][0]);
		}

		Mat22 transpose() const {
			return { mat_[0][0] ,mat_[1][0], mat_[0][1], mat_[1][1] };
		}

		Mat22& operator=(const Mat22& mat22) {
			if (this != &mat22) {
				if (this != nullptr)
					delete &this->mat_;
				this->mat_ = mat22.mat_;
			}
			return *this;
		}
	};

	const Mat22 Mat22::I = { 1,0,0,1 };

	inline Mat22 operator+(const Mat22& a, const Mat22& b) {
		return { a[0].x + b[0].x, a[0].y + b[0].y,
				a[1].x + b[1].x, a[1].y + b[1].y };
	}

	inline void operator+=(Mat22& a, const Mat22& b) {
		a = a + b;
	}

	inline Mat22 operator+(const Mat22& a, float b) {
		return a + b * Mat22::I;
	}

	inline Mat22 operator+(float a, const Mat22& b) {
		return b + a;
	}

	inline void operator+=(Mat22& a, float b) {
		a = a + b;
	}

	inline Mat22 operator-(const Mat22& a, const Mat22& b) {
		return { a[0].x - b[0].x, a[0].y - b[0].y,
				a[1].x - b[1].x, a[1].y - b[1].y };
	}

	inline void operator-=(Mat22& a, const Mat22& b) {
		a = a - b;
	}

	inline Mat22 operator-(const Mat22& a, float b) {
		return a - b * Mat22::I;
	}

	inline Mat22 operator-(float a, const Mat22& b) {
		return a * Mat22::I - b;
	}

	inline void operator-=(Mat22& a, float b) {
		a = a - b;
	}

	inline Mat22 operator*(const Mat22& a, float b) {
		return { a[0].x * b, a[0].y * b,
				a[1].x * b, a[1].y * b };
	}

	inline Mat22 operator*(float a, const Mat22& b) {
		return b * a;
	}

	inline void operator*=(Mat22& a, float b) {
		a = a * b;
	}

	inline Vec2 operator*(const Vec2& a, const Mat22& b) {
		return { a[0] * b[0][0] + a[1] * b[1][0],
				a[0] * b[0][1] + a[1] * b[1][1] };
	}

	inline Vec2 operator*(const Mat22& a, const Vec2& b) {
		return { a[0][0] * b[0] + a[0][1] * b[1],
				a[1][0] * b[0] + a[1][1] * b[1] };
	}

	inline void operator*=(Vec2& a, const Mat22& b) {
		a = a * b;
	}

	inline Mat22 operator*(const Mat22& a, const Mat22& b) {
		return { a[0][0] * b[0][0] + a[0][1] * b[1][0],
				a[0][0] * b[0][1] + a[0][1] * b[1][1],
				a[1][0] * b[0][0] + a[1][1] * b[1][0],
				a[1][0] * b[0][1] + a[1][1] * b[1][1] };
	}

	inline void operator*=(Mat22& a, const Mat22& b) {
		a = a * b;
	}

}