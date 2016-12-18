#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"

#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for(int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if(numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			if(pbodyA && pbodyB)
			{
				p2List_item<Module*>* item = pbodyA->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyA, pbodyB);
					item = item->next;
				}

				item = pbodyB->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyB, pbodyA);
					item = item->next;
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(debug == true)
	{
		world->debugDrawWorld();

		// Render vehicles
		p2List_item<PhysVehicle3D*>* item = vehicles.getFirst();
		while(item)
		{
			item->data->Render();
			item = item->next;
		}

		if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			Sphere s(1);
			s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			float force = 30.0f;
			AddBody(s)->Push(-(App->camera->Z.x * force), -(App->camera->Z.y * force), -(App->camera->Z.z * force));
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for(p2List_item<btTypedConstraint*>* item = constraints.getFirst(); item; item = item->next)
	{
		world->removeConstraint(item->data);
		delete item->data;
	}
	
	constraints.clear();

	for(p2List_item<btDefaultMotionState*>* item = motions.getFirst(); item; item = item->next)
		delete item->data;

	motions.clear();

	for(p2List_item<btCollisionShape*>* item = shapes.getFirst(); item; item = item->next)
		delete item->data;

	shapes.clear();

	for(p2List_item<PhysBody3D*>* item = bodies.getFirst(); item; item = item->next)
		delete item->data;

	bodies.clear();

	for(p2List_item<PhysVehicle3D*>* item = vehicles.getFirst(); item; item = item->next)
		delete item->data;

	vehicles.clear();

	delete vehicle_raycaster;
	delete world;

	return true;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Sphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

PhysBody3D* ModulePhysics3D::AddNonRigidBody(const Cube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cylinder& cylinder, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysVehicle3D* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.add(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x*0.5f, info.chassis_size.y*0.5f, info.chassis_size.z*0.5f));
	shapes.add(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for(int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------

	PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.add(pvehicle);

	return pvehicle;
}

// ---------------------------------------------------------
void ModulePhysics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z), 
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.add(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z), 
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.add(hinge);
	hinge->setDbgDrawSize(2.0f);
}

//Map Creation Functions

