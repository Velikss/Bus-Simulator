#pragma once
#include <pch.hpp>
#include <textures/CubeMapTexture.hpp>
#include <geometries/SkyBoxGeometry.hpp>
#include <scenes/Scene.hpp>
#include <objects/ObjectGroup.hpp>
//#include <animations/MoonLightAnimation.hpp>
//#include <animations/FireWehrAnimation.hpp>

/*
This scene is based on the week 1 scene.
However it now has dynamic lighting with a moon streetlights, a better car, and more.
*/
class MyStreetScene : public Scene
{
    // camera cache for view switching, later maybe cameraStates?
    glm::vec3 cameraCachePos, cameraCachePosGod;
    float cameraGodHeight = 1.75;
    float cameraCacheYaw, cameraCacheYawGod, cameraCachePitch, cameraCachePitchGod;
public:
	MyStreetScene(int screen_Width, int screen_Height) : Scene("MyStreet", screen_Width, screen_Height)
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

        // Move the car with a certains peed.
        const float speed = 0.6;
        const float angle_diff = 6.0;
        if (keys[128 + GLUT_KEY_UP])
            objects.at("car")->MoveForward(speed);
        if (keys[128 + GLUT_KEY_DOWN])
            objects.at("car")->MoveBackward(speed);
        if (keys[128 + GLUT_KEY_LEFT] && keys[128 + GLUT_KEY_UP])
            objects.at("car")->MoveLeft(angle_diff);
        if (keys[128 + GLUT_KEY_RIGHT] && keys[128 + GLUT_KEY_UP])
            objects.at("car")->MoveRight(angle_diff);
        if (keys[128 + GLUT_KEY_LEFT] && keys[128 + GLUT_KEY_DOWN])
            objects.at("car")->MoveLeft(-angle_diff);
        if (keys[128 + GLUT_KEY_RIGHT] && keys[128 + GLUT_KEY_DOWN])
            objects.at("car")->MoveRight(-angle_diff);

