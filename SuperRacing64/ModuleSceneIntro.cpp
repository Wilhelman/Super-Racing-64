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
	
	CreateStraightRoadSegment(vec3(1.0f, 1.0f, 10.0f), 30.0f);

	//CreateStraightRoadSegment(vec3(-4.8f, 1.0f, 35.0f), 30.0f, -25.0f, Y_AXIS);
	



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
	p2List_item<Cube*>* road_item = roads_list.getFirst();

	while (road_item != nullptr)
	{
		road_item->data->Render();
		road_item = road_item->next;
	}
}

void ModuleSceneIntro::RenderWalls() const
{
	p2List_item<Cube*>* wall_item = walls_list.getFirst();

	while (wall_item != nullptr)
	{
		wall_item->data->Render();
		wall_item = wall_item->next;
	}
}

void ModuleSceneIntro::CreateStraightRoadSegment(vec3 position, float length, float angle, vec3 axis)
{
	Cube* road_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
	road_segment->SetPos(position.x, position.y, position.z);

	if (angle != 0.0f && (axis.x != 0.0f || axis.y != 0.0f || axis.z != 0.0f))
		road_segment->SetRotation(angle, axis);

	PhysBody3D* road_segment_body = App->physics->AddBody(*road_segment, STATIC_MASS);

	roads_list.add(road_segment);

	CreateWalls(road_segment, position);
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
