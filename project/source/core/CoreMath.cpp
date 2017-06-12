#include "Pch.h"
#include "Core.h"
#include "DirectXIncl.h"

#define FLOAT_ALMOST_ZERO(F) ((absolute_cast<unsigned>(F) & 0x7f800000L) == 0)

VEC3& VEC3::Normalize()
{
	D3DXVec3Normalize((D3DXVECTOR3*)this, (D3DXVECTOR3*)this);
	return *this;
}

VEC3 VEC3::TransformCoord(const MATRIX& m) const
{
	VEC3 result;
	D3DXVec3TransformCoord((D3DXVECTOR3*)&result, (const D3DXVECTOR3*)this, (const D3DXMATRIX*)&m);
	return result;
}

void MATRIX::Identity()
{
	_11 = 1.f; _12 = 0.f; _13 = 0.f; _14 = 0.f;
	_21 = 0.f; _22 = 1.f; _23 = 0.f; _24 = 0.f;
	_31 = 0.f; _32 = 0.f; _33 = 1.f; _34 = 0.f;
	_41 = 0.f; _42 = 0.f; _43 = 0.f; _44 = 1.f;
}

bool MATRIX::Inverse(MATRIX& result) const
{
	D3DXMATRIX* result_ptr = D3DXMatrixInverse((D3DXMATRIX*)&result, nullptr, (D3DXMATRIX*)this);
	return result_ptr != nullptr;
}

void MATRIX::LookAt(const VEC3& from, const VEC3& to, const VEC3& up)
{
	D3DXMatrixLookAtLH((D3DXMATRIX*)this, (const D3DXVECTOR3*)&from, (const D3DXVECTOR3*)&to, (const D3DXVECTOR3*)&up);
}

void MATRIX::Multiply(const MATRIX& mat1, const MATRIX& mat2)
{
	D3DXMatrixMultiply((D3DXMATRIX*)this, (D3DXMATRIX*)&mat1, (D3DXMATRIX*)&mat2);
}

void MATRIX::Perspective(float fov, float aspect, const VEC2& draw_range)
{
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX*)this, fov, aspect, draw_range.x, draw_range.y);
}

void MATRIX::Translation(const VEC3& pos)
{
	D3DXMatrixTranslation((D3DXMATRIX*)this, pos.x, pos.y, pos.z);
}

float PLANE::DotCoord(const VEC3& v) const
{
	return D3DXPlaneDotCoord((const D3DXPLANE*)this, (const D3DXVECTOR3*)&v);
}

void PLANE::Normalize()
{
	D3DXPlaneNormalize((D3DXPLANE*)this, (D3DXPLANE*)this);
}

