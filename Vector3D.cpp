#include "Vector3D.h"
using namespace DirectX;

vector3d::vector3d()
{
	x = 0;
	y = 0;
	z = 0;
}

vector3d::vector3d(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

vector3d::vector3d(const vector3d& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

//adds vector
vector3d vector3d::operator+(const vector3d& vec)
{
	return vector3d(x + vec.x, y + vec.y, z + vec.z);
}

//plus equals vector
vector3d& vector3d::operator+=(const vector3d& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

//minus vector
vector3d vector3d::operator-(const vector3d& vec)
{
	return vector3d(x - vec.x, y - vec.y, z - vec.z);
}

//minus equals vector
vector3d& vector3d::operator-=(const vector3d& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

//multiplies vector
vector3d vector3d::operator*(float value)
{
	return vector3d(x * value, y * value, z * value);
}

//multiple equals vector
vector3d& vector3d::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

//divides vector
vector3d vector3d::operator/(float value)
{
	assert(value != 0);
	return vector3d(x * value, y * value, z * value);
}

//divide equals vector
vector3d& vector3d::operator/=(float value)
{
	assert(value != 0);
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

//sets vector to another
vector3d& vector3d::operator=(const vector3d& vec)
{
	return vector3d(x = vec.x, y = vec.y, z = vec.z);
}

//finds dot product between 2 vectors
float vector3d::dot_product(const vector3d& vec)
{
	return (vec.x * x) + (vec.y * y) + (vec.z * z);
}

//finds cross product between 2 vectors
vector3d vector3d::cross_product(const vector3d& vec)
{
	return vector3d(vec.y*z - vec.z*y,
					vec.z*x - vec.x*z,
					vec.x*y - vec.y*x);
}

//normalizes vector
vector3d vector3d::normalization()
{
	float unitVector = sqrt((x * x) + (y * y) + (z * z));

	x = x / unitVector;
	y = y / unitVector;
	z = z / unitVector;

	return vector3d();
}

//calculate normal from 3 vectors
void vector3d::ComputeNormal(const vector3d& p0, const vector3d& p1, const vector3d& p2, vector3d& out)
{
	vector3d u = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
	vector3d v = { p2.x - p0.x, p2.y - p0.y, p2.z - p0.z };

	out = { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
}

//square vector
float vector3d::square(const vector3d& vec)
{
	return (sqrt(vec.x), sqrt(vec.y), sqrt(vec.z));
}

//find distance between vector
float vector3d::distance(const vector3d& vec)
{
	return (vec.x-x, vec.y-y, vec.z-z);
}

//find magnitude of vector
float vector3d::magnitude(const vector3d& vec)
{
	return (sqrtf(powf(vec.x, 2) + powf(vec.y, 2) + powf(vec.z, 2)));
}

//finds x of vector
float vector3d::show_X()
{
	return x;
}
//finds y of vector
float vector3d::show_Y()
{
	return y;
}
//finds z of vector
float vector3d::show_Z()
{
	return z;
}

