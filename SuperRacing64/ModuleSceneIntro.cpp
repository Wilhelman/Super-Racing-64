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

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	// Deleting Roads
	for (p2List_item<Cube*>* road_item = roads_list.getFirst(); road_item; road_item = road_item->next)
		delete road_item->data;

	roads_list.clear();

	// Deleting Walls
	for (p2List_item<Cube*>* wall_item = walls_list.getFirst(); wall_item; wall_item = wall_item->next)
		delete wall_item->data;

	walls_list.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	RenderRoads();
	RenderWalls();


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

void ModuleSceneIntro::RenderRoads() const
{
	for (p2List_item<Cube*>* road_item = roads_list.getFirst(); road_item; road_item = road_item->next)
		road_item->data->Render();
}

void ModuleSceneIntro::RenderWalls() const
{
	for (p2List_item<Cube*>* wall_item = walls_list.getFirst(); wall_item; wall_item = wall_item->next)
		wall_item->data->Render();
}

void ModuleSceneIntro::AddRoad(float length, RoadType road_type)
{
	Cube* road_segment = nullptr;

	if (roads_list.count() == 0) // First road
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetPos(0.0f, ROAD_HEIGHT / 2.0f, 10.0f);
		last_road_type = FORWARD_ROAD;
	}
	else // Roads after the first one
	{
		Cube* last_cube = roads_list.getLast()->data;

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
			road_segment = BuildForawardRamp(last_cube, length, X_AXIS);
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
		case NOT_DEF_ROAD:
			break;
		default:
			break;
		}
	}
	App->physics->AddBody(*road_segment, STATIC_MASS);
	roads_list.add(road_segment);
}

Cube * ModuleSceneIntro::BuildForawardRoad(Cube* last_cube, float length)
{ 
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == RIGHT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == LEFT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + (length / 2 - last_cube->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == FORWARD_RAMP)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - 1.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == RIGHT_RAMP)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = FORWARD_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildBackwardRoad(Cube * last_cube, float length)
{
	Cube* road_segment = nullptr;

	if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == RIGHT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == LEFT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - (length / 2 - last_cube->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == BACKWARD_RAMP)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == FORWARD_RAMP)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		vec3 pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = BACKWARD_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildLeftRoad(Cube * last_cube, float length)
{
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == LEFT_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x + length / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = LEFT_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildRightRoad(Cube * last_cube, float length)
{
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == RIGHT_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x - length / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		vec3 pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	last_road_type = RIGHT_ROAD;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildForawardRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length/3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
		
	}
	else if (last_road_type == RIGHT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == LEFT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);		
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);

	}

	last_road_type = FORWARD_RAMP;

	return road_segment;
} //done

Cube * ModuleSceneIntro::BuildBackwardRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = nullptr;

	if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == RIGHT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);																														
	}
	else if (last_road_type == LEFT_ROAD)
	{
		road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
		road_segment->SetRotation(30.0f, axis);
		vec3 pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = BACKWARD_RAMP;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildLeftRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		road_segment->SetRotation(30.0f, axis);
		vec3 pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 + last_cube->size.x / 2 - length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + (last_cube->size.z / 2 - road_segment->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		road_segment->SetRotation(30.0f, axis);
		vec3 pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 + last_cube->size.x / 2 - length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - (last_cube->size.z / 2 - road_segment->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = LEFT_RAMP;

	return road_segment;
}

Cube * ModuleSceneIntro::BuildRightRamp(Cube * last_cube, float length, vec3 axis)
{
	Cube* road_segment = nullptr;

	if (last_road_type == FORWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 - last_cube->size.x / 2 + length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + (last_cube->size.z / 2 - road_segment->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}
	else if (last_road_type == BACKWARD_ROAD)
	{
		road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
		road_segment->SetRotation(-30.0f, axis);
		vec3 pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 - last_cube->size.x / 2 + length / 17.0f, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - (last_cube->size.z / 2 - road_segment->size.z / 2));
		road_segment->SetPos(pos.x, pos.y, pos.z);
	}

	last_road_type = RIGHT_RAMP;

	return road_segment;
}

void ModuleSceneIntro::BuildCircuit_1()
{
	AddRoad(32.0f, FORWARD_ROAD);
	AddRoad(64.0f, LEFT_ROAD);
	AddRoad(40.0f, FORWARD_ROAD);
	AddRoad(104.0f, LEFT_ROAD);
	AddRoad(80.0f, BACKWARD_ROAD);
	AddRoad(54.0f, RIGHT_ROAD);
	AddRoad(32.0f, FORWARD_ROAD);
	AddRoad(48.0f, RIGHT_ROAD);
	AddRoad(24.0f, BACKWARD_RAMP);
	AddRoad(40.0f, BACKWARD_ROAD);
	AddRoad(24.0f, FORWARD_RAMP);
	AddRoad(16.0F, BACKWARD_ROAD);
	AddRoad(88.0f, LEFT_ROAD);
	AddRoad(38.0f, FORWARD_ROAD);
	AddRoad(184.0f, RIGHT_ROAD);
	AddRoad(24.6f, FORWARD_ROAD);
}


void ModuleSceneIntro::CreateWalls(Cube * road, vec3 position)
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