#version 420

//uniform mat4 MVP;
uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;
uniform mat4 mModel_InverseTranpose;

uniform bool bUseOverrideColor;
uniform vec3 colorOverride;


in vec4 vCol;
in vec4 vPos;
in vec4 vNorm;
in vec2 vTextCoords;

out vec4 vertColor;
out vec4 vertNormal;
out vec4 vertWorldPosition;
out vec2 vertTextCoords;

void main()
{
	mat4 MVP = mProj * mView * mModel;

    gl_Position = MVP * vec4(vPos.xyz, 1.0f);
	
	vertWorldPosition = mModel * vec4(vPos.xyz, 1.0f);
	
	
	if (bUseOverrideColor)
	{
		vertColor = vec4(colorOverride.rgb, 1.0f);
	}
	else
	{
	    vertColor = vec4(vCol.rgb, 1.0f);
	}
	
	vertNormal = mModel_InverseTranpose * vec4(vNorm.xyz, 1.0f);
	vertNormal.xyz = normalize(vertNormal.xyz);
	
	// Copy the uvs
	vertTextCoords = vTextCoords;
};
