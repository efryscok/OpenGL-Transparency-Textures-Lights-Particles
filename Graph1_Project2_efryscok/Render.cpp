#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlobalStuff.h"
#include "Utilities.h"
#include "cGameObject.h"

unsigned int g_DebugSphereID = 0;
unsigned int g_1SidedQuadID = 0;
unsigned int g_2SidedQuadID = 0;

void DrawObject(cGameObject* pCurGO, glm::mat4x4 matModel, bool bUseLighting) {
	if (!pCurGO->bIsVisible) {
		return;
	}

	std::string meshModelName = pCurGO->meshName;

	GLuint VAO_ID = 0;
	int numberOfIndices = 0;
	float unitScale = 1.0f;
	std::vector<cPlyVertexXYZ_N_STU> normals;
	if (!g_pTheMeshTypeManager->LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale, normals)) {
		return;
	}

	matModel = glm::translate(matModel, pCurGO->position);

	glm::mat4 matRotation(pCurGO->qOrientaion);
	matModel = matModel * matRotation;

	float actualScale = pCurGO->scale * unitScale;
	matModel = glm::scale(matModel, glm::vec3(actualScale, actualScale, actualScale));

	if (pCurGO->bIsWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	else {
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Setup texture bindings
	std::vector<GLint> uniformLocs;

	uniformLocs.push_back(UniformLoc_texSamp2D_00);
	uniformLocs.push_back(UniformLoc_texSamp2D_01);
	uniformLocs.push_back(UniformLoc_texSamp2D_02);
	uniformLocs.push_back(UniformLoc_texSamp2D_03);

	for (size_t i = 0; i < uniformLocs.size(); ++i) {
		GLuint textureNumber = g_pTextureManager->getTextureIDFromName(pCurGO->textureNames[i]);

		if (textureNumber == 0) {
			textureNumber = g_pTextureManager->getTextureIDFromName("white.bmp");
		}

		glActiveTexture(i + GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureNumber);
		glUniform1i(uniformLocs[i], i);
	}

	glUniform1i(UniformLoc_bUseTextureAsDiffuse, TRUE);
	glUniform1i(UniformLoc_bIsImposterBlackIsAlpha, (pCurGO->bIsImposter) ? TRUE : FALSE);
	glUniform1i(UniformLoc_bUseLighting, (bUseLighting) ? TRUE : FALSE);

	g_pTheShaderManager->useShaderProgram("phong");

	glUniformMatrix4fv(UniformLoc_matModel, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matModel));

	// Setting the uniform colours
	glUniform4f(UniformLoc_ID_objectColour, pCurGO->colour.r, pCurGO->colour.g, pCurGO->colour.b, pCurGO->alpha);
	glUniform1i(UniformLoc_ID_isWireframe, (pCurGO->bIsWireframe) ? TRUE : FALSE);
	glUniform1i(UniformLoc_ID_bUseDebugDiffuseNoLighting, (pCurGO->bUseDebugColours) ? TRUE : FALSE);

	// "turns on" transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set transparency for all objects in scene
	glUniform1f(UniformLoc_alpha, pCurGO->alpha);

	glBindVertexArray(VAO_ID);
	glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);

	// Draw any children
	for (int index = 0; index != pCurGO->vec_pChildren.size(); index++) {
		DrawObject(pCurGO->vec_pChildren[index], matModel, bUseLighting);
	}

	return;
}

