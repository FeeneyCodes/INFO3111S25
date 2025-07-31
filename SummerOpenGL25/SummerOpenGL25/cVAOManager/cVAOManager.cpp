#include "cVAOManager.h"

#include "../GlobalOpenGL.h"

#include <fstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}


bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
		bool hasNormals,
		bool hasColours,
		bool hasTextureCoords,
	    float scaling)	// Set scaling to 1.0 for no change

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	if ( ! this->m_LoadTheModel( fileName, drawInfo, 
		                         hasNormals, hasColours,
		                         hasTextureCoords, scaling))
	{
		this->m_AppendTextToLastError( "Didn't load model", true );
		return false;
	}

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVert) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNorm");
	// Now texture coords
	GLint vTextCoords_location = glGetAttribLocation(shaderProgramID, "vTextCoords");
	

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	// vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVert), 
						   ( void* )offsetof(sVert, x));

	glEnableVertexAttribArray(vnorm_location);	// vNorm
	glVertexAttribPointer(vnorm_location, 4,		// vNorm
		GL_FLOAT, GL_FALSE,
		sizeof(sVert),
		(void*)offsetof(sVert, nx));

	glEnableVertexAttribArray(vcol_location);	// vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
							sizeof(sVert),
						   ( void* )offsetof(sVert,r));

	// Tell it where the uv values are
	glEnableVertexAttribArray(vTextCoords_location);	// vTextCoords
	glVertexAttribPointer(vTextCoords_location,			// vTextCoords
	                      2,		
						   GL_FLOAT, 
		                   GL_FALSE,
							sizeof(sVert),
						   ( void* )offsetof(sVert,u));



	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vnorm_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vTextCoords_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}



// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}


