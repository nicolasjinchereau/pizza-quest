/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Camera.h"
#include "Shader.h"

weak_ptr<Camera> Camera::_activeCamera;

Camera::Camera()
{
	Init();
	SetSize(480);
}

Camera::Camera(int height)
{
	Init();
	SetSize((float)height);
}

void Camera::Init()
{
	// 1 width to avoid division by zero errors
	viewRect.Set(0, 0, 1, 1);

	camSpeed = 400.0f;
	_viewBorder = 0.3f;

	position.set(0, 0);
	target.set(0, 0);
	velocity.set(0, 0);

	delta.set(0, 0);

	identity.SetIdentity();
	mtxCamera.SetIdentity();
	mtxProject.SetIdentity();
	mtxViewProjection.SetIdentity();

	viewPanEdgeDistX = 0;
	viewPanEdgeDistY = 0;
	
	screenToWorldXFactor = 0;
	screenToWorldYFactor = 0;
}

Camera::~Camera()
{

}

void Camera::SetSize(float height)
{
	Rect oldRect = viewRect;

	float worldHeight = height;
	float worldWidth = (float)Graphics::width() / (float)Graphics::height() * (float)worldHeight;

	viewRect.w = worldWidth;
	viewRect.h = worldHeight;

	viewPanEdgeDistX = viewRect.w * _viewBorder;
	viewPanEdgeDistY = viewRect.h * _viewBorder;

	mtxProject.SetOrtho2D(0.0f, (float)viewRect.w, (float)viewRect.h, 0.0f);

	screenToWorldXFactor = (float)viewRect.w / Graphics::width();
	screenToWorldYFactor = (float)viewRect.h / Graphics::height();
}

float Camera::GetSize() const
{
	return viewRect.h;
}

void Camera::PreDrawUpdate()
{
	velocity = target - position;
	const float length = velocity.Length();
	
	if(length > 0.05f)
	{
		velocity /= length;
		velocity *= min(camSpeed * Time::deltaTime(), length);
		position += velocity;
	}

	mtxCamera.SetTranslation(-position.x, -position.y);
	mtxViewProjection = mtxProject * mtxCamera;

	viewRect.x = position.x;
	viewRect.y = position.y;
}

void Camera::LookAt(float wx, float wy, bool immediate)
{
	const float x1 = target.x + (float)viewPanEdgeDistX;
	const float y1 = target.y + (float)viewPanEdgeDistY;
	const float x2 = target.x + (float)(viewRect.w - viewPanEdgeDistX);
	const float y2 = target.y + (float)(viewRect.h - viewPanEdgeDistY);
	
	delta.set(0, 0);

	if(wx > x2) delta.x += (wx - x2);
	if(wx < x1) delta.x -= (x1 - wx);
	if(wy > y2) delta.y += (wy - y2);
	if(wy < y1) delta.y -= (y1 - wy);

	target += delta;

	if(immediate)
		position = target;
}

void Camera::CenterAt(float wx, float wy, bool immediate)
{
	target.set(wx - (float)(viewRect.w * 0.5f),
			   wy - (float)(viewRect.h * 0.5f));

	if(immediate)
		position = target;
}

void Camera::ResetView()
{
	position.set(0, 0);
	target.set(0, 0);
}

float Camera::ScreenToWorldX(float x)
{
	return x * screenToWorldXFactor + viewRect.x;
}

float Camera::ScreenToWorldY(float y)
{
	return y * screenToWorldYFactor + viewRect.y;
}

float Camera::NormalizedToWorldX(float x)
{
	return x * viewRect.w + viewRect.x;
}

float Camera::NormalizedToWorldY(float y)
{
	return y * viewRect.h + viewRect.y;
}

vec2f Camera::NormalizedToWorld(vec2f point)
{
	point.x = point.x * viewRect.w + viewRect.x;
	point.y = point.y * viewRect.h + viewRect.y;
	return point;
}

vec2f Camera::WorldToScreen(const vec2f &point) const
{
	return vec2f((point.x - viewRect.x) / screenToWorldXFactor,
				 (point.y - viewRect.y) / screenToWorldYFactor);
}

vec2f Camera::ScreenToWorld(const vec2f &point) const
{
	return vec2f(point.x * screenToWorldXFactor + viewRect.x,
				 point.y * screenToWorldYFactor + viewRect.y);
}

const mat3f &Camera::matrix() const
{
	return mtxViewProjection;
}

shared_ptr<Camera> Camera::activeCamera()
{
	return _activeCamera.lock();
}

void Camera::activeCamera(const shared_ptr<Camera> &camera)
{
	_activeCamera = camera;
}

float Camera::viewBorder() const
{
	return _viewBorder;
}

void Camera::viewBorder(float setViewBorder)
{
	_viewBorder = setViewBorder;
}
