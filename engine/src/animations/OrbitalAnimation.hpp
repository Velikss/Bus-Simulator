#pragma once
#include <animations/Animation.hpp>

// the way the orbit should be handled; over which axis.
enum OrbitAngle
{
	xy,
	xz,
	zy
};

class OrbitalAnimation : public Animation
{
protected:
	OrbitAngle oAngle;
	float angle = 0.0f; // the angle the object has over its axis from the center.
	float radius; // the distance till the center.
	float speed; // the speed it should move in it's orbit.
	glm::vec3 rotCenter = glm::vec3(0, 0, 0);
public:
	OrbitalAnimation(BaseObject* object, float radius, float speed, OrbitAngle oAngle) : Animation(object)
	{
		rotCenter = object->pPosition;
		this->radius = radius;
		this->speed = speed;
		this->oAngle = oAngle;
		if(oAngle == OrbitAngle::xz)
			object->SetPosition(rotCenter.x + radius, rotCenter.z);
		else if(oAngle == OrbitAngle::zy)
			object->SetPos(rotCenter.x, rotCenter.y, rotCenter.z + radius);
		else
			object->SetPos(rotCenter.x, rotCenter.y + radius, rotCenter.z);
	}
	void Update()
	{
		if (angle > 360.0f)
		{
			angle = 0.0f;
		}

		float radian = glm::radians(angle);

		if (oAngle == OrbitAngle::xz)
		{
			float x = rotCenter.x + (radius * cosf(radian));
			float z = rotCenter.z + (radius * sinf(radian));
			obj->SetPosition(x, z);
		}
		else if (oAngle == OrbitAngle::zy)
		{
			float z = rotCenter.z + (radius * cosf(radian));
			float y = rotCenter.y + (radius * sinf(radian));
			obj->SetPos(rotCenter.x, y, z);
		}
		else
		{
			float x = rotCenter.x + (radius * cosf(radian));
			float y = rotCenter.y + (radius * sinf(radian));
			obj->SetPos(x, y, rotCenter.z);
		}
		angle += speed;
	}
};