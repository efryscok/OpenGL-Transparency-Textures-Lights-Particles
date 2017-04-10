#pragma once

#include <glad/glad.h>		
#include <GLFW/glfw3.h>	

#include <vector>
#include <array>

#include "cGameObject.h"
#include "cMeshTypeManager.h"	
#include "cShaderManager.h"
#include "cContact.h"
#include "cParticleEmitter.h"
#include "Texture\cBasicTextureManager.h"
#include "Lights\CLightDescription.h"

extern GLFWwindow* window;

/*----------*/
/*  CAMERA  */
/*----------*/
extern glm::vec3 gCameraPos;
extern glm::vec3 gTargetPos;
extern float g_CameraMovementSpeed;

/*----------*/
/*  LIGHTS  */
/*----------*/
extern glm::vec3 lightPos;		
extern float lightConstAtten;
extern float lightLinearAtten;
extern float lightQuadAtten;
extern glm::vec3 lightDirection;
extern float lightType;
extern float lightSpotCosCutoffStart;
extern float lightSpotCosCutoffEnd;
extern bool bDrawLightDebugSpheres;
extern bool bDrawLightDebugLocation;


/*---------------------*/
/*  UNIFORM LOCATIONS  */
/*---------------------*/
extern GLint UniformLoc_matModel;
extern GLint UniformLoc_matView;
extern GLint UniformLoc_matProj;

extern GLint UniformLoc_ID_objectColour;
extern GLint UniformLoc_ID_isWireframe;
extern GLint UniformLoc_ID_bUseDebugDiffuseNoLighting;

extern GLint UniformLoc_texSamp2D_00;
extern GLint UniformLoc_texSamp2D_01;
extern GLint UniformLoc_texSamp2D_02;
extern GLint UniformLoc_texSamp2D_03;
extern GLint UniformLoc_bUseTextureAsDiffuse;
extern GLint UniformLoc_bUseLighting;

extern GLint UniformLoc_bIsImposterBlackIsAlpha;

extern GLint UniformLoc_bIsCubeMapObject;
extern GLint UniformLoc_myCubemapSkyBoxSampler;

extern GLint UniformLoc_alpha;


/*-------------------------------*/
/*  MANAGERS AND OBJECT VECTORS  */
/*-------------------------------*/
extern cMeshTypeManager* g_pTheMeshTypeManager;
extern cShaderManager* g_pTheShaderManager;
extern cParticleEmitter* g_pParticles;
extern cBasicTextureManager* g_pTextureManager;

extern cGameObject* g_pObjectToRotate;
extern cGameObject* g_pSkyBoxObject;
extern cGameObject* g_pImposter;

extern int changeTimeout;
extern int numberOfPlanes;
extern float speedOfPlanes;
extern float emitterLifetimeMin;
extern float emitterLifetimeMax;
extern bool flyingEnabled;
extern std::string g_SkyBoxName;

extern std::array<std::string, 7> spaceBox;
extern std::array<std::string, 7> skyBox;

extern std::vector< cParticleEmitter::cParticle > g_vecParticlesToRender;
extern std::vector< cGameObject* > g_vec_pGOs;
extern std::vector< cContact > g_vec_Contacts;
extern std::vector<CLightDescription> g_vec_lights;


/*-----------*/
/*  METHODS  */
/*-----------*/
extern void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern void DrawDebugLightSpheres(void);
extern void DrawDebugBall( glm::vec3 position, glm::vec3 colour, float scale );
extern void DrawQuad( bool bTwoSided, glm::vec3 position, glm::vec3 rotation, std::string texture, float scale, glm::vec3 debugColour, bool bUseDebugColour, bool bIsWireFrame );
extern void DrawObject( cGameObject* pCurGO, glm::mat4x4 matModel, bool bUseLighting = true );
extern cGameObject* findObjectByID( unsigned int ID );
extern void MakeSkyBox(std::array<std::string, 7> &skyBox);