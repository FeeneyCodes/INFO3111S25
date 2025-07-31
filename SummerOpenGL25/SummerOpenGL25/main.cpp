//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "GlobalOpenGL.h"
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>

#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightManager.h"
#include "cMeshObject.h"
#include "cLightHelper/cLightHelper.h"
#include "globalStuff.h"

bool g_ShowLightDebugSpheres = false;


cBasicFlyCamera* g_pFlyCamera = NULL;


double g_getRandBetween0and1(void)
{
    return ((double)rand() / ((double)RAND_MAX));
}

double g_getRandBetween(float min, float max)
{
    double zeroToOne = ((double)rand() / (RAND_MAX));
    double value = (zeroToOne * (max - min)) + min;
    return value;
}


cShaderManager* g_pTheShaderManager = NULL;
cVAOManager* g_pMeshManager = NULL;
cLightManager* g_pLights = NULL;

cMeshObject* g_pSmoothSphere = NULL;

extern unsigned int g_selectedLightIndex;

unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;
// Now getting this from the fly camera
//glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, -30.0f);

void LoadFilesIntoVAOManager(GLuint program);

std::vector<cMeshObject*> g_pMeshesToDraw;
void LoadModelsIntoScene();

void DrawMesh(cMeshObject* pCurrentMesh, GLint program);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// GLFW callback declarations:
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// Set the callbacks for the mouse
// https://www.glfw.org/docs/3.3/input_guide.html#input_mouse
// Set with glfwSetCursorPosCallback(window, cursor_position_callback);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
// Set with glfwSetCursorEnterCallback(window, cursor_enter_callback);
void cursor_enter_callback(GLFWwindow* window, int entered);
// Set with glfwSetMouseButtonCallback(window, mouse_button_callback);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// Set with glfwSetScrollCallback(window, scroll_callback);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// In the mouse_keyboard_async.cpp file:
void handleKeyboardAsync(GLFWwindow* window);
void handleMouseAsync(GLFWwindow* window);


// Sort predicate fucntion shell
//  std::vector<cMeshObject*> vecTransparentThings;
bool pred_isObjectA_Less_Than_ObjectB(cMeshObject* pA, cMeshObject* pB)
{
    // If A is "less than" B (whatever that means
    return true;
}

// Returns NULL if we didn't find the object
cMeshObject* g_pFindObjectByUniqueName(std::string theNameToFind)
{
    for (cMeshObject* pTestMesh : ::g_pMeshesToDraw)
    {
        // Is this it?
        if (pTestMesh->uniqueName == theNameToFind)
        {
            // Yup
            return pTestMesh;
        }
    }
        // Didn't find it
    return NULL;

}



