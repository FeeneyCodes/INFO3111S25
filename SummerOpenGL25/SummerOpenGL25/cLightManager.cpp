#include "cLightManager.h"

cLight::cLight()
{
    this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // RGB is the specular higlight COLOUR (almost always white)
    // W is the "power" starting at 1.0
    this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->atten = glm::vec4(0.0f, 0.01f, 0.01f, 1.0f);
    // For spot lights
    this->direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->param1.x = 0.0f;	// Point light 
    // If it's a spot light, these will be looked at
    this->param1.y = 0.0f;	// Inner angle
    this->param1.z = 0.0f;	// outer angle

    this->param2.x = 0.0f;	// Off 
    // 
}

void cLightManager::GetUniformLocations(GLuint shaderProgram)
{
    this->theLights[0].position_UL = glGetUniformLocation(shaderProgram, "theLights[0].position");
    this->theLights[0].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[0].diffuse");
    this->theLights[0].specular_UL = glGetUniformLocation(shaderProgram, "theLights[0].specular");
    this->theLights[0].atten_UL = glGetUniformLocation(shaderProgram, "theLights[0].atten");
    this->theLights[0].direction_UL = glGetUniformLocation(shaderProgram, "theLights[0].direction");
    this->theLights[0].param1_UL = glGetUniformLocation(shaderProgram, "theLights[0].param1");
    this->theLights[0].param2_UL = glGetUniformLocation(shaderProgram, "theLights[0].param2");

    this->theLights[1].position_UL = glGetUniformLocation(shaderProgram, "theLights[1].position");
    this->theLights[1].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[1].diffuse");
    this->theLights[1].specular_UL = glGetUniformLocation(shaderProgram, "theLights[1].specular");
    this->theLights[1].atten_UL = glGetUniformLocation(shaderProgram, "theLights[1].atten");
    this->theLights[1].direction_UL = glGetUniformLocation(shaderProgram, "theLights[1].direction");
    this->theLights[1].param1_UL = glGetUniformLocation(shaderProgram, "theLights[1].param1");
    this->theLights[1].param1_UL = glGetUniformLocation(shaderProgram, "theLights[1].param1");

    this->theLights[2].position_UL = glGetUniformLocation(shaderProgram, "theLights[2].position");
    this->theLights[2].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[2].diffuse");
    this->theLights[2].specular_UL = glGetUniformLocation(shaderProgram, "theLights[2].specular");
    this->theLights[2].atten_UL = glGetUniformLocation(shaderProgram, "theLights[2].atten");
    this->theLights[2].direction_UL = glGetUniformLocation(shaderProgram, "theLights[2].direction");
    this->theLights[2].param1_UL = glGetUniformLocation(shaderProgram, "theLights[2].param1");
    this->theLights[2].param1_UL = glGetUniformLocation(shaderProgram, "theLights[2].param1");

    this->theLights[3].position_UL = glGetUniformLocation(shaderProgram, "theLights[3].position");
    this->theLights[3].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[3].diffuse");
    this->theLights[3].specular_UL = glGetUniformLocation(shaderProgram, "theLights[3].specular");
    this->theLights[3].atten_UL = glGetUniformLocation(shaderProgram, "theLights[3].atten");
    this->theLights[3].direction_UL = glGetUniformLocation(shaderProgram, "theLights[3].direction");
    this->theLights[3].param1_UL = glGetUniformLocation(shaderProgram, "theLights[3].param1");
    this->theLights[3].param1_UL = glGetUniformLocation(shaderProgram, "theLights[3].param1");

    this->theLights[4].position_UL = glGetUniformLocation(shaderProgram, "theLights[4].position");
    this->theLights[4].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[4].diffuse");
    this->theLights[4].specular_UL = glGetUniformLocation(shaderProgram, "theLights[4].specular");
    this->theLights[4].atten_UL = glGetUniformLocation(shaderProgram, "theLights[4].atten");
    this->theLights[4].direction_UL = glGetUniformLocation(shaderProgram, "theLights[4].direction");
    this->theLights[4].param1_UL = glGetUniformLocation(shaderProgram, "theLights[4].param1");
    this->theLights[4].param1_UL = glGetUniformLocation(shaderProgram, "theLights[4].param1");

    this->theLights[5].position_UL = glGetUniformLocation(shaderProgram, "theLights[5].position");
    this->theLights[5].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[5].diffuse");
    this->theLights[5].specular_UL = glGetUniformLocation(shaderProgram, "theLights[5].specular");
    this->theLights[5].atten_UL = glGetUniformLocation(shaderProgram, "theLights[5].atten");
    this->theLights[5].direction_UL = glGetUniformLocation(shaderProgram, "theLights[5].direction");
    this->theLights[5].param1_UL = glGetUniformLocation(shaderProgram, "theLights[5].param1");
    this->theLights[5].param1_UL = glGetUniformLocation(shaderProgram, "theLights[5].param1");

    this->theLights[6].position_UL = glGetUniformLocation(shaderProgram, "theLights[6].position");
    this->theLights[6].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[6].diffuse");
    this->theLights[6].specular_UL = glGetUniformLocation(shaderProgram, "theLights[6].specular");
    this->theLights[6].atten_UL = glGetUniformLocation(shaderProgram, "theLights[6].atten");
    this->theLights[6].direction_UL = glGetUniformLocation(shaderProgram, "theLights[6].direction");
    this->theLights[6].param1_UL = glGetUniformLocation(shaderProgram, "theLights[6].param1");
    this->theLights[6].param1_UL = glGetUniformLocation(shaderProgram, "theLights[6].param1");

    this->theLights[7].position_UL = glGetUniformLocation(shaderProgram, "theLights[7].position");
    this->theLights[7].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[7].diffuse");
    this->theLights[7].specular_UL = glGetUniformLocation(shaderProgram, "theLights[7].specular");
    this->theLights[7].atten_UL = glGetUniformLocation(shaderProgram, "theLights[7].atten");
    this->theLights[7].direction_UL = glGetUniformLocation(shaderProgram, "theLights[7].direction");
    this->theLights[7].param1_UL = glGetUniformLocation(shaderProgram, "theLights[7].param1");
    this->theLights[7].param1_UL = glGetUniformLocation(shaderProgram, "theLights[7].param1");

    this->theLights[8].position_UL = glGetUniformLocation(shaderProgram, "theLights[8].position");
    this->theLights[8].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[8].diffuse");
    this->theLights[8].specular_UL = glGetUniformLocation(shaderProgram, "theLights[8].specular");
    this->theLights[8].atten_UL = glGetUniformLocation(shaderProgram, "theLights[8].atten");
    this->theLights[8].direction_UL = glGetUniformLocation(shaderProgram, "theLights[8].direction");
    this->theLights[8].param1_UL = glGetUniformLocation(shaderProgram, "theLights[8].param1");
    this->theLights[8].param1_UL = glGetUniformLocation(shaderProgram, "theLights[8].param1");

    this->theLights[9].position_UL = glGetUniformLocation(shaderProgram, "theLights[9].position");
    this->theLights[9].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[9].diffuse");
    this->theLights[9].specular_UL = glGetUniformLocation(shaderProgram, "theLights[9].specular");
    this->theLights[9].atten_UL = glGetUniformLocation(shaderProgram, "theLights[9].atten");
    this->theLights[9].direction_UL = glGetUniformLocation(shaderProgram, "theLights[9].direction");
    this->theLights[9].param1_UL = glGetUniformLocation(shaderProgram, "theLights[9].param1");
    this->theLights[9].param1_UL = glGetUniformLocation(shaderProgram, "theLights[9].param1");
   
}