bool cVAOManager::m_LoadTheModel(std::string fileName,
								 sModelDrawInfo &drawInfo,
								 bool hasNormals,
								 bool hasColours,
	                             bool hasTextures, 
	                             float scaling)
{
	// Open the file. 
	// Read until we hit the word "vertex"
	// Read until we hit the word "face"
	// Read until we hit the word "end_header"

	std::ifstream thePlyFile( fileName.c_str() );
	if ( ! thePlyFile.is_open() )
	{	// Something is wrong...
		std::stringstream ssError;
		ssError << "Can't open >" << fileName << "< file." << std::endl;
		this->m_AppendTextToLastError(ssError.str());
		return false;
	}

	std::string temp; 
	while ( thePlyFile >> temp )
	{
		if ( temp == "vertex" ) 
		{
			break;
		}
	}; 
	// read the number of vertices...
	thePlyFile >> drawInfo.numberOfVertices;	//::g_NumberOfVertices;

	while ( thePlyFile >> temp )
	{
		if ( temp == "face" ) 
		{
			break;
		}
	}; 
	// read the number of triangles...
	thePlyFile >> drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles;


	while ( thePlyFile >> temp )
	{
		if ( temp == "end_header" ) 
		{
			break;
		}
	}; 

	// And now, we start reading vertices... Hazzah!
	
	// This is set up to match the ply (3d model) file. 
	// NOT the shader. 
	struct sVertPly
	{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec4 colour;
		glm::vec2 textureCoords;
	};

	std::vector<sVertPly> vecTempPlyVerts;

	sVertPly tempVert;
	// Load the vertices...
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; // ::g_NumberOfVertices; 
		index++)
	{
		thePlyFile
			>> tempVert.pos.x
			>> tempVert.pos.y
			>> tempVert.pos.z;


		//		tempVert.pos.x *= 10.0f;
		//		tempVert.pos.y *= 10.0f;
		//		tempVert.pos.z *= 10.0f;

		if (hasNormals)
		{
			thePlyFile
				>> tempVert.norm.x
				>> tempVert.norm.y
				>> tempVert.norm.z;
		}

		if (hasColours)
		{
			thePlyFile >> tempVert.colour.x >> tempVert.colour.y
				>> tempVert.colour.z >> tempVert.colour.w;

			// Scale the colour from 0 to 1, instead of 0 to 255
			tempVert.colour.x /= 255.0f;
			tempVert.colour.y /= 255.0f;
			tempVert.colour.z /= 255.0f;
			tempVert.colour.a /= 255.0f;
		}

		if (hasTextures)
		{
			thePlyFile
				>> tempVert.textureCoords.x		// U or S
				>> tempVert.textureCoords.y;	// V or T
		}

		// Add too... what? 
		vecTempPlyVerts.push_back(tempVert);
	}

	// Apply scaling
	for (sVertPly &curVertex : vecTempPlyVerts)
	{
		curVertex.pos.x *= scaling;
		curVertex.pos.y *= scaling;
		curVertex.pos.z *= scaling;
	}



	// Create a local vertex array
	// Note: The format the file (ply) is DIFFERENT from this array:
	// - sVertPly was made to match the file format
	// - sVert was made to match the shader vertex attrib format

	drawInfo.pVertices = new sVert[drawInfo.numberOfVertices];

	if (hasNormals && hasColours)
	{
		for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
		{
			drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
			drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
			drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
			drawInfo.pVertices[index].w = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].nx = vecTempPlyVerts[index].norm.x;
			drawInfo.pVertices[index].ny = vecTempPlyVerts[index].norm.y;
			drawInfo.pVertices[index].nz = vecTempPlyVerts[index].norm.z;
			drawInfo.pVertices[index].nw = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].r = vecTempPlyVerts[index].colour.r;
			drawInfo.pVertices[index].g = vecTempPlyVerts[index].colour.g;
			drawInfo.pVertices[index].b = vecTempPlyVerts[index].colour.b;
			drawInfo.pVertices[index].a = vecTempPlyVerts[index].colour.a;
		}// for ( unsigned int index...
	}
	if (hasNormals && hasColours && hasTextures)
	{
		for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
		{
			drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
			drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
			drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
			drawInfo.pVertices[index].w = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].nx = vecTempPlyVerts[index].norm.x;
			drawInfo.pVertices[index].ny = vecTempPlyVerts[index].norm.y;
			drawInfo.pVertices[index].nz = vecTempPlyVerts[index].norm.z;
			drawInfo.pVertices[index].nw = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].r = vecTempPlyVerts[index].colour.r;
			drawInfo.pVertices[index].g = vecTempPlyVerts[index].colour.g;
			drawInfo.pVertices[index].b = vecTempPlyVerts[index].colour.b;
			drawInfo.pVertices[index].a = vecTempPlyVerts[index].colour.a;

			// Copy the texture stuff as well
			drawInfo.pVertices[index].u = vecTempPlyVerts[index].textureCoords.x;
			drawInfo.pVertices[index].v = vecTempPlyVerts[index].textureCoords.y;

		}// for ( unsigned int index...
	}
	else if (hasNormals)
	{
		for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
		{
			drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
			drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
			drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
			drawInfo.pVertices[index].w = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].nx = vecTempPlyVerts[index].norm.x;
			drawInfo.pVertices[index].ny = vecTempPlyVerts[index].norm.y;
			drawInfo.pVertices[index].nz = vecTempPlyVerts[index].norm.z;
			drawInfo.pVertices[index].nw = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].r = 0.0f;
			drawInfo.pVertices[index].g = 1.0f;
			drawInfo.pVertices[index].b = 0.0f;
			drawInfo.pVertices[index].a = 1.0f;
		}// for ( unsigned int index...
	}
	else if (hasColours)
	{
		for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
		{
			drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
			drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
			drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
			drawInfo.pVertices[index].w = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].nx = 0.0f;
			drawInfo.pVertices[index].ny = 0.0f;
			drawInfo.pVertices[index].nz = 0.0f;
			drawInfo.pVertices[index].nw = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].r = vecTempPlyVerts[index].colour.r;
			drawInfo.pVertices[index].g = vecTempPlyVerts[index].colour.g;
			drawInfo.pVertices[index].b = vecTempPlyVerts[index].colour.b;
			drawInfo.pVertices[index].a = vecTempPlyVerts[index].colour.a;

		}// for ( unsigned int index...
	}
	else
	{
		for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
		{
			drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
			drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
			drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
			drawInfo.pVertices[index].w = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].nx = 0.0f;
			drawInfo.pVertices[index].ny = 0.0f;
			drawInfo.pVertices[index].nz = 0.0f;
			drawInfo.pVertices[index].nw = 1.0f; // when in doubt, set to 1.0f

			drawInfo.pVertices[index].r = 0.0f;
			drawInfo.pVertices[index].g = 1.0f;
			drawInfo.pVertices[index].b = 0.0f;
			drawInfo.pVertices[index].a = 1.0f;
		}// for ( unsigned int index...
	}
	

	// Optional clear array to zero 
	//memset( drawInfo.pVertices, 0, sizeof(sVert) * drawInfo.numberOfVertices);

	


	struct sTriPly
	{
		unsigned int vindex[3];		// the 3 indices
		sVertPly verts[3];			// The actual vertices
	};

	std::vector<sTriPly> vecTempPlyTriangles;
	sTriPly tempTriangle;
	unsigned int discard = 0;
	for ( unsigned int index = 0; index != drawInfo.numberOfTriangles;	// ::g_NumberOfTriangles; 
		  index++ )
	{
		// 3 5622 5601 5640
		thePlyFile >> discard						// the "3" at the start
			       >> tempTriangle.vindex[0]
			       >> tempTriangle.vindex[1]
			       >> tempTriangle.vindex[2];

		//// Look up the vertex that matches the triangle index values.
		//tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
		//tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
		//tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];

		vecTempPlyTriangles.push_back( tempTriangle );
	}//for ( unsigned int index...

	
	// NOW, we need to put them into the vertex array buffer that 
	//	will be passed to OpenGL. Why? 
	// Because we called glDrawArrays(), that's why. 