int main(void)
{
    std::cout << g_getRandBetween(-10.0f, 10.0f) << std::endl;
    std::cout << g_getRandBetween(-10.0f, 10.0f) << std::endl;
    std::cout << g_getRandBetween(0.0f, 10.0f) << std::endl;


    GLFWwindow* window;


    GLuint program;
    GLint mvp_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    ::g_pFlyCamera = new cBasicFlyCamera();
    ::g_pFlyCamera->setEyeLocation(0.0f, 0.0f, -30.0f);

    glfwSetKeyCallback(window, key_callback);
    // And the mouse callbacks, too:
    glfwSetCursorPosCallback(window, cursor_position_callback);   
    glfwSetCursorEnterCallback(window, cursor_enter_callback);    
    glfwSetMouseButtonCallback(window, mouse_button_callback);    
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);



    ::g_pTheShaderManager = new cShaderManager();

    ::g_pTheShaderManager->setBasePath("assets/shaders/");

    cShaderManager::cShader vert_shader;
    vert_shader.fileName = "vertex_shader.glsl";

    cShaderManager::cShader frag_shader;
    frag_shader.fileName = "fragment_shader.glsl";


    if (::g_pTheShaderManager->createProgramFromFile("shader1", vert_shader, frag_shader))
    {
        std::cout << "Shaders succesfully created!" << std::endl;
    }
    else
    {
        std::cout << ::g_pTheShaderManager->getLastError() << std::endl;
    }

    program = ::g_pTheShaderManager->getIDFromFriendlyName("shader1");

    mvp_location = glGetUniformLocation(program, "MVP");

    LoadFilesIntoVAOManager(program);

    LoadModelsIntoScene();

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    ::g_pLights = new cLightManager();
    ::g_pLights->GetUniformLocations(program);

    ::g_pLights->theLights[0].param2.x = 1.0f; // turn on
    ::g_pLights->theLights[0].param1.x = 0.0f; // light type = point light
    ::g_pLights->theLights[0].position = glm::vec4(0.0f, 50.0f, -25.0f, 1.0f);
    ::g_pLights->theLights[0].diffuse = glm::vec4(1.0f);

    ::g_pLights->theLights[0].atten.x = 0.0f; // constant
    ::g_pLights->theLights[0].atten.y = 0.01f; // linear
    ::g_pLights->theLights[0].atten.z = 0.001f; // quadratic



    ::g_pLights->theLights[1].param2.x = 1.0f; // turn on
    ::g_pLights->theLights[1].param1.x = 0.0f; // light type = point light
    ::g_pLights->theLights[1].position = glm::vec4(0.0f, 50.0f, 50.0f, 1.0f);
    ::g_pLights->theLights[1].diffuse = glm::vec4(1.0f);

    ::g_pLights->theLights[1].atten.x = 0.0f; // constant
    ::g_pLights->theLights[1].atten.y = 0.003f; // linear
    ::g_pLights->theLights[1].atten.z = 0.005f; // quadratic

    ::g_pLights->theLights[2].param2.x = 1.0f; // turn on
    ::g_pLights->theLights[2].param1.x = 1.0f; // 1 = SPOTLIGHT (for our shader)
    // Other spotlight thing
    // Direction is relative to the light
    ::g_pLights->theLights[2].direction
        = glm::vec4(0.0, -1.0f, 0.0f, 1.0f);
    // Angles from the Light To Direction "ray"
    // y = inner angle, z = outer angle
    ::g_pLights->theLights[2].param1.z = 10.0f; // Outer
    ::g_pLights->theLights[2].param1.z = 5.0f;  // Inner


    ::g_pLights->theLights[2].position = glm::vec4(0.0f, 50.0f, 0.0f, 1.0f);
    ::g_pLights->theLights[2].diffuse = glm::vec4(1.0f);

    ::g_pLights->theLights[2].atten.x = 0.0f; // constant
    ::g_pLights->theLights[2].atten.y = 0.003f; // linear
    ::g_pLights->theLights[2].atten.z = 0.0005f; // quadratic




//    // Enable blend function
//    glEnable(GL_BLEND);
//    // "alpha" blending transparancy
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    const unsigned int MAX_NUMNBER_OF_SOLID_OBJECT = 1000000;
//    cMeshObject* arrayOfSolidThing[MAX_NUMNBER_OF_SOLID_OBJECT];
//    cMeshObject* arrayOfTransparent[MAX_NUMNBER_OF_SOLID_OBJECT];


    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 matProj, matView;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        GLint proj_location = glGetUniformLocation(program, "mProj");
        GLint view_location = glGetUniformLocation(program, "mView");


        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        matProj = glm::perspective(
            0.6f,           // FOV  60 degrees
            ratio,          // Aspect ratio
            1.0f,           // "near plane"
            1000000.0f);       // "far plane"

        

        matView = glm::mat4(1.0f);

//      Now coming from the FlyCamera
//        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

//        matView = glm::lookAt(
//            g_cameraEye,  
//            cameraTarget,  
//            upVector);     
        matView = glm::lookAt(
            ::g_pFlyCamera->getEyeLocation(),  
            ::g_pFlyCamera->getTargetLocation(),  
            upVector);     

        GLint eyeLocation_UL = glGetUniformLocation(program, "eyeLocation");

//        glUniform3f(eyeLocation_UL,
//            ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z);

        glm::vec3 currentEyeLocation = ::g_pFlyCamera->getEyeLocation();
        glUniform3f(eyeLocation_UL,
            currentEyeLocation.x,
            currentEyeLocation.y,
            currentEyeLocation.z);

        glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(matView));

        ::g_pLights->UpdateShaderUniforms(program);

        std::vector<cMeshObject*> vecSolidThings;
        std::vector<cMeshObject*> vecTransparentThings;


 //       struct cThingDistance
 //       {
 //           float distToCamera;
 //           cMeshObject* pMeshObject;
 //       };
 //       std::vector<cThingDistance> vecTransparentThings;

        // Separate transparent from non-transparent
        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++)
        {
            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
            // Transparent? 
            if (pCurrentMesh->transparencyAlpha < 1.0f)
            {
                // It's transparent
                vecTransparentThings.push_back(pCurrentMesh);
            }
            else
            {
                // It's solid
                vecSolidThings.push_back(pCurrentMesh);
            }
        }

        // Sort transparent from "far from camera to near"
        for (unsigned int index = 0; index != vecTransparentThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecTransparentThings[index];
            // Distance from object to camera
//            float distToCamera = glm::distance(::g_cameraEye, pCurrentMesh->position);
        }
