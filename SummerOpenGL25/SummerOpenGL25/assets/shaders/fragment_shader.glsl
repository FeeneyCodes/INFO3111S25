#version 420

uniform vec3 eyeLocation;

in vec4 vertColor;		/// RGB_A
in vec4 vertNormal;
in vec4 vertWorldPosition;
in vec2 vertTextCoords;

// What we write to the colour buffer (aka "screen")
out vec4 pixelColour;

// Pass the per object specular 'shininess'
uniform vec4 vertSpecular;// = vec4(1.0f, 1.0f, 1.0f, 1.0f);
// How transparent this object is 0 to 1
uniform float alphaTransparency;

vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular);
							
struct sLight
{
	vec4 position;	// xyz, ignoring w (4th parameter)	
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power (starts at 1, goes to 10,000+)
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights, ignoring w (4th parameter)	
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 10;
uniform sLight theLights[NUMBEROFLIGHTS];

// Textures (can have up to 32+ of these)
// BUT keep in mind that's NOT the "total number of textures"
//    it's the max texture PER PIXEL
uniform sampler2D textSampler2D_00;		// Sydney
uniform sampler2D textSampler2D_01;		// Dungeon
uniform sampler2D textSampler2D_02;
uniform sampler2D textSampler2D_03;

// From cMeshObject: float textureMixRatio[NUM_TEXTURES];
// 0.0 = no texture to 1.0 = 100% of that texture
// All the ratios should add up to 1.0f
uniform vec4 texMixRatios;		// x = 0, y = 1, etc. 

// If this is true, then we are drawing the skybox
// (it's false for all other objects)
uniform bool bIsSkyboxObject;
uniform samplerCube skyboxCubeTexture;


uniform sampler2D sampMaskTexture01;
uniform bool bUseMaskingTexture;


// Same uniform as in the vertex shader
uniform bool bUseVertexColour;	// If true, DON'T use the textures
uniform bool bDoNotLight;		// If true, lighting is NOT calcuated

// Default is 0,0,0 (black) so has no impact if not set
uniform vec3 ambientRGB;	// This is added to the AFTER lighting calculation

void main()
{
	
	// Skybox
	// uniform bool bIsSkyboxObject;
	// uniform samplerCube skyboxCubeTexture;
//	if ( bIsSkyboxObject )
	{
		// TEMP DEBUG
		//pixelColour.rgba = vec4( 1.0f, 0.0f, 0.0f, 1.0f);
		
		// Note we are using the normal vectors to cast a ray that
		//	intersects with the cube map.
		// (we completely ignore the UVs on the skybox sphere)
		
//		pixelColour.rgb = texture( skyboxCubeTexture, vertNormal.xyz ).rgb;
		
		vec3 eyeRayIncident = normalize(eyeLocation - vertWorldPosition.xyz);
		
		vec3 reflectRay = reflect(vertNormal.xyz, eyeRayIncident);
		vec3 refractRay = refract(vertNormal.xyz, eyeRayIncident, 1.06f);
		
		vec3 reflectRGB = texture( skyboxCubeTexture, reflectRay ).rgb;
		vec3 refractRGB = texture( skyboxCubeTexture, refractRay ).rgb;	
		
		
		pixelColour.rgb = reflectRGB * 0.5f +
						  refractRGB * 0.5f;
		
//		pixelColour.rgb *= 0.0001f;
//		pixelColour.rgb += vec3(1.0f, 0.0f, 0.0f);
		
		pixelColour.a = 1.0f;
	
		return;
	}

	
	
	vec4 vertexColour = vec4(vertColor);
	
//	vertexColour.r = vertTextCoords.x;		// S or U  (x)
//	vertexColour.g = vertTextCoords.y;		// T or V  (y)
//	vertexColour.b = 0.0f;

	// uniform sampler2D textureNumber01;
	
	// Set the vertex colour in case we are NOT using texture lookup
	vec4 finalTextRGBA = vec4(vertexColour.rgb, 1.0f);
	
	if ( ! bUseVertexColour )
	{
		// Then we are using the textures for the "vertex colour"
		vec4 tex00RGBA = texture( textSampler2D_00, vertTextCoords.xy );
		vec4 tex01RGBA = texture( textSampler2D_01, vertTextCoords.xy );
		vec4 tex02RGBA = texture( textSampler2D_02, vertTextCoords.xy );
		vec4 tex03RGBA = texture( textSampler2D_03, vertTextCoords.xy );

		finalTextRGBA =   tex00RGBA * texMixRatios.x
						+ tex01RGBA * texMixRatios.y
						+ tex02RGBA * texMixRatios.z
						+ tex03RGBA * texMixRatios.w;
	}//if ( ! bUseVertexColour )
					


	
	
	
	if ( bDoNotLight )
	{ 
		// Bypass the lighting calculation
		pixelColour.rgb = finalTextRGBA.rgb;
		// Assume alpha is 1.0f
		pixelColour.a = 1.0f;
		// Early exit of shader
		return;
	}
	
	
		// Another use of textures: "masking"
//	if (bUseMaskingTexture) 
//	{
//		//uniform sampler2D sampMaskTexture01;
//		// Use this texture to "mask" parts of the original texture
//		vec4 tex00RGBA = texture( textSampler2D_00, vertTextCoords.xy ); // Rust
//		vec4 tex01RGBA = texture( textSampler2D_01, vertTextCoords.xy ); // Steel
//		
//		float maskValue = texture( sampMaskTexture01, vertTextCoords.xy ).r;
//		
//		finalTextRGBA.rgb = tex00RGBA.rgb * maskValue 
//                          + tex01RGBA.rgb * (1.0f - maskValue);
//						  
//		if ( maskValue > 0.5f )
//		{
//			discard;
//		}
//	}
	
	
//	vec4 lightContrib = calculateLightContrib(vertexColour.rgb, vertNormal.xyz, vertWorldPosition.xyz, vertSpecular);	

	// Replace the "vertex colour" with the colour form the textures
	vec4 lightContrib = calculateLightContrib(finalTextRGBA.rgb, vertNormal.xyz, vertWorldPosition.xyz, vertSpecular);	
	pixelColour.rgb = lightContrib.rgb;
	
	// Add some ambient if it's too dark
	// You can think of "ambient" light as how bright something
	//	is if it DOESN'T have any light DIRECTLY shining on it.
	pixelColour.rgb += ambientRGB;
	
// Here's where we output the colour WITHOUT the lighting					
//	pixelColour.rgb = finalTextRGBA.rgb;


	
//	pixelColour.a = 1.0f;
	// Alpha or transparency value.
	// 0.0 - totally invisible
	// 1.0 - opaque (solid)
	pixelColour.a = alphaTransparency;
};

// Feeney gave you this (it's inspired by the basic shader in Mike Bailey's Graphic Shaders book)
vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
			
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += ( vertexMaterialColour.rgb * 
			                           theLights[index].diffuse.rgb * 
									   lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;	
		float distanceToLight = length(vLightToVertex);		
		vec3 lightVector = normalize(vLightToVertex);		
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
		lightSpecularContrib = 
		             pow( max(0.0f, dot( eyeVector, reflectVector) ), 
					      objectSpecularPower ) * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
					   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb * lightSpecularContrib.rgb );
	}//for(intindex=0...
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}