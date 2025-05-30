#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// CRC: Class, Responsibility, Collboration
// should usually be a noun (and just one thing)
class cMeshObject
{
	public:
	cMeshObject();
	std::string meshFileName;
	std::string uniqueName;
	glm::vec3 position;
	glm::vec3 orientation; // eurler axis
	//glm::vec3 scaleXYZ; 
	float scale; // uniform scale
	glm::vec3 colourRGB;

	bool bOverrideVertexModelColour;
	bool bIsWireframe;
	bool bIsVisible;

	//std::vector<cMeshObject*> vec_pChildObjects; // for later use potentially
};
