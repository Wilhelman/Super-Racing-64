#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include "glmath.h"

enum body_type
{
	PLAYER_01,
	PLAYER_02,
	NO_DEF
};

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	vec3 GetPos() const;
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;

private:
	bool is_sensor = false;
	vec3 position;

public:
	p2List<Module*> collision_listeners;
	body_type type = NO_DEF;
	btRigidBody* body = nullptr;
};

#endif // __PhysBody3D_H__