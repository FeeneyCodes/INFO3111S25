#include "cVAOManager.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>


// Generates texture coordinates based on a shperical projection from the 
// origin (0,0,0) location of the model. 
// NOTE: Will overwrite any existing texture coordinates 
// Inspired by: http://www.mvps.org/directx/articles/spheremap.htm
// 
//	basedOnNormals = true --> uses normals for calculation, so good for reflections
//	basedOnNormals = false --> uses model coordinates, so more 'shrink wrapped'
//  fast -> approximate or not (not uses sin() )
//  scale -> normally 1.0, but can be the limits of the texture coord
//
// The 'bias' is what coordinates are used for the generation. 
// For example, if POSITIVE_X is for u and POSITIVE_Y is for v, then:
//	u coords: based on -x to +x
//  v coords: based on -y to +y
//
void cVAOManager::GenTextureCoordsSpherical( sModelDrawInfo& meshInfo,
                                enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias, 
                                bool basedOnNormals, float scale, bool fast )
{
	// determine the 'remaining' coordinate...
	bool xUsed = false;
	bool yUsed = false;
	bool zUsed = false;
	if ( uBias == POSITIVE_X || vBias == POSITIVE_X )	xUsed = true;
	if ( uBias == POSITIVE_Y || vBias == POSITIVE_Y )	yUsed = true;
	if ( uBias == POSITIVE_Z || vBias == POSITIVE_Z )	yUsed = true;
	
	for ( unsigned int vertIndex = 0; vertIndex != meshInfo.numberOfVertices; vertIndex++ )
	{
		sVert curVertex = meshInfo.pVertices[vertIndex];
		
		glm::vec3 xyz = glm::vec3( curVertex.x,
		                           curVertex.y, 
								   curVertex.z );
		
		if ( basedOnNormals )
		{
			if ( uBias == POSITIVE_X )		xyz.x = curVertex.nx;
			else if ( uBias == POSITIVE_Y )	xyz.x = curVertex.ny;
			else if ( uBias == POSITIVE_Z )	xyz.x = curVertex.nz;

			if ( vBias == POSITIVE_X )		xyz.y = curVertex.nx;
			else if ( vBias == POSITIVE_Y )	xyz.y = curVertex.ny;
			else if ( vBias == POSITIVE_Z )	xyz.y = curVertex.nz;

			// Fill in the remaining coordinate...
			if ( !xUsed )	xyz.z = curVertex.nx;
			if ( !yUsed )	xyz.z = curVertex.ny;
			if ( !zUsed )	xyz.z = curVertex.nz;
		}
		else
		{
			if ( uBias == POSITIVE_X )		xyz.x = curVertex.x;
			else if ( uBias == POSITIVE_Y )	xyz.x = curVertex.y;
			else if ( uBias == POSITIVE_Z )	xyz.x = curVertex.z;

			if ( vBias == POSITIVE_X )		xyz.y = curVertex.x;
			else if ( vBias == POSITIVE_Y )	xyz.y = curVertex.y;
			else if ( vBias == POSITIVE_Z )	xyz.y = curVertex.z;

			// Fill in the remaining coordinate...
			if ( !xUsed )	xyz.z = curVertex.x;
			if ( !yUsed )	xyz.z = curVertex.y;
			if ( !zUsed )	xyz.z = curVertex.z;
		}

		glm::normalize(xyz);

		if ( fast )
		{
			meshInfo.pVertices[vertIndex].u = ( ( xyz.x / 2.0f) + 0.5f ) * scale;
			meshInfo.pVertices[vertIndex].v = ( ( xyz.y / 2.0f) + 0.5f ) * scale;
		}
		else
		{
//			meshInfo.pVertices[vertIndex].u = ( ( asin(xyz.x) / PI ) + 0.5f ) * scale;
//			meshInfo.pVertices[vertIndex].v = ( ( asin(xyz.y) / PI ) + 0.5f ) * scale;
			meshInfo.pVertices[vertIndex].u = ( ( asin(xyz.x) / 3.14159f ) + 0.5f ) * scale;
			meshInfo.pVertices[vertIndex].v = ( ( asin(xyz.y) / 3.14159f) + 0.5f ) * scale;
		}
	}
	return;
}
