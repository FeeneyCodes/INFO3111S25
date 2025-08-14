#include "GlobalOpenGL.h"   // For OpenGL types like GLint, etc. 
#include "globalStuff.h"


#include <vector>
#include <fstream>
#include <iostream>
#include "cMeshObject.h"
#include "cVAOManager/cVAOManager.h"
#include "cBasicTextureManager/cBasicTextureManager.h"

extern std::vector<cMeshObject*> g_pMeshesToDraw;


void LoadFilesIntoVAOManager(cVAOManager* pTheMeshManager, GLuint program)
{
    //pTheMeshManager = new cVAOManager();

    // Load the dungeon floow model
    sModelDrawInfo meshFloor03;

    // Scale we want for the floor. 
    // They are 500 units wide.
    float newFloorScale = 10.0f / 500.0f;

    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03.ply",
        meshFloor03, program, true, true, true, newFloorScale))
    {
        std::cout << "Floor didn't load not loaded into VAO!" << std::endl;
    }


    sModelDrawInfo meshInfoCow;

    //    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/cow_xyz_n_rgba.ply",
    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/cow_xyz_n_rgba_UV (MeshLab trivial, random).ply",
        meshInfoCow, program, true, true, true, 1.0f))
    {
        std::cout << "Cow not loaded into VAO!" << std::endl;
    }



    sModelDrawInfo WarehouseMeshInfo;

    //    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n_rgba.ply",
//    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n_rgba_UV (MeshLab_XY_Project).ply",
    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n_rgba_UV (Blender Smart UV project).ply",
        WarehouseMeshInfo, program, true, true, true, 1.0f))
    {
        std::cout << "Warehouse NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo SmoothSphereMeshInfo;

    //    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply",
    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba_uv.ply",
        SmoothSphereMeshInfo, program, true, true, true, 1.0f))
    {
        std::cout << "SmoothSphere NOT loaded into VAO!" << std::endl;
    }

//    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/Isoshphere_smooth_xyz_n_rgba_uv.ply",
//        SmoothSphereMeshInfo, program, true, true, true, 1.0f))
//    {
//        std::cout << "SmoothSphere NOT loaded into VAO!" << std::endl;
//    }


//    sModelDrawInfo dolphinMeshInfo;
//    //    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/dolphin_xyz_n_rgba.ply",
//    if (!pTheMeshManager->LoadModelIntoVAO("assets/models/dolphin_xyz_n_rgba_UV.ply",
//        dolphinMeshInfo, program, true, true, true, 1.0f))
//    {
//        std::cout << "Dolphin NOT loaded into VAO!" << std::endl;
//    }


    // Same as above, but we are loading in two steps so we 
    //  can call the GenerateSphericalTextureCoords() in between

    sModelDrawInfo dolphinMeshInfo;
    if (! pTheMeshManager->LoadTheModel_IntoDrawInfoObject("assets/models/dolphin_xyz_n_rgba_UV.ply",
                dolphinMeshInfo, true, true, true, 1.0f))
    {
        std::cout << "Dolphin NOT loaded into VAO!" << std::endl;
    }
    else
    {
        // Dolphin model loaded into the dolphinMeshInfo

        // Overwrite the UV coords with the spherical coordinates
        pTheMeshManager->GenTextureCoordsSpherical(dolphinMeshInfo,
                                cVAOManager::enumTEXCOORDBIAS::POSITIVE_X,
                    	        cVAOManager::enumTEXCOORDBIAS::POSITIVE_Y, 
                                true,   // Used the normals from the model
                                1.0f,   // 1.0 = UVs go from 0.0 to 1.0
                                true); // Can't remember what I was doing here...
        
        // Now load it into the VAO
        pTheMeshManager->LoadModelDrawInfo_IntoVAO(dolphinMeshInfo, program);
    }


    sModelDrawInfo teaPotDrawInfo;
    if (!pTheMeshManager->LoadTheModel_IntoDrawInfoObject("assets/models/Utah_Teapot_xyz_n_rgba_UV.ply",
        teaPotDrawInfo, true, true, true, 1.0f))
    {
        std::cout << "Teapot NOT loaded!" << std::endl;
    }
    else
    {
        // Dolphin model loaded into the dolphinMeshInfo

        // Overwrite the UV coords with the spherical coordinates
        pTheMeshManager->GenTextureCoordsSpherical(teaPotDrawInfo,
            cVAOManager::enumTEXCOORDBIAS::POSITIVE_X,
            cVAOManager::enumTEXCOORDBIAS::POSITIVE_Y,
            true,   // Used the normals from the model
            1.0f,   // 1.0 = UVs go from 0.0 to 1.0
            true); // Can't remember what I was doing here...

        // Now load it into the VAO
        pTheMeshManager->LoadModelDrawInfo_IntoVAO(teaPotDrawInfo, program);
    }


    // Load the original warehouse models
    sModelDrawInfo warehousePartModelInfo;
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Crate1.ply", program);
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Crate2.ply", program);
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Floor.ply", program);
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Loading_001.ply", program);
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Loading_Pallets.ply", program);
    pTheMeshManager->LoadModelIntoVAO_2("assets/models/Warehouse_Loading_Roof.ply", program);


    std::cout << pTheMeshManager->getListOfLoadedModels();

    return;
}

