#include "cGameObject.h"

int cGameObject::m_nextID = 1;

int cGameObject::getID(void) {
	return this->m_uniqueID;
}

cGameObject::cGameObject() {
	this->bIsWireframe = false;
	this->bUseDebugColours = false;
	this->bIsVisible = true;
	this->bIsImposter = false;
	this->bIsUpdatedByPhysics = false;
	this->bCanFly = false;

	this->meshID = 0;

	this->scale = 1.0f;
	this->radius = 0.0f;
	this->alpha = 1.0f;

	this->meshName = "N/A";

	this->textureNames = std::vector<std::string>(32);


	this->m_uniqueID = cGameObject::m_nextID;
	cGameObject::m_nextID++;

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 position;
	glm::vec3 lastPosition;
	glm::vec3 rotSpeed;
	glm::vec3 colour;

	this->velocity = glm::vec3(0.f, 0.f, 0.f);
	this->acceleration = glm::vec3(0.f, 0.f, 0.f);
	this->position = glm::vec3(0.f, 0.f, 0.f);
	this->lastPosition = glm::vec3(0.f, 0.f, 0.f);
	this->rotSpeed = glm::vec3(0.f, 0.f, 0.f);
	this->colour = glm::vec3(1.f, 1.f, 1.f);

	return;
}