PhysBody3D* ModulePhysics3D::CreateRoad(float lenght, Direction dir, Direction prev_dir, int last_width, int width, bool is_trap, obstacle_color color, float angle)
{
	PhysBody3D* ret;
	Cube cube;
	//NORTH------------------------------------
	if (dir == NORTH)
	{
		if (prev_dir == EAST)
		{
			App->scene_intro->actual_pos.x -= last_width / 2;
			App->scene_intro->actual_pos.z += width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x + ((lenght - last_width) / 2) + last_width, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width + 2, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, EAST, UNCOLORED);
		}
		else if (prev_dir == WEST)
		{
			App->scene_intro->actual_pos.x -= last_width / 2;
			App->scene_intro->actual_pos.z -= width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x + ((lenght - last_width) / 2) + last_width, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, WEST, UNCOLORED);
		}
		cube.size.Set(lenght, 1, width);
		App->scene_intro->actual_pos.x += lenght / 2;
		if (angle != 0 && prev_dir == NORTH)
		{
			if (angle >= 20)
				angle = 20;
			cube.SetPos(App->scene_intro->actual_pos.x - (lenght / 2) * (1 - cos(DEGTORAD * angle)), App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z);
			cube.SetRotation(angle, vec3(0, 0, 1));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED, angle, vec3(0,0,1));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED, angle, vec3(0, 0, 1));
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x + (lenght / 2) * (cos(DEGTORAD * angle)) - (lenght / 2)*(1 - cos(DEGTORAD*angle)), App->scene_intro->actual_pos.y + ((lenght)* sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z);
		}
		else if (is_trap == true && prev_dir == NORTH)
		{
			CreateWall(lenght - 1, width, 1, App->scene_intro->actual_pos.x - lenght / 2, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z, EAST, UNCOLORED);
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			Cube low_cube1;
			low_cube1.size.Set(lenght, 1, width);
			low_cube1.SetPos(App->scene_intro->actual_pos.x + (lenght / 2) * (1 - cos(DEGTORAD * 20)), App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght) / 2, App->scene_intro->actual_pos.z);
			low_cube1.SetRotation(20, vec3(0, 0, 1));
			AddBody(low_cube1, 0);
			Cube low_cube2;
			low_cube2.size.Set(lenght, 1, width);
			low_cube2.SetPos(App->scene_intro->actual_pos.x - lenght * (1 - cos(DEGTORAD * 20)), App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght), App->scene_intro->actual_pos.z);
			AddBody(low_cube2, 0);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z +1+ width / 2, dir, UNCOLORED);
			App->scene_intro->Cubes.add(low_cube1);
			App->scene_intro->Cubes.add(low_cube2);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x + lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
		}
		else
		{
			if (prev_dir == NORTH)
			{
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			}
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x + lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);

		}
	}
	//------------------------------------------------------------------------

	//SOUTH------------------------------------
	else if (dir == SOUTH)
	{
		if (prev_dir == EAST)
		{
			App->scene_intro->actual_pos.x += last_width / 2;
			App->scene_intro->actual_pos.z += width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x - ((lenght - last_width) / 2) - last_width, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width + 2, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, EAST, UNCOLORED);
		}
		else if (prev_dir == WEST)
		{
			App->scene_intro->actual_pos.x += last_width / 2;
			App->scene_intro->actual_pos.z -= width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x - ((lenght - last_width) / 2) - last_width, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + width / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, WEST, UNCOLORED);
		}
		cube.size.Set(lenght, 1, width);
		App->scene_intro->actual_pos.x -= lenght / 2;
		if (angle != 0 && prev_dir == SOUTH)
		{
			if (angle >= 20)
				angle = 20;
			cube.SetPos(App->scene_intro->actual_pos.x + (lenght / 2) * (1 - cos(DEGTORAD * angle)), App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z);
			cube.SetRotation(-angle, vec3(0, 0, 1));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED, -angle, vec3(0, 0, 1));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED, -angle, vec3(0, 0, 1));
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x - (lenght / 2) * (cos(DEGTORAD * angle)) + (lenght / 2)*(1 - cos(DEGTORAD*angle)), App->scene_intro->actual_pos.y + ((lenght)* sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z);
		}
		else if (is_trap == true && prev_dir == SOUTH)
		{
			CreateWall(lenght, width, 1, App->scene_intro->actual_pos.x + lenght / 2, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z, EAST, UNCOLORED);
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			Cube low_cube1;
			low_cube1.size.Set(lenght, 1, width);
			low_cube1.SetPos(App->scene_intro->actual_pos.x - (lenght / 2) * (1 - cos(DEGTORAD * 20)), App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght) / 2, App->scene_intro->actual_pos.z);
			low_cube1.SetRotation(-20, vec3(0, 0, 1));
			AddBody(low_cube1, 0);
			Cube low_cube2;
			low_cube2.size.Set(lenght, 1, width);
			low_cube2.SetPos(App->scene_intro->actual_pos.x + lenght * (1 - cos(DEGTORAD * 20)), App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght), App->scene_intro->actual_pos.z);
			AddBody(low_cube2, 0);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
			App->scene_intro->Cubes.add(low_cube1);
			App->scene_intro->Cubes.add(low_cube2);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x - lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
		}
		else 
		{
			if (prev_dir == SOUTH)
			{
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 + width / 2, dir, UNCOLORED);
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - width / 2, dir, UNCOLORED);
			}
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x - lenght / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
		}
	}
	//------------------------------------------------------------------------

	//EAST------------------------------------
	else if (dir == EAST)
	{
		if (prev_dir == NORTH)
		{
			App->scene_intro->actual_pos.x += width / 2;
			App->scene_intro->actual_pos.z -= last_width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + lenght / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + ((lenght - last_width) / 2) + last_width, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, NORTH, UNCOLORED);
		}
		else if (prev_dir == SOUTH)
		{
			App->scene_intro->actual_pos.x -= width / 2;
			App->scene_intro->actual_pos.z -= last_width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + lenght / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + ((lenght - last_width) / 2) + last_width, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, SOUTH, UNCOLORED);
		}

		cube.size.Set(width, 1, lenght);
		App->scene_intro->actual_pos.z += lenght / 2;
		if (angle != 0 && prev_dir == EAST)
		{
			if (angle >= 20)
				angle = 20;
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z - (lenght / 2) * (1 - cos(DEGTORAD * angle)));
			cube.SetRotation(-angle, vec3(1, 0, 0));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z, dir, UNCOLORED, -angle, vec3(1, 0, 0));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z, dir, UNCOLORED, -angle, vec3(1, 0, 0));
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght)* sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z + (lenght / 2) * (cos(DEGTORAD * angle)) - (lenght / 2)*(1 - cos(DEGTORAD*angle)));
		}
		
		else if (is_trap == true && prev_dir == EAST)
		{
			CreateWall(lenght, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght/2, App->scene_intro->actual_pos.z - lenght/2, NORTH, UNCOLORED);
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			Cube low_cube1;
			low_cube1.size.Set(width, 1, lenght);
			low_cube1.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght) / 2, App->scene_intro->actual_pos.z + (lenght/2) * (1 - cos(DEGTORAD * 20)));
			low_cube1.SetRotation(-20, vec3(1, 0, 0));
			AddBody(low_cube1, 0);
			Cube low_cube2;
			low_cube2.size.Set(width, 1, lenght);
			low_cube2.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght), App->scene_intro->actual_pos.z - lenght * (1 - cos(DEGTORAD * 20)));
			AddBody(low_cube2, 0);
			CreateWall(lenght+10, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y-lenght/2, App->scene_intro->actual_pos.z, dir, UNCOLORED);
			CreateWall(lenght+10, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y-lenght/2, App->scene_intro->actual_pos.z, dir, UNCOLORED); 
			App->scene_intro->Cubes.add(low_cube1);
			App->scene_intro->Cubes.add(low_cube2);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + lenght / 2);
		}
		else
		{
			if (prev_dir == EAST)
			{
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, dir, UNCOLORED);
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, dir, UNCOLORED);
			}
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + lenght / 2);
		}
	}
	//------------------------------------------------------------------------

	//WEST------------------------------------
	else if (dir == WEST)
	{
		if (prev_dir == NORTH)
		{
			App->scene_intro->actual_pos.x += width / 2;
			App->scene_intro->actual_pos.z += last_width / 2;
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - lenght / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - ((lenght - last_width) / 2) - last_width, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1, NORTH, UNCOLORED);
		}
		else if (prev_dir == SOUTH)
		{
			App->scene_intro->actual_pos.x -= width / 2;
			App->scene_intro->actual_pos.z += last_width / 2;
			CreateWall(WALLS_HEIGHT, lenght + 2, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z + 1 - lenght / 2, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, lenght - last_width, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - ((lenght - last_width) / 2) - last_width, dir, UNCOLORED);
			CreateWall(WALLS_HEIGHT, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z+1, SOUTH, UNCOLORED);
		}
		cube.size.Set(width, 1, lenght);
		App->scene_intro->actual_pos.z -= lenght / 2;

		if (angle != 0 && prev_dir == WEST)
		{
			if (angle >= 20)
				angle = 20;
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z + (lenght / 2) * (1 - cos(DEGTORAD * angle)));
			cube.SetRotation(angle, vec3(1, 0, 0));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z, dir, UNCOLORED, angle, vec3(1, 0, 0));
			CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y + ((lenght / 2) * sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z, dir, UNCOLORED, angle, vec3(1, 0, 0));
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y + ((lenght)* sin(DEGTORAD*angle)), App->scene_intro->actual_pos.z - (lenght / 2) * (cos(DEGTORAD * angle)) + (lenght / 2)*(1 - cos(DEGTORAD*angle)));
		}

		else if (is_trap == true && prev_dir == WEST)
		{
			CreateWall(lenght, width, 1, App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z + lenght / 2, NORTH, UNCOLORED);
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			Cube low_cube1;
			low_cube1.size.Set(width, 1, lenght);
			low_cube1.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght) / 2, App->scene_intro->actual_pos.z - (lenght / 2) * (1 - cos(DEGTORAD * 20)));
			low_cube1.SetRotation(20, vec3(1, 0, 0));
			AddBody(low_cube1, 0);
			Cube low_cube2;
			low_cube2.size.Set(width, 1, lenght);
			low_cube2.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y - (sin(DEGTORAD * 20) * lenght), App->scene_intro->actual_pos.z + lenght * (1 - cos(DEGTORAD * 20)));
			AddBody(low_cube2, 0);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z, dir, UNCOLORED);
			CreateWall(lenght + 10, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y - lenght / 2, App->scene_intro->actual_pos.z, dir, UNCOLORED);
			App->scene_intro->Cubes.add(low_cube1);
			App->scene_intro->Cubes.add(low_cube2);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - lenght / 2);
		}
		else
		{
			if (prev_dir == WEST)
			{
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x + width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, dir, UNCOLORED);
				CreateWall(WALLS_HEIGHT, lenght, 1, App->scene_intro->actual_pos.x - width / 2, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z, dir, UNCOLORED);
			}
			cube.SetPos(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z);
			App->scene_intro->actual_pos.Set(App->scene_intro->actual_pos.x, App->scene_intro->actual_pos.y, App->scene_intro->actual_pos.z - lenght / 2);
		}
	}
	//------------------------------------------------------------------------

	ret = AddBody(cube, 0);
	if (color == UNCOLORED)
		App->scene_intro->Cubes.add(cube);
	else if (color == RED)
	{
		App->scene_intro->Red_Cubes.add(cube);
		App->scene_intro->Red_bodies.add(ret);
	}
	else if (color == GREEN)
	{
		App->scene_intro->Green_Cubes.add(cube);
		App->scene_intro->Green_bodies.add(ret);
	}
	else if (color == BLUE)
	{
		App->scene_intro->Blue_Cubes.add(cube);
		App->scene_intro->Blue_bodies.add(ret);
	}
	return ret;
}