void cLightManager::UpdateShaderUniforms(GLuint shaderProgram)
{
    for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++)
    {
        glUniform4f(this->theLights[index].position_UL,		// Uniform location
            this->theLights[index].position.x,
            this->theLights[index].position.y,
            this->theLights[index].position.z,
            this->theLights[index].position.w);

        glUniform4f(this->theLights[index].diffuse_UL,		// Uniform location
            this->theLights[index].diffuse.r,
            this->theLights[index].diffuse.g,
            this->theLights[index].diffuse.b,
            this->theLights[index].diffuse.a);

        glUniform4f(this->theLights[index].specular_UL,		// Uniform location
            this->theLights[index].specular.r,
            this->theLights[index].specular.g,
            this->theLights[index].specular.b,
            this->theLights[index].specular.a);

        glUniform4f(this->theLights[index].atten_UL,		// Uniform location
            this->theLights[index].atten.r,
            this->theLights[index].atten.g,
            this->theLights[index].atten.b,
            this->theLights[index].atten.a);

        glUniform4f(this->theLights[index].direction_UL,	// Uniform location
            this->theLights[index].direction.r,
            this->theLights[index].direction.g,
            this->theLights[index].direction.b,
            this->theLights[index].direction.a);

        glUniform4f(this->theLights[index].param1_UL,		// Uniform location
            this->theLights[index].param1.x,
            this->theLights[index].param1.y,
            this->theLights[index].param1.z,
            this->theLights[index].param1.w);

        glUniform4f(this->theLights[index].param2_UL,		// Uniform location
            this->theLights[index].param2.x,
            this->theLights[index].param2.y,
            this->theLights[index].param2.z,
            this->theLights[index].param2.w);

    }//for ( unsigned int index...
}