void DrawDebugLightSpheres(void) {
	cGameObject* pDebugBall = ::findObjectByID(g_DebugSphereID);

	for (size_t i = 0; i < g_vec_lights.size(); ++i) {
		// Draw HOT PINK ball where the light is
		if (::bDrawLightDebugLocation) {
			DrawDebugBall(g_vec_lights[i].position, glm::vec3(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f), 0.25f);
		}

		if (::bDrawLightDebugSpheres) {
			// Draw a red ball
			float distanceAt75Percent = calcApproxDistFromAtten(0.75f, g_vec_lights[i].attenConst, g_vec_lights[i].attenLinear, g_vec_lights[i].attenQuad, 0.0001f, 1000.0f);
			DrawDebugBall(g_vec_lights[i].position, glm::vec3(1.0f, 0.0f, 0.0f), distanceAt75Percent);

			// Draw a yellow ball
			float distanceAt50Percent = calcApproxDistFromAtten( 0.50f, g_vec_lights[i].attenConst, g_vec_lights[i].attenLinear, g_vec_lights[i].attenQuad, 0.0001f, 1000.0f);
			DrawDebugBall(g_vec_lights[i].position, glm::vec3(1.0f, 1.0f, 0.0f), distanceAt50Percent);

			// Draw a green ball
			float distanceAt25Percent = calcApproxDistFromAtten(0.25f, g_vec_lights[i].attenConst, g_vec_lights[i].attenLinear, g_vec_lights[i].attenQuad, 0.0001f, 1000.0f);
			DrawDebugBall(g_vec_lights[i].position, glm::vec3(0.0f, 1.0f, 0.0f), distanceAt25Percent);

			// Draw a blue ball
			float distanceAt01Percent = calcApproxDistFromAtten(0.01f, g_vec_lights[i].attenConst, g_vec_lights[i].attenLinear, g_vec_lights[i].attenQuad, 0.0001f, 1000.0f);
			DrawDebugBall(g_vec_lights[i].position, glm::vec3(0.0f, 0.0f, 1.0f), distanceAt01Percent);
		}
	}

	return;
}

void DrawDebugBall(glm::vec3 position, glm::vec3 colour, float scale) {
	// And even more drawing...
	cGameObject* pDebugBall = ::findObjectByID(g_DebugSphereID);

	// Save old state
	glm::vec3 oldPosition(pDebugBall->position);
	glm::vec3 oldColour(pDebugBall->colour);
	float oldScale = pDebugBall->scale;
	bool oldIsVisible = pDebugBall->bIsVisible;
	bool oldIsWireframe = pDebugBall->bIsWireframe;
	bool oldbUseDebugColours = pDebugBall->bUseDebugColours;

	// Set values
	pDebugBall->position = position;
	pDebugBall->colour = colour;
	pDebugBall->bIsWireframe = true;
	pDebugBall->bIsVisible = true;
	pDebugBall->bUseDebugColours = true;
	pDebugBall->scale = scale;

	glm::mat4x4 matModel(1.0f);	// Identity
	DrawObject(pDebugBall, matModel, false);

	// Restore state...
	pDebugBall->position = oldPosition;
	pDebugBall->colour = oldColour;
	pDebugBall->scale = oldScale;
	pDebugBall->bIsVisible = oldIsVisible;
	pDebugBall->bIsWireframe = oldIsWireframe;
	pDebugBall->bUseDebugColours = oldbUseDebugColours;

	return;
}

void DrawQuad(bool bTwoSided, glm::vec3 position, glm::vec3 rotation, std::string texture, float scale, glm::vec3 debugColour, bool bUseDebugColour, bool bIsWireFrame) {
	cGameObject* pQuad = 0;

	if (bTwoSided) { 
		pQuad = g_pImposter;
	}
	else { 
		pQuad = g_pImposter;
	}

	// Save old state
	glm::vec3 oldPosition(pQuad->position);
	glm::vec3 oldColour(pQuad->colour);
	float oldScale = pQuad->scale;
	bool oldIsVisible = pQuad->bIsVisible;
	bool oldIsWireframe = pQuad->bIsWireframe;
	bool oldbUseDebugColours = pQuad->bUseDebugColours;

	// Set values
	pQuad->position = position;
	pQuad->bUseDebugColours = bUseDebugColour;
	pQuad->colour = debugColour;
	pQuad->bIsWireframe = bIsWireFrame;
	pQuad->bIsVisible = true;
	pQuad->scale = scale;

	glm::mat4x4 matModel(1.0f);
	DrawObject(pQuad, matModel, false);

	// Restore state...
	pQuad->position = oldPosition;
	pQuad->bUseDebugColours = oldbUseDebugColours;
	pQuad->colour = oldColour;
	pQuad->bIsWireframe = oldIsWireframe;
	pQuad->bIsVisible = oldIsVisible;
	pQuad->scale = oldScale;

	return;
}