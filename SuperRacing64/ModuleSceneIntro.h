#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define ROAD_HEIGHT 1.0f
#define ROAD_WIDTH 10.0f

#define STATIC_MASS 0.0f

#define X_AXIS {1.0f, 0.0f, 0.0f}
#define Y_AXIS {0.0f, 1.0f, 0.0f}
#define Z_AXIS {0.0f, 0.0f, 1.0f}

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

	void CreateStraightRoadSegment(vec3 position, float length, float angle = 0.0f, vec3 axis = { 0.0f, 0.0f, 0.0f });
	void CreateInclinedRoadSegment(vec3 position, float length, float angle_1 = 0.0f, vec3 axis_1 = { 0.0f, 0.0f, 0.0f }, float angle_2 = 0.0f, vec3 axis_2 = Z_AXIS);
	void CreateWalls(Cube* road, vec3 position);
	void RotateRoadSegment(Cube* road, float angle, vec3 axis);

	void RenderRoads() const;
	void RenderWalls() const;

public:

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	//Roads
	p2List<Cube*> roads_list;

	//Walls
	p2List<Cube*> walls_list;
};
