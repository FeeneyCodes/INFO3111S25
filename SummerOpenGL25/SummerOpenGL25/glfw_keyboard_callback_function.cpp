#include "GlobalOpenGL.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "globalStuff.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "cMeshObject.h"
#include "cLightManager.h"

// 
// extern glm::vec3 g_cameraEye;
extern std::vector<cMeshObject*> g_pMeshesToDraw;
extern cLightManager* g_pLights;



unsigned int g_selectedObjectIndex = 0;
unsigned int g_selectedLightIndex = 2;


bool isShiftDown(int mods)
{
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        return true;
    }

    return false;
}

bool isCtlDown(int mods)
{
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        return true;
    }

    return false;
}

bool isAltDown(int mods)
{
    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
    {
        return true;
    }

    return false;
}

bool areAnyModifiersDown(int mods)
{
    if (isShiftDown(mods) || isCtlDown(mods) || isAltDown(mods))
    {
        return true;
    }

    return false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float camera_speed = 0.5f;
    const float object_move_speed = 0.7f;

    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        if (key == GLFW_KEY_A)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.x += object_move_speed;
        }

        if (key == GLFW_KEY_D)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.x -= object_move_speed;
        }

        if (key == GLFW_KEY_Q)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.y += object_move_speed;
        }

        if (key == GLFW_KEY_E)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.y -= object_move_speed;
        }

        if (key == GLFW_KEY_W)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.z += object_move_speed;
        }

        if (key == GLFW_KEY_S)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].position.z -= object_move_speed;
        }

        if (key == GLFW_KEY_1)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].atten.y *= 0.99f; // -1% (99%)
        }

        if (key == GLFW_KEY_2)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].atten.y *= 1.01f; // +1% (101%)
        }

        if (key == GLFW_KEY_3)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].atten.z *= 0.99f;    // -0.1%
        }

        if (key == GLFW_KEY_4)
        {
            ::g_pLights->theLights[::g_selectedLightIndex].atten.z *= 1.01f;   // +0.1%
        }
        // ONLY FOR SPOT LIGHTS
        // 5, 6, 7, 8
        if (key == GLFW_KEY_5)
        {
            // Outer angle .1 degree smaller
            ::g_pLights->theLights[::g_selectedLightIndex].param1.z -= 0.1f;   
        }
        if (key == GLFW_KEY_8)
        {
            // Outer angle .1 degree bigger
            ::g_pLights->theLights[::g_selectedLightIndex].param1.z += 0.1f;   
        }
        if (key == GLFW_KEY_6)
        {
            // Outer angle .1 degree smaller
            ::g_pLights->theLights[::g_selectedLightIndex].param1.y -= 0.1f;   
        }
        if (key == GLFW_KEY_7)
        {
            // Outer angle .1 degree bigger
            ::g_pLights->theLights[::g_selectedLightIndex].param1.y += 0.1f;   
        }


        // Select a diffent light
        if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        {
            if (::g_selectedLightIndex >= ::g_pLights->NUMBEROFLIGHTS - 1)
            {
                ::g_selectedLightIndex = 0;
            }
            else
            {
                ++::g_selectedLightIndex;
            }
        }

        if (key == GLFW_KEY_0 && action == GLFW_RELEASE)
        {
            ::g_ShowLightDebugSpheres = true;
        }
        if (key == GLFW_KEY_9 && action == GLFW_RELEASE)
        {
            ::g_ShowLightDebugSpheres = false;
        }

    }

    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        if (key == GLFW_KEY_A)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x += object_move_speed;
        }

        if (key == GLFW_KEY_D)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.x -= object_move_speed;
        }

        if (key == GLFW_KEY_Q)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y += object_move_speed;
        }

        if (key == GLFW_KEY_E)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.y -= object_move_speed;
        }

        if (key == GLFW_KEY_W)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z += object_move_speed;
        }

        if (key == GLFW_KEY_S)
        {
            ::g_pMeshesToDraw[::g_selectedObjectIndex]->position.z -= object_move_speed;
        }

        if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        {
            if (::g_selectedObjectIndex >= ::g_pMeshesToDraw.size() - 1)
            {
                ::g_selectedObjectIndex = 0;  
            }
            else 
            {
                ++::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_O && action == GLFW_RELEASE)
        {
            if (::g_selectedObjectIndex == 0)
            {
                ::g_selectedObjectIndex = ::g_pMeshesToDraw.size() - 1;
            }
            else
            {
                --::g_selectedObjectIndex;
            }
        }

        if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
        {
            std::ofstream mySaveFile("my_scene.scene");

            mySaveFile << ::g_pMeshesToDraw.size() << std::endl;

            for (int index = 0; index != ::g_pMeshesToDraw.size(); index++)
            {
                std::cout << ::g_pMeshesToDraw[index]->meshFileName << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->position.x << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->position.y << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->position.z << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->orientation.x << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->orientation.y << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->orientation.z << std::endl;
                std::cout << ::g_pMeshesToDraw[index]->scale << std::endl;


                mySaveFile << ::g_pMeshesToDraw[index]->meshFileName << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->position.x << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->position.y << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->position.z << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->orientation.x << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->orientation.y << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->orientation.z << std::endl;
                mySaveFile << ::g_pMeshesToDraw[index]->scale << std::endl;
            }
            mySaveFile.close();
        }


        if (key == GLFW_KEY_L && action == GLFW_RELEASE)
        {
            std::ifstream mySaveFile("my_scene.scene");

            if (!mySaveFile.is_open())
            {
                std::cout << "Did not open scene file!" << std::endl;
                return;
            }

            for (auto ptr : ::g_pMeshesToDraw) {
                delete ptr;
            }
            ::g_pMeshesToDraw.clear();

            int meshesInScene = 0;
            mySaveFile >> meshesInScene;

            for (int index = 0; index != meshesInScene; index++)
            {
                cMeshObject* pNewObject = new cMeshObject();

                mySaveFile >> pNewObject->meshFileName;
                mySaveFile >> pNewObject->position.x;
                mySaveFile >> pNewObject->position.y;
                mySaveFile >> pNewObject->position.z;
                mySaveFile >> pNewObject->orientation.x;
                mySaveFile >> pNewObject->orientation.y;
                mySaveFile >> pNewObject->orientation.z;
                mySaveFile >> pNewObject->scale;

                ::g_pMeshesToDraw.push_back(pNewObject);
            }

            mySaveFile.close();
        }
    }

    if (!areAnyModifiersDown(mods))
    {
 //       if (key == GLFW_KEY_A)
 //       {
 //           ::g_cameraEye.x += camera_speed;
 //       }
 //
 //       if (key == GLFW_KEY_D)
 //       {
 //           ::g_cameraEye.x -= camera_speed;
 //       }
 //
 //       if (key == GLFW_KEY_W)
 //       {
 //           ::g_cameraEye.z += camera_speed;
 //       }
 //
 //       if (key == GLFW_KEY_S)
 //       {
 //           ::g_cameraEye.z -= camera_speed;
 //       }
 //
 //       if (key == GLFW_KEY_Q)
 //       {
 //           ::g_cameraEye.y += camera_speed;
 //       }
 //
 //       if (key == GLFW_KEY_E)
 //       {
 //           ::g_cameraEye.y -= camera_speed;
 //       }
    }

 
}