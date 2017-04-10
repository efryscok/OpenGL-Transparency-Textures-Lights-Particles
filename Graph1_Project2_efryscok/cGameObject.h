#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

#include "cParticleEmitter.h"

class cGameObject {
public:
	bool bIsWireframe;
	bool bUseDebugColours;
	bool bIsVisible;
	bool bIsImposter;
	bool bIsUpdatedByPhysics;
	bool bCanFly;

	int meshID;

	float scale;
	float radius;
	float alpha;

	std::string meshName;

	std::vector<std::string> textureNames;
	std::vector<cGameObject*> vec_pChildren;

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 position;
	glm::vec3 lastPosition;
	glm::vec3 rotSpeed;
	glm::vec3 colour;

	glm::quat qOrientaion;

	cParticleEmitter particles;

public:
	cGameObject();

	int getID(void);

private:
	int m_uniqueID;
	static int m_nextID;
};