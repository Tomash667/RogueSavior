#include "Pch.h"
#include "Core.h"
#include "DirectXIncl.h"

#define FLOAT_ALMOST_ZERO(F) ((absolute_cast<unsigned>(F) & 0x7f800000L) == 0)

const VEC2 VEC2::Zero = { 0.f, 0.f };
const VEC2 VEC2::One = { 1.f, 1.f };
const VEC2 VEC2::UnitX = { 1.f, 0.f };
const VEC2 VEC2::UnitY = { 0.f, 1.f };

const VEC3 VEC3::Zero = { 0.f, 0.f, 0.f };
const VEC3 VEC3::One = { 1.f, 1.f, 1.f };
const VEC3 VEC3::UnitX = { 1.f, 0.f, 0.f };
const VEC3 VEC3::UnitY = { 0.f, 1.f, 0.f };
const VEC3 VEC3::UnitZ = { 0.f, 0.f, 1.f };
const VEC3 VEC3::Up = { 0.f, 1.f, 0.f };
const VEC3 VEC3::Down = { 0.f, -1.f, 0.f };
const VEC3 VEC3::Right = { 1.f, 0.f, 0.f };
const VEC3 VEC3::Left = { -1.f, 0.f, 0.f };
const VEC3 VEC3::Forward = { 0.f, 0.f, -1.f };
const VEC3 VEC3::Backward = { 0.f, 0.f, 1.f };

const VEC4 VEC4::Zero = { 0.f, 0.f, 0.f, 0.f };
const VEC4 VEC4::One = { 1.f, 1.f, 1.f, 1.f };
const VEC4 VEC4::UnitX = { 1.f, 0.f, 0.f, 0.f };
const VEC4 VEC4::UnitY = { 0.f, 1.f, 0.f, 0.f };
const VEC4 VEC4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
const VEC4 VEC4::UnitW = { 0.f, 0.f, 0.f, 1.f };

const MATRIX MATRIX::IdentityMatrix = {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

const QUAT QUAT::Identity = { 0.f, 0.f, 0.f, 1.f };

bool Intersect3Planes(const PLANE& P1, const PLANE& P2, const PLANE& P3, VEC3& OutP)
{
	float fDet;
	float MN[9] = { P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, P3.x, P3.y, P3.z };
	float IMN[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float MD[3] = { -P1.w, -P2.w , -P3.w };

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
	planes[0].x = worldViewProj._14 + worldViewProj._11;
	planes[0].y = worldViewProj._24 + worldViewProj._21;
	planes[0].z = worldViewProj._34 + worldViewProj._31;
	planes[0].w = worldViewProj._44 + worldViewProj._41;
	planes[0].Normalize();

	// Right clipping plane
	planes[1].x = worldViewProj._14 - worldViewProj._11;
	planes[1].y = worldViewProj._24 - worldViewProj._21;
	planes[1].z = worldViewProj._34 - worldViewProj._31;
	planes[1].w = worldViewProj._44 - worldViewProj._41;
	planes[1].Normalize();

	// Top clipping plane
	planes[2].x = worldViewProj._14 - worldViewProj._12;
	planes[2].y = worldViewProj._24 - worldViewProj._22;
	planes[2].z = worldViewProj._34 - worldViewProj._32;
	planes[2].w = worldViewProj._44 - worldViewProj._42;
	planes[2].Normalize();

	// Bottom clipping plane
	planes[3].x = worldViewProj._14 + worldViewProj._12;
	planes[3].y = worldViewProj._24 + worldViewProj._22;
	planes[3].z = worldViewProj._34 + worldViewProj._32;
	planes[3].w = worldViewProj._44 + worldViewProj._42;
	planes[3].Normalize();

	// Near clipping plane
	planes[4].x = worldViewProj._13;
	planes[4].y = worldViewProj._23;
	planes[4].z = worldViewProj._33;
	planes[4].w = worldViewProj._43;
	planes[4].Normalize();

	// Far clipping plane
	planes[5].x = worldViewProj._14 - worldViewProj._13;
	planes[5].y = worldViewProj._24 - worldViewProj._23;
	planes[5].z = worldViewProj._34 - worldViewProj._33;
	planes[5].w = worldViewProj._44 - worldViewProj._43;
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
		points[i] = VEC3::Transform(P[i], worldViewProjInv);
}

bool FrustumPlanes::PointInFrustum(const VEC3 &p) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(planes[i].DotCoordinate(p) <= 0.f)
			return false;
	}

	return true;
}

bool FrustumPlanes::BoxToFrustum(const BOX& box) const
{
	VEC3 vmin;

	for(int i = 0; i < 6; i++)
	{
		if(planes[i].x <= 0.0f)
			vmin.x = box.v1.x;
		else
			vmin.x = box.v2.x;

		if(planes[i].y <= 0.0f)
			vmin.y = box.v1.y;
		else
			vmin.y = box.v2.y;

		if(planes[i].z <= 0.0f)
			vmin.z = box.v1.z;
		else
			vmin.z = box.v2.z;

		if(planes[i].DotCoordinate(vmin) < 0.0f)
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
		if(planes[i].DotCoordinate(sphere_center) <= sphere_radius)
			return false;
	}

	return true;
}

bool FrustumPlanes::SphereInFrustum(const VEC3& sphere_center, float sphere_radius) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(planes[i].DotCoordinate(sphere_center) < sphere_radius)
			return false;
	}

	return true;
}
