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
	BuildWalls();

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

	for (p2List_item<Cube*>* road_item = roads_circuit_2.getFirst(); road_item; road_item = road_item->next)
	{
		delete road_item->data;
		road_item->data = nullptr;
	}

	roads_circuit_2.clear();

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

void ModuleSceneIntro::AddConstruction(float length, ConstructionDirection road_type, ConstructionType construction_type, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (roads_circuit_1.count() == 0 && (construction_type == CIRCUIT_1 || construction_type == WALLS)) // First road CIRCUIT 1
	{
		if (isWall)
		{
			construction_segment = new Cube(WALL_WIDTH, WALL_HEIGHT, length);
			construction_segment->SetPos(0.0f, WALL_HEIGHT / 2.0f, 10.0f);
		}
		else
		{
			construction_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
			construction_segment->SetPos(0.0f, ROAD_HEIGHT / 2.0f, 10.0f);
		}
		last_direction = FORWARD_DIRECTION;
	}
	else if ((roads_circuit_2.count() == 0 && construction_type == CIRCUIT_2) 
			|| (walls_list.count() == 0 && construction_type == WALLS)) // First road CIRCUIT 2
	{
		if (isWall)
		{
			construction_segment = new Cube(length, WALL_HEIGHT, WALL_WIDTH);
			construction_segment->SetPos(-21.0f - WALL_WIDTH, WALL_HEIGHT / 2.0f + + ROAD_HEIGHT / 2, 31.0f + ROAD_WIDTH / 2 + WALL_WIDTH / 2);
		}
		else
		{
			construction_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
			construction_segment->SetPos(-21.0f, ROAD_HEIGHT / 2.0f, 31.0f);
		}
		last_direction = RIGHT_DIRECTION;
	}
	else // Roads after the first one
	{
		Cube* last_cube = nullptr;

		if (construction_type == CIRCUIT_1)
			last_cube = roads_circuit_1.getLast()->data;
		else if(construction_type == CIRCUIT_2)
			last_cube = roads_circuit_2.getLast()->data;
		else
			last_cube = walls_list.getLast()->data;

		switch (road_type)
		{
		case FORWARD_DIRECTION:
			if (isWall)
				construction_segment = BuildForward(last_cube, length, true);
			else	
				construction_segment = BuildForward(last_cube, length);
			break;
		case BACKWARD_DIRECTION:
			if (isWall)
				construction_segment = BuildBackward(last_cube, length, true);
			else
				construction_segment = BuildBackward(last_cube, length);
			break;
		case RIGHT_DIRECTION:
			if (isWall)
				construction_segment = BuildRight(last_cube, length, true);
			else
				construction_segment = BuildRight(last_cube, length);
			break;
		case LEFT_DIRECTION:
			if (isWall)
				construction_segment = BuildLeft(last_cube, length, true);
			else
				construction_segment = BuildLeft(last_cube, length);
			break;
		case FORWARD_RAMP:
			if (isWall)
				construction_segment = BuildForwardRamp(last_cube, length, X_AXIS, true);
			else
				construction_segment = BuildForwardRamp(last_cube, length, X_AXIS);
			break;
		case BACKWARD_RAMP:
			if (isWall)
				construction_segment = BuildBackwardRamp(last_cube, length, X_AXIS, true);
			else
				construction_segment = BuildBackwardRamp(last_cube, length, X_AXIS);
			break;
			break;
		/*case RIGHT_RAMP: // TODO: delete this if not used
			construction_segment = BuildRightRamp(last_cube, length, Z_AXIS);
			break;
		case LEFT_RAMP:
			construction_segment = BuildLeftRamp(last_cube, length, Z_AXIS);
			break;*/
		case DIRECTION_NOT_DEF:
			break;
		default:
			break;
		}
	}

	if (isWall)
		construction_segment->color = Red;
	else
		construction_segment->color = Grey;
	
	App->physics->AddBody(*construction_segment, STATIC_MASS);

	if (construction_type == CIRCUIT_1)
		roads_circuit_1.add(construction_segment);
	else if (construction_type == CIRCUIT_2)
		roads_circuit_2.add(construction_segment);
	else if (construction_type == WALLS)
		walls_list.add(construction_segment);

}

