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

uniform int isWireframe;		
uniform vec4 objectColour;
uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProj;
uniform sLightDesc theLights[NUMLIGHTS];

in vec3 vColour;		
in vec3 vPosition;	
in vec3 vNormal;
in vec3 vTexSTU;

out vec4 diffuseColour;	
out vec3 vertMV;
out vec3 vertWorld;
out vec3 vertNorm;
out vec3 lightVector[NUMLIGHTS];
out vec3 text_STU;

void main() {
	mat4 matMV = matView * matModel;
	mat4 matMVP = matProj * matView * matModel;
	vertMV = vec3(matMV * vec4(vPosition.xyz, 1.0f)).xyz;
	
	mat4 matNormal = inverse( transpose(matModel) );
	vertNorm = mat3(matNormal) * normalize(vNormal.xyz);
	vertWorld = vec3(matModel * vec4(vPosition.xyz, 1.0f));
		
	for (int index = 0; index < NUMLIGHTS; ++index) {
		lightVector[index].xyz = theLights[index].position.xyz - vertWorld.xyz;
	}

	gl_Position = matMVP * vec4(vPosition.xyz, 1.0f);
	
	if (isWireframe == 1) {
		diffuseColour = objectColour;
	}
	else {	
		diffuseColour = vec4(vColour, 1.0f);
	}
	
	text_STU = vTexSTU;
}