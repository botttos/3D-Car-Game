#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

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

	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 3, actual_pos.z + 16, NORTH));	//1rst sensor
	Map.add(App->physics->CreateWall(10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, UNCOLORED));		//back start wall
	Map.add(App->physics->CreateRoad(70, EAST, EAST, 0, 30));
	Map.add(App->physics->CreateWall(10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, RED));		//1rst wall
	Map.add(App->physics->CreateRoad(10, EAST, EAST, 0, 30));
	Map.add(App->physics->CreateRoad(50, EAST, EAST, 30, 30, true, BLUE));
	Map.add(App->physics->CreateRoad(50, EAST, EAST, 30, 30));
	Map.add(App->physics->CreateWall(10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, BLUE));	//2nd wall
	Map.add(App->physics->CreateRoad(50, EAST, EAST, 30, 30));
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 3, actual_pos.z, NORTH));	//2nd sensor
	Map.add(App->physics->CreateRoad(50, NORTH, EAST, 30, 30));
	Map.add(App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, true, BLUE));
	Map.add(App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 20));
	Map.add(App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 15));
	Map.add(App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 10));
	Map.add(App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 5));
	US_2nd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x, actual_pos.y + 10, actual_pos.z - 10, 3, RED, 2, 10.0f));
	US_2nd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x, actual_pos.y + 10, actual_pos.z, 3, GREEN, 2, 10.0f));	
	Map.add(App->physics->CreateRoad(50, NORTH, NORTH, 30, 30));
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 3, actual_pos.z, EAST));	//3rd sensor
	Map.add(App->physics->CreateWall(10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, GREEN));	//3rd wall
	Map.add(App->physics->CreateRoad(30, WEST, NORTH, 30, 30));
	Map.add(App->physics->CreateWall(10, 17, 1, actual_pos.x + 23, actual_pos.y, actual_pos.z, NORTH, UNCOLORED));	//closing wall
	Map.add(App->physics->CreateRoad(50, WEST, WEST, 30, 65));
	Map.add(App->physics->CreateWall(20, 65, 1, actual_pos.x, actual_pos.y, actual_pos.z + 25, NORTH, RED)); //4th wall
	Map.add(App->physics->CreateWall(10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z + 10, NORTH, UNCOLORED)); //3rd road walls 1
	Map.add(App->physics->CreateWall(10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 10, NORTH, UNCOLORED)); //3rd road walls 1.5
	Map.add(App->physics->CreateWall(10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z - 30, NORTH, BLUE)); //3rd road walls 2
	Map.add(App->physics->CreateWall(10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 50, NORTH, UNCOLORED)); //3rd road walls 2.5
	Map.add(App->physics->CreateWall(10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z - 70, NORTH, UNCOLORED)); //3rd road walls 3
	Map.add(App->physics->CreateWall(10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 90, NORTH, UNCOLORED)); //3rd road walls 3.5
	Map.add(App->physics->CreateWall(10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z - 110, NORTH, GREEN)); //3rd road walls 4
	Map.add(App->physics->CreateWall(10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 130, NORTH, UNCOLORED)); //3rd road walls 4.5
	//US_3rd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x + 15, actual_pos.y + 10, actual_pos.z, 3, RED, 3, 10.0f));
	Map.add(App->physics->CreateRoad(50, WEST, WEST, 65, 65));
	Map.add(App->physics->CreateWall(20, 150, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, UNCOLORED));	//midwall
	Map.add(App->physics->CreateRoad(50, WEST, WEST, 65, 65));
	Map.add(App->physics->CreateRoad(50, WEST, WEST, 65, 65));
	checkpoints.add(App->physics->CreateWallSensor(65, 1, actual_pos.x, actual_pos.y + 3, actual_pos.z + 2, SOUTH));	//4th sensor
	Map.add(App->physics->CreateWall(10, 65, 1, actual_pos.x, actual_pos.y, actual_pos.z, SOUTH, BLUE));	//4th wall
	Map.add(App->physics->CreateRoad(50, SOUTH, WEST, 65, 30));
	Map.add(App->physics->CreateWall(10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, WEST, GREEN));	//5th wall
	Map.add(App->physics->CreateRoad(50, SOUTH, SOUTH, 30, 30, true, RED));
	Map.add(App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 20));
	Map.add(App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 15));
	App->physics->CreateDemolitionTrap(actual_pos.x, actual_pos.y + 40, actual_pos.z, 3, GREEN, 4, 32, 25.0f);
	Map.add(App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 10));
	Map.add(App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 5));
	Map.add(App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30));
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x + 10, actual_pos.y + 3, actual_pos.z, WEST));	//5th sensor

	for (p2List_item<PhysBody3D*>* item = checkpoints.getFirst(); item; item = item->next)
	{
		item->data->SetAsSensor(true);
		item->data->collision_listeners.add(this);
		checkpoints_bools.add(false);
		checkpoints_num++;
	}

	for (p2List_item<PhysBody3D*>* item = US_2nd_road_bodies.getFirst(); item; item = item->next)
		item->data->collision_listeners.add(this);

	for (p2List_item<PhysBody3D*>* item = US_3rd_road_bodies.getFirst(); item; item = item->next)
		item->data->collision_listeners.add(this);

	for (p2List_item<PhysBody3D*>* item = US_4th_road_bodies.getFirst(); item; item = item->next)
		item->data->collision_listeners.add(this);

	//RED
	for (p2List_item<Cube>* item = Red_Cubes.getFirst(); item; item = item->next)
		item->data.color = Red;

	for (p2List_item<Sphere>* item = Red_Spheres.getFirst(); item; item = item->next)
		item->data.color = Red;
	//--

	//BLUE
	for (p2List_item<Cube>* item = Blue_Cubes.getFirst(); item; item = item->next)
		item->data.color = Blue;

	for (p2List_item<Sphere>* item = Blue_Spheres.getFirst(); item; item = item->next)
		item->data.color = Blue;
	//--

	//GREEN
	for (p2List_item<Cube>* item = Green_Cubes.getFirst(); item; item = item->next)
		item->data.color = Green;

	for (p2List_item<Sphere>* item = Green_Spheres.getFirst(); item; item = item->next)
		item->data.color = Green;
	//--

	//UNCOLORED
	for (p2List_item<Sphere>* item = US_2nd_road.getFirst(); item; item = item->next)
		item->data.color = White;

	for (p2List_item<Sphere>* item = US_3rd_road.getFirst(); item; item = item->next)
		item->data.color = White;

	for (p2List_item<Sphere>* item = US_4th_road.getFirst(); item; item = item->next)
		item->data.color = White;
	//--

	for (p2List_item<Cube>* item = Cubes.getFirst(); item; item = item->next)
		item->data.color = { 0.95f, 0.7f, 0.4f };	//TODO: repaint road

	for (p2List_item<Cube>* item = Uncolored_Cubes.getFirst(); item; item = item->next)
		item->data.color = { 0.8f, 0.85f, 0.9f };	//TODO: repaint walls

	for (p2List_item<PhysBody3D*>* item = US_2nd_road_bodies.getFirst(); item; item = item->next)
	{
		All_Spheres_bodies.add(item->data);
		item->data->GetRigidBody()->forceActivationState(WANTS_DEACTIVATION);
	}
	for (p2List_item<PhysBody3D*>* item = US_3rd_road_bodies.getFirst(); item; item = item->next)
	{
		All_Spheres_bodies.add(item->data);
		item->data->GetRigidBody()->forceActivationState(WANTS_DEACTIVATION);
	}
	for (p2List_item<PhysBody3D*>* item = US_4th_road_bodies.getFirst(); item; item = item->next)
	{
		All_Spheres_bodies.add(item->data);
		item->data->GetRigidBody()->forceActivationState(WANTS_DEACTIVATION);
	}
	for (p2List_item<PhysBody3D*>* item = Red_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = Blue_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = Green_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

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
	//TRANSFORMATIONS
	p2List_item<Sphere>* spheres = Red_Spheres.getFirst();
	for (p2List_item<PhysBody3D*>* item = Red_Spheres_bodies.getFirst(); item; item = item->next)
	{
		if (checkpoints_bools.getFirst()->next->data == true)
		{
			item->data->GetTransform(&spheres->data.transform);
			spheres = spheres->next;
		}
	}

	spheres = Blue_Spheres.getFirst();
	for (p2List_item<PhysBody3D*>* item = Blue_Spheres_bodies.getFirst(); item; item = item->next)
	{
		item->data->GetTransform(&spheres->data.transform);
		spheres = spheres->next;
	}

	spheres = Green_Spheres.getFirst();
	for (p2List_item<PhysBody3D*>* item = Green_Spheres_bodies.getFirst(); item; item = item->next)
	{
		item->data->GetTransform(&spheres->data.transform);
		spheres = spheres->next;
	}

	spheres = US_2nd_road.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_2nd_road_bodies.getFirst(); item; item = item->next)
	{
		item->data->GetTransform(&spheres->data.transform);
		spheres = spheres->next;
	}

	spheres = US_3rd_road.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_3rd_road_bodies.getFirst(); item; item = item->next)
	{
		item->data->GetTransform(&spheres->data.transform);
		spheres = spheres->next;
	}

	spheres = US_4th_road.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_4th_road_bodies.getFirst(); item; item = item->next)
	{
		item->data->GetTransform(&spheres->data.transform);
		spheres = spheres->next;
	}
	//--

	for (p2List_item<Cube>* item = Cubes.getFirst(); item; item = item->next)
		item->data.Render();

	for (p2List_item<Cube>* item = Uncolored_Cubes.getFirst(); item; item = item->next)
		item->data.Render();

	//UNCOLORED SPHERES
	for (p2List_item<Sphere>* item = US_2nd_road.getFirst(); item; item = item->next)
		item->data.Render();

	for (p2List_item<Sphere>* item = US_3rd_road.getFirst(); item; item = item->next)
		item->data.Render();

	for (p2List_item<Sphere>* item = US_4th_road.getFirst(); item; item = item->next)
		item->data.Render();
	//--

	//RED
	if (App->player->red_off == false)
	{
		for (p2List_item<Cube>* item = Red_Cubes.getFirst(); item; item = item->next)
			item->data.Render();

		for (p2List_item<Sphere>* item = Red_Spheres.getFirst(); item; item = item->next)
			item->data.Render();
	}
	//--

	//GREEN
	if (App->player->green_off == false)
	{
		for (p2List_item<Cube>* item = Green_Cubes.getFirst(); item; item = item->next)
			item->data.Render();

		for (p2List_item<Sphere>* item = Green_Spheres.getFirst(); item; item = item->next)
			item->data.Render();
	}
	//--

	//BLUE
	if (App->player->blue_off == false)
	{
		for (p2List_item<Cube>* item = Blue_Cubes.getFirst(); item; item = item->next)
			item->data.Render();

		for (p2List_item<Sphere>* item = Blue_Spheres.getFirst(); item; item = item->next)
			item->data.Render();
	}
	//--

	//VICTORY?
	if (checkpoints_bools.find(false) == -1)
		App->player->victory = true;
	//--

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body2 == App->player->vehicle && checkpoints.find(body1) != -1)
	{
		p2List_item<bool>* item_bools = checkpoints_bools.getFirst();
		for (p2List_item<PhysBody3D*>* item = checkpoints.getFirst(); item; item = item->next, item_bools = item_bools->next)
		{
			if (body1 == item->data)
			{
				item_bools->data = true;
				if (item->data == checkpoints.getFirst()->data)
				{
					App->player->timer.Start();
					checkpoints_bools.getLast()->data = false;
				}
			}
		}
	}
	else if (body2 == App->player->vehicle && All_Spheres_bodies.find(body1) != -1)
	{
		LOG("car crashed against sphere");
		App->player->defeat = true;
	}

	//TODO: add collision sound against: 	App->scene_intro->Red_bodies/Blue_bodies/Green_bodies; && App->scene_intro->Uncolored_Cubes
}

