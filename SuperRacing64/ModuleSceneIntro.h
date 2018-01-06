#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define ROAD_HEIGHT 1.0f
#define ROAD_WIDTH 10.0f
#define WALL_HEIGHT 5.0f
#define WALL_WIDTH 2.0f

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

	ROAD_NOT_DEF
};

enum Circuit
{
	CIRCUIT_1,
	CIRCUIT_2,

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

	void AddRoad(float length, RoadType road_type, Circuit circuit);
	Cube* BuildForawardRoad(Cube* last_cube, float length);
	Cube* BuildBackwardRoad(Cube* last_cube, float length);
	Cube* BuildLeftRoad(Cube* last_cube, float length);
	Cube* BuildRightRoad(Cube* last_cube, float length);
	Cube* BuildForwardRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildBackwardRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildLeftRamp(Cube* last_cube, float length, vec3 axis);
	Cube* BuildRightRamp(Cube* last_cube, float length, vec3 axis);

	void CreateWall(float x, float y, float z, float length, RoadType wall_type);
	Cube* BuildWall(float x, float y, float z, float length, float width, float height);
	void SetUpWalls();

	void BuildCircuit_1();
	void BuildCircuit_2();

	void RenderRoads() const;
	void RenderWalls() const;

public:
	int current_players = 0;
	bool p1_HasFallen = false;
	bool p2_HasFallen = false;
	bool race_started = false;
	bool countdown_played = false;

	uint countdown_fx = 0u;

	PhysBody3D* pb_chassis = nullptr;
	Cube p_chassis;

	PhysBody3D* pb_wheel = nullptr;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2 = nullptr;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel = nullptr;
	PhysMotor3D* right_wheel = nullptr;

	//Roads
	p2List<Cube*> roads_circuit_1;
	p2List<Cube*> roads_circuit_2;

	//Walls
	p2List<Cube*> walls_list;

	RoadType last_road_type = ROAD_NOT_DEF;

	//Start sensor
	PhysBody3D* start_sensor = nullptr;

	//Sensors circuit 1
	PhysBody3D* second_sensor_c1 = nullptr;
	PhysBody3D* third_sensor_c1 = nullptr;
	PhysBody3D* fourth_sensor_c1 = nullptr;

	//Sensor circuit 2
	PhysBody3D* second_sensor_c2 = nullptr;
	PhysBody3D* third_sensor_c2 = nullptr;
	PhysBody3D* fourth_sensor_c2 = nullptr;
	PhysBody3D* ground_sensor = nullptr;

	Timer timer_to_beat;

private:
	int did_player_loose = 0; //0 -> not_played, 1 -> Player01 win, 2 -> Player02 win, 3 -> Lose
};