#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	actual_pos.x = 0;
	actual_pos.y = 0;
	actual_pos.z = 0;

	//Map.add(App->physics->CreateRoad(50, NORTH, NORTH, 10, 20));
	//Map.add(App->physics->CreateRoad(50, NORTH, NORTH, 20, 10, 20.0));

	/*Map.add(App->physics->CreateRoad(50, EAST, NORTH, 0, 15));
	Map.add(App->physics->CreateRoad(100, NORTH, EAST, 15, 30));
	Map.add(App->physics->CreateRoad(200, WEST, NORTH, 30, 25));
	Map.add(App->physics->CreateRoad(100, SOUTH, WEST, 25, 20));
	Map.add(App->physics->CreateRoad(50, WEST, SOUTH, 20, 10));
	Map.add(App->physics->CreateRoad(200, NORTH, WEST, 10, 20));
	Map.add(App->physics->CreateRoad(50, NORTH, NORTH, 20, 10, 20.0));
	Map.add(App->physics->CreateRoad(50, EAST, NORTH, 10, 20));*/

	Map.add(App->physics->CreateRoad(50, EAST, WEST, 0, 20));
	Map.add(App->physics->CreateWall(10, 20, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, RED));
	Map.add(App->physics->CreateRoad(50, EAST, WEST, 0, 20));
	Map.add(App->physics->CreateRoad(50, EAST, EAST, 20, 20, 0, RED));
	Map.add(App->physics->CreateRoad(50, EAST, EAST, 20, 20));


	/*Cube cube;
	cube.size.Set(50, 1, 50);
	cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 25);
	App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 25);
	App->physics->AddBody(cube);*/
	//Cubes.add(cube);
	//Map.add(App->physics->CreateWall(10, 50, 1, 0, 0, NORTH, GREEN));	//We will need to use actual_pos to help positioning walls
	/*Cube cube2;
	cube2.size.Set(50, 1, 50);
	cube2.SetPos(0,0,0);
	cube2.SetRotation(45, vec3(1, 0, 0));
	App->physics->AddBody(cube2);
	Cubes.add(cube2);*/
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