//	::g_NumberOfVertsToDraw = ::g_NumberOfTriangles * 3;	// 3 because "triangles"

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

	// sVert* pVertices = 0;
//	pVertices = new sVert[::g_NumberOfVertsToDraw];

	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	// Optional clear array to zero 
	//memset( drawInfo.pIndices, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);

	// Allocate on the HEAP, so infinite size... 
	// delete pVertices			/// error!
	// delete [] pVertices		/// correct!!
	unsigned int indexBufferIndex = 0;
	for ( unsigned int triIndex = 0; 
		  triIndex != drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles; 
		  triIndex++, indexBufferIndex += 3 )
	{
		sTriPly& curTri = vecTempPlyTriangles[triIndex];

		// This is taking an array of triangles vertex indices 
		// (which is a 2D array of indices, really) and converting
		//  it into a 1D array of indices...)
		drawInfo.pIndices[indexBufferIndex + 0] = curTri.vindex[0];
		drawInfo.pIndices[indexBufferIndex + 1] = curTri.vindex[1];
		drawInfo.pIndices[indexBufferIndex + 2] = curTri.vindex[2];

		//pVertices[ vertIndex + 0 ].x = curTri.verts[0].pos.x;
		//pVertices[ vertIndex + 0 ].y = curTri.verts[0].pos.y;
		//pVertices[ vertIndex + 0 ].z = curTri.verts[0].pos.z;
		//pVertices[ vertIndex + 0 ].r = curTri.verts[0].colour.x;
		//pVertices[ vertIndex + 0 ].g = curTri.verts[0].colour.y;
		//pVertices[ vertIndex + 0 ].b = curTri.verts[0].colour.z;
//
		//pVertices[ vertIndex + 1 ].x = curTri.verts[1].pos.x;
		//pVertices[ vertIndex + 1 ].y = curTri.verts[1].pos.y;
		//pVertices[ vertIndex + 1 ].z = curTri.verts[1].pos.z;
		//pVertices[ vertIndex + 1 ].r = curTri.verts[1].colour.x;
		//pVertices[ vertIndex + 1 ].g = curTri.verts[1].colour.y;
		//pVertices[ vertIndex + 1 ].b = curTri.verts[1].colour.z;
//
		//pVertices[ vertIndex + 2 ].x = curTri.verts[2].pos.x;
		//pVertices[ vertIndex + 2 ].y = curTri.verts[2].pos.y;
		//pVertices[ vertIndex + 2 ].z = curTri.verts[2].pos.z;
		//pVertices[ vertIndex + 2 ].r = curTri.verts[2].colour.x;
		//pVertices[ vertIndex + 2 ].g = curTri.verts[2].colour.y;
		//pVertices[ vertIndex + 2 ].b = curTri.verts[2].colour.z;
//
	}// for ( unsigned int triIndex = 0...

	return true;
}


std::string cVAOManager::getLastError(bool bAndClear /*=true*/)
{
	std::string theLastError = this->m_lastErrorString;

	if ( bAndClear )
	{
		this->m_lastErrorString = "";
	}

	return theLastError;
}

void cVAOManager::m_AppendTextToLastError(std::string text, bool addNewLineBefore /*=true*/)
{
	std::stringstream ssError;
	ssError << this->m_lastErrorString;

	if ( addNewLineBefore )
	{
		ssError << std::endl;
	}

	ssError << text;

	this->m_lastErrorString = ssError.str();

	return;

}
