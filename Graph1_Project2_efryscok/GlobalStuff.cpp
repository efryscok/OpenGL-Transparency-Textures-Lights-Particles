#include "GlobalStuff.h"

GLFWwindow* window = 0;


/*----------*/
/*  CAMERA  */
/*----------*/
glm::vec3 gCameraPos(0.1f, 0.1f, 100.f);
glm::vec3 gTargetPos(0.f, 0.f, 0.f);
float g_CameraMovementSpeed = 0.5f;


/*----------*/
/*  LIGHTS  */
/*----------*/
glm::vec3 lightPos(5.4f, 5.0f, 19.4f);
float lightConstAtten = 0.0f;
float lightLinearAtten = 0.0477747f;
float lightQuadAtten = 0.000683441f;
bool bDrawLightDebugSpheres = false;
bool bDrawLightDebugLocation = false;


/*---------------------*/
/*  UNIFORM LOCATIONS  */
/*---------------------*/
GLint UniformLoc_matModel = -1;
GLint UniformLoc_matView = -1;
GLint UniformLoc_matProj = -1;

GLint UniformLoc_ID_objectColour = -1;
GLint UniformLoc_ID_isWireframe = -1;
GLint UniformLoc_ID_bUseDebugDiffuseNoLighting = -1;

GLint UniformLoc_texSamp2D_00 = -1;
GLint UniformLoc_texSamp2D_01 = -1;
GLint UniformLoc_texSamp2D_02 = -1;
GLint UniformLoc_texSamp2D_03 = -1;

GLint UniformLoc_bUseTextureAsDiffuse = -1;
GLint UniformLoc_bUseLighting = -1;

GLint UniformLoc_bIsImposterBlackIsAlpha = -1;

GLint UniformLoc_bIsCubeMapObject = -1;
GLint UniformLoc_myCubemapSkyBoxSampler = -1;

GLint UniformLoc_alpha = -1;


/*-------------------------------*/
/*  MANAGERS AND OBJECT VECTORS  */
/*-------------------------------*/
cMeshTypeManager* g_pTheMeshTypeManager = 0;
cShaderManager* g_pTheShaderManager = 0;
cParticleEmitter* g_pParticles = 0;
cBasicTextureManager* g_pTextureManager = 0;

cGameObject* g_pObjectToRotate = 0;
cGameObject* g_pSkyBoxObject = 0;
cGameObject* g_pImposter = 0;

int changeTimeout = 0;
int numberOfPlanes = 250;
float speedOfPlanes = 5.0f;
float emitterLifetimeMin = 0.1f;
float emitterLifetimeMax = 1.0f;
bool flyingEnabled = false;
std::string g_SkyBoxName = "Space";

std::array<std::string, 7> spaceBox = {
	"Space",
	"SpaceBox_right1_posX.bmp",
	"SpaceBox_left2_negX.bmp",
	"SpaceBox_top3_posY.bmp",
	"SpaceBox_bottom4_negY.bmp",
	"SpaceBox_front5_posZ.bmp",
	"SpaceBox_back6_negZ.bmp"
};
std::array<std::string, 7> skyBox = {
	"Sky",
	"TropicalSunnyDayLeft2048.bmp",
	"TropicalSunnyDayRight2048.bmp",
	"TropicalSunnyDayDown2048.bmp",
	"TropicalSunnyDayUp2048.bmp",
	"TropicalSunnyDayFront2048.bmp",
	"TropicalSunnyDayBack2048.bmp"
};

std::vector< cParticleEmitter::cParticle > g_vecParticlesToRender;
std::vector< cGameObject* > g_vec_pGOs;
std::vector< cContact > g_vec_Contacts;
std::vector<CLightDescription> g_vec_lights;