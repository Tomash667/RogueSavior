#pragma once

//-----------------------------------------------------------------------------
struct INT2;
struct VEC2;
struct VEC3;
struct VEC4;

//-----------------------------------------------------------------------------
// Random numbers
//-----------------------------------------------------------------------------
// Pseudo random number generator
struct RNG
{
	unsigned long val;

	RNG() : val(1)
	{
	}

	int Rand()
	{
		val = val * 214013L + 2531011L;
		return ((val >> 16) & 0x7fff);
	}

	int RandTmp()
	{
		int tval = val * 214013L + 2531011L;
		return ((tval >> 16) & 0x7fff);
	}
};
extern RNG _RNG;

inline int Rand()
{
	return _RNG.Rand();
}
inline int Rand(int a)
{
	return _RNG.Rand() % a;
}
inline void Srand(int x)
{
	_RNG.val = x;
}
inline uint RandVal()
{
	return _RNG.val;
}
inline int RandTmp()
{
	return _RNG.RandTmp();
}

// Random float number in range <0,1>
inline float Random()
{
	return (float)Rand() / RAND_MAX;
}

// Random number in range <0,a>
template<typename T>
inline T Random(T a)
{
	assert(a >= 0);
	return Rand(a + 1);
}
template<>
inline float Random(float a)
{
	return ((float)Rand() / RAND_MAX)*a;
}

// Random number in range <a,b>
template<typename T>
inline T Random(T a, T b)
{
	assert(b >= a);
	return Rand() % (b - a + 1) + a;
}
template<>
inline float Random(float a, float b)
{
	assert(b >= a);
	return ((float)Rand() / RAND_MAX)*(b - a) + a;
}

// Return normalized number in range <-val,val>
inline float RandomNormalized(float val)
{
	return (Random(-val, val) + Random(-val, val)) / 2;
}

// Return element based on chance
template<typename T>
inline T Chance(int c, T a, T b)
{
	return (Rand() % c == 0 ? a : b);
}
template<typename T>
inline T Chance(int chance_a, int chance_b, int chance_c, T a, T b, T c)
{
	int ch = Rand() % (chance_a + chance_b + chance_c);
	if(ch < chance_a)
		return a;
	else if(ch < chance_a + chance_b)
		return b;
	else
		return c;
}

//-----------------------------------------------------------------------------
// Math functions
//-----------------------------------------------------------------------------
// Clamp value to range
template<typename T>
inline T Clamp(T value, T min, T max)
{
	if(value > max)
		return max;
	else if(value < min)
		return min;
	else
		return value;
}

// Compare two floats using epsilon
inline bool Equal(float a, float b)
{
	return abs(a - b) < std::numeric_limits<float>::epsilon();
}

// Return min max value
template<typename T>
inline void MinMax(T a, T b, T& min, T& max)
{
	if(a > b)
	{
		min = b;
		max = a;
	}
	else
	{
		min = a;
		max = b;
	}
}

// Find min value from any arguments count
template<typename T, typename T2, typename Arg>
inline T Min(T a, T2 b)
{
	if(a > b)
		return b;
	else
		return a;
}
template<typename T, typename T2, typename... Args>
inline T Min(T a, T2 b, Args... args)
{
	if(a > b)
		return Min(b, args...);
	else
		return Min(a, args...);
}

// Find min value from any arguments count
template<typename T, typename T2, typename Arg>
inline T Max(T a, T2 b)
{
	if(a < b)
		return b;
	else
		return a;
}
template<typename T, typename T2, typename... Args>
inline T Max(T a, T2 b, Args... args)
{
	if(a < b)
		return Max(b, args...);
	else
		return Max(a, args...);
}

// Distance between two 2d points
template<typename T>
inline T Distance(T x1, T y1, T x2, T y2)
{
	T x = abs(x1 - x2);
	T y = abs(y1 - y2);
	return sqrt(x*x + y*y);
}
inline float DistanceSqrt(float x1, float y1, float x2, float y2)
{
	float x = abs(x1 - x2),
		y = abs(y1 - y2);
	return x*x + y*y;
}

