#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	enabled = true;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	if (!App->audio->PlayMusic("audio/music/07_Mario_Circuit.ogg"))
		LOG("Error playing music in ModuleSceneIntro Start");

	countdown_fx = App->audio->LoadFx("audio/fx/countdown_fx.wav");
	

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Roads
	BuildCircuit_1();
	BuildCircuit_2();

	//Walls
	SetUpWalls();

	// Start sensor -----------------
	Cube cube_sensor_c11(10.0f, 2.0f, 1.0f);
	cube_sensor_c11.SetPos(0.0f, 2.0f, 0.0f);
	start_sensor = App->physics->AddBody(cube_sensor_c11, App->scene_intro, true, STATIC_MASS);
	start_sensor->SetPos(cube_sensor_c11.GetPos().x, cube_sensor_c11.GetPos().y, cube_sensor_c11.GetPos().z);
	
	// Sensors circuit 1 -------------
	Cube cube_sensor_c12(1.0f, 2.0f, 10.0f);
	cube_sensor_c12.SetPos(70.0f, 2.0f, 71.0f);
	second_sensor_c1 = App->physics->AddBody(cube_sensor_c12, App->scene_intro, true, STATIC_MASS);
	second_sensor_c1->SetPos(cube_sensor_c12.GetPos().x, cube_sensor_c12.GetPos().y, cube_sensor_c12.GetPos().z);

	Cube cube_sensor_c13(10.0f, 2.0f, 1.0f);
	cube_sensor_c13.SetPos(86.0f, 13.5f, -12.588f);
	third_sensor_c1 = App->physics->AddBody(cube_sensor_c13, App->scene_intro, true, STATIC_MASS);
	third_sensor_c1->SetPos(cube_sensor_c13.GetPos().x, cube_sensor_c13.GetPos().y, cube_sensor_c13.GetPos().z);

	Cube cube_sensor_c14(1.0f, 2.0f, 10.0f);
	cube_sensor_c14.SetPos(125.0f, 1.5f, -73.588f);
	fourth_sensor_c1 = App->physics->AddBody(cube_sensor_c14, App->scene_intro, true, STATIC_MASS);
	fourth_sensor_c1->SetPos(cube_sensor_c14.GetPos().x, cube_sensor_c14.GetPos().y, cube_sensor_c14.GetPos().z);

	// Sensors circuit 2 -------------
	Cube cube_sensor_c21(10.0f, 2.0f, 1.0f);
	cube_sensor_c21.SetPos(-40.0f, 13.5f, -50.58f);
	second_sensor_c2 = App->physics->AddBody(cube_sensor_c21, App->scene_intro, true, STATIC_MASS);
	second_sensor_c2->SetPos(cube_sensor_c21.GetPos().x, cube_sensor_c21.GetPos().y, cube_sensor_c21.GetPos().z);

	Cube cube_sensor_c22(10.0f, 2.0f, 1.0f);
	cube_sensor_c22.SetPos(26.0f, 13.5f, -144.588f);
	third_sensor_c2 = App->physics->AddBody(cube_sensor_c22, App->scene_intro, true, STATIC_MASS);
	third_sensor_c2->SetPos(cube_sensor_c22.GetPos().x, cube_sensor_c22.GetPos().y, cube_sensor_c22.GetPos().z);

	Cube cube_sensor_c23(10.0f, 2.0f, 1.0f);
	cube_sensor_c23.SetPos(176.0f, 13.5f, -136.588f);
	fourth_sensor_c2 = App->physics->AddBody(cube_sensor_c23, App->scene_intro, true, STATIC_MASS);
	fourth_sensor_c2->SetPos(cube_sensor_c23.GetPos().x, cube_sensor_c23.GetPos().y, cube_sensor_c23.GetPos().z);

	Cube cube_ground_sensor(500.0f, 0.5f, 500.0f);
	cube_ground_sensor.SetPos(0.0, 0.5f, 0.0f);
	ground_sensor = App->physics->AddBody(cube_ground_sensor, App->scene_intro, true, STATIC_MASS);
	ground_sensor->SetPos(cube_ground_sensor.GetPos().x, cube_ground_sensor.GetPos().y, cube_ground_sensor.GetPos().z);

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
	Cube ground(1000, 1, 1000);
	ground.axis = true;
	ground.color = Green;
	ground.Render();

	RenderRoads();
	RenderWalls();

	//main menu settings

	if (current_players != 0) {
		if (timer_to_beat.Read() > 195000) {
			current_players = 0;
			App->player->last_sensor = nullptr;
			App->player->ResetVehicle();
			if (App->player2->enabled) {
				App->player2->vehicle->SetPos(1000, -4000, 0);
				App->player2->enabled = false;
				App->player2->CleanUp();
			}
			did_player_loose = 3;
		}
	}

	switch (current_players)
	{
	case 0: {
		char title[250];
		switch (did_player_loose)
		{
		case 0:
			sprintf_s(title, "Press 1 -> ONE PLAYER MODE | Press 2 -> TWO PLAYER MODE | ESC -> QUIT GAME");
			break;
		case 1:
			sprintf_s(title, "PLAYER 01 WIN! ---- Press 1 -> ONE PLAYER MODE | Press 2 -> TWO PLAYER MODE | ESC -> QUIT GAME");
			break;
		case 2:
			sprintf_s(title, "PLAYER 02 WIN! ---- Press 1 -> ONE PLAYER MODE | Press 2 -> TWO PLAYER MODE | ESC -> QUIT GAME");
			break;
		case 3:
			sprintf_s(title, "YOU LOSE... Try again ---- Press 1 -> ONE PLAYER MODE | Press 2 -> TWO PLAYER MODE | ESC -> QUIT GAME");
			break;
		default:
			break;
		}
		
		App->window->SetTitle(title);
		break;
	}
	case 1:
	{
		if (race_started == false)
			race_started = true;

		float sec = (float)App->player->p_timer.Read() / 1000.0f;
		float min = sec / 60.0f;
		float hour = min / 60.0f;

		int sec_int = (int)sec;

		if (min < 1.0f)
			min = 0.0f;
		if (sec < 1.0f)
			sec = 0.0f;
		if (hour < 1.0f)
			hour = 0.0f;

		if (min > 0.0f)
			sec_int -= (int)min * 60;

		char title2[250];
		if(App->player->laps ==3)
			sprintf_s(title2, "%.1f Km/h | Remaining laps: %i | Current time: %i:%i:%i | Backspace to respawn", App->player->vehicle->GetKmh(), App->player->laps, (int)hour, (int)min, sec_int);
		else {
			float l_sec = (float)App->player->last_time / 1000.0f;
			float l_min = l_sec / 60.0f;
			float l_hour = l_min / 60.0f;

			int l_sec_int = (int)l_sec;

			if (l_min < 1.0f)
				l_min = 0.0f;
			if (l_sec < 1.0f)
				l_sec = 0.0f;
			if (l_hour < 1.0f)
				l_hour = 0.0f;

			if (l_min > 0.0f)
				l_sec_int -= (int)l_min * 60;

			sprintf_s(title2, "%.1f Km/h | Remaining laps: %i | Current time: %i:%i:%i | Last lap: %i:%i:%i | To beat in 3 laps: 00:03:15 | Backspace to respawn", App->player->vehicle->GetKmh(), App->player->laps, (int)hour, (int)min, sec_int, (int)l_hour, (int)l_min, l_sec_int);
		}

		App->window->SetTitle(title2);
		break;
	}
	case 2: {

		float l_sec = (float)timer_to_beat.Read() / 1000.0f;
		float l_min = l_sec / 60.0f;
		float l_hour = l_min / 60.0f;

		int l_sec_int = (int)l_sec;

		if (l_min < 1.0f)
			l_min = 0.0f;
		if (l_sec < 1.0f)
			l_sec = 0.0f;
		if (l_hour < 1.0f)
			l_hour = 0.0f;

		if (l_min > 0.0f)
			l_sec_int -= (int)l_min * 60;
		if (!App->player2->enabled) {
			App->player2->enabled = true;
			App->player2->Init();
			App->player2->Start();
		}
		if (!race_started)
			race_started = true;

		char title3[250];
		sprintf_s(title3, "Player 1 - Remaining laps: %i | Player 2 - Remaining laps: %i | To beat in 3 laps: 00:03:15 | Timer: %i:%i:%i", App->player->laps, App->player2->laps, (int)l_hour, (int)l_min, l_sec_int);
		App->window->SetTitle(title3);
		break;
	}
	default:
		break;
	}

	if (race_started && !countdown_played)
	{
		App->audio->PlayFx(countdown_fx);
		countdown_played = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && current_players == 0)
	{
		timer_to_beat.Start();
		current_players = 1;
		App->player->laps = 3;
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && current_players == 0)
	{
		timer_to_beat.Start();
		current_players = 2;
		App->player->laps = 3;
		App->player2->laps = 3;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

	if (body1 == ground_sensor && (body2->type == PLAYER_01 || body2->type == PLAYER_02))
	{
		if (body2->type == PLAYER_01)
			p1_HasFallen = true;
		else if (body2->type == PLAYER_02)
			p2_HasFallen = true;
	}

	if (body1 == start_sensor && (App->player->last_sensor == fourth_sensor_c1 || App->player->last_sensor == fourth_sensor_c2) && App->player->last_sensor != nullptr)
	{
		if (body2->type == PLAYER_01)
		{
			App->player->last_sensor = body1;
			App->player->laps--;
			App->player->last_time = App->player->p_timer.Read();
			App->player->p_timer.Stop();
			App->player->p_timer.Start();
			if (App->player->laps == 0){
				did_player_loose = 1;
				current_players = 0;
				App->player->last_sensor = nullptr;
				App->player->ResetVehicle();
				if (App->player2->enabled) {
					App->player2->vehicle->SetPos(1000, -4000, 0);
					App->player2->enabled = false;
					App->player2->CleanUp();
				}
			}
		}else if (body2->type == PLAYER_02)		//TODO SET LAST WINNER 
		{
			App->player2->last_sensor = body1;
			App->player2->laps--;
			if (App->player2->laps == 0) 
			{
				did_player_loose = 2;
				current_players = 0;
				App->player2->enabled = false;
				App->player2->CleanUp();
			}
		}
	}
	else if (body1 == second_sensor_c1 && (App->player->last_sensor == start_sensor || App->player->last_sensor == nullptr))
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
	else if (body1 == third_sensor_c1 && App->player->last_sensor == second_sensor_c1)
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
	else if (body1 == fourth_sensor_c1 && App->player->last_sensor == third_sensor_c1)
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
	else if (body1 == second_sensor_c2 && (App->player->last_sensor == start_sensor || App->player->last_sensor == nullptr))
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
	else if (body1 == third_sensor_c2 && App->player->last_sensor == second_sensor_c2)
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
	else if (body1 == fourth_sensor_c2 && App->player->last_sensor == third_sensor_c2)
	{
		if (body2->type == PLAYER_01)
			App->player->last_sensor = body1;
		else if (body2->type == PLAYER_02)
			App->player2->last_sensor = body1;;
	}
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

void ModuleSceneIntro::CreateWall(float x, float y, float z, float length, RoadType wall_type)
{
	Cube* tmp_wall = nullptr;

	switch (wall_type)
	{
	case FORWARD_ROAD:
		tmp_wall = BuildWall(x, y, z, WALL_WIDTH, WALL_HEIGHT, length);
		break;
	case BACKWARD_ROAD:
		tmp_wall = BuildWall(x, y, z, WALL_WIDTH, WALL_HEIGHT, length);
		break;
	case RIGHT_ROAD:
		tmp_wall = BuildWall(x, y, z, length, WALL_HEIGHT, WALL_WIDTH);
		break;
	case LEFT_ROAD:
		tmp_wall = BuildWall(x, y, z, length, WALL_HEIGHT, WALL_WIDTH);
		break;
	case ROAD_NOT_DEF:
		break;
	default:
		break;
	}

	walls_list.add(tmp_wall);
}

Cube* ModuleSceneIntro::BuildWall(float x, float y, float z, float length, float width, float height)
{
	Cube* tmp_wall = new Cube(length, width, height);
	tmp_wall->SetPos(x, y, z);
	tmp_wall->color = Red;
	App->physics->AddBody(*tmp_wall, STATIC_MASS);

	return tmp_wall;
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

Cube * ModuleSceneIntro::BuildLeftRamp(Cube * last_cube, float length, vec3 axis) // TODO: delete if not used
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

Cube * ModuleSceneIntro::BuildRightRamp(Cube * last_cube, float length, vec3 axis) // TODO: delete if not used
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
	AddRoad(60.0f, FORWARD_ROAD, CIRCUIT_2);
	//AddRoad(10.0f, FORWARD_RAMP, CIRCUIT_2);
}

void ModuleSceneIntro::SetUpWalls()
{
	// Circuit 1 right walls

	CreateWall(-ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, 10.0f - 24.6f + ROAD_WIDTH / 2.0f, 32.0f + 24.6 + ROAD_WIDTH, FORWARD_ROAD);
	CreateWall(27.0f, 0.5f, 31.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 64.0f, LEFT_ROAD);
	CreateWall(64.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, 46.0f + ROAD_WIDTH, 40.0f, FORWARD_ROAD);
	CreateWall(111.0f + ROAD_WIDTH / 2.0f, 0.5f, 71.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 104.0f + ROAD_WIDTH, LEFT_ROAD);
	CreateWall(168.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, 36.0f - ROAD_WIDTH / 2.0f, 80.0f + ROAD_WIDTH, BACKWARD_ROAD);
	CreateWall(146.0f, 0.5f, -9.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 54.0f, LEFT_ROAD);
	CreateWall(124.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, 12.0f - ROAD_WIDTH / 2.0f, 32.0f + ROAD_WIDTH, FORWARD_ROAD);
	CreateWall(105.0f, 0.5f, 33.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 48.0f - 2.0f*ROAD_WIDTH, RIGHT_ROAD);
	CreateWall(86.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, -60.58f + ROAD_WIDTH / 2.0f - WALL_WIDTH, 16.0f + ROAD_WIDTH / 2.0f, BACKWARD_ROAD);
	CreateWall(125.0f + ROAD_WIDTH / 2.0f, 0.5f, -73.58f + ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 88.0f - ROAD_WIDTH, LEFT_ROAD);
	CreateWall(174.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, -59.58f + ROAD_WIDTH / 2.0f, 38.0f - ROAD_WIDTH, FORWARD_ROAD);
	CreateWall(87.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, -35.58f - ROAD_WIDTH / 2.0f, 184.0f - ROAD_WIDTH, RIGHT_ROAD);

	// Circuit 1 left walls
	CreateWall(ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, 10.0f - 24.6f + ROAD_WIDTH + WALL_WIDTH, 32.0f + 24.6, FORWARD_ROAD);
	CreateWall(27.0f + ROAD_WIDTH, 0.5f, 31.0f - ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 64.0f, LEFT_ROAD);
	CreateWall(64.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, 46.0f, 40.0f, FORWARD_ROAD);
	CreateWall(111.0f + ROAD_WIDTH / 2.0f, 0.5f, 71.0f - ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 104.0f - ROAD_WIDTH, LEFT_ROAD);
	CreateWall(168.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f - ROAD_WIDTH, 0.5f, 36.0f - ROAD_WIDTH / 2.0f, 80.0f - ROAD_WIDTH, BACKWARD_ROAD);
	CreateWall(146.0f + WALL_WIDTH / 2.0f, 0.5f, -9.0f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f + ROAD_WIDTH, 54.0f - 2.0f*ROAD_WIDTH + WALL_WIDTH, LEFT_ROAD);
	CreateWall(124.0f + ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 0.5f, 12.0f + ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 32.0f + ROAD_WIDTH + WALL_WIDTH, FORWARD_ROAD);
	CreateWall(105.0f, 0.5f, 33.0f + ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 48.0f, RIGHT_ROAD);
	CreateWall(86.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f - ROAD_WIDTH, 0.5f, 30.0f, ROAD_WIDTH, BACKWARD_ROAD);
	CreateWall(86.0f + ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f - ROAD_WIDTH, 0.5f, -60.58f - WALL_WIDTH, 16.0f + 1.5f*ROAD_WIDTH, BACKWARD_ROAD);
	CreateWall(125.0f + ROAD_WIDTH / 2.0f, 0.5f, -73.58f - ROAD_WIDTH / 2.0f - WALL_WIDTH / 2.0f, 88.0f + ROAD_WIDTH, LEFT_ROAD);
	CreateWall(174.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, -59.58f + ROAD_WIDTH / 2.0f, 38.0f + ROAD_WIDTH, FORWARD_ROAD);
	CreateWall(87.0f + ROAD_WIDTH / 2.0f + WALL_WIDTH / 2.0f, 0.5f, -35.58f + ROAD_WIDTH / 2.0f, 184.0f - ROAD_WIDTH, RIGHT_ROAD);
}