void LoadTexturesIntoTextureManager(cBasicTextureManager* pTheTextureManager)
{
    pTheTextureManager->SetBasePath("assets/textures");

    // 
    if (pTheTextureManager->Create2DTextureFromBMPFile("Sydney_Sweeney.bmp", true))
    {
        std::cout << "Loaded Sydney_Sweeney.bmp OK";
    };

    pTheTextureManager->Create2DTextureFromBMPFile("Dungeons_2_Texture_01_A.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Grass_Texture_1.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Lava_Texture.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Stone_Texture_1.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Stone_Texture_2.bmp", true);

    pTheTextureManager->Create2DTextureFromBMPFile("cow_xyz_n_rgba_UV_Blender_UV_Unwrap_orange.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("cow_xyz_n_rgba_UV_Blender_UV_Unwrap_grey.bmp", true);

    pTheTextureManager->Create2DTextureFromBMPFile("Brushed_Metal_Texture.bmp", true);

    // Load the original warehouse model textures
    pTheTextureManager->Create2DTextureFromBMPFile("Warehouse_WoodCrate2.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Warehouse_WoodCrate_.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Warehouse_Fiber.bmp", true);
    pTheTextureManager->Create2DTextureFromBMPFile("Warehouse_WrappedPallet.bmp", true);

    //    if ( ::g_pTheTextures->CreateCubeTextureFromBMPFiles(
    //        "Space",


        //GLuint SydSwee_TID = ::g_pTheTextures->getTextureIDFromName("Sydney_Sweeney.bmp");


    return;
}

void LoadModelsIntoScene()
{
//    cMeshObject* pSkyboxSphere = new cMeshObject();
//    pSkyboxSphere->uniqueName = "Skybox_Sphere";
//    pSkyboxSphere->meshFileName = "assets/models/Isoshphere_smooth_xyz_n_rgba_uv.ply";
//    pSkyboxSphere->textureNames[0] = "Sydney_Sweeney.bmp";
//    pSkyboxSphere->textureMixRatio[0] = 1.0f;
//    pSkyboxSphere->scale = 5.0f;
//    pSkyboxSphere->position = glm::vec3(15.0f, 0.0f, -15.0f);
//    ::g_pMeshesToDraw.push_back(pSkyboxSphere);
        
        
    {
        glm::vec3 warehouseLocationXYZ = glm::vec3(-50.0f, -20.0f, 100.0f);
        glm::vec3 warehouseOrientation = glm::vec3(0.0f, 90.0f, 0.0f);

        // The original warehouse model made of different parts and textures

        // Warehouse_Crate1.ply    Warehouse_WoodCrate2.png
        cMeshObject* pWarehouse_Crate1 = new cMeshObject();
        pWarehouse_Crate1->position = warehouseLocationXYZ;
        pWarehouse_Crate1->orientation = warehouseOrientation;
        pWarehouse_Crate1->meshFileName = "assets/models/Warehouse_Crate1.ply";
        pWarehouse_Crate1->textureNames[0] = "Warehouse_WoodCrate2.bmp";
        pWarehouse_Crate1->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Crate1);


        // Warehouse_Crate2.ply    Warehouse_WoodCrate_.png
        cMeshObject* pWarehouse_Crate2 = new cMeshObject();
        pWarehouse_Crate2->position = warehouseLocationXYZ;
        pWarehouse_Crate2->orientation = warehouseOrientation;
        pWarehouse_Crate2->meshFileName = "assets/models/Warehouse_Crate2.ply";
        pWarehouse_Crate2->textureNames[0] = "Warehouse_WoodCrate_.bmp";
        pWarehouse_Crate2->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Crate2);

        // Warehouse_Floor.ply     Warehouse_Fiber.png
        cMeshObject* pWarehouse_Floor = new cMeshObject();
        pWarehouse_Floor->position = warehouseLocationXYZ;
        pWarehouse_Floor->orientation = warehouseOrientation;
        pWarehouse_Floor->meshFileName = "assets/models/Warehouse_Floor.ply";
        pWarehouse_Floor->textureNames[0] = "Warehouse_Fiber.bmp";
        pWarehouse_Floor->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Floor);

        // Warehouse_Loading_001.ply   Warehouse_Fiber.png
        cMeshObject* pWarehouse_Loading_001 = new cMeshObject();
        pWarehouse_Loading_001->position = warehouseLocationXYZ;
        pWarehouse_Loading_001->orientation = warehouseOrientation;
        pWarehouse_Loading_001->meshFileName = "assets/models/Warehouse_Loading_001.ply";
        pWarehouse_Loading_001->textureNames[0] = "Warehouse_Fiber.bmp";
        pWarehouse_Loading_001->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Loading_001);

        // Warehouse_Loading_Pallets.ply   Warehouse_WrappedPallet.png
        cMeshObject* pWarehouse_Loading_Pallets = new cMeshObject();
        pWarehouse_Loading_Pallets->position = warehouseLocationXYZ;
        pWarehouse_Loading_Pallets->orientation = warehouseOrientation;
        pWarehouse_Loading_Pallets->meshFileName = "assets/models/Warehouse_Loading_Pallets.ply";
        pWarehouse_Loading_Pallets->textureNames[0] = "Warehouse_WrappedPallet.bmp";
        pWarehouse_Loading_Pallets->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Loading_Pallets);

        // Warehouse_Loading_Roof.ply  Warehouse_Fiber.png
        cMeshObject* pWarehouse_Loading_Roof = new cMeshObject();
        pWarehouse_Loading_Roof->position = warehouseLocationXYZ;
        pWarehouse_Loading_Roof->orientation = warehouseOrientation;
        pWarehouse_Loading_Roof->meshFileName = "assets/models/Warehouse_Loading_Roof.ply";
        pWarehouse_Loading_Roof->textureNames[0] = "Warehouse_Fiber.bmp";
        pWarehouse_Loading_Roof->textureMixRatio[0] = 1.0f;
        ::g_pMeshesToDraw.push_back(pWarehouse_Loading_Roof);
    }




    cMeshObject* pWarehouse = new cMeshObject();