void ModuleSceneIntro::ResetSpheres()
{
	//2nd road
	p2List_item<vec3>* position_item = US_2nd_road_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_2nd_road_bodies.getFirst(); item; item = item->next, position_item = position_item->next)
	{
		item->data->SetPos(position_item->data.x, position_item->data.y, position_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}

	//3rd road
	position_item = US_3rd_road_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_3rd_road_bodies.getFirst(); item; item = item->next, position_item = position_item->next)
	{
		item->data->SetPos(position_item->data.x, position_item->data.y, position_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}

	//4th road
	position_item =US_4th_road_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = US_4th_road_bodies.getFirst(); item; item = item->next, position_item = position_item->next)
	{
		item->data->SetPos(position_item->data.x, position_item->data.y, position_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}
	//--

	p2List_item<vec3>* position_red_item = Red_Spheres_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = Red_Spheres_bodies.getFirst(); item; item = item->next, position_red_item = position_red_item->next)
	{
		item->data->SetPos(position_red_item->data.x, position_red_item->data.y, position_red_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}

	p2List_item<vec3>* position_blue_item = Blue_Spheres_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = Blue_Spheres_bodies.getFirst(); item; item = item->next, position_blue_item = position_blue_item->next)
	{
		item->data->SetPos(position_blue_item->data.x, position_blue_item->data.y, position_blue_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}

	p2List_item<vec3>* position_green_item = Green_Spheres_positions.getFirst();
	for (p2List_item<PhysBody3D*>* item = Green_Spheres_bodies.getFirst(); item; item = item->next, position_green_item = position_green_item->next)
	{
		item->data->SetPos(position_green_item->data.x, position_green_item->data.y, position_green_item->data.z);
		item->data->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		item->data->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	}

	//reset color filter
	for (p2List_item<PhysBody3D*>* item = Red_bodies.getFirst(); item; item = item->next)
		item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

	for (p2List_item<PhysBody3D*>* item = Blue_bodies.getFirst(); item; item = item->next)
		item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

	for (p2List_item<PhysBody3D*>* item = Green_bodies.getFirst(); item; item = item->next)
		item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

	App->player->red_off = false;
	App->player->blue_off = false;
	App->player->green_off = false;
	//--
}