//        std::sort(vecTransparentThings.begin(),
//            vecTransparentThings.end(),
 //           pred_isObjectA_Less_Than_ObjectB );

        // Sort them
        // 1 pass of the bubble sort
        // Monkey sort
        // Beer at the camp fire sort
        
        // Memory = zero
        // CPU bound
        // 500-1000


        // Draw NON transparent (solid) thing FIRST
        for (unsigned int index = 0; index != vecSolidThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecSolidThings[index];
            DrawMesh(pCurrentMesh, program);
        }


        // Enable blend function
        glEnable(GL_BLEND);
        // "alpha" blending transparancy
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ALl transparent things second
        for (unsigned int index = 0; index != vecTransparentThings.size(); index++)
        {
            cMeshObject* pCurrentMesh = vecTransparentThings[index];
            DrawMesh(pCurrentMesh, program);
        }

        // Disable it again
        glDisable(GL_BLEND);




//        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++)
//        {
//            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
//            DrawMesh(pCurrentMesh, program);
//        }


        if (::g_ShowLightDebugSpheres)
        {
            /*::g_pSmoothSphere = new cMeshObject();
            ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
            ::g_pSmoothSphere->bIsWireframe = true;
            ::g_pSmoothSphere->bOverrideVertexModelColour = true;
            ::g_pSmoothSphere->colourRGB = glm::vec4(1.0f);
            ::g_pSmoothSphere->scale = 0.2f;

            ::g_pSmoothSphere->position = glm::vec3(
                ::g_pLights->theLights[::g_selectedLightIndex].position.x,
                ::g_pLights->theLights[::g_selectedLightIndex].position.y,
                ::g_pLights->theLights[::g_selectedLightIndex].position.z);

            DrawMesh(g_pSmoothSphere, program);*/
            ::g_pSmoothSphere = new cMeshObject();
            ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
            ::g_pSmoothSphere->bIsWireframe = true;
            ::g_pSmoothSphere->bOverrideVertexModelColour = true;
            ::g_pSmoothSphere->position = glm::vec3(
                ::g_pLights->theLights[::g_selectedLightIndex].position.x,
                ::g_pLights->theLights[::g_selectedLightIndex].position.y,
                ::g_pLights->theLights[::g_selectedLightIndex].position.z);

            cLightHelper lightHelper;

            const float errorValueforLightLevelGuess = 0.01f;
            const float infiniteDistance = 10000.0f;

            // where the light located
            ::g_pSmoothSphere->scale = 0.1f;
            ::g_pSmoothSphere->colourRGB = glm::vec3(1.0f);
            DrawMesh(g_pSmoothSphere, program);

            float distanceAt75Percent = lightHelper.calcApproxDistFromAtten(0.75f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt75Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
            DrawMesh(g_pSmoothSphere, program);

            float distanceAt50Percent = lightHelper.calcApproxDistFromAtten(0.5f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt50Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
            DrawMesh(g_pSmoothSphere, program);

            float distanceAt25Percent = lightHelper.calcApproxDistFromAtten(0.25f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt25Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 0.0f, 1.0f);
            DrawMesh(g_pSmoothSphere, program);

            // 10% brightness - "dark"
            float distanceAt10Percent = lightHelper.calcApproxDistFromAtten(0.1f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt10Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 1.0f, 1.0f);
            DrawMesh(g_pSmoothSphere, program);


        }//if (::g_ShowLightDebugSpheres)

        // Per frame stuff:

        // "flicker" the lights a little bit
//        double aRandom = (::g_getRandBetween0and1() - 0.5) * 0.01;
//       ::g_pLights->theLights[::g_selectedLightIndex].atten.y += (float)aRandom;
//        ::g_pLights->theLights[::g_selectedLightIndex].position.x += 0.01f;

        

        // Per frame update
        // 
        // Point the spotlight at Betsy the cow
        cMeshObject* pBetsy = ::g_pFindObjectByUniqueName("Betsy");
        if (pBetsy)       // is (pBetsy != 0) or pBetsy != NULL
        {
            // Direction is the ray between the light and the cow
            glm::vec3 pointTowardCow
                = pBetsy->position - glm::vec3(::g_pLights->theLights[2].position);
            // Normalize this ray (make it length 1.0f)
            pointTowardCow = glm::normalize(pointTowardCow);

            ::g_pLights->theLights[2].direction = glm::vec4(pointTowardCow, 1.0f);
        }



        // Place stuff on the window title
 
        std::stringstream ssWindowTitle;

//        ssWindowTitle << "Camera (XYZ)" << ::g_cameraEye.x << ","
//            << ::g_cameraEye.y << ", " << ::g_cameraEye.z
        ssWindowTitle << "Camera (XYZ)" 
            << ::g_pFlyCamera->getEyeLocation().x << ","
            << ::g_pFlyCamera->getEyeLocation().y << ", " 
            << ::g_pFlyCamera->getEyeLocation().z
            << " "
            << "Light[" << ::g_selectedLightIndex << "]: "
            << "(xyz): " 
            << ::g_pLights[::g_selectedLightIndex].theLights->position.x
            << ", "
            << ::g_pLights[::g_selectedLightIndex].theLights->position.y
            << ", "
            << ::g_pLights[::g_selectedLightIndex].theLights->position.z
            << "  atten(Lin,Quad): "
            << ::g_pLights[::g_selectedLightIndex].theLights->atten.y
            << ", "
            << ::g_pLights[::g_selectedLightIndex].theLights->atten.z;

        glfwSetWindowTitle(window, ssWindowTitle.str().c_str());

       /* if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/cow.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices, 
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/Utah_Teapot.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/dolphin.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }*/


        // Switch the back buffers (double buffered framebuffer)
        glfwSwapBuffers(window);
        // Process mouse and keyboard events
        glfwPollEvents();

        handleKeyboardAsync(window);
        handleMouseAsync(window);

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void LoadFilesIntoVAOManager(GLuint program)
{
    ::g_pMeshManager = new cVAOManager();

    // Load the dungeon floow model
    sModelDrawInfo meshFloor03;

    // Scale we want for the floor. 
    // They are 500 units wide.
    float newFloorScale = 10.0f/ 500.0f;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03_xyz_n.ply",
        meshFloor03, program, true, false, false, newFloorScale))
    {
        std::cout << "Floor didn't load not loaded into VAO!" << std::endl;
    }


    sModelDrawInfo meshInfoCow;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/cow_xyz_n_rgba.ply",
        meshInfoCow, program, true, true, false, 1.0f))
    {
        std::cout << "Cow not loaded into VAO!" << std::endl;
    }


    sModelDrawInfo dolphinMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/dolphin_xyz_n_rgba.ply",
        dolphinMeshInfo, program, true, true, false, 1.0f))
    {
        std::cout << "Dolphin NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo WarehouseMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n_rgba.ply",
        WarehouseMeshInfo, program, true, true, false, 1.0f))
    {
        std::cout << "Warehouse NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo SmoothSphereMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply",
        SmoothSphereMeshInfo, program, true, true, false, 1.0f))
    {
        std::cout << "SmoothSphere NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo bottleMesh;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Dungeon_models/Props and Decorations/SM_Item_Bottle_01__.ply",
        bottleMesh, program, true, true, false, 1.0f))
    {
        std::cout << "SmoothSphere NOT loaded into VAO!" << std::endl;
    }

    return;
}

