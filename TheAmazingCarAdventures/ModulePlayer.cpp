#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysBody3D.h"


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	red_off = false;
	blue_off = false;
	green_off = false;

	car_brake_fx = App->audio->LoadFx("sounds/brake.wav");
	change_color_fx = App->audio->LoadFx("sounds/change_color.wav");
	win_fx = App->audio->LoadFx("sounds/Nice.wav");
	crash_wall_fx = App->audio->LoadFx("sounds/car_wall_hit.wav");
	crash_sphere_fx = App->audio->LoadFx("sounds/crash_sphere.wav");

	// Car properties ----------------------------------------
	// Piece
	car.num_chassis = 8;
	car.chassis_size = new vec3[car.num_chassis];
	//Body
	car.chassis_size[0].Set(2.5f, 1.7f, 2.0f); //W, H, L
											   //Back body
	car.chassis_size[1].Set(2.5f, 1.4f, 0.5f);
	//Face body
	car.chassis_size[2].Set(2.5f, 1.2f, 1.5f);
	//Back Colors
	car.chassis_size[3].Set(0.4, 0.2f, 0.2f);
	car.chassis_size[4].Set(0.4, 0.2f, 0.2f);
	car.chassis_size[5].Set(0.4, 0.2f, 0.2f);
	//Mudguard
	car.chassis_size[6].Set(2.5f, 0.2f, 0.02f);
	//Top
	car.chassis_size[7].Set(1.5f, 0.02f, 1.3f);

	// Position
	car.chassis_offset = new vec3[car.num_chassis];
	//Body
	car.chassis_offset[0].Set(0.0f, 1.7f, -0.8f); //LEFT, UP, BACK
												  //Back body
	car.chassis_offset[1].Set(0.0f, 1.5f, -2.05f);
	//Face body
	car.chassis_offset[2].Set(0, 1.48f, 0.9f);
	//Back Colors
	car.chassis_offset[3].Set(0.5, 2.4f, -1.8f);
	car.chassis_offset[4].Set(0.0, 2.4f, -1.8f);
	car.chassis_offset[5].Set(-0.5, 2.4f, -1.8f);
	//Mudguard
	car.chassis_offset[6].Set(0.0f, 0.9f, -2.3f);
	//Top
	car.chassis_offset[7].Set(0.0f, 2.58f, -0.8f);

	// Color
	car.chassis_color = new vec3[car.num_chassis];
	//Body
	car.chassis_color[0] = { White.r, White.g, White.b };
	//Back body
	car.chassis_color[1] = { White.r, White.g, White.b };
	//Face body
	car.chassis_color[2] = { White.r, White.g, White.b };
	//Back Colors
	car.chassis_color[3] = { Red.r, Red.g, Red.b };
	car.chassis_color[4] = { Blue.r, Blue.g, Blue.b };
	car.chassis_color[5] = { Green.r, Green.g, Green.b };
	//Mudguard
	car.chassis_color[6] = { Black.r, Black.g, Black.b };
	//Top
	car.chassis_color[7] = { Grey.r, Grey.g, Grey.b };

	car.mass = 600.0f;
	car.suspensionStiffness = 3.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;
	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = 2.7 * 0.5f;
	float half_length = 4 * 0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 0.5f, 16);

	vehicle->GetTransform(&initial_car_matrix);
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

void ModulePlayer::ResetCar()
{
	vehicle->SetPos(0, 0.5f, 16);
	vehicle->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	vehicle->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	vehicle->SetTransform(&initial_car_matrix);
	car.chassis_color[7] = { Grey.r, Grey.g, Grey.b };
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (vehicle->GetPosY() <= -50)
	{
		ResetCar();
		App->scene_intro->ResetSpheres();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;
	}

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (vehicle->GetKmh() >= 2.5f)
			acceleration = MAX_ACCELERATION;
		else
		{
			brake = -BRAKE_POWER;
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		float vel = vehicle->GetKmh();
		if (vehicle->GetKmh() <= 2.5f)
			acceleration = -MAX_ACCELERATION;
		else
		{
			brake = BRAKE_POWER;
			if (vehicle->GetKmh() >= 40.5f)
			{
				App->audio->PlayFx(car_brake_fx);
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->audio->PlayFx(change_color_fx);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		red_off = false;
		blue_off = false;
		green_off = false;
		car.chassis_color[7] = { Grey.r, Grey.g, Grey.b };
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && red_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
				
		App->audio->PlayFx(change_color_fx);

		red_off = true;
		blue_off = false;
		green_off = false;

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		
		int act_road_num = App->scene_intro->checkpoints_num;
		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getLast(); item; item = item->prev)
		{
			if (item->data == true)
			{
				switch (act_road_num)
				{
				case 4:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_4th_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);					
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);					
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}
		car.chassis_color[7] = { Red.r, Red.g, Red.b };
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && blue_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		
		App->audio->PlayFx(change_color_fx);

		blue_off = true;
		red_off = false;
		green_off = false;

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		int act_road_num = App->scene_intro->checkpoints_num;
		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getLast(); item; item = item->prev)
		{
			if (item->data == true)
			{
				switch (act_road_num)
				{
				case 4:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_4th_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}
		car.chassis_color[7] = { Blue.r, Blue.g, Blue.b };
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN && green_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

		App->audio->PlayFx(change_color_fx);

		green_off = true;
		red_off = false;
		blue_off = false;

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	
		int act_road_num = App->scene_intro->checkpoints_num;
		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getLast(); item; item = item->prev)
		{
			if (item->data == true)
			{
				switch (act_road_num)
				{
				case 4:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_4th_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
						item->data->GetRigidBody()->activate(true);
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}
		car.chassis_color[7] = { Green.r, Green.g, Green.b };
	}
	
	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		ResetCar();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;

		App->scene_intro->ResetSpheres();
	}

	vehicle->Render();

	if (victory == true)
	{
		LOG("VICTORY");
		App->audio->PlayFx(win_fx);
		timer.Stop();
		if (best_time == 0 || best_time > timer.Read())
			best_time = timer.Read();

		App->scene_intro->ResetSpheres();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;

		victory = false;
	}

	if (defeat == true)
	{
		LOG("DEFEAT");
		timer.Stop();

		App->scene_intro->ResetSpheres();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;

		defeat = false;

		ResetCar();
	}

	char title[100];
	float time = (float)timer.Read() / 1000.0f;
	sprintf_s(title, "Speed: %.1f Km/h, Actual Time: %.2f, Best Time: %.2f, Creators Best Time: 31.54", vehicle->GetKmh(), time , best_time / 1000.0f);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}



