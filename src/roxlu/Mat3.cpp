#include "Mat3.h"
#include "Utils.h"

namespace roxlu {

// copy constructor.
Mat3::Mat3(const Mat3& o)
	:flag(o.flag)
{
	m[0] = o.m[0];
	m[1] = o.m[1];
	m[2] = o.m[2];
	m[3] = o.m[3];
	m[4] = o.m[4];
	m[5] = o.m[5];
	m[6] = o.m[6];
	m[7] = o.m[7];
	m[8] = o.m[8];
}

bool Mat3::isIdentity() const {
	return flag == IDENTITY;
}

bool Mat3::isZero() const {
	for(int i = 0; i < 9; ++i) {
		if(m[i] != 0.0) {
			return false;
		}
	}
	return true;
}

void Mat3::setRows(const Vec3& row0, const Vec3& row1, const Vec3& row2) {
	m[0] = row0.x;
	m[3] = row0.y;
	m[6] = row0.z;

	m[1] = row1.x;
	m[4] = row1.y;
	m[7] = row1.z;
	
	m[2] = row2.x;
	m[5] = row2.y;
	m[8] = row2.z;
}


void Mat3::getRows(Vec3& row0, Vec3& row1, Vec3& row2) {
	row0.x = m[0];
	row0.y = m[3];
	row0.z = m[6];

	row1.x = m[1];
	row1.y = m[4];
	row1.z = m[7];
	
	row2.x = m[2];
	row2.y = m[5];
	row2.z = m[8];
}

Vec3 Mat3::getRow(unsigned int i) {
	return Vec3(m[i], m[i+3], m[i+6]);
}

void Mat3::setColumns(const Vec3& col0, const Vec3& col1, const Vec3& col2) {
	m[0] = col0.x;
	m[1] = col0.y;
	m[2] = col0.z;
	
	m[3] = col1.x;
	m[4] = col1.x;
	m[5] = col1.x;
	
	m[6] = col2.x;
	m[7] = col2.y;
	m[8] = col2.z;
}

void Mat3::getColumns(Vec3& col0, Vec3& col1, Vec3& col2) {
	col0.x = m[0];
	col0.y = m[1];
	col0.z = m[2];
	
	col1.x = m[3];
	col1.y = m[4];
	col1.z = m[5];
	
	col2.x = m[6];
	col2.y = m[7];
	col2.z = m[8];
}

Vec3 Mat3::getColumn(unsigned int i) {
	int dx = 3*i;
	return Vec3(m[dx], m[dx+1], m[dx+2]);
}


void Mat3::identity() {
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	
	m[3] = 0.0f;
	m[4] = 1.0f;
	m[5] = 0.0f;
	
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 1.0f;
}


Mat3 inverse(Mat3& o) {
	Mat3 r;
	float cofactor0 = o.m[4]*o.m[8] - o.m[5]*o.m[7];
    float cofactor3 = o.m[2]*o.m[7] - o.m[1]*o.m[8];
    float cofactor6 = o.m[1]*o.m[5] - o.m[2]*o.m[4];
	float det = o.m[0]*cofactor0 + o.m[3]*cofactor3 + o.m[6]*cofactor6;

    if (IS_ZERO(det)) {
		printf("Mat3::inverse error: singular matrix\n");
        return r;
    }
	
	float inv_det = 1.0f / det;
	
	r.m[0] = inv_det*cofactor0;
    r.m[1] = inv_det*cofactor3;
   	r.m[2] = inv_det*cofactor6;
   
    r.m[3] = inv_det*(o.m[5]*o.m[6] - o.m[3]*o.m[8]);
	r.m[4] = inv_det*(o.m[0]*o.m[8] - o.m[2]*o.m[6]);
    r.m[5] = inv_det*(o.m[2]*o.m[3] - o.m[0]*o.m[5]);

	r.m[6] = inv_det*(o.m[3]*o.m[7] - o.m[4]*o.m[6]);
    r.m[7] = inv_det*(o.m[1]*o.m[6] - o.m[0]*o.m[7]);
    r.m[8] = inv_det*(o.m[0]*o.m[4] - o.m[1]*o.m[3]);

    return r;
}


// invert ourself, use inverse(Mat3) to get a copy
Mat3& Mat3::inverse() {
	*this = roxlu::inverse(*this);
	return *this;
}


// Set Euler rotation
Mat3& Mat3::rotation(float rotX, float rotY, float rotZ) {
	rotX = rotX * DEG_TO_RAD;
	rotY = rotY * DEG_TO_RAD;
	rotZ = rotZ * DEG_TO_RAD;
	
	float cx = cosf(rotX);
	float sx = sinf(rotX);
	float cy = cosf(rotY);
	float sy = sinf(rotY);
	float cz = cosf(rotZ);
	float sz = sinf(rotZ);
	
	m[0] = (cy * cz);
	m[3] = -(cy * sz);
	m[6] = sy;
	
	m[1] = (sx * sy * cz) + (cx * sz);
	m[4] = -(sx * sy * sz) + (cx * cz);
	m[7] = -(sx * cy);
	
	m[2] = -(cx * sy * cz) + (sx * sz);
	m[5] = (cx * sy * sz) + (sx * cz);
	m[8] = (cx * cy);
	return *this;
}

// rotate around given axis
Mat3& Mat3::rotation(float a, float x, float y, float z) {
	a = a * DEG_TO_RAD;
	float c = cosf(a);
	float s = sinf(a);
	float t = 1.0f - c;
	
	Vec3 ax(x,y,z);
	ax.normalize();
	
	float tx = t*ax.x;
	float ty = t*ax.y;
	float tz = t*ax.z;
	
	float sx = s*ax.x;
	float sy = s*ax.y;
	float sz = s*ax.z;
	
	float txy = tx * ax.y;
	float tyz = tx * ax.z; // @todo is this correct?
	float txz = tx * ax.z;
	
	m[0] = tx * ax.x + c;
	m[3] = txy - sz;
	m[6] = txz + sy;
	
	m[1] = txy + sz;
	m[4] = ty * ax.y + c;
	m[7] = tyz - sx;
	
	m[2] = txz - sy;
	m[5] = tyz + sx;
	m[8] = tz * ax.z +c;
	return *this;
}

Mat3& Mat3::rotationX(float a) {
	a = a * DEG_TO_RAD;
	float s = sinf(a);
	float c = cosf(a);
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	
	m[3] = 0.0f;
	m[4] = c;
	m[5] = s;
	
	m[6] = 0.0f;
	m[7] = -s;
	m[8] = c;
	
	return *this;
}

Mat3& Mat3::rotationY(float a) {
	a = a * DEG_TO_RAD;
	float s = sinf(a);
	float c = cosf(a);
	m[0] = c;
	m[1] = 0.0f;
	m[2] = -s;
	
	m[3] = 0.0f;
	m[4] = 1.0f;
	m[5] = 0.0f;
	
	m[6] = s;
	m[7] = 0.0f;
	m[8] = c;
	
	return *this;
}

Mat3& Mat3::rotationZ(float a) {
	a = a * DEG_TO_RAD;
	float s = sinf(a);
	float c = cosf(a);
	m[0] = c;
	m[1] = s;
	m[2] = 0.0f;
	
	m[3] = -s;
	m[4] = c;
	m[5] = 0.0f;

	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 1.0f;
	
	return *this;
}

// Operators.
//------------------------------------------------------------------------------
bool Mat3::operator==(const Mat3& o) const {
	for(unsigned int i = 0; i < 9; ++i) {
		if(m[i] != o.m[i]) {
			return false;
		}
	}
	return true;
}

bool Mat3::operator!=(const Mat3& o) const {
	for(unsigned int i = 0; i < 9; ++i) {
		if(m[i] != o.m[i]) {
			return true;
		}
	}
	return true;
}

Mat3& Mat3::operator=(const Mat3& o) {
	if(this == &o) {
		return *this;
	}
	
	m[0] = o.m[0];
	m[1] = o.m[1];
	m[2] = o.m[2];
	m[3] = o.m[3];
	m[4] = o.m[4];
	m[5] = o.m[5];
	m[6] = o.m[6];
	m[7] = o.m[7];
	m[8] = o.m[8];
	return *this;
}

Mat3 Mat3::operator-() {
	float f[9];
	f[0] = m[0];
	f[1] = m[1];
	f[2] = m[2];
	f[3] = m[3];
	f[4] = m[4];
	f[5] = m[5];
	f[6] = m[6];
	f[7] = m[7];
	f[8] = m[8];
	
	m[0] = -f[0];
	m[1] = -f[1];
	m[2] = -f[2];
	m[3] = -f[3];
	m[4] = -f[4];
	m[5] = -f[5];
	m[6] = -f[6];
	m[7] = -f[7];
	m[8] = -f[8];
	return *this;
}


Mat3 Mat3::operator*(const Mat3& o) const {
	Mat3 r;
	
	r.m[0] = m[0] * o.m[0] + m[3] * o.m[1] + m[6] * o.m[2];
	r.m[1] = m[1] * o.m[0] + m[4] * o.m[1] + m[7] * o.m[2];
	r.m[2] = m[2] * o.m[0] + m[5] * o.m[1] + m[8] * o.m[2];
	
	r.m[3] = m[0] * o.m[3] + m[3] * o.m[4] + m[6] * o.m[5];
	r.m[4] = m[1] * o.m[3] + m[4] * o.m[4] + m[7] * o.m[5];
	r.m[5] = m[2] * o.m[3] + m[5] * o.m[4] + m[8] * o.m[5];
	
	r.m[6] = m[0] * o.m[6] + m[3] * o.m[7] + m[6] * o.m[8];
	r.m[7] = m[1] * o.m[6] + m[4] * o.m[7] + m[7] * o.m[8];
	r.m[8] = m[2] * o.m[6] + m[5] * o.m[7] + m[8] * o.m[8];
	
	return r;
}

Mat3& Mat3::operator*=(const Mat3& o) {
	float f[9];
	f[0] = m[0] * o.m[0] + m[3] * o.m[1] + m[6] * o.m[2];
	f[1] = m[1] * o.m[0] + m[4] * o.m[1] + m[7] * o.m[2];
	f[2] = m[2] * o.m[0] + m[5] * o.m[1] + m[8] * o.m[2];
	
	f[3] = m[0] * o.m[3] + m[3] * o.m[4] + m[6] * o.m[5];
	f[4] = m[1] * o.m[3] + m[4] * o.m[4] + m[7] * o.m[5];
	f[5] = m[2] * o.m[3] + m[5] * o.m[4] + m[8] * o.m[5];
	
	f[6] = m[0] * o.m[6] + m[3] * o.m[7] + m[6] * o.m[8];
	f[7] = m[1] * o.m[6] + m[4] * o.m[7] + m[7] * o.m[8];
	f[8] = m[2] * o.m[6] + m[5] * o.m[7] + m[8] * o.m[8];

	m[0] = f[0];
	m[1] = f[1];
	m[2] = f[2];

	m[3] = f[3];
	m[4] = f[4];
	m[5] = f[5];

	m[6] = f[6];
	m[7] = f[7];
	m[8] = f[8];
	
	return *this;
}

Mat3& Mat3::operator*=(const float s) {
	m[0] = m[0] * s;
	m[1] = m[1] * s;
	m[2] = m[2] * s;
	m[3] = m[3] * s;
	m[4] = m[4] * s;
	m[5] = m[5] * s;
	m[6] = m[6] * s;
	m[7] = m[7] * s;
	m[8] = m[8] * s;
	return *this;
}

Mat3 Mat3::operator*(const float s) const{
	Mat3 r;
	r.m[0] = m[0] * s;
	r.m[1] = m[1] * s;
	r.m[2] = m[2] * s;
	r.m[3] = m[3] * s;
	r.m[4] = m[4] * s;
	r.m[5] = m[5] * s;
	r.m[6] = m[6] * s;
	r.m[7] = m[7] * s;
	r.m[8] = m[8] * s;
	return *this;
}

Mat3 operator*(const float s, const Mat3& o) {
	Mat3 r;
	r.m[0] = o.m[0] * s;
	r.m[1] = o.m[1] * s;
	r.m[2] = o.m[2] * s;
	r.m[3] = o.m[3] * s;
	r.m[4] = o.m[4] * s;		
	r.m[5] = o.m[5] * s;
	r.m[6] = o.m[6] * s;
	r.m[7] = o.m[7] * s;
	r.m[8] = o.m[8] * s;
	return r;
}

Mat3 Mat3::operator+(const Mat3& o) const {
	Mat3 r;
	r.m[0] = m[0] + o.m[0];
	r.m[1] = m[1] + o.m[1];
	r.m[2] = m[2] + o.m[2];
	r.m[3] = m[3] + o.m[3];
	r.m[4] = m[4] + o.m[4];
	r.m[5] = m[5] + o.m[5];
	r.m[6] = m[6] + o.m[6];
	r.m[7] = m[7] + o.m[7];
	r.m[8] = m[8] + o.m[8];
	return r;
}

Mat3& Mat3::operator+=(const Mat3& o) {
	m[0] = m[0] + o.m[0];
	m[1] = m[1] + o.m[1];
	m[2] = m[2] + o.m[2];
	m[3] = m[3] + o.m[3];
	m[4] = m[4] + o.m[4];
	m[5] = m[5] + o.m[5];
	m[6] = m[6] + o.m[6];
	m[7] = m[7] + o.m[7];
	m[8] = m[8] + o.m[8];
	return *this;
}


Mat3 Mat3::operator-(const Mat3& o) const {
	Mat3 r;
	r.m[0] = m[0] - o.m[0];
	r.m[1] = m[1] - o.m[1];
	r.m[2] = m[2] - o.m[2];
	r.m[3] = m[3] - o.m[3];
	r.m[4] = m[4] - o.m[4];
	r.m[5] = m[5] - o.m[5];
	r.m[6] = m[6] - o.m[6];
	r.m[7] = m[7] - o.m[7];
	r.m[8] = m[8] - o.m[8];
	return r;
}

Mat3& Mat3::operator-=(const Mat3& o) {
	m[0] = m[0] - o.m[0];
	m[1] = m[1] - o.m[1];
	m[2] = m[2] - o.m[2];
	m[3] = m[3] - o.m[3];
	m[4] = m[4] - o.m[4];
	m[5] = m[5] - o.m[5];
	m[6] = m[6] - o.m[6];
	m[7] = m[7] - o.m[7];
	m[8] = m[8] - o.m[8];
	return *this;
}

} // roxlu