// Clip value to range
inline float Clip(float f, float range = PI * 2)
{
	int n = (int)floor(f / range);
	return f - range * n;
}

// Return angle between two points
float Angle(float x1, float y1, float x2, float y2);

// Return difference between two angles
inline float AngleDiff(float a, float b)
{
	assert(a >= 0.f && a < PI * 2 && b >= 0.f && b < PI * 2);
	return min((2 * PI) - abs(a - b), abs(b - a));
}

// Return true if float is not zero
inline bool NotZero(float a)
{
	return abs(a) >= std::numeric_limits<float>::epsilon();
}

// Return true if float is zero
inline bool IsZero(float a)
{
	return abs(a) < std::numeric_limits<float>::epsilon();
}

// Return sign of value
template<typename T>
inline T Sign(T f)
{
	if(f > 0)
		return 1;
	else if(f < 0)
		return -1;
	else
		return 0;
}

// Return linear interpolation of value
inline float Lerp(float a, float b, float t)
{
	return (b - a)*t + a;
}
inline int Lerp(int a, int b, float t)
{
	return int(t*(b - a)) + a;
}

// Return shortes direction between angles
float ShortestArc(float a, float b);

// Linear interpolation between two angles
void LerpAngle(float& angle, float from, float to, float t);

// Return true if value is in range
template<typename T>
inline bool InRange(T v, T left, T right)
{
	return (v >= left && v <= right);
}
template<typename T>
inline bool InRange(T left, T a, T b, T right)
{
	return (a >= left && b >= a && b <= right);
}

// Return true if number is in other numeric type range
template<typename T>
inline bool InRange(__int64 value)
{
	return value >= std::numeric_limits<T>::min() && value <= std::numeric_limits<T>::max();
}

// Return spherical interpolation between two angles
inline float Slerp(float a, float b, float t)
{
	float angle = ShortestArc(a, b);
	return a + angle * t;
}

