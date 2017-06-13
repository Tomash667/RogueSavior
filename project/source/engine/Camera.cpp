#include "Pch.h"
#include "Core.h"
#include "Camera.h"

Camera::Camera() : from(0,5,5), to(0,1,0), up(0,1,0), draw_range(0.01f, 50.f), fov(PI/4), aspect(800.f/600.f), update_view(true), update_proj(true),
update_frustum(true)
{
}

const FrustumPlanes& Camera::GetFrustum()
{
	UpdateFrustum();
	return frustum;
}

const MATRIX& Camera::GetViewProjectionMatrix()
{
	UpdateFrustum();
	return mat_viewproj;
}

const MATRIX& Camera::GetProjectionMatrix()
{
	if(update_proj)
		UpdateProjection();
	return mat_proj;
}

const MATRIX& Camera::GetViewMatrix()
{
	if(update_view)
		UpdateView();
	return mat_view;
}

void Camera::SetAspect(float _aspect)
{
	if(_aspect != aspect)
	{
		aspect = _aspect;
		update_proj = true;
	}
}

void Camera::SetDrawRange(const VEC2& _draw_range)
{
	if(_draw_range != draw_range)
	{
		draw_range = _draw_range;
		update_proj = true;
	}
}

void Camera::SetFov(float _fov)
{
	if(_fov != fov)
	{
		fov = _fov;
		update_proj = true;
	}
}

void Camera::SetFrom(const VEC3& _from)
{
	if(_from != from)
	{
		from = _from;
		update_view = true;
	}
}

void Camera::SetPerspective(float _fov, float _aspect, const VEC2& _draw_range)
{
	if(_fov != fov || _aspect != aspect || _draw_range != draw_range)
	{
		fov = _fov;
		aspect = _aspect;
		draw_range = _draw_range;
		update_proj = true;
	}
}

void Camera::SetTo(const VEC3& _to)
{
	if(_to != to)
	{
		to = _to;
		update_view = true;
	}
}

void Camera::SetUp(const VEC3& _up)
{
	if(_up != up)
	{
		up = _up;
		update_view = true;
	}
}

void Camera::SetView(const VEC3& _from, const VEC3& _to, const VEC3& _up)
{
	if(_from != from || _to != to || _up != up)
	{
		from = _from;
		to = _to;
		up = _up;
		update_view = true;
	}
}

void Camera::UpdateFrustum()
{
	if(update_view)
		UpdateView();
	if(update_proj)
		UpdateProjection();
	if(update_frustum)
	{
		mat_viewproj.Multiply(mat_view, mat_proj);
		frustum.Set(mat_viewproj);
		update_frustum = false;
	}
}

void Camera::UpdateProjection()
{
	mat_proj.Perspective(fov, aspect, draw_range);
	update_proj = false;
	update_frustum = true;
}

void Camera::UpdateView()
{
	mat_view.LookAt(from, to, up);
	update_view = false;
	update_frustum = true;
}
