#pragma once
#include <animations/LightObjectBindingAnimation.hpp>
#include <objects/Light.hpp>

/*
The animation for the sirene on the car.
It takes in a light and an object that it needs to be attached to.
*/
class FireWehrAnimation : public LightObjectBindingAnimation
{
	bool outward = true; // bool for increasing or decreasing the range of the light.
public:
	float lightDiff = 1.0; // how fast the light should increase/decrease.
	float lightRange = 30; // the maximum range of the light.
	FireWehrAnimation(BaseObject* target, Light* tomove) : LightObjectBindingAnimation(target, tomove)
	{
	}
	void Update()
	{
		// call base.
		LightObjectBindingAnimation::Update();

		// switch increase and decrease on 0><radius
		if (tomove->radius >= lightRange || tomove->radius <= 0) outward = !outward;
		if (outward)
			tomove->radius += lightDiff;
		else
			tomove->radius -= lightDiff;
	}
};