void LoadModelsIntoScene()
{
    cMeshObject* pFloor = new cMeshObject();
    pFloor->bOverrideVertexModelColour = true;
    pFloor->colourRGB = glm::vec3(0.7f, 0.7f, 0.7f);
    //pFloor->position.x = -10.f;
    //pFloor->orientation.z = 90.0f;
    pFloor->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03_xyz_n.ply";
    ::g_pMeshesToDraw.push_back(pFloor);


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
            pFloor->position.y = -10.0f;

            pFloor->specularHihglightRGB = glm::vec3(1.0f, 1.0f, 1.0f);
            pFloor->specularPower = 1000.0f;

            pFloor->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_03_xyz_n.ply";
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
        pCow->specularPower = ::g_getRandBetween(1.0, 10'000.0f);
        pCow->position.x = 0.f;
        pCow->position.y = 0.0f;
        pCow->position.z = z;
        // Transparent cows
        pCow->transparencyAlpha = 0.6f;
        pCow->meshFileName = "assets/models/cow_xyz_n_rgba.ply";
        ::g_pMeshesToDraw.push_back(pCow);
    }


    cMeshObject* pBottle = new cMeshObject();
    pBottle->bOverrideVertexModelColour = true;
    pBottle->colourRGB = glm::vec3(1.0f, 1.0f, 1.0f);
    pBottle->meshFileName = "assets/models/Dungeon_models/Props and Decorations/SM_Item_Bottle_01__.ply";
    ::g_pMeshesToDraw.push_back(pBottle);




    cMeshObject* pCow = new cMeshObject();
    pCow->bOverrideVertexModelColour = true;
    pCow->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
    pCow->position.x = -10.f;
    pCow->orientation.z = 90.0f;
    pCow->uniqueName = "Betsy";
    pCow->meshFileName = "assets/models/cow_xyz_n_rgba.ply";

    cMeshObject* pCow2 = new cMeshObject();
    pCow2->bIsWireframe = false;
    //pCow2->bOverrideVertexModelColour = true;
    //pCow2->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
    pCow2->position.x = 10.f;
    pCow2->scale = 0.5f;
    pCow2->meshFileName = "assets/models/cow_xyz_n_rgba.ply";

    ::g_pMeshesToDraw.push_back(pCow);
    ::g_pMeshesToDraw.push_back(pCow2);

    cMeshObject* pDolphin = new cMeshObject();
    pDolphin->meshFileName = "assets/models/dolphin_xyz_n_rgba.ply";
    pDolphin->scale = 0.02f;
    pDolphin->position.y = 10.0f;
    pDolphin->orientation.z = 45.0f;

    ::g_pMeshesToDraw.push_back(pDolphin);

    cMeshObject* pDolphin2 = new cMeshObject();
    pDolphin2->meshFileName = "assets/models/dolphin_xyz_n_rgba.ply";
    pDolphin2->scale = 0.02f;
    pDolphin2->position.y = -10.0f;
    pDolphin2->orientation.z = -45.0f;

    ::g_pMeshesToDraw.push_back(pDolphin2);

    cMeshObject* pWarehouse = new cMeshObject();
    pWarehouse->meshFileName = "assets/models/Warehouse_xyz_n_rgba.ply";
    pWarehouse->position.y = -20.0f;
    pWarehouse->position.z = 300.0f;
    pWarehouse->position.x = -500.0f;
    pWarehouse->orientation.y = 90.0f;

    ::g_pMeshesToDraw.push_back(pWarehouse);
}

