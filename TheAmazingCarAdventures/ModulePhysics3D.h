#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2List.h"
#include "Primitive.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 
#define WALLS_HEIGHT 10

class DebugDrawer;
struct PhysBody3D;
struct PhysVehicle3D;
struct VehicleInfo;

//Directions:
//		 x		 NORTH
//		-x		 SOUTH
//		 z		 EAST
//		-z		 WEST
enum Direction
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};

enum obstacle_color
{
	RED,
	GREEN,
	BLUE,
	UNCOLORED
};

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	PhysBody3D* AddBody(const Sphere& sphere, float mass = 1.0f);
	PhysBody3D* AddBody(const Cube& cube, float mass = 1.0f);
	PhysBody3D* AddNonRigidBody(const Cube& cube, float mass);
	PhysBody3D* AddBody(const Cylinder& cylinder, float mass = 1.0f);
	PhysVehicle3D* AddVehicle(const VehicleInfo& info);

	void AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

	PhysBody3D* CreateRoad(float lenght, Direction dir, Direction prev_dir, int last_widht, int width, bool is_trap = false, obstacle_color color = UNCOLORED, float angle = 0);
	PhysBody3D* CreateWall(float height, float lenght, float width, int x, int y, int z, Direction dir, obstacle_color color, float angle = 0, vec3 axis = (0, 0, 0));
	PhysBody3D* CreateWall(Cube& cube,float height, float lenght, float width, int x, int y, int z, Direction dir, obstacle_color color, float angle = 0, vec3 axis = (0, 0, 0));
	PhysBody3D* CreateDemolitionBall(int x, int y, int z, float radius, obstacle_color color = UNCOLORED, uint road = 2, float mass = 1.0f);
	PhysBody3D* CreateWallSensor(float lenght, float width, int x, int y, int z, Direction dir);
	PhysBody3D* CreateDemolitionTrap(int x, int y, int z, float radius, obstacle_color color, uint road, uint num_spheres, float mass);

private:

	bool debug;

	btDefaultCollisionConfiguration*	collision_conf;
	btCollisionDispatcher*				dispatcher;
	btBroadphaseInterface*				broad_phase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld*			world;
	btDefaultVehicleRaycaster*			vehicle_raycaster;
	DebugDrawer*						debug_draw;

	p2List<btCollisionShape*> shapes;
	p2List<PhysBody3D*> bodies;
	p2List<btDefaultMotionState*> motions;
	p2List<btTypedConstraint*> constraints;
	p2List<PhysVehicle3D*> vehicles;
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0,0,0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	Line line;
	Primitive point;
};