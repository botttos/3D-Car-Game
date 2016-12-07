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

	//Map.add(App->physics->CreateFlat(20, EAST, EAST));
	//Map.add(App->physics->CreateFlat(20, NORTH, EAST));
	//Map.add(App->physics->CreateFlat(40, WEST, NORTH));
	//Map.add(App->physics->CreateFlat(20, SOUTH, WEST));
	Cube cube;
	cube.size.Set(50, 1, 50);
	cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 25);
	App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 25);
	ret = App->physics->AddBody(cube, 0);
	App->scene_intro->Cubes.add(cube);
	Map.add(App->physics->CreateWall(10, 50, 1, 0, 0, NORTH, GREEN));	//We will need to use actual_pos to help positioning walls

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