//    pWarehouse->meshFileName = "assets/models/Warehouse_xyz_n_rgba_UV (MeshLab_XY_Project).ply";
    pWarehouse->meshFileName = "assets/models/Warehouse_xyz_n_rgba_UV (Blender Smart UV project).ply";
    pWarehouse->uniqueName = "The Warehouse";
    pWarehouse->position.y = -20.0f;
    pWarehouse->position.z = 100.0f;
    pWarehouse->position.x = 50.0f;
//    pWarehouse->orientation.y = 90.0f;

    pWarehouse->textureNames[0] = "Sydney_Sweeney.bmp";
    pWarehouse->textureMixRatio[0] = 1.0f;

    pWarehouse->textureNames[1] = "Dungeons_2_Texture_01_A.bmp";
    pWarehouse->textureMixRatio[1] = 0.0f;

    pWarehouse->textureNames[2] = "Stone_Texture_2.bmp";
    pWarehouse->textureMixRatio[2] = 0.0f;

    pWarehouse->textureMixRatio[3] = 0.0f;

    ::g_pMeshesToDraw.push_back(pWarehouse);






    cMeshObject* pFloor = new cMeshObject();
    pFloor->bOverrideVertexModelColour = true;
    pFloor->colourRGB = glm::vec3(0.7f, 0.7f, 0.7f);
    //pFloor->position.x = -10.f;
    //pFloor->orientation.z = 90.0f;
    pFloor->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03.ply";
    pFloor->textureNames[0] = "Dungeons_2_Texture_01_A.bmp";
    pFloor->textureMixRatio[0] = 0.5f;
    ::g_pMeshesToDraw.push_back(pFloor);



    cMeshObject* pTeaPot = new cMeshObject();
    pTeaPot->position.x = -30.f;
    pTeaPot->position.y = 10.0f;
    pTeaPot->scale = 0.2f;
    pTeaPot->meshFileName = "assets/models/Utah_Teapot_xyz_n_rgba_UV.ply";
