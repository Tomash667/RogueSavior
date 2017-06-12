#pragma once

class Camera
{
public:
	Camera();
	float GetAspect() { return aspect; }
	const VEC2& GetDrawRange() { return draw_range; }
	float GetFov() { return fov; }
	const VEC3& GetFrom() { return from; }
	const FrustumPlanes& GetFrustum();
	const MATRIX& GetViewProjectionMatrix();
	const MATRIX& GetProjectionMatrix();
	const VEC3& GetTo() { return to; }
	const VEC3& GetUp() { return up; }
	const MATRIX& GetViewMatrix();
	void SetAspect(float aspect);
	void SetDrawRange(const VEC2& draw_range);
	void SetFov(float fov);
	void SetFrom(const VEC3& from);
	void SetPerspective(float fov, float aspect, const VEC2& draw_range);
	void SetTo(const VEC3& to);
	void SetUp(const VEC3& up);
	void SetView(const VEC3& from, const VEC3& to, const VEC3& up = VEC3(0, 1, 0));

private:
	void UpdateFrustum();
	void UpdateProjection();
	void UpdateView();

	VEC3 from, to, up;
	VEC2 draw_range;
	float fov, aspect;
	MATRIX mat_view, mat_proj, mat_viewproj;
	FrustumPlanes frustum;
	bool update_view, update_proj, update_frustum;
};
