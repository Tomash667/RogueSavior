#include "Pch.h"
#include "Core.h"
#include "Camera.h"
#include "Game.h"
#include "Input.h"
#include "Player.h"
#include "SceneNode.h"

void Player::Update(float dt)
{
	const float c_cam_angle_min = PI + 0.1f;
	const float c_cam_angle_max = PI*1.8f - 0.1f;

	auto& camera = Game::Get().GetCamera();
	int div = 400;
	camera.rot.y += -float(Input.GetMouseMove().y) / div;
	if(camera.rot.y > c_cam_angle_max)
		camera.rot.y = c_cam_angle_max;
	if(camera.rot.y < c_cam_angle_min)
		camera.rot.y = c_cam_angle_min;
	camera.rot.x = Clip(camera.rot.x + float(Input.GetMouseMove().x) / div);

	

	// scroll distance
	camera.distance = Clamp(camera.distance - float(Input.GetMouseWheel()), 0.5f, 6.f);


	// rotate player to face camera
	float dif = abs(camera.rot.x - unit->node->rot.y);
	if(dif >= 0.1f)
		unit->node->rot.y = camera.rot.x;

	int movex = 0, movey = 0;
	if(Input.Down('W'))
		movey = 1;
	if(Input.Down('S'))
		movey -= 1;
	if(Input.Down('A'))
		movex = -1;
	if(Input.Down('D'))
		movex += 1;

	if(movex != 0 || movey != 0)
	{
		float speed, dir;
		if(movey == 1)
		{
			if(movex == 0)
			{
				dir = ToRadians(180.f);
				speed = 1.f;
			}
			else
			{
				speed = 0.75f;
				if(movex == -1)
					dir = ToRadians(145.f);
				else
					dir = ToRadians(215.f);
			}
		}
		else if(movey == -1)
		{
			speed = 0.25f;
			if(movex == 0)
				dir = ToRadians(0.f);
			else if(movex == -1)
				dir = ToRadians(35.f);
			else
				dir = ToRadians(325.f);
		}
		else
		{
			speed = 0.5f;
			if(movex == -1)
				dir = ToRadians(90.f);
			else
				dir = ToRadians(270.f);
		}

		dir = Clip(dir + camera.rot.x);

		VEC3 move = VEC3(sin(dir), 0, cos(dir)) * 20.f * speed * dt;
		unit->node->pos += move;
	}

	if(unit->CanAct())
	{
		if(Input.Pressed('R') && unit->CanReload())
			unit->Reload();
	}
}
