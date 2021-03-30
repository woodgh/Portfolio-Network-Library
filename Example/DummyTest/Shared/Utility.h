/*
 * Utility.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _UTILITY_H_
#define _UTILITY_H_

/*
* struct Vector3
*/
typedef struct Vector3
{
	float X = 0.f;

	float Y = 0.f;

	float Z = 0.f;

public:
	inline float Length(void)
	{
		return sqrtf(X * X + Y * Y + Z * Z);
	}

public:
	inline Vector3 operator + (Vector3 Vector)
	{
		return {
			X + Vector.X,
			Y + Vector.Y,
			Z + Vector.Z
		};
	}

	inline Vector3 operator - (Vector3 Vector)
	{
		return {
			X - Vector.X,
			Y - Vector.Y,
			Z - Vector.Z
		};
	}

	inline Vector3 operator * (Vector3 Vector)
	{
		return {
			X * Vector.X,
			Y * Vector.Y,
			Z * Vector.Z
		};
	}

	inline Vector3 operator / (Vector3 Vector)
	{
		return {
			X / Vector.X,
			Y / Vector.Y,
			Z / Vector.Z
		};
	}

	inline Vector3 operator += (Vector3 Vector)
	{
		X += Vector.X;
		Y += Vector.Y;
		Z += Vector.Z;

		return *this;
	}

	inline Vector3 operator -= (Vector3 Vector)
	{
		X -= Vector.X;
		Y -= Vector.Y;
		Z -= Vector.Z;

		return *this;
	}

} Vector3;

inline Vector3 operator + (Vector3 Vector, float Value)
{
	return {
		Vector.X + Value,
		Vector.Y + Value,
		Vector.Z + Value
	};
}

inline Vector3 operator - (Vector3 Vector, float Value)
{
	return {
		Vector.X - Value,
		Vector.Y - Value,
		Vector.Z - Value
	};
}

inline Vector3 operator * (Vector3 Vector, float Value)
{
	return {
		Vector.X * Value,
		Vector.Y * Value,
		Vector.Z * Value
	};
}

inline Vector3 operator / (Vector3 Vector, float Value)
{
	return {
		Vector.X / Value,
		Vector.Y / Value,
		Vector.Z / Value
	};
}

/**
* 벡터 정규화
*/
inline Vector3 Normalize(Vector3 Vector)
{
	if (Vector.Length() <= 0.f)
		return Vector;

	return {
		Vector.X / Vector.Length(),
		Vector.Y / Vector.Length(),
		Vector.Z / Vector.Length()
	};
}

/**
* 벡터 전방 이동하기
*/
inline Vector3 MoveFowards(Vector3 Location, Vector3 Velocity, float DeltaTime)
{
	return Location + Velocity * DeltaTime;
}

/**
* 거리 구하기 (typename T)
*/
template< typename T >
inline float Distance(T Src, T Dst)
{
	return std::abs(Dst - Src);
}

/**
* 거리 구하기 (Vector3)
*/
template<>
inline float Distance(Vector3 Location, Vector3 TargetLocation)
{
	return sqrtf(
		powf(TargetLocation.X - Location.X, 2.0f) + 
		powf(TargetLocation.Y - Location.Y, 2.0f)
	);
}

/*
 * class RangeIterator
*/
template< typename T >
class RangeIterator
{
public:
	RangeIterator(T Cur)
		: cur_(Cur)
	{
	}

	RangeIterator(T Begin, T End)
		: begin_(Begin)
		, end_(End)
	{
	}

	~RangeIterator(void) = default;

public:
	inline RangeIterator begin(void)
	{
		return { begin_ };
	}

	inline RangeIterator end(void)
	{
		return { end_ };
	}

public:
	inline RangeIterator& operator++()
	{
		++cur_;

		return *this;
	}

	inline bool operator!=(const RangeIterator& It) const
	{
		return cur_ < It.cur_;
	}

	inline T operator*() const
	{
		return cur_;
	}

private:
	T begin_ = 0;

	T end_ = 0;

	T cur_ = 0;
};

/**
* 시작부터 마지막 숫자까지 Range for Base
*/
template< typename T >
RangeIterator< T > Range(T Begin, T End)
{
	return { Begin, End };
}

/**
* 0부터 마지막 숫자까지 Range for Base
*/
template< typename T >
RangeIterator< T > Range(T End)
{
	return { 0, End };
}

/*
 * class Randomize
*/
class Randomize
{
public:
	Randomize(void)
		: engine_(device_())
	{
	}

	~Randomize(void) = default;

public:
	// Min과 Max 사이의 난수 발생
	template< typename T >
	inline typename std::enable_if< std::is_floating_point< T >::value, T >::type operator()(T Min, T Max)
	{
		return std::uniform_real_distribution < T >(Min, Max - 1)(engine_);
	}

private:
	std::random_device device_;

	std::mt19937_64 engine_;
};

#endif//_UTILITY_H_