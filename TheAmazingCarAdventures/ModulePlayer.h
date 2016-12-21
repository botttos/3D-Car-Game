#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Timer.h"
#include "PhysVehicle3D.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 700.0f
#define TURN_DEGREES 12.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

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

	void ResetCar();

private:
	btRigidBody* chassis_vehicle;
	float best_time = 0;
	mat4x4 initial_car_matrix;
	float car_y;
	VehicleInfo car;

public:
	int car_brake_fx;
	int change_color_fx;
	int win_fx;
	int crash_wall_fx;
};