#include <glad/glad.h>	
#include <GLFW/glfw3.h>		
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <array>

#include "GlobalStuff.h"
#include "vert_XYZ_RGB.h"
#include "Utilities.h"

float deltaTime;

void CollisionStep(float deltaTime);
void DrawParticles();
void DrawScene();
void DrawSkyBox();
void error_callback(int error, const char* description);
cGameObject* findObjectByID(unsigned int ID);
void Initialize();
bool LoadObjectsIntoScene(std::vector<cGameObject*> &vec_pGOs);
void MakeSkyBox(std::array<std::string, 7> &skyBox);
void PhysicsStep(float deltaTime);
bool testSphereSphereCollision(cGameObject* pBallA, cGameObject* pBallB);

int main(void) {
	// Initialize glad and glfw
	Initialize();

	std::string throwAway;
	std::cin >> throwAway >> numberOfPlanes >> throwAway >> speedOfPlanes >> throwAway >> emitterLifetimeMin >> throwAway >> emitterLifetimeMax;

	/*------------------*/
	/*	SHADER MANAGER	*/
	/*------------------*/

	g_pTheShaderManager = new cShaderManager();
	g_pTheShaderManager->setBasePath("assets/shaders/");

	cShaderManager::cShader verShader("phongVert.glsl");
	cShaderManager::cShader fragShader("phongFrag.glsl");
	if (!g_pTheShaderManager->createProgramFromFile("phong", verShader, fragShader)) {
		std::cout << g_pTheShaderManager->getLastError() << std::endl;
		return -1;
	}
	GLuint shadProgID = g_pTheShaderManager->getIDFromFriendlyName("phong");


	/*----------------*/
	/*	MESH MANAGER  */
	/*----------------*/

	g_pTheMeshTypeManager = new cMeshTypeManager();
	g_pTheMeshTypeManager->SetBaseFilePath("assets/models/");
	g_pTheMeshTypeManager->ScaleEverythingToBoundingBoxOnLoad(true, 10.0f);

	std::vector<std::string> plyFiles = {
		"utah_teapot.ply",
		"isosphere.ply",
		"isosphere_inverted.ply",
		"tie_fighter.ply",
		"small_plane.ply",
		"medium_plane.ply",
		"large_plane.ply",
		"2_sided_quad.ply"
	};

	for (std::vector<std::string>::iterator it = plyFiles.begin(); it != plyFiles.end(); ++it) {
		if (g_pTheMeshTypeManager->LoadPlyFileIntoGLBuffer(shadProgID, *it)) {
			std::cout << std::setw(10) << std::left << "[SUCCESS] " << "Success in loading model: " << *it << std::endl;
		}
		else {
			std::cout << std::setw(10) << std::left << "[ERROR] " << "Failed in loading model: " << *it << std::endl;
		}
	}


	/*--------------------*/
	/*  PARTICLE EMITTER  */
	/*--------------------*/

	g_pParticles = new cParticleEmitter();

	cParticleEmitter::sParticleDesc emitDescript;
	emitDescript.lifetimeMin = 2.0f;
	emitDescript.lifetimeMax = 4.0f;
	emitDescript.particlesCreatedPerUpdate = 20;
	emitDescript.ongoingForce = glm::vec3(0.0f, 0.5f, 0.0f);
	emitDescript.initVelMin = glm::vec3(-0.2f, 0.0f, -0.2f);
	emitDescript.initVelMax = glm::vec3(0.2f, 1.0f, 0.2f);
	g_pParticles->allocateParticles(2000, emitDescript);


	/*-------------------*/
	/*  TEXTURE MANAGER  */
	/*-------------------*/

	g_pTextureManager = new cBasicTextureManager();
	g_pTextureManager->SetBasePath("assets/textures");

	std::vector<std::string> textureFiles = {
		"bricks.bmp",
		"fence.bmp",
		"fence_inverted.bmp",
		"white.bmp",
		"smoke.bmp"
	};

	for (std::vector<std::string>::iterator it = textureFiles.begin(); it != textureFiles.end(); ++it) {
		if (g_pTextureManager->Create2DTextureFromBMPFile(*it, true)) {
			std::cout << std::setw(10) << std::left << "[SUCCESS] " << "Success in loading texture: " << *it << std::endl;
		}
		else {
			std::cout << std::setw(10) << std::left << "[ERROR] " << "Failed in loading texture: " << *it << std::endl;
		}
	}


	/*------------------*/
	/*  SKYBOX TEXTURE  */
	/*------------------*/

	g_pTextureManager->SetBasePath("assets/textures/skybox");

	//MakeSkyBox(spaceBox);
	MakeSkyBox(skyBox);

	/*-------------------------*/
	/*  GET UNIFORM LOCATIONS  */
	/*-------------------------*/

	UniformLoc_matModel = glGetUniformLocation(shadProgID, "matModel");
	UniformLoc_matView = glGetUniformLocation(shadProgID, "matView");
	UniformLoc_matProj = glGetUniformLocation(shadProgID, "matProj");

	UniformLoc_ID_objectColour = glGetUniformLocation(shadProgID, "objectColour");
	UniformLoc_ID_isWireframe = glGetUniformLocation(shadProgID, "isWireframe");
	UniformLoc_ID_bUseDebugDiffuseNoLighting = glGetUniformLocation(shadProgID, "bUseDebugDiffuseNoLighting");

	UniformLoc_texSamp2D_00 = glGetUniformLocation(shadProgID, "texSamp2D_00");
	UniformLoc_texSamp2D_01 = glGetUniformLocation(shadProgID, "texSamp2D_01");
	UniformLoc_texSamp2D_02 = glGetUniformLocation(shadProgID, "texSamp2D_02");
	UniformLoc_texSamp2D_03 = glGetUniformLocation(shadProgID, "texSamp2D_03");

	UniformLoc_bUseTextureAsDiffuse = glGetUniformLocation(shadProgID, "bUseTextureAsDiffuse");
	UniformLoc_bUseLighting = glGetUniformLocation(shadProgID, "bUseLighting");

	UniformLoc_bIsImposterBlackIsAlpha = glGetUniformLocation(shadProgID, "bIsImposterBlackIsAlpha");

	UniformLoc_bIsCubeMapObject = glGetUniformLocation(shadProgID, "bIsCubeMapObject");
	UniformLoc_myCubemapSkyBoxSampler = glGetUniformLocation(shadProgID, "myCubemapSkyBoxSampler");

	UniformLoc_alpha = glGetUniformLocation(shadProgID, "alpha");

	
	/*----------*/
	/*  LIGHTS  */
	/*----------*/
	CLightDescription thisLight;
	thisLight.attenConst = 0.0f;
	thisLight.attenLinear = 0.05f;
	thisLight.attenQuad = 0.005f;

	float gridLimit = 50.0f;
	float gridStep = 51.f;

	for (float x = -gridLimit; x <= (gridLimit + gridStep); x += gridStep) {
		for (float y = -gridLimit; y <= (gridLimit + gridStep); y += gridStep) {
			for (float z = -gridLimit; z <= (gridLimit + gridStep); z += gridStep) {
				thisLight.position = glm::vec3(x, y, z);
				thisLight.UniformLocation_position = glGetUniformLocation(shadProgID, std::string("theLights[" + std::to_string(g_vec_lights.size()) + "].position").c_str());
				thisLight.UniformLocation_diffuse = glGetUniformLocation(shadProgID, std::string("theLights[" + std::to_string(g_vec_lights.size()) + "].diffuse").c_str());
				thisLight.UniformLocation_atten = glGetUniformLocation(shadProgID, std::string("theLights[" + std::to_string(g_vec_lights.size()) + "].attenuation").c_str());// = 0.0f;		// Can set to 1.0 to "turn off" a light
				g_vec_lights.push_back(thisLight);
			}
		}
	}

	if (!LoadObjectsIntoScene(g_vec_pGOs)) {
		std::cout << "WARNING: Could not load all models into the scene." << std::endl;
	}

	glEnable(GL_DEPTH_TEST);

	double lastTime = glfwGetTime();
	static const float MAXDELTATIME = 0.1f;	// 100 ms

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4x4 matProjection = glm::perspective(0.6f, ratio, 0.01f, 10000.0f);
		glm::mat4x4 matView = glm::lookAt(gCameraPos, gTargetPos, glm::vec3(0.0f, 1.0f, 0.0f));

		deltaTime = static_cast<float>(glfwGetTime() - lastTime);
		lastTime = glfwGetTime();
		if (deltaTime > MAXDELTATIME) {
			deltaTime = MAXDELTATIME;
		}

		// Only needs to be set if window is created and-or is resized
		glUniformMatrix4fv(UniformLoc_matProj, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matProjection));

		// This is set once at the start of the "scene" draw.
		glUniformMatrix4fv(UniformLoc_matView, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matView));

		for (size_t i = 0; i < g_vec_lights.size(); ++i) {
			glUniform4f(g_vec_lights[i].UniformLocation_position, g_vec_lights[i].position.x, g_vec_lights[i].position.y, g_vec_lights[i].position.z, 1.0f);
			glUniform4f(g_vec_lights[i].UniformLocation_diffuse, 1.0, 1.0f, 1.0f, 1.0f);
			glUniform4f(g_vec_lights[i].UniformLocation_atten, g_vec_lights[i].attenConst, g_vec_lights[i].attenLinear, g_vec_lights[i].attenQuad, 1.0f);
		}

		if (flyingEnabled) {
			for (std::vector<cGameObject*>::iterator it = g_vec_pGOs.begin(); it != g_vec_pGOs.end(); ++it) {
				glm::vec3 forward = (*it)->qOrientaion * glm::vec3(0.f, 0.f, 1.f);
				if ((*it)->bCanFly) {
					glm::vec3 rotationVector;
					if ((*it)->position.x > 15.f) {
						rotationVector.x = glm::radians(1.f);
					}
					else if ((*it)->position.x < -15.f) {
						rotationVector.x = glm::radians(-1.f);
					}
					else {
						rotationVector.x = glm::radians(getRandFloat(-1.f, 1.f));
					}
					if ((*it)->position.y > 15.f) {
						rotationVector.y = glm::radians(1.f);
					}
					else if ((*it)->position.y < -15.f) {
						rotationVector.y = glm::radians(-1.f);
					}
					else {
						rotationVector.y = glm::radians(getRandFloat(-1.f, 1.f));
					}
					if ((*it)->position.z > 15.f) {
						rotationVector.z = glm::radians(1.f);
					}
					else if ((*it)->position.z < -15.f) {
						rotationVector.z = glm::radians(-1.f);
					}
					else {
						rotationVector.z = glm::radians(getRandFloat(-1.f, 1.f));
					}
					(*it)->qOrientaion *= glm::quat(rotationVector);
					(*it)->position += forward * speedOfPlanes * deltaTime;
				}
			}
		}

		// *********************************
		//PhysicsStep(deltaTime);
		//CollisionStep(deltaTime);
		DrawSkyBox();
		DrawScene();
		DrawParticles();
		//DrawDebugLightSpheres();
		// *********************************

		// Show or "present" what we drew...
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Bye bye...
	delete g_pTheMeshTypeManager;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void CollisionStep(float deltaTime) {
	g_vec_Contacts.clear();

	// For each ball, check for collision with other balls
	for (int indexOut = 0; indexOut != g_vec_pGOs.size(); indexOut++) {
		cGameObject* pOutBall = g_vec_pGOs[indexOut];

		for (int indexIn = 0; indexIn != g_vec_pGOs.size(); indexIn++) {
			cGameObject* pInBall = g_vec_pGOs[indexIn];

			if (pOutBall != pInBall) {

				pOutBall->colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
				pInBall->colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

				if (testSphereSphereCollision(pOutBall, pInBall)) {
					cContact curContact;
					curContact.objectID = pOutBall->getID();
					g_vec_Contacts.push_back(curContact);
				}
			}
		}
	}

	// Colour all the balls white
	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		cGameObject* pCurGO = g_vec_pGOs[index];
		pCurGO->colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	}

	// For each ball, check for collision with triangles (environment)
	for (int indexOut = 0; indexOut != g_vec_pGOs.size(); indexOut++) {
		cGameObject* pOutBall = g_vec_pGOs[indexOut];
	}

	// Calculate reponse
	if (!g_vec_Contacts.empty()) {
		int sexyBreakPoint = 0;

		for (int index = 0; index != g_vec_Contacts.size(); index++) {
			unsigned int ID = g_vec_Contacts[index].objectID;
			cGameObject* pCurGO = findObjectByID(ID);
			pCurGO->colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
		}
	}

	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		cGameObject* pCurGO = g_vec_pGOs[index];

		if (pCurGO->bIsUpdatedByPhysics) {
			float groundSurface = 0.0f;

			if (pCurGO->position.y <= ((groundSurface + pCurGO->radius))) {
				pCurGO->velocity.y = -pCurGO->velocity.y;
				pCurGO->velocity.y = 0.0f;
			}
		}
	}
	return;
}

