#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
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

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2.5, 1.7, 4);
	car.chassis_offset.Set(0, 1.7, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 5.88f;
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

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
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
	vehicle->SetPos(0, 0.5f, 7);

	vehicle->GetTransform(&initial_car_matrix);
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION + 500;
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

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (vehicle->GetKmh() <= 0)
			acceleration = -MAX_ACCELERATION;
		else
			brake = BRAKE_POWER;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		red_off = false;
		blue_off = false;
		green_off = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && red_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Red_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
				
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
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}

		/*for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_Spheres_bodies.getFirst(); item; item = item->next)
		{
			btRigidBody* temp = item->data->GetRigidBody();
			temp->activate(true);
		}*/
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN && blue_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Blue_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		
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
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}		
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN && green_off == false)
	{
		for (p2List_item<PhysBody3D*>* item = App->scene_intro->Green_bodies.getFirst(); item; item = item->next)
			item->data->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

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
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 3:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_3rd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 2:
					for (p2List_item<PhysBody3D*>* item = App->scene_intro->US_2nd_road_bodies.getFirst(); item; item = item->next)
					{
						btRigidBody* temp = item->data->GetRigidBody();
						temp->activate(true);
					}
					break;
				case 1:
					break;
				}
			}
			act_road_num--;
		}
	}
	
	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		vehicle->SetPos(0, 0.5f, 7);
		vehicle->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		vehicle->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		vehicle->SetTransform(&initial_car_matrix);

		timer.Start();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;

		App->scene_intro->ResetSpheres();
	}

	vehicle->Render();

	if (victory == true)
	{
		LOG("VICTORY");
		timer.Stop();
		if (best_time == 0 || best_time > timer.Read())
			best_time = timer.Read();

		App->scene_intro->ResetSpheres();

		for (p2List_item<bool>* item = App->scene_intro->checkpoints_bools.getFirst(); item; item = item->next)
			item->data = false;

		victory = false;
	}

	char title[80];
	sprintf_s(title, "Speed: %.1f Km/h, Actual Time: %d, Best Time: %d", vehicle->GetKmh(), timer.Read() / 1000, best_time);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}



