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
// Texture loading stuff
#include "cBasicTextureManager/cBasicTextureManager.h"

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
// Textures
cBasicTextureManager* g_pTheTextures = NULL;

cMeshObject* g_pSmoothSphere = NULL;

extern unsigned int g_selectedLightIndex;

unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;
// Now getting this from the fly camera
//glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, -30.0f);


// Moved to LoadModelsAndTextures
void LoadFilesIntoVAOManager(cVAOManager* pTheMeshManager, GLuint program);
void LoadModelsIntoScene();
void LoadTexturesIntoTextureManager(cBasicTextureManager* pTheTextureManager);


std::vector<cMeshObject*> g_pMeshesToDraw;


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


    // 
    GLint maxTextureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS = " << maxTextureUnits << std::endl;
    std::cout << "(This is the total number of textures that can be sampled AT THE SAME TIME in a shader)" << std::endl;

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "GL_MAX_TEXTURE_SIZE = " << maxTextureSize << std::endl;
    std::cout << "(This is the largest dimension your texture can have)" << std::endl;

    GLint maxCombinedTextureUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << maxTextureUnits << std::endl;
    std::cout << "(This is the total number of texture UNITS your card has)" << std::endl;


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


    ::g_pMeshManager = new cVAOManager();
    LoadFilesIntoVAOManager(::g_pMeshManager, program);


    // *******************************************************
    //    _____         _                         
    //   |_   _|____  _| |_ _   _ _ __ ___  ___ _ 
    //     | |/ _ \ \/ / __| | | | '__/ _ \/ __(_)
    //     | |  __/>  <| |_| |_| | | |  __/\__ \_ 
    //     |_|\___/_/\_\\__|\__,_|_|  \___||___(_)
    //                                            
    // Load the textures
    ::g_pTheTextures = new cBasicTextureManager();

    LoadTexturesIntoTextureManager(::g_pTheTextures);

    // *******************************************************



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

    // uniform vec3 ambientRGB;
    GLint ambientRGB_UL = glGetUniformLocation(program, "ambientRGB");
    float overallAmbient = 0.0f;
    glUniform3f(ambientRGB_UL, overallAmbient, overallAmbient, overallAmbient);


//    // Enable blend function
//    glEnable(GL_BLEND);
//    // "alpha" blending transparancy
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    const unsigned int MAX_NUMNBER_OF_SOLID_OBJECT = 1000000;
//    cMeshObject* arrayOfSolidThing[MAX_NUMNBER_OF_SOLID_OBJECT];
//    cMeshObject* arrayOfTransparent[MAX_NUMNBER_OF_SOLID_OBJECT];

    
    // Set this "masking" texture
    //  uniform sampler2D sampMaskTexture01;
    //  uniform bool bUseMaskingTexture;
    GLint bUseMaskingTexture_ID = glGetUniformLocation(program, "bUseMaskingTexture");
    // Set to false at the start
    glUniform1f(bUseMaskingTexture_ID, (GLfloat)GL_TRUE);

    // The skybox textue likely won't change, so we are setting it once at the start
    GLuint sunnydaytextID = ::g_pTheTextures->getTextureIDFromName("SunnyDay");

    // Chose a unique texture unit. Here I pick 20 just because...
    glActiveTexture(GL_TEXTURE20);	
    // Note this ISN'T GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_CUBE_MAP, sunnydaytextID);    // <-- 0 is the texture unit

    // uniform sampler2D textSampler2D_00;	
    GLint skyboxCubeTexture_UL = glGetUniformLocation(program, "skyboxCubeTexture");
    glUniform1i(skyboxCubeTexture_UL, 20);   // (Uniform ID, Texture Unit #)



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
            1000.0f);       // "far plane"

        

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


        // ************************************************
        //    ____  _          _               
        //   / ___|| | ___   _| |__   _____  __
        //   \___ \| |/ / | | | '_ \ / _ \ \/ /
        //    ___) |   <| |_| | |_) | (_) >  < 
        //   |____/|_|\_\\__, |_.__/ \___/_/\_\
        //               |___/                 

        cMeshObject* pSkyBox = g_pFindObjectByUniqueName("skybox_mesh");
        GLint bIsSkyboxObject_UL = glGetUniformLocation(program, "bIsSkyboxObject");
        glUniform1f(bIsSkyboxObject_UL, 1.0f);  // Or GL_TRUE

        if (pSkyBox != NULL)
        {
            pSkyBox->bIsVisible = true;

            // Move this mesh to where the camera is
            pSkyBox->position = g_pFlyCamera->getEyeLocation();

            // uniform bool bIsSkyboxObject;

            DrawMesh(pSkyBox, program);


            pSkyBox->bIsVisible = false;

        }//if (pSkyBox != NULL)

        glUniform1f(bIsSkyboxObject_UL, 0.0f);  // Or GL_FALSE
        //// ************************************************




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

            // Created the debug sphere, yet?
            if (::g_pSmoothSphere == NULL)
            {
                // Nope
                ::g_pSmoothSphere = new cMeshObject();
            }
