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

vector3d vector3d::operator+(const vector3d& vec)
{
	return vector3d(x + vec.x, y + vec.y, z + vec.z);
}

vector3d& vector3d::operator+=(const vector3d& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

vector3d vector3d::operator-(const vector3d& vec)
{
	return vector3d(x - vec.x, y - vec.y, z - vec.z);
}

vector3d& vector3d::operator-=(const vector3d& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

vector3d vector3d::operator*(float value)
{
	return vector3d(x * value, y * value, z * value);
}

vector3d& vector3d::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

vector3d vector3d::operator/(float value)
{
	assert(value != 0);
	return vector3d(x * value, y * value, z * value);
}

vector3d& vector3d::operator/=(float value)
{
	assert(value != 0);
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

vector3d& vector3d::operator=(const vector3d& vec)
{
	return vector3d(x = vec.x, y = vec.y, z = vec.z);
}

float vector3d::dot_product(const vector3d& vec)
{
	return (vec.x * x) + (vec.y * y) + (vec.z * z);
}

vector3d vector3d::cross_product(const vector3d& vec)
{
	return vector3d(vec.y*z - vec.z*y,
					vec.z*x - vec.x*z,
					vec.x*y - vec.y*x);
}

vector3d vector3d::normalization()
{
	//TO DO
	return vector3d();
}

void vector3d::ComputeNormal(const vector3d& p0, const vector3d& p1, const vector3d& p2, vector3d& out)
{
	vector3d u = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
	vector3d v = { p2.x - p0.x, p2.y - p0.y, p2.z - p0.z };

	out = { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
	
	//return vector3d(out.x, out.y, out.z);
}

float vector3d::square(const vector3d& vec)
{
	//TO DO
	return 0.0f;
}

float vector3d::distance(const vector3d& vec)
{
	//TO DO
	return 0.0f;
}

float vector3d::magnitude(const vector3d& vec)
{
	return (sqrtf(powf(vec.x, 2) + powf(vec.y, 2) + powf(vec.z, 2)));
}

float vector3d::show_X()
{
	return x;
}
float vector3d::show_Y()
{
	return y;
}
float vector3d::show_Z()
{
	return z;
}
void vector3d::disp()
{
	cout << x << " " << y << " " << z << endl;
}

