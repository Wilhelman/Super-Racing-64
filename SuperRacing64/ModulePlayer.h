#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1500.0f
#define BACK_ACCELERATION -500.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	PhysBody3D* last_sensor = nullptr;
	int laps = 2;
	Timer p_timer;

	PhysVehicle3D* vehicle = nullptr;
	float turn = 0.0f;
	float acceleration = 0.0f;
	float brake = 0.0f;

	uint acceleration_fx = 0u;

	bool p1_running = false;
};