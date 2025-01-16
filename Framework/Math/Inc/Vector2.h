#pragma once

namespace VEngine::Math
{
	struct Vector2
	{
		union
		{
			struct { float x, y;};
			std::array<float, 2> v; //v[0] = x 
		};

		constexpr Vector2() noexcept : Vector2(0.0f) {}
		explicit constexpr Vector2(float f) noexcept : Vector2(f, f) {}
		constexpr Vector2(float x, float y) noexcept : x{ x }, y{ y } {}

		//common vectors
		const static Vector2 Zero;
		const static Vector2 One;
		const static Vector2 XAxis;
		const static Vector2 YAxis;

		//operator overloads
		constexpr Vector2 operator-() { return { -x, -y }; }
		constexpr Vector2 operator+(const Vector2& rhs) const { return { x + rhs.x, y + rhs.y }; }
		constexpr Vector2 operator-(const Vector2& rhs) const { return { x - rhs.x, y - rhs.y }; }
		constexpr Vector2 operator*(float s) const { return { x * s , y * s}; }
		constexpr Vector2 operator/(float s) const { return { x / s, y / s }; }

		constexpr Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
		constexpr Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
		constexpr Vector2& operator/=(float s) { x /= s; y /= s; return *this; }

	};
}
