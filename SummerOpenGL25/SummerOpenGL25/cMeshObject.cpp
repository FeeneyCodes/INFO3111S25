#include "cMeshObject.h"

cMeshObject::cMeshObject()
{
	this->meshFileName = "";
	this->uniqueName = "";
	this->position = glm::vec3(0.0f);
	this->orientation = glm::vec3(0.0f);
	this->scale = 1.0f;
	this->colourRGB = glm::vec3(1.0f);

	this->bOverrideVertexModelColour = false;
	this->bIsWireframe = false;
	this->bIsVisible = true;

	this->textureNames[0] = "";
	this->textureNames[1] = "";
	this->textureNames[2] = "";
	this->textureNames[3] = "";


	this->textureMixRatio[0] = 1.0f;
	this->textureMixRatio[1] = 0.0f;
	this->textureMixRatio[2] = 0.0f;
	this->textureMixRatio[3] = 0.0f;

}