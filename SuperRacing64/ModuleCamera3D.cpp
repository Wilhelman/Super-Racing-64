#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModulePlayer.h"
#include "PhysVehicle3d.h"

#define X_CAM_OFFSET	15
#define Y_CAM_OFFSET	10
#define Z_CAM_OFFSET	20

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
	enabled = true;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	/*
	vec3 newPos(0,0,0);
	float speed = 3.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 50.0f * dt;

	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if(App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	
	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}*/
	
	/*
	Position.x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - X_CAM_OFFSET * App->player->vehicle->vehicle->getForwardVector().getX();
	Position.y = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + Y_CAM_OFFSET * App->player->vehicle->vehicle->getUpAxis();
	Position.z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - Z_CAM_OFFSET * App->player->vehicle->vehicle->getForwardVector().getZ();

	float x_value = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * App->player->vehicle->vehicle->getForwardVector().getX();
	float z_value = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * App->player->vehicle->vehicle->getForwardVector().getZ();

	LookAt(vec3(x_value, 1, z_value));*/
	


	// Recalculate matrix -------------
	switch (App->scene_intro->current_players)
	{
	case 0: {
		if (going_left)
		{
			if (x < 100)
				x += 0.20f;
			else
				going_left = false;
		}
		else
		{
			if (x > 35)
				x -= 0.20f;
			else
				going_left = true;
		}

		Position = { x, 120, -250 };
		LookAt(vec3(x, -100, 50));

		break;
	}
	case 1: {
		Position.x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - X_CAM_OFFSET * App->player->vehicle->vehicle->getForwardVector().getX();
		Position.y = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + Y_CAM_OFFSET * App->player->vehicle->vehicle->getUpAxis();
		Position.z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - Z_CAM_OFFSET * App->player->vehicle->vehicle->getForwardVector().getZ();

		float x_value = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * App->player->vehicle->vehicle->getForwardVector().getX();
		float z_value = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * App->player->vehicle->vehicle->getForwardVector().getZ();

		LookAt(vec3(x_value, 1, z_value));
		break;
	}
	case 2: {
		Position = { 65.35f, 228.46f, -68.17f };
		LookAt(vec3(65, 0, -50));
		break;
	}
	default:
		break;
	}


	CalculateViewMatrix();
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}