Cube * ModuleSceneIntro::BuildForward(Cube* last_cube, float length, bool isWall)
{ 
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(WALL_WIDTH, WALL_HEIGHT, length);
	else
		construction_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);
	
	vec3 pos(0.0f, 0.0f, 0.0f);

	// TODO: do a switch
	if (last_direction == FORWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
	else if (last_direction == RIGHT_DIRECTION)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);
	else if (last_direction == LEFT_DIRECTION)
		pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + construction_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + (length / 2 - last_cube->size.z / 2));
	else if (last_direction == FORWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - 1.0f);
	else if (last_direction == RIGHT_RAMP)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2);

	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = FORWARD_DIRECTION;

	return construction_segment;
}

Cube * ModuleSceneIntro::BuildBackward(Cube * last_cube, float length, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(WALL_WIDTH, WALL_HEIGHT, length);
	else
		construction_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);

	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_direction == BACKWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
	else if (last_direction == RIGHT_DIRECTION)
		pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2);
	else if (last_direction == LEFT_DIRECTION)
		pos = vec3(last_cube->GetPos().x + last_cube->size.x / 2 + construction_segment->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - (length / 2 - last_cube->size.z / 2));
	else if (last_direction == BACKWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);
	else if (last_direction == FORWARD_RAMP)
		pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - last_cube->size.z / 4.15f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);

	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = BACKWARD_DIRECTION;

	return construction_segment;
}

Cube * ModuleSceneIntro::BuildLeft(Cube * last_cube, float length, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(length, WALL_HEIGHT, WALL_WIDTH);
	else
		construction_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);

	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_direction == FORWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x + construction_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + construction_segment->size.z / 2);
	else if (last_direction == LEFT_DIRECTION)
		pos = vec3(last_cube->GetPos().x + length / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
	else if (last_direction == BACKWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x + construction_segment->size.x / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - construction_segment->size.z / 2);
	
	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = LEFT_DIRECTION;

	return construction_segment;
}

Cube * ModuleSceneIntro::BuildRight(Cube * last_cube, float length, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(length, WALL_HEIGHT, WALL_WIDTH);
	else
		construction_segment = new Cube(length, ROAD_HEIGHT, ROAD_WIDTH);
	
	vec3 pos(0.0f, 0.0f, 0.0f);

	if (last_direction == FORWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x - construction_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z + last_cube->size.z / 2 + construction_segment->size.z / 2);
	else if (last_direction == RIGHT_DIRECTION)
		pos = vec3(last_cube->GetPos().x - length / 2 - last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z);
	else if (last_direction == BACKWARD_DIRECTION)
		pos = vec3(last_cube->GetPos().x - construction_segment->size.x / 2 + last_cube->size.x / 2, last_cube->GetPos().y, last_cube->GetPos().z - last_cube->size.z / 2 - construction_segment->size.z / 2);
		
	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = RIGHT_DIRECTION;

	return construction_segment;
}

Cube * ModuleSceneIntro::BuildForwardRamp(Cube * last_cube, float length, vec3 axis, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(WALL_WIDTH, WALL_HEIGHT, length);
	else
		construction_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);

	vec3 pos(0.0f, 0.0f, 0.0f);
	construction_segment->SetRotation(-30.0f, axis);

	if (last_direction == FORWARD_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length/3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
	else if (last_direction == RIGHT_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);
	else if (last_direction == LEFT_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z + last_cube->size.z / 2 + length / 2 - length / 17.0f);		
	else if (last_direction == BACKWARD_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y - length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + 2.0f);

	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = FORWARD_RAMP;

	return construction_segment;
}

