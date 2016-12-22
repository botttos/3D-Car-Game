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
	cannon_tp_fx = App->audio->LoadFx("sounds/cannon_shot.wav");

	crash_time.Start();
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	actual_pos.x = 0;
	actual_pos.y = 0;
	actual_pos.z = 0;

	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 5, actual_pos.z + 16, NORTH));	//1rst sensor

	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, UNCOLORED);	//back start wall
	App->physics->CreateRoad(50, EAST, EAST, 0, 30);
	Cube c1;
	Red_bodies.add(App->physics->CreateWall(c1, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, RED));		//1rst wall
	Red_Cubes.add(c1);

	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -20);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -15);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -10);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -5);
	Cube c2;
	Blue_bodies.add(App->physics->CreateWall(c2, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, BLUE));		//1rst wall
	Blue_Cubes.add(c2);
	App->physics->CreateRoad(10, EAST, EAST, 30, 30);
	App->physics->CreateRoad(50, EAST, EAST, 30, 30, true, RED);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -15);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -10);
	App->physics->CreateRoad(20, EAST, EAST, 30, 30, false, UNCOLORED, -5);
	Cube c3;
	Green_bodies.add(App->physics->CreateWall(c3, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, GREEN));		//1rst wall
	Green_Cubes.add(c3);
	App->physics->CreateRoad(30, EAST, EAST, 30, 30);
	Cube c4;
	Blue_bodies.add(App->physics->CreateWall(c4, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, NORTH, BLUE));	//2nd wall
	Blue_Cubes.add(c4);
	App->physics->CreateRoad(40, EAST, EAST, 30, 30);
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 5, actual_pos.z, NORTH));	//2nd sensor

	App->physics->CreateRoad(50, NORTH, EAST, 30, 30);
	App->physics->CreateRoad(50, NORTH, NORTH, 30, 30);
	Cube c5;
	Red_bodies.add(App->physics->CreateWall(c5, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, RED));	//3rd wall
	Red_Cubes.add(c5);	
	App->physics->CreateRoad(50, NORTH, NORTH, 30, 30);
	Cube c6;
	Green_bodies.add(App->physics->CreateWall(c6, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, GREEN));	//3rd wall
	Green_Cubes.add(c6);	
	App->physics->CreateRoad(50, NORTH, NORTH, 30, 30);
	App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, true, BLUE);
	App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 20);
	App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 15);
	App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 10);
	US_2nd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x - 20, actual_pos.y + 15, actual_pos.z - 10, 3, RED, 2, 10.0f));
	US_2nd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x - 20, actual_pos.y + 15, actual_pos.z, 3, GREEN, 2, 10.0f));
	US_2nd_road_bodies.add(App->physics->CreateDemolitionBall(actual_pos.x - 20, actual_pos.y + 15, actual_pos.z + 10, 3, BLUE, 2, 10.0f));
	App->physics->CreateRoad(20, NORTH, NORTH, 30, 30, false, UNCOLORED, 5);
	App->physics->CreateRoad(50, NORTH, NORTH, 30, 30);
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 5, actual_pos.z, EAST));	//3rd sensor
	Cube c7;
	Blue_bodies.add(App->physics->CreateWall(c7, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, BLUE));	//3rd wall
	Blue_Cubes.add(c7);
	App->physics->CreateRoad(30, WEST, NORTH, 30, 30);
	sensor = App->physics->CreateWallSensor(10, 1, actual_pos.x - 25, actual_pos.y + 3, actual_pos.z - 2, NORTH);
	sensor->SetAsSensor(true);
	sensor->collision_listeners.add(this);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 17, 1, actual_pos.x + 23, actual_pos.y, actual_pos.z, NORTH, UNCOLORED);	//closing wall
	App->physics->CreateRoad(50, WEST, WEST, 30, 65);
	App->physics->CreateRoad(50, WEST, WEST, 65, 65);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z + 50, NORTH, UNCOLORED); //3rd road walls 0
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z + 30, NORTH, UNCOLORED); //3rd road walls 0.5
	Cube c8;
	Red_bodies.add(App->physics->CreateWall(c8, 10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z + 10, NORTH, RED)); //3rd road walls 1
	Red_Cubes.add(c8);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 10, NORTH, UNCOLORED); //3rd road walls 1.5
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z - 30, NORTH, UNCOLORED); //3rd road walls 2
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 50, NORTH, UNCOLORED); //3rd road walls 2.5
	Cube c9;
	Green_bodies.add(App->physics->CreateWall(c9, 10, 18, 1, actual_pos.x-9, actual_pos.y, actual_pos.z - 70, NORTH, GREEN)); //3rd road walls 3
	Green_Cubes.add(c9);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 9, actual_pos.y, actual_pos.z - 70, NORTH, UNCOLORED); //3rd road walls 3
	Cube c9_5;
	Blue_bodies.add(App->physics->CreateWall(c9_5, 10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 90, NORTH, BLUE)); //3rd road walls 3.5
	Blue_Cubes.add(c9_5);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 35, 1, actual_pos.x, actual_pos.y, actual_pos.z - 110, NORTH, UNCOLORED); //3rd road walls 4
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 25, actual_pos.y, actual_pos.z - 130, NORTH, UNCOLORED); //3rd road walls 4.5
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 18, 1, actual_pos.x + 9, actual_pos.y, actual_pos.z - 150, NORTH, UNCOLORED); //3rd road walls 5																												
	App->physics->CreateRoad(50, WEST, WEST, 65, 65);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 20, 220, 1, actual_pos.x, actual_pos.y, actual_pos.z, EAST, UNCOLORED);	//midwall
	App->physics->CreateRoad(50, WEST, WEST, 65, 65);
	App->physics->CreateRoad(50, WEST, WEST, 65, 65);
	App->physics->CreateRoad(50, WEST, WEST, 65, 65);
	checkpoints.add(App->physics->CreateWallSensor(65, 1, actual_pos.x, actual_pos.y + 5, actual_pos.z + 2, SOUTH));	//4th sensor
	cannon_ball = App->physics->CreateDemolitionBall(actual_pos.x - 24, actual_pos.y + 4, actual_pos.z + 8, 5, UNCOLORED, 5, 10.0f);
	cannon_ball->GetPos(&cannon_ball_init_pos.x, &cannon_ball_init_pos.y, &cannon_ball_init_pos.z);
	cannon_sphere.color = Indigo;
	cannon_ball->GetPosZ(&cannon_ball_z);
	cannon_ball->collision_listeners.add(this);
	Cube c10;
	Red_bodies.add(App->physics->CreateWall(c10, 10, 50, 1, actual_pos.x + 7, actual_pos.y, actual_pos.z, SOUTH, RED));	//4th wall
	Red_Cubes.add(c10);
	App->physics->CreateRoad(50, SOUTH, WEST, 65, 30);
	
	Cube c11;
	Green_bodies.add(App->physics->CreateWall(c11, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, WEST, GREEN));
	Green_Cubes.add(c11);

	App->physics->CreateRoad(50, SOUTH, SOUTH, 30, 30);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 3, 1, actual_pos.x, actual_pos.y, actual_pos.z + 10, WEST, UNCOLORED);	//receptor1
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 3, 1, actual_pos.x, actual_pos.y, actual_pos.z - 8, WEST, UNCOLORED);	//receptor2
	sensor2_3 = App->physics->CreateWallSensor(30, 1, actual_pos.x, actual_pos.y + 3, actual_pos.z, WEST);
	sensor2_3->SetAsSensor(true);
	sensor2_3->collision_listeners.add(this);
	//Cube c12;
	//Green_bodies.add(App->physics->CreateWall(c12, 10, 30, 1, actual_pos.x, actual_pos.y, actual_pos.z, WEST, GREEN));
	//Green_Cubes.add(c12);
	App->physics->CreateRoad(50, SOUTH, SOUTH, 30, 30);
	App->physics->CreateRoad(50, SOUTH, SOUTH, 30, 30);
	App->physics->CreateRoad(15, SOUTH, SOUTH, 30, 30);
	App->physics->CreateRoad(50, SOUTH, SOUTH, 30, 30);
	App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 20);
	App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 15);
	App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 10);
	App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30, false, UNCOLORED, 5);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 4, 10, 1, actual_pos.x + 8, actual_pos.y, actual_pos.z + 14, WEST, UNCOLORED);	//receptor3
	cannon_ball2 = App->physics->CreateDemolitionBall(actual_pos.x + 10, actual_pos.y + 4, actual_pos.z + 10, 6, UNCOLORED, 6, 10.0f);
	cannon_ball2->GetPos(&cannon_ball2_init_pos.x, &cannon_ball2_init_pos.y, &cannon_ball2_init_pos.z);
	cannon_sphere2.color = Indigo;
	cannon_ball2->GetPosX(&cannon_ball2_x);
	cannon_ball2->collision_listeners.add(this);
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 4, 10, 1, actual_pos.x + 8, actual_pos.y, actual_pos.z - 13, WEST, UNCOLORED);	//receptor3
	cannon_ball3 = App->physics->CreateDemolitionBall(actual_pos.x + 10, actual_pos.y + 4, actual_pos.z - 8, 6, UNCOLORED, 7, 10.0f);
	cannon_ball3->GetPos(&cannon_ball3_init_pos.x, &cannon_ball3_init_pos.y, &cannon_ball3_init_pos.z);
	cannon_sphere3.color = Indigo;
	cannon_ball3->GetPosX(&cannon_ball3_x);
	cannon_ball3->collision_listeners.add(this);
	App->physics->CreateRoad(20, SOUTH, SOUTH, 30, 30);
	checkpoints.add(App->physics->CreateWallSensor(30, 1, actual_pos.x + 10, actual_pos.y + 5, actual_pos.z, WEST));	//5th sensor
	App->physics->CreateWall(Uncolored_Cubes[App->physics->Cube_num], &Uncolored_Cubes_Bodies[App->physics->Cube_num], 10, 15, 1, actual_pos.x, actual_pos.y, actual_pos.z - 8, EAST, UNCOLORED);	//midwall

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

	for (int i = 0; i < SCENE_INTRO_U_CUBES; i++)
		Uncolored_Cubes[i].color = { 0.8f, 0.85f, 0.9f };	//TODO: repaint walls

	for (p2List_item<PhysBody3D*>* item = US_2nd_road_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = US_3rd_road_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = US_4th_road_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = Red_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = Blue_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (p2List_item<PhysBody3D*>* item = Green_Spheres_bodies.getFirst(); item; item = item->next)
		All_Spheres_bodies.add(item->data);

	for (int i = 0; i < SCENE_INTRO_U_CUBES; i++)
		Uncolored_Cubes_Bodies[i]->collision_listeners.add(this);

	delay_c3.Start();

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
	float z_cannon;
	cannon_ball->GetPosZ(&z_cannon);
	if (z_cannon == cannon_ball_z && cannon_ball_reached_sensor == true)
	{
		App->audio->PlayFx(cannon_tp_fx);
		cannon_ball->GetRigidBody()->activate(true);
		cannon_ball->Push(0, 0, 550);
		cannon_ball_reached_sensor = false;
	}
	cannon_ball->GetTransform(&cannon_sphere.transform);
	cannon_sphere.Render();


	float x_cannon2;
	cannon_ball2->GetPosX(&x_cannon2);
	if (x_cannon2 == cannon_ball2_x && cannon_ball2_reached_sensor == true)
	{
		App->audio->PlayFx(cannon_tp_fx);
		cannon_ball2->GetRigidBody()->activate(true);
		cannon_ball2->Push(700, 0, 0);
		cannon_ball2_reached_sensor = false;
	}
	cannon_ball2->GetTransform(&cannon_sphere2.transform);
	cannon_sphere2.Render();

	float x_cannon3;
	cannon_ball3->GetPosX(&x_cannon3);
	if (x_cannon3 == cannon_ball3_x && cannon_ball3_reached_sensor == true)
	{
		if (delay_c3.Read() > 800)
		{
			App->audio->PlayFx(cannon_tp_fx);
			cannon_ball3->GetRigidBody()->activate(true);
			cannon_ball3->Push(700, 0, 0);
			cannon_ball3_reached_sensor = false;
		}
	}
	cannon_ball3->GetTransform(&cannon_sphere3.transform);
	cannon_sphere3.Render();

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

	for (int i = 0; i < SCENE_INTRO_U_CUBES; i++)
		Uncolored_Cubes[i].Render();

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
		App->audio->PlayFx(App->player->crash_sphere_fx);
		App->player->defeat = true;
	}

	if (body2 == cannon_ball && body1 == sensor)
	{
		cannon_ball->SetPos(cannon_ball_init_pos.x, cannon_ball_init_pos.y, cannon_ball_init_pos.z);
		cannon_ball->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball_reached_sensor = true;
	}

	if (body2 == App->player->vehicle && body1 == cannon_ball)
	{
		App->audio->PlayFx(App->player->crash_sphere_fx);
		App->player->defeat = true;
		cannon_ball->SetPos(cannon_ball_init_pos.x, cannon_ball_init_pos.y, cannon_ball_init_pos.z);
		cannon_ball->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball_reached_sensor = true;
	}

	if (body2 == cannon_ball2 && body1 == sensor2_3)
	{
		cannon_ball2->SetPos(cannon_ball2_init_pos.x, cannon_ball2_init_pos.y, cannon_ball2_init_pos.z);
		cannon_ball2->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball2->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball2_reached_sensor = true;
	}

	if (body2 == App->player->vehicle && body1 == cannon_ball2)
	{
		App->audio->PlayFx(App->player->crash_sphere_fx);
		App->player->defeat = true;
		cannon_ball2->SetPos(cannon_ball2_init_pos.x, cannon_ball2_init_pos.y, cannon_ball2_init_pos.z);
		cannon_ball2->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball2->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball2_reached_sensor = true;
	}

	if (body2 == cannon_ball3 && body1 == sensor2_3)
	{
		cannon_ball3->SetPos(cannon_ball3_init_pos.x, cannon_ball3_init_pos.y, cannon_ball3_init_pos.z);
		cannon_ball3->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball3->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball3_reached_sensor = true;
	}

	if (body2 == App->player->vehicle && body1 == cannon_ball3)
	{
		App->audio->PlayFx(App->player->crash_sphere_fx);
		App->player->defeat = true;
		cannon_ball3->SetPos(cannon_ball3_init_pos.x, cannon_ball3_init_pos.y, cannon_ball3_init_pos.z);
		cannon_ball3->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		cannon_ball3->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		cannon_ball3_reached_sensor = true;
	}

	if (body2 == App->player->vehicle)
	{
		for (int i = 0; i < SCENE_INTRO_U_CUBES; i++)
		{
			if (Uncolored_Cubes_Bodies[i] == body1 && crash_time.Read() > 2000)
			{
				App->audio->PlayFx(App->player->crash_wall_fx);
				crash_time.Start();
			}
		}
	}
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
