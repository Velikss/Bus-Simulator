#pragma once
#include <objects/Light.hpp>
#include <animations/OrbitalAnimation.hpp>

/*
Animation for the moon to shine with certain intensity depending on the height of the moon itself.
*/
class MoonLightAnimation : public OrbitalAnimation
{
public:
	float lightIntensity = 0.05f;
	float maxLightIntensity = 220.0f;
	MoonLightAnimation(BaseObject* object, float radius, float speed, OrbitAngle oAngle) : OrbitalAnimation(object, radius, speed, oAngle)
	{
	}
	void Update()
	{
		// call base update.
		OrbitalAnimation::Update();
		Light* moon = (Light*)obj;
		if (moon->pPosition.y <= 0)
			moon->radius = 0;
		if (moon->pPosition.y > 0)
			moon->radius = radius * (lightIntensity * moon->pPosition.y);
		if (moon->radius > maxLightIntensity)
			moon->radius = maxLightIntensity;
	}
};