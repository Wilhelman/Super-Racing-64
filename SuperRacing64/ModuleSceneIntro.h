#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define ROAD_HEIGHT 1.0f
#define ROAD_WIDTH 10.0f
#define WALL_WIDTH 1.0f
#define WALL_HEIGHT 3.0F

#define STATIC_MASS 0.0f

#define X_AXIS {1.0f, 0.0f, 0.0f}
#define Y_AXIS {0.0f, 1.0f, 0.0f}
#define Z_AXIS {0.0f, 0.0f, 1.0f}

struct PhysBody3D;
struct PhysMotor3D;

enum ConstructionDirection
{
	FORWARD_DIRECTION,
	BACKWARD_DIRECTION,
	RIGHT_DIRECTION,
	LEFT_DIRECTION,
	FORWARD_RAMP,
	BACKWARD_RAMP,
	RIGHT_RAMP,
	LEFT_RAMP,

	DIRECTION_NOT_DEF
};

enum ConstructionType
{
	CIRCUIT_1,
	CIRCUIT_2,
	WALLS,

	CIRCUIT_NOT_DEF
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
	void AddConstruction(float length, ConstructionDirection road_type, ConstructionType construction_type, bool isWall = false);

	Cube* BuildForward(Cube* last_cube, float length, bool isWall = false);
	Cube* BuildBackward(Cube* last_cube, float length, bool isWall = false);
	Cube* BuildLeft(Cube* last_cube, float length, bool isWall = false);
	Cube* BuildRight(Cube* last_cube, float length, bool isWall = false);
	Cube* BuildForwardRamp(Cube* last_cube, float length, vec3 axis, bool isWall = false);
	Cube* BuildBackwardRamp(Cube* last_cube, float length, vec3 axis, bool isWall = false);
	//Cube* BuildLeftRamp(Cube* last_cube, float length, vec3 axis, bool isWall = false);
	//Cube* BuildRightRamp(Cube* last_cube, float length, vec3 axis, bool isWall = false);

	void BuildCircuit_1();
	void BuildCircuit_2();
	void BuildWalls();

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
	p2List<Cube*> roads_circuit_1;
	p2List<Cube*> roads_circuit_2;

	//Walls
	p2List<Cube*> walls_list;

	ConstructionDirection last_direction = DIRECTION_NOT_DEF;
};
