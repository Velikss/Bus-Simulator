#pragma once
#include <animations/Animation.hpp>
#include <objects/Light.hpp>

/*
It takes in a light and an object that it needs to be attached to.
And moves the light to the location of the passed object.
*/
class LightObjectBindingAnimation : public Animation
{
protected:
	Light* tomove; // the light
public:
	LightObjectBindingAnimation(BaseObject* target, Light* tomove) : Animation(target)
	{
		this->tomove = tomove;
	}
	void Update()
	{
		// move the light to the xz positon of the object.
		tomove->SetPosition(obj->pPosition.x, obj->pPosition.z);
	}
};