        // if godmode is enabled with space and c one is able to fly.
        if (keys[' '] && !camera.lockHeight)
            camera.cameraHeight += 0.1f;
        if (keys['c'] && !camera.lockHeight)
            camera.cameraHeight -= 0.1f;
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
        else if (k == 'v')
        {
            // toggle the view mode, we know use caching with the properties above, later maybe cameraStates/multiple cameras?
            camera.lockHeight = !camera.lockHeight;
            if (!camera.lockHeight)
            {
                camera.cameraHeight = cameraGodHeight;
                cameraCachePos = camera.GetPosition();

                cameraCachePos = camera.GetPosition();
                cameraCachePitch = camera.GetPitch();
                cameraCacheYaw = camera.GetYaw();
                if (cameraCachePosGod.x != 0.0f && cameraCachePosGod.z != 0.000f)
                    camera.SetPosition(cameraCachePosGod);
                if (cameraCachePitchGod != 0.0f)
                    camera.SetPitch(cameraCachePitchGod);
                if (cameraCacheYawGod != 0.0f)
                    camera.SetYaw(cameraCacheYawGod);
            }
            else
            {
                cameraGodHeight = camera.cameraHeight;
                camera.cameraHeight = 1.75f;
                cameraCachePosGod = camera.GetPosition();
                cameraCachePitchGod = camera.GetPitch();
                cameraCacheYawGod = camera.GetYaw();
                camera.SetPosition(cameraCachePos);
                camera.SetPitch(cameraCachePitch);
                camera.SetYaw(cameraCacheYaw);
            }
        }
        else // call base if it isn't a special key but a movement key.
            Scene::KeyDown(k, a, b);
    }

    void InitShaders() // load the shaders.
    {
        Shader vert = Shader::fromFile("resources/shaders/vertexshader.vert");
        Shader materialfragment = Shader::fromFile("resources/shaders/materialfragmentshader.frag");
        Shader normalfragment = Shader::fromFile("resources/shaders/normalfragmentshader.frag");
        shaders.insert({ "material", new ShaderProgram(vert, materialfragment) });
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
        moon->SetPos(0, 0, 0);
        //animations.push_back(new MoonLightAnimation(moon, 150, 0.1, OrbitAngle::xy));
        lights.push_back(moon);

        // create a street or plane from the street model.
        BaseObject* street1 = new BaseObject(*models.at("street"));
        street1->SetLength(100);
        street1->SetWidth(200);
        objects.insert({ "street1", street1 });

        // create a street or plane from the street model.
        BaseObject* grasslawn1 = new BaseObject(*models.at("grasslawn"));
        grasslawn1->SetPosScale1(80.0, 0.001, 0);
        objects.insert({ "grasslawn1", grasslawn1 });

        // create a street or plane from the street model.
        BaseObject* grasslawn2 = new BaseObject(*models.at("grasslawn"));
        grasslawn2->SetPosScale1(0, 0.001, 0);
        grasslawn2->SetWidth(80);
        objects.insert({ "grasslawn2", grasslawn2 });

        // create a street or plane from the street model.
        BaseObject* grasslawn3 = new BaseObject(*models.at("grasslawn"));
        grasslawn3->SetPosScale1(-80.0, 0.001, 0);
        objects.insert({ "grasslawn3", grasslawn3 });

        // create the car
        BaseObject* car = new BaseObject(meshes.at("Porsche_911_GT2"));
        car->SetPositionalHeigth(1);
        car->SetScale(2);
        car->SetPosition(15, 15);
        // add a blue light to the car.
        Light* carAlarm = new Light(meshes.at("carAlarm"), true);
        carAlarm->ambient = glm::vec3(0.5, 0.5, 1.0);
        carAlarm->diffuse = glm::vec3(0.5, 0.5, 1.0);
        carAlarm->specular = glm::vec3(1.0, 1.0, 1.0);
        carAlarm->SetScale(0.2);
        carAlarm->SetPositionalHeigth(2.65);
        //animations.push_back(new FireWehrAnimation(car, carAlarm));
        carAlarm->radius = 25;
        lights.push_back(carAlarm);
        objects.insert({ "car", car });
        
        // create trees
        for (int i = 0; i < 20; i++)
        {
            BaseObject* tree = new BaseObject(*models.at("tree"));
            tree->SetPosition(-95 + 10 * i, -45);
            objects.insert({ "tree" + std::to_string(i), tree });
        }
        // lights near the trees
        for (int i = 0; i <= 18; i+=3)
        {
            Light* parkLight = new Light(meshes.at("parkLight"), true);
            parkLight->SetPos(-90 + 10 * i, -1.0, -45);
            parkLight->radius = 35;
            lights.push_back(parkLight);
        }

        // create the houses
        for (int i = 0; i < 8; i++)
        {
            ObjectGroup* house = new ObjectGroup(*(ModelGroup*)models.at("house"));
            house->SetPosition(-80 + (25 * i), 43.35);
            objects.insert({ "house" + std::to_string(i), house });
        }

        // laterns at the grasslawn/road split.
        for (int i = 0; i <= 18; i += 3)
        {
            BaseObject* lantern = new BaseObject(meshes.at("pole"));
            lantern->SetScale(0.1);
            lantern->SetHeight(6);
            lantern->SetPositionalHeigth(-0.5);
            // add the round point light to the lantern pole.
            Light* poleLight = new Light(meshes.at("poleLight"), true);
            poleLight->SetScale(0.3);
            poleLight->SetPositionalHeigth(6);
            poleLight->radius = 25;
            lights.push_back(poleLight);
            poleLight->SetPosition(-90 + 10 * i, 10);
            lantern->SetPosition(-90 + 10 * i, 10);
            objects.insert({ "lantern" + std::to_string(i), lantern });
        }

        // lanterns near the houses.
        for (int i = 0; i <= 18; i += 3)
        {
            BaseObject* lantern = new BaseObject(meshes.at("pole"));
            lantern->SetScale(0.1);
            lantern->SetHeight(6);
            lantern->SetPositionalHeigth(-0.5);
            Light* poleLight = new Light(meshes.at("poleLight"), true);
            poleLight->SetScale(0.3);
            poleLight->SetPositionalHeigth(6);
            poleLight->radius = 25;
            lights.push_back(poleLight);
            poleLight->SetPosition(-90 + 10 * i, 30);
            lantern->SetPosition(-90 + 10 * i, 30);
            objects.insert({ "lantern" + std::to_string(i + 1), lantern });
        }

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

        // create a grasslawn model.
        Model* grasslawn = new Model(meshes.at("grasslawn"), true);
        grasslawn->SetLength(20);
        grasslawn->SetWidth(40);
        models.insert({ "grasslawn", grasslawn });

        // create a tree model
        Model* tree = new Model(meshes.at("tree"));
        tree->SetScale(0.2);
        tree->SetPositionalHeigth(16);
        models.insert({ "tree", tree });

        // create a house model
        ModelGroup* house = new ModelGroup;
        Model* houseBase = new Model(meshes.at("houseBase"));
        houseBase->SetPos(0, 4.5, 0);
        Model* roof = new Model(meshes.at("roof"));
        roof->SetPos(0, 12.5, 0);
        house->AddObject(houseBase);
        house->AddObject(roof);
        house->SetPositionalHeigth(3.4);
        models.insert({ "house", house });
    }

    void InitMeshes()
    {
        meshes.insert({ "moon", new Mesh(shaders.at("normal"), geometries.at("sphere"), textures.at("moon")) });
        meshes.insert({ "houseBase", new Mesh(shaders.at("normal"), geometries.at("houseBase"), textures.at("stoneHouse")) });
        meshes.insert({ "roof", new Mesh(shaders.at("normal"), geometries.at("roof"), textures.at("roof")) });
        meshes.insert({ "skybox", new Mesh(shaders.at("skybox"), geometries.at("skybox"), textures.at("skybox")) });
        meshes.insert({ "street", new Mesh(shaders.at("normal"), geometries.at("street"), textures.at("street")) });
        meshes.insert({ "grasslawn", new Mesh(shaders.at("normal"), geometries.at("grasslawn"), textures.at("grass")) });
        meshes.insert({ "parkLight", new Mesh(shaders.at("normal"), geometries.at("cube")) });
        meshes.insert({ "poleLight", new Mesh(shaders.at("normal"), geometries.at("sphere")) });
        meshes.insert({ "pole", new Mesh(shaders.at("normal"), geometries.at("cylinder")) });
        
        meshes.insert({ "Porsche_911_GT2", new Mesh(shaders.at("material"), geometries.at("Porsche_911_GT2")) });
        meshes.at("Porsche_911_GT2")->material = materials.at("red plastic");
        
        meshes.insert({ "tree", new Mesh(shaders.at("normal"), geometries.at("tree")) });
        meshes.at("tree")->color = glm::vec3(0, 0.3, 0);

        meshes.insert({ "carAlarm", new Mesh(shaders.at("normal"), geometries.at("sphere")) });
        meshes.at("carAlarm")->color = glm::vec3(0, 0, 1.0);
    }

    void InitGeometries()
    {
        geometries.insert({ "skybox", new SkyBoxGeometry() });
        geometries.insert({ "tree", new Geometry(Geometry::FromObjFile("resources/geometries/tree.obj")) });
        geometries.insert({ "plane", new Geometry(Geometry::FromObjFile("resources/geometries/plane.obj")) });
        geometries.insert({ "cube", new Geometry(Geometry::FromObjFile("resources/geometries/box.obj")) });
        geometries.insert({ "sphere", new Geometry(Geometry::FromObjFile("resources/geometries/sphere.obj")) });
        geometries.insert({ "cylinder", new Geometry(Geometry::FromObjFile("resources/geometries/cylinder32.obj")) });
        geometries.insert({ "Porsche_911_GT2", new Geometry(Geometry::FromObjFile("resources/geometries/Porsche_911_GT2.obj")) });

        // beneath are the geometries which need their UVS resized for a better texture mapping.
        geometries.insert({ "houseBase", new Geometry(Geometry::FromObjFile("resources/geometries/houseBase.obj")) });
        geometries.at("houseBase")->resizeUVs(3);
        geometries.insert({ "roof", new Geometry(Geometry::FromObjFile("resources/geometries/roof.obj")) });
        geometries.at("roof")->resizeUVs(5);
        geometries.insert({ "street", new Geometry(Geometry::FromObjFile("resources/geometries/plane.obj")) });
        geometries.at("street")->resizeUVs(10);
        geometries.insert({ "grasslawn", new Geometry(Geometry::FromObjFile("resources/geometries/plane.obj")) });
        geometries.at("grasslawn")->resizeUVs(10);
    }

    void InitTextures()
    {
        textures.insert({ "roof", new Texture(Texture::fromFile("resources/textures/roof.jpg")) });
        textures.insert({ "stoneHouse", new Texture(Texture::fromFile("resources/textures/stone.jpg")) });
        textures.insert({ "grass", new Texture(Texture::fromFile("resources/textures/grass.jpg")) });
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