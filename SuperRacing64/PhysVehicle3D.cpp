#include "PhysVehicle3D.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Blue;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();

	//chassis
	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	chassis.Render();


	//pilot chassis
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&info.chassis_pilot.transform);
	btVector3 back_offset(info.chassis_pilot_offset.x, info.chassis_pilot_offset.y, info.chassis_pilot_offset.z);
	back_offset = back_offset.rotate(q.getAxis(), q.getAngle());
	info.chassis_pilot.transform.M[12] += back_offset.getX();
	info.chassis_pilot.transform.M[13] += back_offset.getY();
	info.chassis_pilot.transform.M[14] += back_offset.getZ();
	//info.chassis_right_back_corner.SetRotation(90, { 0,1,0 });
	info.chassis_pilot.Render();

	//back chassis
	Cube b_chassis(info.chassis_back_size.x, info.chassis_back_size.y, info.chassis_back_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&b_chassis.transform);
	btVector3 b_offset(info.chassis_back_offset.x, info.chassis_back_offset.y, info.chassis_back_offset.z);
	b_offset = b_offset.rotate(q.getAxis(), q.getAngle());

	b_chassis.transform.M[12] += b_offset.getX();
	b_chassis.transform.M[13] += b_offset.getY();
	b_chassis.transform.M[14] += b_offset.getZ();

	b_chassis.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}