PhysBody3D* ModulePhysics3D::CreateWall(float height, float lenght, float width, int x, int y, int z, Direction dir, obstacle_color color, float angle, vec3 axis)
{
	PhysBody3D* ret;
	Cube cube;

	if (dir == NORTH || dir == SOUTH)
		cube.size.Set(lenght, height, width);
	else if (dir == EAST || dir == WEST)
		cube.size.Set(width, height, lenght);

	cube.SetPos(x, y, z);
	if (angle != 0)
		cube.SetRotation(angle, axis);

	switch (color)
	{
	case RED:
		App->scene_intro->Red_Cubes.add(cube);
		break;
	case GREEN:
		App->scene_intro->Green_Cubes.add(cube);
		break;
	case BLUE:
		App->scene_intro->Blue_Cubes.add(cube);
		break;
	case UNCOLORED:
		App->scene_intro->Uncolored_Cubes.add(cube);
		break;
	}

	ret = AddBody(cube, 0);
	if (color == RED)
		App->scene_intro->Red_bodies.add(ret);
	else if (color == BLUE)
		App->scene_intro->Blue_bodies.add(ret);
	else if (color == GREEN)
		App->scene_intro->Green_bodies.add(ret);

	return ret;
}

PhysBody3D * ModulePhysics3D::CreateDemolitionBall(int x, int y, int z, float radius, float mass, obstacle_color color)
{
	PhysBody3D* ret;

	Sphere sphere(radius);	
	sphere.SetPos(x, y, z);

	/*
			RED		balls over		BLUE	cubes
			BLUE	balls over		GREEN	cubes
			GREEN	balls over		RED		cubes
	*/

	if (color == RED)
		CreateWall(radius * 2, radius * 2, 1, x, y - radius - 0.5f, z, NORTH, BLUE, 90, vec3(1, 0, 0));
	if (color == BLUE)
		CreateWall(radius * 2, radius * 2, 1, x, y - radius - 0.5f, z, NORTH, GREEN, 90, vec3(1, 0, 0));
	if (color == GREEN)
		CreateWall(radius * 2, radius * 2, 1, x, y - radius - 0.5f, z, NORTH, RED, 90, vec3(1, 0, 0));

	switch (color)
	{
	case RED:
		App->scene_intro->Red_Spheres.add(sphere);
		App->scene_intro->Red_Spheres_positions.add(vec3(x, y, z));
		break;
	case GREEN:
		App->scene_intro->Green_Spheres.add(sphere);
		App->scene_intro->Green_Spheres_positions.add(vec3(x, y, z));
		break;
	case BLUE:
		App->scene_intro->Blue_Spheres.add(sphere);
		App->scene_intro->Blue_Spheres_positions.add(vec3(x, y, z));
		break;
	case UNCOLORED:
		App->scene_intro->Uncolored_Spheres.add(sphere);
		break;
	}

	ret = AddBody(sphere, mass);
	if (color == RED)
		App->scene_intro->Red_Spheres_bodies.add(ret);
	else if (color == BLUE)
		App->scene_intro->Blue_Spheres_bodies.add(ret);
	else if (color == GREEN)
		App->scene_intro->Green_Spheres_bodies.add(ret);

	return ret;
}

PhysBody3D * ModulePhysics3D::CreateWallSensor(float lenght, float width, int x, int y, int z, Direction dir)
{
	PhysBody3D* ret;
	Cube cube;

	if (dir == NORTH || dir == SOUTH)
		cube.size.Set(lenght, 3, width);
	else if (dir == EAST || dir == WEST)
		cube.size.Set(width, 3, lenght);

	cube.SetPos(x, y, z);
	ret = AddNonRigidBody(cube, 0);
	return ret;
}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
