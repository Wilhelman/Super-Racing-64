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

enum RoadType
{
	FORWARD_ROAD,
	BACKWARD_ROAD,
	RIGHT_ROAD,
	LEFT_ROAD,
	FORWARD_RAMP,
	BACKWARD_RAMP,
	RIGHT_RAMP,
	LEFT_RAMP,

	NOT_DEF_ROAD
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);


	void CreateWalls(Cube* road, vec3 position);

	void AddRoad(float length, RoadType road_type);
	Cube* BuildForawardRoad(Cube* last_cube, float length);
	Cube* BuildBackwardRoad(Cube* last_cube, float length);
	Cube* BuildLeftRoad(Cube* last_cube, float length);
	Cube* BuildRightRoad(Cube* last_cube, float length);
	Cube* BuildForawardRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildBackwardRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildLeftRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildRightRamp(Cube* last_cube, float length, vec3 axis);

	void BuildCircuit_1();

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

	RoadType last_road_type = NOT_DEF_ROAD;
};
