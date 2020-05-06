#pragma once
#include <pch.hpp>
#include <shaders/ShaderProgram.hpp>
#include <objects/BaseObject.hpp>
#include <objects/Light.hpp>
#include <scenes/Camera.hpp>
#include <animations/Animation.hpp>
//#include <animations/Animation.hpp>
#include <GLFW/glfw3.h>

class Scene
{
protected:
	// Input properties and chaching, might be delegated later.
	enum class KeyOP
	{
		UP, DOWN
	};
	bool keys[256] = {false};
	// mouse input cache.
	glm::vec2 screenCenter;
	int screenCenter_X, screenCenter_Y;
public:
	string name; // name of the scene.
	Camera *camera;

	// all buffers for the scene.
	std::map<string, Texture*> textures;
	std::map<string, Geometry*> geometries;
	std::map<string, Mesh*> meshes;
	std::map<string, ShaderProgram*> shaders;
	std::map<string, Model*> models;

	// all objects & corresponding in the scene.
	std::map<string, BaseObject*> objects;
	std::vector<Animation*> animations;

	// Lighting and background.
	std::vector<Light*> lights;
	BaseObject* skybox = nullptr;

	Scene(string name, int screen_Width, int screen_Height)
	{
		this->name = name;
		screenCenter_X = screen_Width / 2;
		screenCenter_Y = screen_Height / 2;
		screenCenter = glm::vec2(screenCenter_X, screenCenter_Y);
	}

	virtual void Update()
	{
		camera->ProcessUpdates();
		for (auto& animation : animations)
			animation->Update();
	}

	virtual void Load() = 0;

	void Render()
	{
		for (auto& shader : shaders)
		{
			camera->SetTransformationOnShader(shader.second);
			shader.second->setInt("lightsSize", shaders.size());
			shader.second->setInt("skybox", 0);
			for(int i = 0; i < lights.size(); i++)
				lights[i]->PutOnShader(shader.second, i);
		}
		for (auto& light : lights)
			light->Render();
		for (auto& obj : objects)
			obj.second->Render();

		glDepthFunc(GL_LEQUAL);
		skybox->Render();
		glDepthFunc(GL_LESS);
	}

	// input passthrough
	virtual void KeyDown(unsigned char k, int a, int b)
	{
		keys[k] = true;
	}
	virtual void KeyUp(unsigned char k, int a, int b)
	{
		keys[k] = false;
	}

	virtual void MoveMouse(int x, int y)
	{
        const float sensitivity = 2.5f;
        x *= sensitivity;
        y *= sensitivity;

		camera->LookMouseDiff(x, y);
	}
};