// Count 1 bits in value
inline int CountBits(int i)
{
	// It's write-only code. Just put a comment that you are not meant to understand or maintain this code, just worship the gods that revealed it to mankind.
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// Return true if value is power of 2
inline bool IsPow2(int x)
{
	return ((x > 0) && ((x & (x - 1)) == 0));
}

// Return float infinity
inline float Inf()
{
	return std::numeric_limits<float>::infinity();
}

// Convert angle from degrees to radians
inline float ToRadians(float degrees)
{
	return degrees * PI / 180;
}

// Convert angle from radians to degrees
inline float ToDegrees(float radians)
{
	return radians * 180 / PI;
}

// Round float to int
inline int Roundi(float value)
{
	return (int)round(value);
}

// Return module
inline int Modulo(int a, int mod)
{
	if(a >= 0)
		return a % mod;
	else
		return a + mod * ((-a / mod) + 1);
}

//-----------------------------------------------------------------------------
// 2D int point
//-----------------------------------------------------------------------------
struct INT2
{
	int x, y;

	INT2() {}
	template<typename T, typename T2>
	INT2(T x, T2 y) : x(int(x)), y(int(y)) {}
	explicit INT2(int v) : x(v), y(v) {}
	INT2(int x, int y) : x(x), y(y) {}
	explicit INT2(const VEC2& v);
	explicit INT2(const VEC3& v);

	int operator ()(int shift) const
	{
		return x + y * shift;
	}

	bool operator == (const INT2& i) const
	{
		return (x == i.x && y == i.y);
	}

	bool operator != (const INT2& i) const
	{
		return (x != i.x || y != i.y);
	}

	INT2 operator + (const INT2& xy) const
	{
		return INT2(x + xy.x, y + xy.y);
	}

	INT2 operator - (const INT2& xy) const
	{
		return INT2(x - xy.x, y - xy.y);
	}

	INT2 operator * (int a) const
	{
		return INT2(x*a, y*a);
	}

	INT2 operator * (float a) const
	{
		return INT2(int(a*x), int(a*y));
	}

	INT2 operator / (int a) const
	{
		return INT2(x / a, y / a);
	}

	void operator += (const INT2& xy)
	{
		x += xy.x;
		y += xy.y;
	}

	void operator -= (const INT2& xy)
	{
		x -= xy.x;
		y -= xy.y;
	}

	void operator *= (int a)
	{
		x *= a;
		y *= a;
	}

	void operator /= (int a)
	{
		x /= a;
		y /= a;
	}

	int Lerp(float t) const
	{
		return int(t*(y - x)) + x;
	}

	// Return random value in range <x,y>
	int Random() const
	{
		return ::Random(x, y);
	}

	VEC2 ToVEC2() const;
};

// Return random value in range <x,y>
inline int Random(const INT2& a)
{
	return Random(a.x, a.y);
}

// Return random point
inline INT2 Random(const INT2& a, const INT2& b)
{
	return INT2(Random(a.x, b.x), Random(a.y, b.y));
}

// Clamp value to range <x,y>
inline int Clamp(int f, const INT2& i)
{
	if(f > i.y)
		return i.y;
	else if(f < i.x)
		return i.x;
	else
		return f;
}

// Linear interpolation between two points
inline INT2 Lerp(const INT2& i1, const INT2& i2, float t)
{
	return INT2((int)Lerp(float(i1.x), float(i2.x), t), (int)Lerp(float(i1.y), float(i2.y), t));
}

// Return min x,y from two points
inline INT2 Min(const INT2& i1, const INT2& i2)
{
	return INT2(min(i1.x, i2.x), min(i1.y, i2.y));
}

// Return max x,y from two points
inline INT2 Max(const INT2& i1, const INT2& i2)
{
	return INT2(max(i1.x, i2.x), max(i1.y, i2.y));
}

// Return square distance between two points
inline int Distance(const INT2& pt1, const INT2& pt2)
{
	return abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y);
}

//-----------------------------------------------------------------------------
// Rectangle using int
//-----------------------------------------------------------------------------
struct Rect
{
	INT2 p1, p2;

	Rect() {}
	Rect(int x1, int y1, int x2, int y2) : p1(x1, y1), p2(x2, y2) {}
	Rect(const INT2& p1, const INT2& p2) : p1(p1), p2(p2) {}
	Rect(const Rect& box) : p1(box.p1), p2(box.p2) {}

	static Rect Create(const INT2& pos, const INT2& size)
	{
		Rect box;
		box.Set(pos, size);
		return box;
	}

	Rect operator += (const INT2& p) const
	{
		return Rect(p1.x + p.x, p1.y + p.y, p2.x + p.x, p2.y + p.y);
	}

	void Set(const INT2& pos, const INT2& size)
	{
		p1 = pos;
		p2 = pos + size;
	}

	void Set(int x1, int y1, int x2, int y2)
	{
		p1.x = x1;
		p1.y = y1;
		p2.x = x2;
		p2.y = y2;
	}

	INT2 Random() const
	{
		return INT2(::Random(p1.x, p2.x), ::Random(p1.y, p2.y));
	}

	Rect LeftBottomPart() const
	{
		return Rect(p1.x, p1.y, p1.x + (p2.x - p1.x) / 2, p1.y + (p2.y - p1.y) / 2);
	}
	Rect RightBottomPart() const
	{
		return Rect(p1.x + (p2.x - p1.x) / 2, p1.y, p2.x, p1.y + (p2.y - p1.y) / 2);
	}
	Rect LeftTopPart() const
	{
		return Rect(p1.x, p1.y + (p2.y - p1.y) / 2, p1.x + (p2.x - p1.x) / 2, p2.y);
	}
	Rect RightTopPart() const
	{
		return Rect(p1.x + (p2.x - p1.x) / 2, p1.y + (p2.y - p1.y) / 2, p2.x, p2.y);
	}

