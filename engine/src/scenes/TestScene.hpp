#pragma once
#include <pch.hpp>
#include <textures/CubeMapTexture.hpp>
#include <geometries/SkyBoxGeometry.hpp>
#include <scenes/Scene.hpp>
#include <objects/ObjectGroup.hpp>
#include <animations/MoonLightAnimation.hpp>
#include <animations/FireWehrAnimation.hpp>

/*
This scene is based on the week 1 scene.
However it now has dynamic lighting with a moon streetlights, a better car, and more.
*/
class TestScene : public Scene
{
public:
    TestScene(int screen_Width, int screen_Height) : Scene("TestScene", screen_Width, screen_Height)
	{
        // construct
	}

    void Update()
    {
        // call camera updates, etc.
        Scene::Update();

        // for smoothing key presses are cached and performed in the update function.
        if (keys['w'])
            camera.Forward();
        if (keys['s'])
            camera.BackWard();
        if (keys['a'])
            camera.MoveLeft();
        if (keys['d'])
            camera.MoveRight();

        if (keys['i'])
            camera.LookUp();
        if (keys['k'])
            camera.LookDown();
        if (keys['j'])
            camera.LookLeft();
        if (keys['l'])
            camera.LookRight();
    }

	void Load()
	{
        // main load function.
        InitShaders();
        InitTextures();
        InitGeometries();
        InitMeshes();
        InitModels();
        InitObjects();
	}

    void KeyDown(unsigned char k, int a, int b)
    {
        if (k == 27) // esc-key
            exit(0);
        else if (k == 'q') // toggles skybox
            skybox->render = !skybox->render;
        else // call base if it isn't a special key but a movement key.
            Scene::KeyDown(k, a, b);
    }

    void InitShaders() // load the shaders.
    {
        Shader vert = Shader::fromFile("resources/shaders/vertexshader.vert");
        Shader normalfragment = Shader::fromFile("resources/shaders/normalfragmentshader.frag");
        shaders.insert({ "normal", new ShaderProgram(vert, normalfragment) });

        Shader skyboxvert = Shader::fromFile("resources/shaders/skybox.vert");
        Shader skyboxfrag = Shader::fromFile("resources/shaders/skybox.frag");
        shaders.insert({ "skybox", new ShaderProgram(skyboxvert, skyboxfrag) });
    }

    //------------------------------------------------------------
    // void InitBuffers()
    // Allocates and fills buffers
    //------------------------------------------------------------
    void InitObjects()
    {
        // create a moon and at it to the lights vector.
        Light* moon = new Light(meshes.at("moon"), true);
        moon->SetScale(2.0);
        moon->SetPos(0, 5, 0);
        moon->radius = 100;
        lights.push_back(moon);

        // create a street or plane from the street model.
        BaseObject* street1 = new BaseObject(*models.at("street"));
        street1->SetLength(100);
        street1->SetWidth(200);
        objects.insert({ "street1", street1 });

        // create the skybox
        skybox = new BaseObject(meshes.at("skybox"));
        skybox->SetScale(500);
    }

    void InitModels()
    {
        // create a street model.
        Model* street = new Model(meshes.at("street"), true);
        street->SetLength(20);
        street->SetWidth(40);
        models.insert({ "street", street });
    }

    void InitMeshes()
    {
        meshes.insert({ "street", new Mesh(shaders.at("normal"), geometries.at("street"), textures.at("street")) });
        meshes.insert({ "moon", new Mesh(shaders.at("normal"), geometries.at("sphere"), textures.at("moon")) });
        meshes.insert({ "skybox", new Mesh(shaders.at("skybox"), geometries.at("skybox"), textures.at("skybox")) });
    }

    void InitGeometries()
    {
        geometries.insert({ "street", new Geometry(Geometry::FromObjFile("resources/geometries/plane.obj")) });;
        geometries.insert({ "sphere", new Geometry(Geometry::FromObjFile("resources/geometries/sphere.obj")) });
        geometries.insert({ "skybox", new SkyBoxGeometry() });
    }

    void InitTextures()
    {
        textures.insert({ "street", new Texture(Texture::fromFile("resources/textures/street.jpg")) });
        textures.insert({ "moon", new Texture(Texture::fromFile("resources/textures/moon.jpg")) });

        // load the skybox textures into a cubemap.
        std::vector<string> cubemap
                {
                        "resources/textures/skybox/right.bmp", // right
                        "resources/textures/skybox/left.bmp", // left
                        "resources/textures/skybox/top.bmp", // top
                        "resources/textures/skybox/bottom.bmp", // bottom
                        "resources/textures/skybox/back.bmp", // back
                        "resources/textures/skybox/front.bmp" // front
                };
        textures.insert({ "skybox", new CubeMapTexture(CubeMapTexture::fromFile(cubemap)) });

        for (auto& tex : textures)
        {
            tex.second->Bind();
            tex.second->SetProperty(GL_TEXTURE_WRAP_S, GL_REPEAT);
            tex.second->SetProperty(GL_TEXTURE_WRAP_T, GL_REPEAT);
            tex.second->SetProperty(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            tex.second->SetProperty(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            tex.second->UnBind();
        }
    }
};