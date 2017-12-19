#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

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

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Roads
	BuildCircuit_1();
	BuildCircuit_2();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	// Deleting Roads
	for (p2List_item<Cube*>* road_item = roads_circuit_1.getFirst(); road_item; road_item = road_item->next)
	{
		delete road_item->data;
		road_item->data = nullptr;
	}

	roads_circuit_1.clear();

	// Deleting Walls
	for (p2List_item<Cube*>* wall_item = walls_list.getFirst(); wall_item; wall_item = wall_item->next)
	{
		delete wall_item->data;
		wall_item->data = nullptr;
	}

	walls_list.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Cube ground( 1000, 1, 1000);
	ground.axis = true;
	ground.color = Green;
	ground.Render();

	RenderRoads();
	RenderWalls();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

void ModuleSceneIntro::RenderRoads() const
{
	for (p2List_item<Cube*>* road_item = roads_circuit_1.getFirst(); road_item; road_item = road_item->next)
		road_item->data->Render();
	for (p2List_item<Cube*>* road_item = roads_circuit_2.getFirst(); road_item; road_item = road_item->next)
		road_item->data->Render();
}

void ModuleSceneIntro::RenderWalls() const
{
	for (p2List_item<Cube*>* wall_item = walls_list.getFirst(); wall_item; wall_item = wall_item->next)
		wall_item->data->Render();
}

void ModuleSceneIntro::AddRoad(float length, RoadType road_type, Circuit circuit)
{
	Cube* road_segment = nullptr;

	if (roads_circuit_1.count() == 0 && circuit == CIRCUIT_1) // First road CIRCUIT 1
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetPos(0.0f, ROAD_HEIGHT / 2.0f, 10.0f);
		last_road_type = FORWARD_ROAD;
	}
	else if (roads_circuit_2.count() == 0 && circuit == CIRCUIT_2) // First road CIRCUIT 2
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		road_segment->SetPos(-21.0f, ROAD_HEIGHT / 2.0f, 31.0f);
		last_road_type = RIGHT_ROAD;
	}
	else // Roads after the first one
	{
		Cube* last_cube = nullptr;

		if (circuit == CIRCUIT_1)
			last_cube = roads_circuit_1.getLast()->data;
		else
			last_cube = roads_circuit_2.getLast()->data;

		switch (road_type)
		{
		case FORWARD_ROAD:
			road_segment = BuildForawardRoad(last_cube, length);
			break;
		case BACKWARD_ROAD:
			road_segment = BuildBackwardRoad(last_cube, length);
			break;
		case RIGHT_ROAD:
			road_segment = BuildRightRoad(last_cube, length);
			break;
		case LEFT_ROAD:
			road_segment = BuildLeftRoad(last_cube, length);
			break;
		case FORWARD_RAMP:
			road_segment = BuildForwardRamp(last_cube, length, X_AXIS);
			break;
		case BACKWARD_RAMP:
			road_segment = BuildBackwardRamp(last_cube, length, X_AXIS);
			break;
		case RIGHT_RAMP:
			road_segment = BuildRightRamp(last_cube, length, Z_AXIS);
			break;
		case LEFT_RAMP:
			road_segment = BuildLeftRamp(last_cube, length, Z_AXIS);
			break;
		case ROAD_NOT_DEF:
			break;
		default:
			break;
		}
	}

	road_segment->color = Grey;
	App->physics->AddBody(*road_segment, STATIC_MASS);

	if (circuit == CIRCUIT_1)
		roads_circuit_1.add(road_segment);
	else
		roads_circuit_2.add(road_segment);
}

Cube * ModuleSceneIntro::BuildForawardRoad(Cube* last_cube, float length)
{ 
	Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length); 
	vec3 pos(0.0f, 0.0f, 0.0f);

	// TODO: do a switch
	if (last_road_type == FORWARD_ROAD)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
	else if (last_road_type == RIGHT_ROAD)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
	else if (last_road_type == LEFT_ROAD)
		pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + (length / 2 - last_cube->size.z / 2));
	else if (last_road_type == FORWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - 1.0f);
	else if (last_road_type == RIGHT_RAMP)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);

	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = FORWARD_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildBackwardRoad(Cube * last_cube, float length)
{
	Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_road_type == BACKWARD_ROAD)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
	else if (last_road_type == RIGHT_ROAD)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
	else if (last_road_type == LEFT_ROAD)
		pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - (length / 2 - last_cube->size.z / 2));
	else if (last_road_type == BACKWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);
	else if (last_road_type == FORWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);

	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = BACKWARD_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildLeftRoad(Cube * last_cube, float length)
{
	Cube* road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_road_type == FORWARD_ROAD)
		pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
	else if (last_road_type == LEFT_ROAD)
		pos = vec3(last_cube->GetPos().x + length / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
	else if (last_road_type == BACKWARD_ROAD)
		pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
	
	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = LEFT_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildRightRoad(Cube * last_cube, float length)
{
	Cube* road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_road_type == FORWARD_ROAD)
		pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
	else if (last_road_type == RIGHT_ROAD)
		pos = vec3(last_cube->GetPos().x - length / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
	else if (last_road_type == BACKWARD_ROAD)
		pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
		
	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = RIGHT_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildForwardRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
	vec3 pos(0.0f, 0.0f, 0.0f);
	road_segment->SetRotation(-30.0f, axis);

	if (last_road_type == FORWARD_ROAD)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length/3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
	else if (last_road_type == RIGHT_ROAD)
		pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
	else if (last_road_type == LEFT_ROAD)
		pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);		
	else if (last_road_type == BACKWARD_ROAD)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);

	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = FORWARD_RAMP;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildBackwardRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
	vec3 pos(0.0f, 0.0f, 0.0f);
	road_segment->SetRotation(30.0f, axis);

	if (last_road_type == BACKWARD_ROAD)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	else if (last_road_type == RIGHT_ROAD)
		pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	else if (last_road_type == LEFT_ROAD)
		pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	
	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = BACKWARD_RAMP;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildLeftRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
	vec3 pos(0.0f, 0.0f, 0.0f);
	road_segment->SetRotation(30.0f, axis);

	if (last_road_type == FORWARD_ROAD)
		pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 + last_cube->size.x / 2 - length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + (last_cube->size.z / 2 - road_segment->size.z / 2));
	else if (last_road_type == BACKWARD_ROAD)
		pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 + last_cube->size.x / 2 - length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - (last_cube->size.z / 2 - road_segment->size.z / 2));

	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = LEFT_RAMP;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildRightRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
	vec3 pos(0.0f, 0.0f, 0.0f);
	road_segment->SetRotation(-30.0f, axis);

	if (last_road_type == FORWARD_ROAD)
		pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 - last_cube->size.x / 2 + length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + (last_cube->size.z / 2 - road_segment->size.z / 2));
	else if (last_road_type == BACKWARD_ROAD)
		pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 - last_cube->size.x / 2 + length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - (last_cube->size.z / 2 - road_segment->size.z / 2));
	else if (last_road_type == RIGHT_ROAD)
		pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 - last_cube->size.x / 2 + length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - (last_cube->size.z / 2 - road_segment->size.z / 2));
	
	road_segment->SetPos(pos.x, pos.y, pos.z);
	last_road_type = RIGHT_RAMP;

	return road_segment;
}