void DrawParticles() {
	// Place the emitter at the light #0 location...
	//g_pParticles->position = gCameraPos;
	// Inidcate where the eye is (for the imposters)
	//g_pParticles->particleSettings.imposterEyeLocation = gCameraPos;
	//g_pParticles->Update(deltaTime);

	for (std::vector<cGameObject*>::iterator it = g_vec_pGOs.begin(); it != g_vec_pGOs.end(); ++it) {
		if ((*it)->bCanFly) {
			glm::vec3 forward = (*it)->qOrientaion * glm::vec3(0.f, 0.f, 1.f);
			(*it)->particles.position = (*it)->position - forward;
			// Inidcate where the eye is (for the imposters)
			(*it)->particles.particleSettings.imposterEyeLocation = gCameraPos;
			(*it)->particles.Update(deltaTime);

			int numParticles = (*it)->particles.getParticlesCopy(g_vecParticlesToRender);
			for (int index = 0; index != numParticles; index++) {
				DrawQuad(true, g_vecParticlesToRender[index].position, g_vecParticlesToRender[index].rotation, "", 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.f), false, false);
			}
		}
	}
}

void DrawScene() {
	// Sort objects by distance from camera
	std::vector<cGameObject*> vecGO_Sorted;
	vecGO_Sorted.reserve(g_vec_pGOs.size());
	// Copy to vector
	for (std::vector<cGameObject*>::iterator itGO = g_vec_pGOs.begin(); itGO != g_vec_pGOs.end(); itGO++) {
		vecGO_Sorted.push_back(*itGO);
	}

	// Do one pass of bubble sort, because the objects are likely to be in almost the correct order from frame to frame		
	for (int index = 0; index != vecGO_Sorted.size() - 1; index++) {
		cGameObject* p1 = vecGO_Sorted[index];
		cGameObject* p2 = vecGO_Sorted[index + 1];

		// Use glm distance 
		glm::vec3 p1Pos(p1->position);
		glm::vec3 p2Pos(p2->position);

		if (glm::distance(gCameraPos, p1Pos) < glm::distance(gCameraPos, p2Pos)) {
			vecGO_Sorted[index] = p2;
			vecGO_Sorted[index + 1] = p1;
		}
	}

	// Copy order back to main vectro
	for (int index = 0; index != vecGO_Sorted.size(); index++) {
		g_vec_pGOs[index] = vecGO_Sorted[index];
	}

	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		cGameObject* pCurGO = g_vec_pGOs[index];

		// Set identity matrix for all "parent" or "root" objects
		glm::mat4x4 matModel = glm::mat4x4(1.0f);

		DrawObject(pCurGO, matModel);
	}
}

