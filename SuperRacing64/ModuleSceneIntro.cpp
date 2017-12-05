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
	AddRoad(30.0f, FORWARD_ROAD);
	AddRoad(50.0f, LEFT_ROAD);
	AddRoad(30.0f, BACKWARD_ROAD);
	AddRoad(30.0f, LEFT_ROAD);

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
	if (roads_list.count() == 0) // First road
	{
		Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, 30.0f);
		road_segment->SetPos(0.0f, 1.0f, 10.0f);
		PhysBody3D* road_segment_body = App->physics->AddBody(*road_segment, STATIC_MASS);
		roads_list.add(road_segment);
		last_road_type = FORWARD_ROAD;
	}
	else
	{
		Cube* last_cube = roads_list.getLast()->data;
		vec3 pos;
		Cube* road_segment;

		switch (road_type)
		{
		case FORWARD_ROAD:
			if (last_road_type == FORWARD_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == RIGHT_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == LEFT_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + (length / 2 - last_cube->size.z / 2));
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			last_road_type = FORWARD_ROAD;
			break;
		case BACKWARD_ROAD:
			if (last_road_type == BACKWARD_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == RIGHT_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - road_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == LEFT_ROAD)
			{
				road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
				pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + road_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - (length / 2 - last_cube->size.z / 2));
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			last_road_type = BACKWARD_ROAD;
			break;
		case RIGHT_ROAD:
			if (last_road_type == FORWARD_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == RIGHT_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x - length / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == BACKWARD_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x - road_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			last_road_type = RIGHT_ROAD;
			break;
		case LEFT_ROAD:
			if (last_road_type == FORWARD_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + road_segment->size.z / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == LEFT_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x + length / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			else if (last_road_type == BACKWARD_ROAD)
			{
				road_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
				pos = vec3(last_cube->GetPos().x + road_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - road_segment->size.z / 2);
				road_segment->SetPos(pos.x, pos.y, pos.z);
			}
			last_road_type = LEFT_ROAD;
			break;
		case NOT_DEF_ROAD:
			break;
		default:
			break;
		}
		PhysBody3D* road_segment_body = App->physics->AddBody(*road_segment, STATIC_MASS);
		roads_list.add(road_segment);
	}
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

void ModuleSceneIntro::RotateRoadSegment(Cube* road, float angle, vec3 axis)
{
	road->SetRotation(angle, axis);
}