//    pTeaPot->textureNames[0] = "Sydney_Sweeney.bmp";
    pTeaPot->textureNames[0] = "Brushed_Metal_Texture.bmp";
    pTeaPot->textureMixRatio[0] = 1.0f;
    ::g_pMeshesToDraw.push_back(pTeaPot);


    // HACK: Load the dungeon map
//    XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//    X...X.........X.......X.....X
//    X.XXX.X.....XXX.X.XXX.X.XXX.X
//    X...........X...X...X.X.X...X
//    X...........X.XXXXX.X.XXX.X.X
//    X.......X...X...X...X.X...X.X
//    X.......X.XXXXX.X.XXX.X.X.XXX
//    X.........X.....X.X.....X.X..
//    X.XX....XXX.XXXXX.XXXXXXX.X.X
//    ........X...X.X...X...X...X.X
//    X.......X.XXX.X.XXX.X.XXXXX.X
//    X.........X...X.....X.X.....X
//    X.......XXX.X.XXXXXXX.X.XXXXX
//    X...........X.......X.......X
//    XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    std::vector<std::string> vecTheMap;
    std::ifstream theDungeonMapFile("assets/DungeonMap.txt");
    if (theDungeonMapFile.is_open())
    {
        // Openned the map
        std::string theRow;
        while (theDungeonMapFile >> theRow)
        {
            vecTheMap.push_back(theRow);
        }
    }

    // Cell 3, 4
    char theCell = vecTheMap[3][4];

    const float floorTileWidth = 10.0f;
    const float floorOffset = -50.0f;

    for (unsigned int row = 0; row != vecTheMap.size(); row++)
    {
        for (unsigned int col = 0; col != vecTheMap[0].length(); col++)
        {
            cMeshObject* pFloor = new cMeshObject();
            pFloor->bOverrideVertexModelColour = true;
            pFloor->colourRGB = glm::vec3(0.7f, 0.7f, 0.7f);
            pFloor->position.x = row * floorTileWidth + floorOffset;
            pFloor->position.z = col * floorTileWidth + floorOffset;
            pFloor->position.y = -25.0f;

            pFloor->specularHihglightRGB = glm::vec3(1.0f, 1.0f, 1.0f);
            pFloor->specularPower = 1000.0f;

            pFloor->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03.ply";

            pFloor->textureNames[0] = "Dungeons_2_Texture_01_A.bmp";
            pFloor->textureMixRatio[0] = 1.0f;

            ::g_pMeshesToDraw.push_back(pFloor);
        }
    }

    for (float z = -100.0f; z < 101.0f; z += 10.0f)
    {
        cMeshObject* pCow = new cMeshObject();
        pCow->bOverrideVertexModelColour = true;
        pCow->colourRGB =
            glm::vec3(
                ::g_getRandBetween(0.3f, 1.0f),
                ::g_getRandBetween(0.3f, 1.0f),
                ::g_getRandBetween(0.3f, 1.0f));
        pCow->specularHihglightRGB = glm::vec3(1.0f, 1.0f, 1.0f);
        pCow->specularPower = (float)::g_getRandBetween(1.0f, 10'000.0f);
        pCow->position.x = 0.f;
        pCow->position.y = 0.0f;
        pCow->position.z = z;
        // Transparent cows
        pCow->transparencyAlpha = 0.6f;
        pCow->meshFileName = "assets/models/cow_xyz_n_rgba_UV (MeshLab trivial, random).ply";

        pCow->textureNames[0] = "Lava_Texture.bmp";
        pCow->textureMixRatio[0] = 1.0f;

        ::g_pMeshesToDraw.push_back(pCow);
    }


    //cMeshObject* pBottle = new cMeshObject();
    //pBottle->bOverrideVertexModelColour = true;
    //pBottle->colourRGB = glm::vec3(1.0f, 1.0f, 1.0f);
    //pBottle->meshFileName = "assets/models/Dungeon_models/Props and Decorations/SM_Item_Bottle_01__.ply";
    //::g_pMeshesToDraw.push_back(pBottle);




    cMeshObject* pCow = new cMeshObject();
    pCow->bOverrideVertexModelColour = true;
    pCow->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
    pCow->position.x = -10.f;
    pCow->orientation.z = 90.0f;
    pCow->uniqueName = "Betsy";
    pCow->meshFileName = "assets/models/cow_xyz_n_rgba_UV (MeshLab trivial, random).ply";
    pCow->textureNames[0] = "Stone_Texture_1.bmp";
    pCow->textureMixRatio[0] = 1.0f;

    cMeshObject* pCow2 = new cMeshObject();
    pCow2->bIsWireframe = false;
    //pCow2->bOverrideVertexModelColour = true;
    //pCow2->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
    pCow2->position.x = 10.f;
    pCow2->scale = 0.5f;
    pCow2->meshFileName = "assets/models/cow_xyz_n_rgba_UV (MeshLab trivial, random).ply";
    pCow2->textureNames[0] = "cow_xyz_n_rgba_UV_Blender_UV_Unwrap_orange.bmp";
    pCow2->textureMixRatio[0] = 1.0f;

    ::g_pMeshesToDraw.push_back(pCow);
    ::g_pMeshesToDraw.push_back(pCow2);

    cMeshObject* pCow3 = new cMeshObject();
    pCow3->bIsWireframe = false;
    pCow3->position.x = 20.f;
    pCow3->position.y = 10.f;
    pCow3->scale = 0.5f;
    pCow3->meshFileName = "assets/models/cow_xyz_n_rgba_UV (MeshLab trivial, random).ply";

    pCow3->textureNames[3] = "cow_xyz_n_rgba_UV_Blender_UV_Unwrap_grey.bmp";
    pCow3->textureMixRatio[0] = 0.0f;
    pCow3->textureMixRatio[1] = 0.0f;
    pCow3->textureMixRatio[2] = 0.0f;
    pCow3->textureMixRatio[3] = 1.0f;   // <-- Grass is #3

    ::g_pMeshesToDraw.push_back(pCow3);

    cMeshObject* pDolphin = new cMeshObject();
    pDolphin->meshFileName = "assets/models/dolphin_xyz_n_rgba_UV.ply";
    pDolphin->scale = 0.01f;
    pDolphin->position.y = 10.0f;
    pDolphin->orientation.z = 45.0f;
    pDolphin->textureNames[0] = "Sydney_Sweeney.bmp";
    pDolphin->textureMixRatio[0] = 1.0f;

    ::g_pMeshesToDraw.push_back(pDolphin);

    cMeshObject* pDolphin2 = new cMeshObject();
    pDolphin2->meshFileName = "assets/models/dolphin_xyz_n_rgba_UV.ply";
    pDolphin2->scale = 0.02f;
    pDolphin2->position.y = -10.0f;
    pDolphin2->orientation.z = -45.0f;
    pDolphin2->textureNames[0] = "Dungeons_2_Texture_01_A.bmp";
    pDolphin2->textureMixRatio[0] = 1.0f;

    ::g_pMeshesToDraw.push_back(pDolphin2);


    return;
}