void ModuleSceneIntro::BuildCircuit_1()
{
	AddRoad(32.0f, FORWARD_ROAD, CIRCUIT_1);
	AddRoad(64.0f, LEFT_ROAD, CIRCUIT_1);
	AddRoad(40.0f, FORWARD_ROAD, CIRCUIT_1);
	AddRoad(104.0f, LEFT_ROAD, CIRCUIT_1);
	AddRoad(80.0f, BACKWARD_ROAD, CIRCUIT_1);
	AddRoad(54.0f, RIGHT_ROAD, CIRCUIT_1);
	AddRoad(32.0f, FORWARD_ROAD, CIRCUIT_1);
	AddRoad(48.0f, RIGHT_ROAD, CIRCUIT_1);
	AddRoad(24.0f, BACKWARD_RAMP, CIRCUIT_1);
	AddRoad(40.0f, BACKWARD_ROAD, CIRCUIT_1);
	AddRoad(24.0f, FORWARD_RAMP, CIRCUIT_1);
	AddRoad(16.0F, BACKWARD_ROAD, CIRCUIT_1);
	AddRoad(88.0f, LEFT_ROAD, CIRCUIT_1);
	AddRoad(38.0f, FORWARD_ROAD, CIRCUIT_1);
	AddRoad(184.0f, RIGHT_ROAD, CIRCUIT_1);
	AddRoad(24.6f, FORWARD_ROAD, CIRCUIT_1);
}

void ModuleSceneIntro::BuildCircuit_2()
{
	AddRoad(48.0f, RIGHT_ROAD, CIRCUIT_2);
	AddRoad(32.0f, BACKWARD_ROAD, CIRCUIT_2);
	AddRoad(24.0f, BACKWARD_RAMP, CIRCUIT_2);
	AddRoad(48.0f, BACKWARD_ROAD, CIRCUIT_2);
	AddRoad(40.0f, RIGHT_ROAD, CIRCUIT_2);
	AddRoad(48.0f, BACKWARD_ROAD, CIRCUIT_2);
	AddRoad(96.0f, LEFT_ROAD, CIRCUIT_2);
	AddRoad(24.0f, BACKWARD_ROAD, CIRCUIT_2);
	AddRoad(150.0f, LEFT_ROAD, CIRCUIT_2);
	AddRoad(80.0f, FORWARD_ROAD, CIRCUIT_2);
	AddRoad(10.0f, FORWARD_RAMP, CIRCUIT_2);
}

void ModuleSceneIntro::CreateWalls(Cube * road, vec3 position) // TODO: to be deleted proabably
{
	if (road != nullptr)
	{
		int num_cubes = road->size.z / 2.0f;
		int cubes_setted = 0;
		int cube_offset = 0;

		vec3 bottom_left(position.x + road->size.x / 2, position.y, position.z - road->size.z / 2);
		vec3 bottom_right(position.x - road->size.x / 2, position.y, position.z - road->size.z / 2);

		while (cubes_setted < num_cubes)
		{
			Cube* cube_left = new Cube(1.0f, 3.0f, 2.0f);

			if (cubes_setted % 2 == 0)
				cube_left->color = Red;
			else
				cube_left->color = White;

			cube_left->SetPos(bottom_left.x + cube_left->size.x / 2, bottom_left.y + cube_left->size.y / 3, bottom_left.z + cube_left->size.z / 2 + cube_offset);
			
			App->physics->AddBody(*cube_left, STATIC_MASS);
			walls_list.add(cube_left);

			Cube* cube_right = new Cube(1.0f, 3.0f, 2.0f);

			if (cubes_setted % 2 == 0)
				cube_right->color = Red;
			else
				cube_right->color = White;

			cube_right->SetPos(bottom_right.x - cube_left->size.x / 2, bottom_right.y + cube_left->size.y / 3, bottom_right.z + cube_left->size.z / 2 + cube_offset);

			App->physics->AddBody(*cube_right, STATIC_MASS);
			walls_list.add(cube_right);

			cubes_setted++;
			cube_offset += cube_right->size.z;
		}
	}
} 