#version 400

struct sLightDesc {
	vec4 position;
	vec4 diffuse;
	vec4 attenuation;
	vec4 specular;
	vec4 direction;
	vec4 typeAndParams;
};

const int NUMLIGHTS = 27;

uniform bool bUseDebugDiffuseNoLighting;
uniform bool bIsCubeMapObject;
uniform bool bUseTextureAsDiffuse;
uniform bool bUseLighting;
uniform bool bIsImposterBlackIsAlpha;
uniform float alpha;
uniform vec3 eyeLocation;
uniform sampler2D texSamp2D_00;			
uniform sampler2D texSamp2D_01;		
uniform sampler2D texSamp2D_02;			
uniform sampler2D texSamp2D_03;		
uniform samplerCube myCubemapSkyBoxSampler;
uniform sLightDesc theLights[NUMLIGHTS];

in vec4 diffuseColour;	
in vec3 vertMV;
in vec3 vertWorld;
in vec3 vertNorm;
in vec3 lightVector[NUMLIGHTS];
in vec3 lightHalfVector[NUMLIGHTS];
in vec3 text_STU;

out vec4 out_Colour;

vec3 calcLightContribution( vec3 diffuseObjectColour, vec3 vecView, int lightIndex ) {
	float distanceFromLight = length(lightVector[lightIndex].xyz); 
	vec3 lightVec = normalize(lightVector[lightIndex].xyz);

	float atten = 1.0f /( theLights[lightIndex].attenuation.x + 
	                      theLights[lightIndex].attenuation.y * distanceFromLight +
					      theLights[lightIndex].attenuation.z * distanceFromLight * distanceFromLight );

	if ( int(theLights[lightIndex].typeAndParams.x) == 1 ) {
		float spotCos = dot( lightVec, -theLights[lightIndex].direction.xyz );
		
		if ( spotCos < theLights[lightIndex].typeAndParams.z ) {	
			atten = 0.0f;		
		}
		else if ( spotCos < theLights[lightIndex].typeAndParams.y ) {
			atten *= smoothstep(theLights[lightIndex].typeAndParams.y, theLights[lightIndex].typeAndParams.z, spotCos);
		}	
	}
	
 	vec3 diffuse = max(0.0, dot(vertNorm.xyz, lightVec)) * diffuseObjectColour;

	diffuse *= atten;
	
		vec3 viewVector = normalize(eyeLocation - vertWorld);
	vec3 vecReflect = normalize(reflect(-lightVec,vertNorm));
	float specReflect = max(dot(vecReflect, viewVector), 0.0f);	// Silimar to diffuse, but uses view
	
	float lightSpecShinniness = theLights[lightIndex].specular.w;
	float specPower = pow( specReflect, lightSpecShinniness );
	vec3 specular = specPower * theLights[lightIndex].specular.xyz;

	specular *= atten;
	specular = clamp(specular, 0.0f, 1.0f);
	
	vec3 result = clamp(diffuse + specular, 0.0, 1.0f);
	
	return result;
}

void main()
{	
	if (bUseDebugDiffuseNoLighting) {
		out_Colour.xyz = diffuseColour.xyz;
		out_Colour.a = 1.0f;
		return;
	}
	
	if (bIsCubeMapObject) {
		out_Colour =    texture( myCubemapSkyBoxSampler, vertNorm.xyz );
		out_Colour.a = 1.0f;
		return;
	}

	vec3 objectDiffuseColour = diffuseColour.xyz;
	
	float alphaOut = alpha;
	
	if (bUseTextureAsDiffuse) {
		vec3 diffuse0 = texture( texSamp2D_00, text_STU.xy ).rgb;
		vec3 diffuse1 = texture( texSamp2D_01, text_STU.xy ).rgb;
		vec3 diffuse2 = texture( texSamp2D_02, text_STU.xy ).rgb;
		vec3 diffuse3 = texture( texSamp2D_03, text_STU.xy ).rgb;
		
		objectDiffuseColour = diffuse1 * diffuse2 * diffuse3;
	}
	
	if (!bUseLighting) {
		out_Colour = vec4(objectDiffuseColour, alphaOut);	
	}
	else {
		vec3 vecView = normalize(vertMV);
		
		vec3 lightContrib = vec3(0.0f, 0.0f, 0.0f);
		for (int index = 0; index < NUMLIGHTS; ++index) {
			lightContrib += calcLightContribution(objectDiffuseColour, vecView, index);
		}
		
		out_Colour = vec4(lightContrib, alphaOut);	
	}

	if (bIsImposterBlackIsAlpha) {
		if (((out_Colour.r + out_Colour.g + out_Colour.b)/3.0f) < 0.2f) {
			out_Colour.w = 0.0f;
		}
	}
	
	out_Colour *= 1.25f;
}