void DrawSkyBox() {
	// Place the skybox at the same location as the camera 'eye'
	g_pSkyBoxObject->position = gCameraPos;
	
	glUniform1i(UniformLoc_bIsCubeMapObject, TRUE);

	// Bind the appropriate texture unit... 
	GLuint texNumberSky = g_pTextureManager->getTextureIDFromName(g_SkyBoxName);
	const unsigned int TEXTURE_UNIT_80_MAGIC_NUMBER = 80;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texNumberSky);
	glUniform1i(UniformLoc_myCubemapSkyBoxSampler, texNumberSky);

	glDisable(GL_DEPTH);
	glDepthMask(GL_FALSE);

	DrawObject( g_pSkyBoxObject, glm::mat4x4(1.0f), false );

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH);

	glUniform1i(UniformLoc_bIsCubeMapObject, FALSE);

	return;
}

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

cGameObject* findObjectByID(unsigned int ID) {
	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		if (g_vec_pGOs[index]->getID() == ID) {
			return g_vec_pGOs[index];
		}
	}
	return 0;
}

void Initialize() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1200, 800, "Simple example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << ", " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void PhysicsStep(float deltaTime)
{
	glm::vec3 gravityForce(0.0f, 0.0f, 0.0f);

	// Rotational "speed" using SLERP of quaternion
	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		cGameObject* pGO = g_vec_pGOs[index];

		// Represents the change over 1 second.
		glm::quat qRotChangeZeroSeconds;
		glm::quat qRotChange(pGO->rotSpeed);
		glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds, qRotChange, deltaTime);

		pGO->qOrientaion *= qRotChangeThisFrame;
	}

	for (int index = 0; index != g_vec_pGOs.size(); index++) {
		cGameObject* pCurGO = g_vec_pGOs[index];

		if (pCurGO->bIsUpdatedByPhysics) {
			// Update velocity based on acceleration
			pCurGO->velocity.x += (pCurGO->acceleration.x + gravityForce.x) * deltaTime;
			pCurGO->velocity.y += (pCurGO->acceleration.y + gravityForce.y) * deltaTime;
			pCurGO->velocity.z += (pCurGO->acceleration.z + gravityForce.z) * deltaTime;

			pCurGO->lastPosition = pCurGO->position;

			// Update position based on velocity
			pCurGO->position.x += pCurGO->velocity.x * deltaTime;
			pCurGO->position.y += pCurGO->velocity.y * deltaTime;
			pCurGO->position.z += pCurGO->velocity.z * deltaTime;
		}
	}

	return;
}

bool testSphereSphereCollision(cGameObject* pBallA, cGameObject* pBallB) {
	if (glm::distance(pBallA->position, pBallB->position) <= pBallA->radius + pBallB->radius) {
		return true;
	}
	return false;
}