Cube * ModuleSceneIntro::BuildBackwardRamp(Cube * last_cube, float length, vec3 axis, bool isWall)
{
	Cube* construction_segment = nullptr;

	if (isWall)
		construction_segment = new Cube(WALL_WIDTH, WALL_HEIGHT, length);
	else
		construction_segment = new Cube(ROAD_WIDTH, ROAD_HEIGHT, length);

	vec3 pos(0.0f, 0.0f, 0.0f);
	construction_segment->SetRotation(30.0f, axis);

	if (last_direction == BACKWARD_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x, last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	else if (last_direction == RIGHT_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x - (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	else if (last_direction == LEFT_DIRECTION)
		pos = pos = vec3(last_cube->GetPos().x + (last_cube->size.x / 2 - construction_segment->size.x / 2), last_cube->GetPos().y + length / 3.965f, last_cube->GetPos().z - last_cube->size.z / 2 - length / 2 + length / 17.0f);
	
	construction_segment->SetPos(pos.x, pos.y, pos.z);
	last_direction = BACKWARD_RAMP;

	return construction_segment;
}


/* todo: DELETE THIS
Cube * ModuleSceneIntro::BuildLeftRamp(Cube * last_cube, float length, vec3 axis, bool isWall) // TODO: delete if not used
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

Cube * ModuleSceneIntro::BuildRightRamp(Cube * last_cube, float length, vec3 axis, bool isWall) // TODO: delete if not used
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

*/

void ModuleSceneIntro::BuildCircuit_1()
{
	AddConstruction(32.0f, FORWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(64.0f, LEFT_DIRECTION, CIRCUIT_1);
	AddConstruction(40.0f, FORWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(104.0f, LEFT_DIRECTION, CIRCUIT_1);
	AddConstruction(80.0f, BACKWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(54.0f, RIGHT_DIRECTION, CIRCUIT_1);
	AddConstruction(32.0f, FORWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(48.0f, RIGHT_DIRECTION, CIRCUIT_1);
	AddConstruction(24.0f, BACKWARD_RAMP, CIRCUIT_1);
	AddConstruction(40.0f, BACKWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(24.0f, FORWARD_RAMP, CIRCUIT_1);
	AddConstruction(16.0F, BACKWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(88.0f, LEFT_DIRECTION, CIRCUIT_1);
	AddConstruction(38.0f, FORWARD_DIRECTION, CIRCUIT_1);
	AddConstruction(184.0f, RIGHT_DIRECTION, CIRCUIT_1);
	AddConstruction(24.6f, FORWARD_DIRECTION, CIRCUIT_1);
}

void ModuleSceneIntro::BuildCircuit_2()
{
	AddConstruction(32.0f, RIGHT_DIRECTION, CIRCUIT_2);
	AddConstruction(32.0f, BACKWARD_DIRECTION, CIRCUIT_2);
	AddConstruction(24.0f, BACKWARD_RAMP, CIRCUIT_2);
	AddConstruction(48.0f, BACKWARD_DIRECTION, CIRCUIT_2);
	AddConstruction(40.0f, RIGHT_DIRECTION, CIRCUIT_2);
	AddConstruction(48.0f, BACKWARD_DIRECTION, CIRCUIT_2);
	AddConstruction(96.0f, LEFT_DIRECTION, CIRCUIT_2);
	AddConstruction(24.0f, BACKWARD_DIRECTION, CIRCUIT_2);
	AddConstruction(140.0f, LEFT_DIRECTION, CIRCUIT_2);
	AddConstruction(85.0f, FORWARD_DIRECTION, CIRCUIT_2);
	//AddRoad(10.0f, FORWARD_RAMP, CIRCUIT_2);
}

void ModuleSceneIntro::BuildWalls()
{
	AddConstruction(32.0f, RIGHT_DIRECTION, WALLS, true);
	AddConstruction(32.0f + ROAD_WIDTH, BACKWARD_DIRECTION, WALLS,  true);
	AddConstruction(24.0f - WALL_WIDTH, BACKWARD_RAMP, WALLS, true);
	AddConstruction(48.0f, BACKWARD_DIRECTION, WALLS, true);
	AddConstruction(40.0f - ROAD_WIDTH, RIGHT_DIRECTION, WALLS, true);
	AddConstruction(48.0f + 2*ROAD_WIDTH, BACKWARD_DIRECTION, WALLS, true);
	AddConstruction(96.0f, LEFT_DIRECTION, WALLS, true);
	AddConstruction(24.0f, BACKWARD_DIRECTION, WALLS, true);
	AddConstruction(140.0f + ROAD_WIDTH, LEFT_DIRECTION, WALLS, true);
	AddConstruction(85.0f + WALL_WIDTH, FORWARD_DIRECTION, WALLS, true);
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