bool Intersect3Planes(const PLANE& P1, const PLANE& P2, const PLANE& P3, VEC3& OutP)
{
	float fDet;
	float MN[9] = { P1.a, P1.b, P1.c, P2.a, P2.b, P2.c, P3.a, P3.b, P3.c };
	float IMN[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float MD[3] = { -P1.d, -P2.d , -P3.d };

	IMN[0] = MN[4] * MN[8] - MN[5] * MN[7];
	IMN[3] = -(MN[3] * MN[8] - MN[5] * MN[6]);
	IMN[6] = MN[3] * MN[7] - MN[4] * MN[6];

	fDet = MN[0] * IMN[0] + MN[1] * IMN[3] + MN[2] * IMN[6];

	if(FLOAT_ALMOST_ZERO(fDet))
		return false;

	IMN[1] = -(MN[1] * MN[8] - MN[2] * MN[7]);
	IMN[4] = MN[0] * MN[8] - MN[2] * MN[6];
	IMN[7] = -(MN[0] * MN[7] - MN[1] * MN[6]);
	IMN[2] = MN[1] * MN[5] - MN[2] * MN[4];
	IMN[5] = -(MN[0] * MN[5] - MN[2] * MN[3]);
	IMN[8] = MN[0] * MN[4] - MN[1] * MN[3];

	fDet = 1.0f / fDet;

	IMN[0] *= fDet;
	IMN[1] *= fDet;
	IMN[2] *= fDet;
	IMN[3] *= fDet;
	IMN[4] *= fDet;
	IMN[5] *= fDet;
	IMN[6] *= fDet;
	IMN[7] *= fDet;
	IMN[8] *= fDet;

	OutP.x = IMN[0] * MD[0] + IMN[1] * MD[1] + IMN[2] * MD[2];
	OutP.y = IMN[3] * MD[0] + IMN[4] * MD[1] + IMN[5] * MD[2];
	OutP.z = IMN[6] * MD[0] + IMN[7] * MD[1] + IMN[8] * MD[2];

	return true;
}

void FrustumPlanes::Set(const MATRIX& worldViewProj)
{
	// Left clipping plane
	planes[0].a = worldViewProj._14 + worldViewProj._11;
	planes[0].b = worldViewProj._24 + worldViewProj._21;
	planes[0].c = worldViewProj._34 + worldViewProj._31;
	planes[0].d = worldViewProj._44 + worldViewProj._41;
	planes[0].Normalize();

	// Right clipping plane
	planes[1].a = worldViewProj._14 - worldViewProj._11;
	planes[1].b = worldViewProj._24 - worldViewProj._21;
	planes[1].c = worldViewProj._34 - worldViewProj._31;
	planes[1].d = worldViewProj._44 - worldViewProj._41;
	planes[1].Normalize();

	// Top clipping plane
	planes[2].a = worldViewProj._14 - worldViewProj._12;
	planes[2].b = worldViewProj._24 - worldViewProj._22;
	planes[2].c = worldViewProj._34 - worldViewProj._32;
	planes[2].d = worldViewProj._44 - worldViewProj._42;
	planes[2].Normalize();

	// Bottom clipping plane
	planes[3].a = worldViewProj._14 + worldViewProj._12;
	planes[3].b = worldViewProj._24 + worldViewProj._22;
	planes[3].c = worldViewProj._34 + worldViewProj._32;
	planes[3].d = worldViewProj._44 + worldViewProj._42;
	planes[3].Normalize();

	// Near clipping plane
	planes[4].a = worldViewProj._13;
	planes[4].b = worldViewProj._23;
	planes[4].c = worldViewProj._33;
	planes[4].d = worldViewProj._43;
	planes[4].Normalize();

	// Far clipping plane
	planes[5].a = worldViewProj._14 - worldViewProj._13;
	planes[5].b = worldViewProj._24 - worldViewProj._23;
	planes[5].c = worldViewProj._34 - worldViewProj._33;
	planes[5].d = worldViewProj._44 - worldViewProj._43;
	planes[5].Normalize();
}

void FrustumPlanes::GetPoints(VEC3* points) const
{
	assert(points);

	Intersect3Planes(planes[4], planes[0], planes[3], points[0]);
	Intersect3Planes(planes[4], planes[1], planes[3], points[1]);
	Intersect3Planes(planes[4], planes[0], planes[2], points[2]);
	Intersect3Planes(planes[4], planes[1], planes[2], points[3]);
	Intersect3Planes(planes[5], planes[0], planes[3], points[4]);
	Intersect3Planes(planes[5], planes[1], planes[3], points[5]);
	Intersect3Planes(planes[5], planes[0], planes[2], points[6]);
	Intersect3Planes(planes[5], planes[1], planes[2], points[7]);
}

void FrustumPlanes::GetPoints(const MATRIX& worldViewProj, VEC3* points)
{
	assert(points);

	MATRIX worldViewProjInv;
	worldViewProj.Inverse(worldViewProjInv);

	VEC3 P[] = {
		VEC3(-1.f, -1.f, 0.f), VEC3(+1.f, -1.f, 0.f),
		VEC3(-1.f, +1.f, 0.f), VEC3(+1.f, +1.f, 0.f),
		VEC3(-1.f, -1.f, 1.f), VEC3(+1.f, -1.f, 1.f),
		VEC3(-1.f, +1.f, 1.f), VEC3(+1.f, +1.f, 1.f) };

	for(int i = 0; i < 8; ++i)
		points[i] = P[i].TransformCoord(worldViewProjInv);
}

bool FrustumPlanes::PointInFrustum(const VEC3 &p) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(planes[i].DotCoord(p) <= 0.f)
			return false;
	}

	return true;
}

bool FrustumPlanes::BoxToFrustum(const BOX& box) const
{
	VEC3 vmin;

	for(int i = 0; i < 6; i++)
	{
		if(planes[i].a <= 0.0f)
			vmin.x = box.v1.x;
		else
			vmin.x = box.v2.x;

		if(planes[i].b <= 0.0f)
			vmin.y = box.v1.y;
		else
			vmin.y = box.v2.y;

		if(planes[i].c <= 0.0f)
			vmin.z = box.v1.z;
		else
			vmin.z = box.v2.z;

		if(planes[i].DotCoord(vmin) < 0.0f)
			return false;
	}

	return true;
}

bool FrustumPlanes::BoxInFrustum(const BOX& box) const
{
	if(!PointInFrustum(box.v1)) return false;
	if(!PointInFrustum(box.v2)) return false;
	if(!PointInFrustum(VEC3(box.v2.x, box.v1.y, box.v1.z))) return false;
	if(!PointInFrustum(VEC3(box.v1.x, box.v2.y, box.v1.z))) return false;
	if(!PointInFrustum(VEC3(box.v2.x, box.v2.y, box.v1.z))) return false;
	if(!PointInFrustum(VEC3(box.v1.x, box.v1.y, box.v2.z))) return false;
	if(!PointInFrustum(VEC3(box.v2.x, box.v1.y, box.v2.z))) return false;
	if(!PointInFrustum(VEC3(box.v1.x, box.v2.y, box.v2.z))) return false;

	return true;
}

bool FrustumPlanes::SphereToFrustum(const VEC3& sphere_center, float sphere_radius) const
{
	sphere_radius = -sphere_radius;

	for(int i = 0; i < 6; ++i)
	{
		if(planes[i].DotCoord(sphere_center) <= sphere_radius)
			return false;
	}

	return true;
}

bool FrustumPlanes::SphereInFrustum(const VEC3& sphere_center, float sphere_radius) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(planes[i].DotCoord(sphere_center) < sphere_radius)
			return false;
	}

	return true;
}