void DrawMesh(cMeshObject* pCurrentMesh, GLint program)
{
    if (!pCurrentMesh->bIsVisible)
    {
        return;
    }

    glm::mat4 matModel;
    GLint Model_location = glGetUniformLocation(program, "mModel");

    GLint useOverrideColor_location = glGetUniformLocation(program, "bUseOverrideColor");
    GLint overrideColor_location = glGetUniformLocation(program, "colorOverride");

    if (pCurrentMesh->bOverrideVertexModelColour)
    {
        glUniform3f(overrideColor_location, 
            pCurrentMesh->colourRGB.r,
            pCurrentMesh->colourRGB.g, 
            pCurrentMesh->colourRGB.b);

        glUniform1f(useOverrideColor_location, GL_TRUE); // 1.0f

    }
    else
    {
        glUniform1f(useOverrideColor_location, GL_FALSE);
    }

    // Copy over the transparency
    // uniform float alphaTransparency;
    GLint alphaTransparency_UL 
        = glGetUniformLocation(program, "alphaTransparency");
    // Set it
    glUniform1f(alphaTransparency_UL, pCurrentMesh->transparencyAlpha);

    // Set the specular value
    //uniform vec4 vertSpecular;
    GLint vertSpecular_UL = glGetUniformLocation(program, "vertSpecular");
    // Copy the object specular to the shader
    glUniform4f(vertSpecular_UL,
        pCurrentMesh->specularHihglightRGB.r,
        pCurrentMesh->specularHihglightRGB.g,
        pCurrentMesh->specularHihglightRGB.b,
        pCurrentMesh->specularPower);




    //         mat4x4_identity(m);
    matModel = glm::mat4(1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), pCurrentMesh->position);

    //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.x),
        glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.y),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
        glm::radians(pCurrentMesh->orientation.z),
        glm::vec3(0.0f, 0.0f, 1.0f));

    float uniformScale = pCurrentMesh->scale;
    glm::mat4 scaleXYZ = glm::scale(glm::mat4(1.0f),
        glm::vec3(uniformScale, uniformScale, uniformScale));

    matModel = matModel * translation * rotateX * rotateY * rotateZ * scaleXYZ;


    //m = m * rotateZ;

    //mat4x4_mul(mvp, p, m);
    //mvp = matProj * matView * matModel;

    if (pCurrentMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniformMatrix4fv(Model_location, 1, GL_FALSE, glm::value_ptr(matModel));

    GLint mModelIt_location = glGetUniformLocation(program, "mModel_InverseTranpose");

    // gets rid of any translation (movement) and scaling. leaves only roation
    glm::mat4 matModelIt = glm::inverse(glm::transpose(matModel));
    glUniformMatrix4fv(mModelIt_location, 1, GL_FALSE, glm::value_ptr(matModelIt));

    //glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
    sModelDrawInfo modelToDraw;

    if (::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName,
        modelToDraw))
    {
        glBindVertexArray(modelToDraw.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
            GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}
