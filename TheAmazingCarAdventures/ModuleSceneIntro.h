#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define SCENE_INTRO_U_CUBES 103

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	vec3 actual_pos;
	p2List<PhysBody3D*> Map;
	p2List<Cube> Cubes;
	Cube Uncolored_Cubes[SCENE_INTRO_U_CUBES];
	PhysBody3D* Uncolored_Cubes_Bodies[SCENE_INTRO_U_CUBES];
	p2List<Cube> Red_Cubes;
	p2List<Cube> Green_Cubes;
	p2List<Cube> Blue_Cubes;
	p2List<Sphere> Red_Spheres;
	p2List<Sphere> Green_Spheres;
	p2List<Sphere> Blue_Spheres;

	p2List<Sphere> US_2nd_road;
	p2List<PhysBody3D*> US_2nd_road_bodies;
	p2List<vec3> US_2nd_road_positions;

	p2List<Sphere> US_3rd_road;
	p2List<PhysBody3D*> US_3rd_road_bodies;
	p2List<vec3> US_3rd_road_positions;

	p2List<Sphere> US_4th_road;
	p2List<PhysBody3D*> US_4th_road_bodies;
	p2List<vec3> US_4th_road_positions;
	
	p2List<PhysBody3D*> Red_bodies;
	p2List<PhysBody3D*> Blue_bodies;
	p2List<PhysBody3D*> Green_bodies;
	p2List<PhysBody3D*> Red_Spheres_bodies;
	p2List<vec3> Red_Spheres_positions;
	p2List<PhysBody3D*> Blue_Spheres_bodies;
	p2List<vec3> Blue_Spheres_positions;
	p2List<PhysBody3D*> Green_Spheres_bodies;
	p2List<vec3> Green_Spheres_positions;

	p2List<PhysBody3D*> All_Spheres_bodies;

	p2List<PhysBody3D*> checkpoints;
	p2List<bool> checkpoints_bools;
	uint checkpoints_num = 0;

	PhysBody3D* cannon_ball;
	Sphere cannon_sphere;
	float cannon_ball_z;
	vec3 cannon_ball_init_pos;
	PhysBody3D* sensor;
	bool cannon_ball_reached_sensor = true;

	PhysBody3D* cannon_ball2;
	Sphere cannon_sphere2;
	float cannon_ball2_x;
	vec3 cannon_ball2_init_pos;
	PhysBody3D* sensor2_3;
	bool cannon_ball2_reached_sensor = true;

	Timer delay_c3;
	PhysBody3D* cannon_ball3;
	Sphere cannon_sphere3;
	float cannon_ball3_x;
	vec3 cannon_ball3_init_pos;
	bool cannon_ball3_reached_sensor = true;

	void ResetSpheres();

	int cannon_tp_fx;
	Timer crash_time;
};
