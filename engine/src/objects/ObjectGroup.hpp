#pragma once
#include <pch.hpp>
#include <objects/BaseObject.hpp>

/*
This is a composition of objects to draw, but they can be moved all together.
*/
class ObjectGroup : public BaseObject
{
	std::vector<BaseObject*> objects;
public:
	ObjectGroup() : BaseObject(nullptr)
	{
	}

	void AddObject(BaseObject* object)
	{
		objects.push_back(object);
	}

	virtual void SetPos(float x, float y, float z)
	{
		float height = 0.0;
		for (auto& obj : objects)
			height += obj->mesh->geo->height;
		if (height > 0)
		{
            pPosition.x = x;
            pPosition.y = y - (height > 0 ? (height / 2) : 0);
            pPosition.z = z;
		}
	}

	virtual void SetPositionalHeigth(float y)
	{
		float height = 0.0;
		for (auto& obj : objects)
			height += obj->mesh->geo->height;
        pPosition.y = y - (height > 0 ? (height / 2) : 0);
	}

	virtual void Render() override
	{
		for (auto& obj : objects)
		{
			glm::vec3 _pos = obj->pPosition + pPosition;
			glm::vec3 _rot = obj->rot + rot;
			glm::vec3 _scale = obj->scale + scale;
			obj->Render(_pos, _rot, _scale);
		}
	}
};

typedef ObjectGroup ModelGroup;