	INT2 Size() const
	{
		return INT2(p2.x - p1.x, p2.y - p1.y);
	}
};

//-----------------------------------------------------------------------------
// 2D float point
//-----------------------------------------------------------------------------
struct VEC2
{
	float x, y;

	VEC2() {}
	VEC2(float x, float y) : x(x), y(y) {}
	VEC2(const VEC2& v) : x(v.x), y(v.y) {}

	VEC2& operator += (const VEC2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	VEC2& operator -= (const VEC2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	VEC2& operator *= (float f)
	{
		x *= f;
		y *= f;
		return *this;
	}

	VEC2& operator /= (float f)
	{
		float fi = 1.f / f;
		x *= fi;
		y *= fi;
		return *this;
	}

	VEC2 operator + () const
	{
		return *this;
	}

	VEC2 operator - () const
	{
		return VEC2(-x, -y);
	}

	VEC2 operator + (const VEC2& v) const
	{
		return VEC2(x + v.x, y + v.y);
	}

	VEC2 operator - (const VEC2& v) const
	{
		return VEC2(x - v.x, y - v.y);
	}

	VEC2 operator * (float f) const
	{
		return VEC2(x * f, y * f);
	}

	VEC2 operator / (float f) const
	{
		float fi = 1.f / f;
		return VEC2(x * fi, y * fi);
	}

	friend VEC2 operator * (float, const VEC2& v);

	bool operator == (const VEC2& v) const
	{
		return x == v.x && y == v.y;
	}

	bool operator != (const VEC2& v) const
	{
		return x != v.x || y != v.y;
	}

	// Clamp values to <0,1> range
	VEC2 Clamp() const
	{
		return VEC2(::Clamp(x, 0.f, 1.f),
			::Clamp(y, 0.f, 1.f));
	}

	// Clamp point to range
	VEC2 Clamp(const VEC2& min, const VEC2& max) const
	{
		return VEC2(::Clamp(x, min.x, max.x), ::Clamp(y, min.y, max.y));
	}

	// Clip values to <0,2PI> range
	VEC2 Clip() const
	{
		return VEC2(::Clip(x), ::Clip(y));
	}

	// Compare two points using epsilon
	bool Equal(const VEC2& v) const
	{
		return ::Equal(x, v.x) && ::Equal(y, v.y);
	}

	// Return random value in range <x,y>
	float Random() const
	{
		return ::Random(x, y);
	}

	// Construct random point in range <a,b>
	static VEC2 Random(float a, float b)
	{
		return VEC2(::Random(a, b), ::Random(a, b));
	}
};

inline VEC2 operator * (float f, const VEC2& v)
{
	return v * f;
}

// Slerp between two angles x,y
inline VEC2 Slerp(const VEC2& a, const VEC2& b, float t)
{
	return VEC2(Slerp(a.x, b.x, t), Slerp(a.y, b.y, t));
}

// Return random value from range <x,y>
inline float Random(const VEC2& v)
{
	return Random(v.x, v.y);
}

// Return random value inside circle with radius r
inline VEC2 RandomCirclePt(float r)
{
	float a = Random(),
		b = Random();
	if(b < a)
		std::swap(a, b);
	return VEC2(b*r*cos(2 * PI*a / b), b*r*sin(2 * PI*a / b));
}

// Return min for points
inline void Min(const VEC2& v1, const VEC2& v2, VEC2& out)
{
	out.x = min(v1.x, v2.x);
	out.y = min(v1.y, v2.y);
}

// Return max for points
inline void Max(const VEC2& v1, const VEC2& v2, VEC2& out)
{
	out.x = max(v1.x, v2.x);
	out.y = max(v1.y, v2.y);
}

// Return min and max value for points
inline void MinMax(const VEC2& a, const VEC2& b, VEC2& min, VEC2& max)
{
	MinMax(a.x, b.x, min.x, max.x);
	MinMax(a.y, b.y, min.y, max.y);
}

// Clamp value to <x,y> range
inline float Clamp(float f, const VEC2& v)
{
	if(f > v.y)
		return v.y;
	else if(f < v.x)
		return v.x;
	else
		return f;
}
// Return random value in range <vmin,vmax>
inline VEC2 Random(const VEC2& vmin, const VEC2& vmax)
{
	return VEC2(Random(vmin.x, vmax.x), Random(vmin.y, vmax.y));
}

// Return distance between two points
inline float Distance(const VEC2& p1, const VEC2& p2)
{
	float x = abs(p1.x - p2.x);
	float y = abs(p1.y - p2.y);
	return sqrt(x*x + y*y);
}
inline float DistanceQqrt(const VEC2& p1, const VEC2& p2)
{
	float x = abs(p1.x - p2.x);
	float y = abs(p1.y - p2.y);
	return x*x + y*y;
}

// Return angle between two points
inline float Angle(const VEC2& v1, const VEC2& v2)
{
	return Angle(v1.x, v1.y, v2.x, v2.y);
}

//-----------------------------------------------------------------------------
// 3D float point
//-----------------------------------------------------------------------------
struct VEC3
{
	float x, y, z;

	VEC3() {}
	VEC3(float x, float y, float z) : x(x), y(y), z(z) {}
	VEC3(const VEC3& v) : x(v.x), y(v.y), z(v.z) {}

	VEC3& operator += (const VEC3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	VEC3& operator -= (const VEC3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	VEC3& operator *= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	VEC3& operator /= (float f)
	{
		float fi = 1.f / f;
		x *= fi;
		y *= fi;
		z *= fi;
		return *this;
	}

	VEC3 operator + () const
	{
		return *this;
	}

	VEC3 operator - () const
	{
		return VEC3(-x, -y, -z);
	}

	VEC3 operator + (const VEC3& v) const
	{
		return VEC3(x + v.x, y + v.y, z + v.z);
	}

	VEC3 operator - (const VEC3& v) const
	{
		return VEC3(x - v.x, y - v.y, z - v.z);
	}

	VEC3 operator * (float f) const
	{
		return VEC3(x * f, y * f, z * f);
	}

	VEC3 operator / (float f) const
	{
		float fi = 1.f / f;
		return VEC3(x * fi, y * fi, z * fi);
	}

	friend VEC3 operator * (float, const VEC3& v);

	bool operator == (const VEC3& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator != (const VEC3& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	// Clamp point to <0,1> range
	VEC3 Clamp(const VEC3& v) const
	{
		return VEC3(::Clamp(v.x, 0.f, 1.f),
			::Clamp(v.y, 0.f, 1.f),
			::Clamp(v.z, 0.f, 1.f));
	}

	// Return 2d dot product
	float Dot2d() const
	{
		return (x*x + z*z);
	}

	// Compare two points using epsilon
	bool Equal(const VEC3& v) const
	{
		return ::Equal(x, v.x) && ::Equal(y, v.y) && ::Equal(z, v.z);
	}

	// Return true if all values are positive
	inline bool IsPositive() const
	{
		return x >= 0.f && y >= 0.f && z >= 0.f;
	}

	// Construct random point in range <a,b>
	static VEC3 Random(float a, float b)
	{
		return VEC3(::Random(a, b), ::Random(a, b), ::Random(a, b));
	}
};

inline VEC3 operator * (float f, const VEC3& v)
{
	return v * f;
}

// Return min for points
inline void Min(const VEC3& v1, const VEC3& v2, VEC3& out)
{
	out.x = min(v1.x, v2.x);
	out.y = min(v1.y, v2.y);
	out.z = min(v1.z, v2.z);
}

// Return max for points
inline void Max(const VEC3& v1, const VEC3& v2, VEC3& out)
{
	out.x = max(v1.x, v2.x);
	out.y = max(v1.y, v2.y);
	out.z = max(v1.z, v2.z);
}

// Return min and max for points
inline void MinMax(const VEC3& a, const VEC3& b, VEC3& min, VEC3& max)
{
	MinMax(a.x, b.x, min.x, max.x);
	MinMax(a.y, b.y, min.y, max.y);
	MinMax(a.z, b.z, min.z, max.z);
}

// Clamp point to range <vmin,vmax)
inline VEC3 Clamp(const VEC3& v, const VEC3& min, const VEC3& max)
{
	return VEC3(Clamp(v.x, min.x, max.x), Clamp(v.y, min.y, max.y), Clamp(v.z, min.z, max.z));
}

// Return random point
inline VEC3 Random(const VEC3& vmin, const VEC3& vmax)
{
	return VEC3(Random(vmin.x, vmax.x), Random(vmin.y, vmax.y), Random(vmin.z, vmax.z));
}

// Return 2d distance between two points
inline float Distance2d(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		z = abs(v1.z - v2.z);
	return sqrt(x*x + z*z);
}

// Return distance between two points
inline float Distance(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		y = abs(v1.y - v2.y),
		z = abs(v1.z - v2.z);
	return sqrt(x*x + y*y + z*z);
}
inline float DistanceSqrt(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		y = abs(v1.y - v2.y),
		z = abs(v1.z - v2.z);
	return x*x + y*y + z*z;
}

// Return 2d angle between two points
inline float Angle2d(const VEC3& v1, const VEC3& v2)
{
	return Angle(v1.x, v1.z, v2.x, v2.z);
}

// Return 2d dot product
inline float Dot2d(const VEC3& v1, const VEC3& v2)
{
	return (v1.x*v2.x + v1.z*v2.z);
}

//-----------------------------------------------------------------------------
// 4D float point
struct VEC4
{
	float x, y, z, w;

	VEC4() {}
	VEC4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	VEC4(const VEC4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	VEC4& operator += (const VEC4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	VEC4& operator -= (const VEC4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	VEC4& operator *= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	VEC4& operator /= (float f)
	{
		float fi = 1.f / f;
		x *= fi;
		y *= fi;
		z *= fi;
		w *= fi;
		return *this;
	}

	VEC4 operator + () const
	{
		return *this;
	}

	VEC4 operator - () const
	{
		return VEC4(-x, -y, -z, -w);
	}

	VEC4 operator + (const VEC4& v) const
	{
		return VEC4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	VEC4 operator - (const VEC4& v) const
	{
		return VEC4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	VEC4 operator * (float f) const
	{
		return VEC4(x * f, y * f, z * f, w * f);
	}

	VEC4 operator / (float f) const
	{
		float fi = 1.f / f;
		return VEC4(x * fi, y * fi, z * fi, w * fi);
	}

	friend VEC4 operator * (float, const VEC4& v);

	bool operator == (const VEC4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator != (const VEC4& v) const
	{
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	// Clamp all values to 0-1 range
	VEC4 Clamp() const
	{
		return VEC4(::Clamp(x, 0.f, 1.f),
			::Clamp(y, 0.f, 1.f),
			::Clamp(z, 0.f, 1.f),
			::Clamp(w, 0.f, 1.f));
	}

	// Compare VEC4 float values if almost equal
	bool Equal(const VEC4& v) const
	{
		return ::Equal(x, v.x) && ::Equal(y, v.y) && ::Equal(z, v.z) && ::Equal(w, v.w);
	}
};

inline VEC4 operator * (float f, const VEC4& v)
{
	return v * f;
}

//-----------------------------------------------------------------------------
// 2d box using floats
//-----------------------------------------------------------------------------
struct BOX2D
{
	VEC2 v1, v2;

	BOX2D() {}
	BOX2D(float minx, float miny, float maxx, float maxy) : v1(minx, miny), v2(maxx, maxy)
	{
	}
	BOX2D(const VEC2& v1, const VEC2& v2) : v1(v1), v2(v2)
	{
	}
	BOX2D(const BOX2D& box) : v1(box.v1), v2(box.v2)
	{
	}
	BOX2D(float x, float y) : v1(x, y), v2(x, y)
	{
	}
	explicit BOX2D(const VEC2& v) : v1(v), v2(v)
	{
	}
	BOX2D(const BOX2D& box, float margin) : v1(box.v1.x - margin, box.v1.y - margin), v2(box.v2.x + margin, box.v2.y + margin)
	{
	}
	explicit BOX2D(const Rect& r) : v1(r.p1.ToVEC2()), v2(r.p2.ToVEC2())
	{
	}

	static BOX2D Create(const INT2& pos, const INT2& size)
	{
		BOX2D box;
		box.Set(pos, size);
		return box;
	}

	void operator += (const VEC2& v)
	{
		v1 += v;
		v2 += v;
	}

	BOX2D operator / (const VEC2& v)
	{
		return BOX2D(v1.x / v.x, v1.y / v.y, v2.x / v.x, v2.y / v.y);
	}

	void Set(const INT2& pos, const INT2& size)
	{
		v1.x = (float)pos.x;
		v1.y = (float)pos.y;
		v2.x = v1.x + (float)size.x;
		v2.y = v1.y + (float)size.y;
	}

	VEC2 Midpoint() const
	{
		return v1 + (v2 - v1) / 2;
	}

	void Move(const VEC2& pos)
	{
		VEC2 dif = pos - v1;
		v1 += dif;
		v2 += dif;
	}

	bool IsInside(const VEC3& pos) const
	{
		return pos.x >= v1.x && pos.x <= v2.x && pos.z >= v1.y && pos.z <= v2.y;
	}

	bool IsInside(const INT2& pos) const
	{
		float x = (float)pos.x;
		float y = (float)pos.y;
		return x >= v1.x && x <= v2.x && y >= v1.y && y <= v2.y;
	}

	float SizeX() const { return abs(v2.x - v1.x); }
	float SizeY() const { return abs(v2.y - v1.y); }
	VEC2 Size() const { return VEC2(SizeX(), SizeY()); }

	bool IsValid() const
	{
		return v1.x <= v2.x && v1.y <= v2.y;
	}

	VEC2 GetRandomPos() const
	{
		return VEC2(::Random(v1.x, v2.x), ::Random(v1.y, v2.y));
	}
	VEC3 GetRandomPos3(float y = 0.f) const
	{
		return VEC3(::Random(v1.x, v2.x), y, ::Random(v1.y, v2.y));
	}

	VEC2 LeftTop() const
	{
		return v1;
	}
	VEC2 RightBottom() const
	{
		return v2;
	}
	VEC2 RightTop() const
	{
		return VEC2(v2.x, v1.y);
	}
	VEC2 LeftBottom() const
	{
		return VEC2(v1.x, v2.y);
	}

	VEC3 LeftTop3() const
	{
		return VEC3(v1.x, v1.y, 0);
	}
	VEC3 RightTop3() const
	{
		return VEC3(v2.x, v1.y, 0);
	}
	VEC3 LeftBottom3() const
	{
		return VEC3(v1.x, v2.y, 0);
	}
	VEC3 RightBottom3() const
	{
		return VEC3(v2.x, v2.y, 0);
	}

	BOX2D LeftBottomPart() const
	{
		return BOX2D(v1.x, v1.y, v1.x + (v2.x - v1.x) / 2, v1.y + (v2.y - v1.y) / 2);
	}
	BOX2D RightBottomPart() const
	{
		return BOX2D(v1.x + (v2.x - v1.x) / 2, v1.y, v2.x, v1.y + (v2.y - v1.y) / 2);
	}
	BOX2D LeftTopPart() const
	{
		return BOX2D(v1.x, v1.y + (v2.y - v1.y) / 2, v1.x + (v2.x - v1.x) / 2, v2.y);
	}
	BOX2D RightTopPart() const
	{
		return BOX2D(v1.x + (v2.x - v1.x) / 2, v1.y + (v2.y - v1.y) / 2, v2.x, v2.y);
	}

	void ToRectangle(float& x, float& y, float& w, float& h) const
	{
		x = v1.x + (v2.x - v1.x) / 2;
		y = v1.y + (v2.y - v1.y) / 2;
		w = (v2.x - v1.x) / 2;
		h = (v2.y - v1.y) / 2;
	}
	
	float& Left() { return v1.x; }
	float& Right() { return v2.x; }
	float& Top() { return v1.y; }
	float& Bottom() { return v2.y; }

	float Left() const { return v1.x; }
	float Right() const { return v2.x; }
	float Top() const { return v1.y; }
	float Bottom() const { return v2.y; }
};

//-----------------------------------------------------------------------------
// 3d float box
//-----------------------------------------------------------------------------
struct BOX
{
	VEC3 v1, v2;

	BOX() {}
	BOX(float minx, float miny, float minz, float maxx, float maxy, float maxz) : v1(minx, miny, minz), v2(maxx, maxy, maxz)
	{
	}
	BOX(const VEC3& v1, const VEC3& v2) : v1(v1), v2(v2)
	{
	}
	BOX(const BOX& box) : v1(box.v1), v2(box.v2)
	{
	}
	BOX(float x, float y, float z) : v1(x, y, z), v2(x, y, z)
	{
	}
	explicit BOX(const VEC3& v) : v1(v), v2(v)
	{
	}

	void Create(const VEC3& _v1, const VEC3& _v2)
	{
		MinMax(_v1, _v2, v1, v2);
	}

	VEC3 Midpoint() const
	{
		return v1 + (v2 - v1) / 2;
	}

	float SizeX() const { return abs(v2.x - v1.x); }
	float SizeY() const { return abs(v2.y - v1.y); }
	float SizeZ() const { return abs(v2.z - v1.z); }
	VEC3 Size() const { return VEC3(SizeX(), SizeY(), SizeZ()); }
	VEC2 SizeXZ() const { return VEC2(SizeX(), SizeZ()); }

	bool IsValid() const
	{
		return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
	}

	bool IsInside(const VEC3& v) const
	{
		return v.x >= v1.x && v.x <= v2.x && v.y >= v1.y && v.y <= v2.y && v.z >= v1.z && v.z <= v2.z;
	}

	VEC3 GetRandomPos() const
	{
		return VEC3(::Random(v1.x, v2.x), ::Random(v1.y, v2.y), ::Random(v1.z, v2.z));
	}
};

//-----------------------------------------------------------------------------
// 4x4 float matrix
//-----------------------------------------------------------------------------
struct MATRIX
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;

		};
		float m[4][4];
	};

	// Set to identity matrix
	void Identity();
	// Inverse matrix into result
	bool Inverse(MATRIX& result) const;
	// Multiply two matrices and store result in this matrix
	void Multiply(MATRIX& mat1, MATRIX& mat2);
};

//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------
struct QUAT
{
	float x, y, z, w;
};

//-----------------------------------------------------------------------------
// Definitions for forwarded types
//-----------------------------------------------------------------------------
inline INT2::INT2(const VEC2& v) : x(int(v.x)), y(int(v.y)) {}
inline INT2::INT2(const VEC3& v) : x(int(v.x)), y(int(v.z)) {}
inline VEC2 INT2::ToVEC2() const
{
	return VEC2(float(x), float(y));
}
