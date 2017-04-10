#include <vector>
#include <iostream>
#include <iomanip>

#include "cGameObject.h"
#include "Utilities.h"
#include "GlobalStuff.h"

extern unsigned int g_DebugSphereID;
extern unsigned int g_1SidedQuadID;
extern unsigned int g_2SidedQuadID;

#include <glm/glm.hpp>

bool LoadObjectsIntoScene( std::vector< cGameObject* > &vec_pGOs ) {
	float gridLimit = 10.f;
	float gridStep = 2.5f;

	bool fenceOrSmoke = true;
	//for ( float x = -gridLimit; x <= (gridLimit + gridStep); x += gridStep )
	//{
	//	for ( float y = -gridLimit; y <= (gridLimit + gridStep); y += gridStep )
	//	{
	//		for ( float z = -gridLimit; z <= (gridLimit + gridStep); z += gridStep )
	//		{
	//			cGameObject* pObject = new cGameObject();
	//			pObject->bIsUpdatedByPhysics = false;
	//			pObject->bIsWireframe = false;
	//			pObject->position = glm::vec3(x, y, z);
	//			pObject->scale = getRandFloat( 0.5f, gridStep );
	//			pObject->scale = 1.f;
	//			pObject->alpha = getRandFloat(0.1f, 1.0f);
	//			pObject->meshName = (g_SkyBoxName == "Space") ? "tie_fighter.ply" : "jet_plane.ply";
	//			pObject->textureNames[1] = "bricks.bmp";
	//			pObject->textureNames[2] = (fenceOrSmoke) ? "fence.bmp" : "fence_inverted.bmp";
	//			fenceOrSmoke = !fenceOrSmoke;
	//			vec_pGOs.push_back( pObject );
	//		}
	//	}
	//}
	cParticleEmitter::sParticleDesc emitDescript;
	emitDescript.lifetimeMin = emitterLifetimeMin;
	emitDescript.lifetimeMax = emitterLifetimeMax;
	emitDescript.particlesCreatedPerUpdate = 20;
	emitDescript.ongoingForce = glm::vec3(0.0f, 0.5f, 0.0f);
	emitDescript.initVelMin = glm::vec3(-0.2f, 0.0f, -0.2f);
	emitDescript.initVelMax = glm::vec3(0.2f, 1.0f, 0.2f);

	for (int i = 0; i < numberOfPlanes; ++i) {
		cGameObject* pObject = new cGameObject();
		pObject->bIsUpdatedByPhysics = false;
		pObject->bIsWireframe = false;
		pObject->position = glm::vec3(getRandFloat(-15.f, 15.f), getRandFloat(-15.f, 15.f), getRandFloat(-15.f, 15.f));
		pObject->qOrientaion = glm::quat(glm::vec3(glm::radians(getRandFloat(0.f, 360.f)), glm::radians(getRandFloat(0.f, 360.f)), glm::radians(getRandFloat(0.f, 360.f))));
		pObject->alpha = getRandFloat(0.1f, 1.0f);
		pObject->scale = 3.f;
		switch (static_cast<int>(getRandFloat(0, 2))) {
		case 0:
			pObject->meshName = "small_plane.ply";
			break;
		case 1:
			pObject->meshName = "medium_plane.ply";
			break;
		case 2:
			pObject->meshName = "large_plane.ply";
			break;
		}
		//pObject->meshName = (g_SkyBoxName == "Space") ? "tie_fighter.ply" : "jet_plane.ply";
		pObject->textureNames[1] = "bricks.bmp";
		pObject->textureNames[2] = (fenceOrSmoke) ? "fence.bmp" : "fence_inverted.bmp";
		pObject->bCanFly = true;
		pObject->particles.allocateParticles(2000, emitDescript);
		fenceOrSmoke = !fenceOrSmoke;
		vec_pGOs.push_back( pObject );
	}

	// Add a debug sphere
	cGameObject* pSphere = new cGameObject();
	pSphere->bIsUpdatedByPhysics = false;
	pSphere->bIsWireframe = true;
	pSphere->position = glm::vec3(0.f, 0.f, 0.f);
	pSphere->scale = 1.0f;
	pSphere->meshName = "isosphere_inverted.ply";
	pSphere->bIsVisible = false;
	g_DebugSphereID = pSphere->getID();
	//vec_pGOs.push_back( pSphere );
	
	// Add a rotation object
	g_pObjectToRotate = new cGameObject();
	g_pObjectToRotate->bIsUpdatedByPhysics = true;
	g_pObjectToRotate->bIsWireframe = false;
	g_pObjectToRotate->qOrientaion = glm::quat( glm::vec3(-3.14f/2.0f, 0.0f, 0.0f) );
	g_pObjectToRotate->meshName = "utah_teapot.ply";
	g_pObjectToRotate->scale = 15.0f;
	//vec_pGOs.push_back( g_pObjectToRotate );

	// Add a skybox object
	g_pSkyBoxObject = new cGameObject();
	g_pSkyBoxObject->bIsUpdatedByPhysics = false;
	g_pSkyBoxObject->colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	g_pSkyBoxObject->meshName = "isosphere_inverted.ply";
	g_pSkyBoxObject->scale = 10.0f;

	cGameObject* p2SidedQuad = new cGameObject();
	p2SidedQuad->scale = 0.5f;
	p2SidedQuad->meshName = "2_sided_quad.ply";
	p2SidedQuad->textureNames[1] = "smoke.bmp";
	p2SidedQuad->bIsImposter = true;
	g_2SidedQuadID = p2SidedQuad->getID();
	g_pImposter = p2SidedQuad;
	//vec_pGOs.push_back(p2SidedQuad);

	return true;
}

void MakeSkyBox(std::array<std::string, 7> &skyBox) {
	GLenum glErrorEnum = 0;
	std::string errorString, errorDetails;
	g_SkyBoxName = skyBox[0];

	if (g_pTextureManager->CreateNewCubeTextureFromBMPFiles(skyBox[0], skyBox[1], skyBox[2], skyBox[3], skyBox[4], skyBox[5], skyBox[6], true, glErrorEnum, errorString, errorDetails)) {
		std::cout << std::setw(10) << std::left << "[SUCCESS] " << "Success in loaded skybox: " << skyBox[0] << std::endl;
	}
	else {
		std::cout << std::setw(10) << std::left << "[ERROR] " << "Failed in loaded skybox: " << skyBox[0] << "\n\tError: " << errorString << "\n\tDetails: " << errorDetails << std::endl;
	}
}
