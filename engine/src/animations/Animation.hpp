#pragma once
#include <pch.hpp>
#include <objects/BaseObject.hpp>

/*
Basic Animation abstract class which has an update function.
*/
class Animation
{
protected:
	BaseObject* obj;
public:
	Animation(BaseObject* obj)
	{
		this->obj = obj;
	}
	virtual void Update() = 0;
};