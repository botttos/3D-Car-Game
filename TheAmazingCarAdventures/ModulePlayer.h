#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Timer.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 700.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 700.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

public:
	bool red_off;
	bool blue_off;
	bool green_off;

	bool victory = false;
	bool defeat = false;
	Timer timer;

private:
	btRigidBody* chassis_vehicle;
	float best_time = 0;
	mat4x4 initial_car_matrix;
};