#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>

// The vertex structure, as it is in the SHADER (on the GPU)
// This is also called the 'vertex layout'. 
// This means that if the shader vertex layout changed, this
//  vertex structure will also need to change (usually). 
struct sVert
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float r, g, b, a;
	// Now with texure coordinates!
	float u, v;
};


// This represents a single mesh (3D object) structure in 'indexed' format. 
// Its layout is set up to match how the GPU sees the mesh, rather
//  than how the file was, etc. 
// It's also storing the infomation that we need to tell the GPU
//  which model we want to draw. 
struct sModelDrawInfo
{
	sModelDrawInfo(); 

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVert* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;
};



class cVAOManager
{
public:
	enum enumTEXCOORDBIAS
	{
		POSITIVE_X, POSITIVE_Y, POSITIVE_Z
	};

	bool LoadModelIntoVAO(std::string fileName,
		sModelDrawInfo& drawInfo,
		unsigned int shaderProgramID,
		bool hasNormals,
		bool hasColours,
		bool hasTextureCoords,				// TODO:
		float scaling);			// Keep at 1.0 for no change

	void GenTextureCoordsSpherical(sModelDrawInfo& meshInfo,
		enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias,
		bool basedOnNormals, float scale, bool fast);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);


	std::string getLastError(bool bAndClear = true);

private:

	std::map< std::string /*model name*/,
		      sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	// Loads the ply model file into a temporary array
	bool m_LoadTheModel( std::string fileName, 
						 sModelDrawInfo &drawInfo,
						 bool hasNormals,
						 bool hasColours,
						 bool hasTextureCoords,
						 float scaling);

	std::string m_lastErrorString;
	void m_AppendTextToLastError(std::string text, bool addNewLineBefore = true);
};

#endif	// _cVAOManager_HG_