//            ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba.ply";
            ::g_pSmoothSphere->meshFileName = "assets/models/Isoshphere_smooth_inverted_normals_xyz_n_rgba_uv.ply";
            ::g_pSmoothSphere->bIsWireframe = true;
            ::g_pSmoothSphere->bOverrideVertexModelColour = true;
            //
            ::g_pSmoothSphere->bDoNotLight = true;
            ::g_pSmoothSphere->bUseVertexColours = true;
            //
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
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.5f, 0.0f, 0.0f);
            DrawMesh(g_pSmoothSphere, program);

            float distanceAt50Percent = lightHelper.calcApproxDistFromAtten(0.5f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt50Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 0.5f, 0.0f);
            DrawMesh(g_pSmoothSphere, program);

            float distanceAt25Percent = lightHelper.calcApproxDistFromAtten(0.25f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt25Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 0.0f, 0.5f);
            DrawMesh(g_pSmoothSphere, program);

            // 10% brightness - "dark"
            float distanceAt10Percent = lightHelper.calcApproxDistFromAtten(0.1f,
                errorValueforLightLevelGuess, infiniteDistance,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.x,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.y,
                ::g_pLights->theLights[::g_selectedLightIndex].atten.z);

            ::g_pSmoothSphere->scale = distanceAt10Percent;
            ::g_pSmoothSphere->colourRGB = glm::vec3(0.0f, 0.5f, 0.5f);
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

//        // Change texture ratio on the warehouse
//        cMeshObject* pWarehouse = g_pFindObjectByUniqueName("The Warehouse");
//        if (pWarehouse)
//        {
//            double currentTime = glfwGetTime();
//            float ratio = fabs(glm::sin(currentTime));
//
//            pWarehouse->textureMixRatio[0] = ratio;
//            pWarehouse->textureMixRatio[2] = 1.0f - ratio;
//        }



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

// Moved to LoadModelsAndTextures
// void LoadFilesIntoVAOManager(GLuint program)

// Moved to LoadModelsAndTextures
// void LoadModelsIntoScene()



void SetUpTexturesForObjectDraw(cMeshObject* pCurrentMesh, GLint program)
{
//    GLuint Syd_TexID = ::g_pTheTextures->getTextureIDFromName("Sydney_Sweeney.bmp");
//    GLuint Syd_TexID = ::g_pTheTextures->getTextureIDFromName("Texture_01_A.bmp");

    {   // Texture sampler00:
        GLuint textureID00 = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[0]);
        // Bind this texture to the sampler
        // Choose a texture unit... 
        // Unit: #0
        glActiveTexture(GL_TEXTURE0);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, textureID00);    // <-- 0 is the texture unit
        // At this point, texture "textID00" is bound to texture unit #0

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_00;	
        GLint textSampler2D_00_UL = glGetUniformLocation(program, "textSampler2D_00");
        glUniform1i(textSampler2D_00_UL, 0);   // (Uniform ID, Texture Unit #)
    }

    {   // Texture sampler01:
        GLuint textID01 = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[1]);
        // Bind this texture to the sampler
        // Choose a texture unit... 
        // Unit: #1
        glActiveTexture(GL_TEXTURE1);	// GL_TEXTURE0 = 33984
        // Bind texture to tell texture unit what it's bound to
        glBindTexture(GL_TEXTURE_2D, textID01);    // Note: NOT GL_TEXTURE1

        // Get the sampler (shader) uniform location
        // uniform sampler2D textSampler2D_01;	
        GLint textSampler2D_01_UL = glGetUniformLocation(program, "textSampler2D_01");
        glUniform1i(textSampler2D_01_UL, 1);
    }
    {   // Texture sampler 2:
        
        // Texture bound to texture unit:
        GLuint textID02 = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[2]);
        glActiveTexture(GL_TEXTURE2);	
        glBindTexture(GL_TEXTURE_2D, textID02);   

        // Sampler tied to texture unit
        // uniform sampler2D textSampler2D_02;
        GLint textSampler2D_02_UL = glGetUniformLocation(program, "textSampler2D_02");
        glUniform1i(textSampler2D_02_UL, 2);
    }

    {   // Texture sampler 3:

        // Texture bound to texture unit:
        GLuint textID03 = ::g_pTheTextures->getTextureIDFromName(pCurrentMesh->textureNames[3]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textID03);

        // Sampler tied to texture unit
        // uniform sampler2D textSampler2D_03;
        GLint textSampler2D_03_UL = glGetUniformLocation(program, "textSampler2D_03");
        glUniform1i(textSampler2D_03_UL, 3);
    }


    
    // Also the  mix ratios
    // uniform vec4 texMixRatios;	
    GLint texMixRatios_UL = glGetUniformLocation(program, "texMixRatios");
    glUniform4f(texMixRatios_UL,
        pCurrentMesh->textureMixRatio[0],
        pCurrentMesh->textureMixRatio[1],
        pCurrentMesh->textureMixRatio[2],
        pCurrentMesh->textureMixRatio[3]);


    // Set up masking texture
    //  uniform sampler2D sampMaskTexture01;
    {   // Texture sampler 8:
        GLuint maskTextureID = ::g_pTheTextures->getTextureIDFromName("MaskingTexture.bmp");
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, maskTextureID);

        GLint sampMaskTexture01_UL = glGetUniformLocation(program, "sampMaskTexture01");
        glUniform1i(sampMaskTexture01_UL, 8);   
    }



    return;
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

    // If true, DON'T use the textures
    // uniform bool bUseVertexColour;	
    GLint bUseVertexColour_UL = glGetUniformLocation(program, "bUseVertexColour");
    if (pCurrentMesh->bUseVertexColours)
    {
        glUniform1f(bUseVertexColour_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bUseVertexColour_UL, (GLfloat)GL_FALSE);
    }


    // If true, lighting is NOT calcuated
    // uniform bool bDoNotLight;		
    GLint bDoNotLight_UL = glGetUniformLocation(program, "bDoNotLight");
    if (pCurrentMesh->bDoNotLight)
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
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



    SetUpTexturesForObjectDraw(pCurrentMesh, program);


    // 


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


//    // Any child meshes?
//    for (cMeshObject* pChildMesh : pCurrentMesh->vec_pChildObjects)
//    {
//        DrawMesh(pChildMesh);
//    }

    return;
}
