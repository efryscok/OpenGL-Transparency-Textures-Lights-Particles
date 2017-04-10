#include <sstream>

#include "GlobalStuff.h"

bool isShiftDownAlone(int mods) {
	if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
		return true;
	}
	return false;
}

bool isCrtlDownAlone(int mods) {
	if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
		return true;
	}
	return false;
}

bool isAltDownAlone(int mods) {
	if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
		return true;
	}
	return false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float increase = 1.05f;
	float decrease = 0.95f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

		switch (key) {
		case GLFW_KEY_U:
			g_pObjectToRotate->rotSpeed.x += -0.01f;
			break;
		case GLFW_KEY_J:
			g_pObjectToRotate->rotSpeed.x += 0.01f;
			break;
		case GLFW_KEY_H: {
			// Combine by multiplying (just like a matrix)
			glm::quat rotationChange(glm::vec3(0.0f, 0.05f, 0.0f));
			g_pObjectToRotate->qOrientaion *= rotationChange;
			break;
		}
		case GLFW_KEY_K: {
			// Combine by multiplying (just like a matrix)
			glm::quat rotationChange(glm::vec3(0.0f, -0.05f, 0.0f));
			g_pObjectToRotate->qOrientaion *= rotationChange;
			break;
		}
		case GLFW_KEY_Y: {
			// Combine by multiplying (just like a matrix)
			glm::quat rotationChange(glm::vec3(0.0f, 0.0f, 0.05f));
			g_pObjectToRotate->qOrientaion *= rotationChange;
			break;
		}
		case GLFW_KEY_I: {
			// Combine by multiplying (just like a matrix)
			glm::quat rotationChange(glm::vec3(0.0f, 0.0f, -0.05f));
			g_pObjectToRotate->qOrientaion *= rotationChange;
			break;
		}
		case GLFW_KEY_A:
			if (isShiftDownAlone(mods)) {
				lightPos.x -= g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.x > 10.f) {
					gCameraPos.x *= decrease;
				}
				else if (gCameraPos.x < -10.f) {
					gCameraPos.x *= increase;
				}
				else {
					gCameraPos.x -= g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_D:
			if (isShiftDownAlone(mods)) {
				lightPos.x += g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.x > 10.f) {
					gCameraPos.x *= increase;
				}
				else if (gCameraPos.x < -10.f) {
					gCameraPos.x *= decrease;
				}
				else {
					gCameraPos.x += g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_W:
			if (isShiftDownAlone(mods)) {
				lightPos.z -= g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.z > 10.f) {
					gCameraPos.z *= decrease;
				}
				else if (gCameraPos.z < -10.f) {
					gCameraPos.z *= increase;
				}
				else {
					gCameraPos.z -= g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_S:
			if (isShiftDownAlone(mods)) {
				lightPos.z += g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.z > 10.f) {
					gCameraPos.z *= increase;
				}
				else if (gCameraPos.z < -10.f) {
					gCameraPos.z *= decrease;
				}
				else {
					gCameraPos.z += g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_Q:
			if (isShiftDownAlone(mods)) {
				lightPos.y -= g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.y > 10.f) {
					gCameraPos.y *= decrease;
				}
				else if (gCameraPos.y < -10.f) {
					gCameraPos.y *= increase;
				}
				else {
					gCameraPos.y -= g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_E:
			if (isShiftDownAlone(mods)) {
				lightPos.y += g_CameraMovementSpeed;
			}
			else {
				if (gCameraPos.y > 10.f) {
					gCameraPos.y *= increase;
				}
				else if (gCameraPos.y < -10.f) {
					gCameraPos.y *= decrease;
				}
				else {
					gCameraPos.y += g_CameraMovementSpeed;
				}
			}
			break;
		case GLFW_KEY_1:
			if (isShiftDownAlone(mods)) {
				for (size_t i = 0; i < g_vec_lights.size(); ++i) {
					g_vec_lights[i].attenLinear *= 0.99f;
				}	// Drop it by 1%
			}
			break;
		case GLFW_KEY_2:
			if (isShiftDownAlone(mods)) {
				for (size_t i = 0; i < g_vec_lights.size(); ++i) {
					g_vec_lights[i].attenLinear *= 1.01f;
				}	// Increase by 1%
			}
			break;
		case GLFW_KEY_3:
			if (isShiftDownAlone(mods)) {
				for (size_t i = 0; i < g_vec_lights.size(); ++i) {
					g_vec_lights[i].attenQuad *= 0.99f;
				}
			}
			break;
		case GLFW_KEY_4:
			if (isShiftDownAlone(mods)) {
				for (size_t i = 0; i < g_vec_lights.size(); ++i) {
					g_vec_lights[i].attenQuad *= 1.01f;
				}
			}
			break;
		case GLFW_KEY_5:
			if (isShiftDownAlone(mods)) { ::bDrawLightDebugSpheres = false; }
			break;
		case GLFW_KEY_6:
			if (isShiftDownAlone(mods)) { ::bDrawLightDebugSpheres = true; }
			break;
		case GLFW_KEY_7:
			if (isShiftDownAlone(mods)) { ::bDrawLightDebugLocation = false; }
			break;
		case GLFW_KEY_8:
			if (isShiftDownAlone(mods)) { ::bDrawLightDebugLocation = true; }
			break;
		case GLFW_KEY_ENTER:
			if (changeTimeout <= 0) {
				flyingEnabled = !flyingEnabled;
				changeTimeout = 2;
			}
			--changeTimeout;
			break;
		case GLFW_KEY_SPACE:
			gCameraPos = glm::vec3(0.1f, 0.1f, 100.f);
			break;
		}

	// Update the title
	std::stringstream ssTitle;
	ssTitle << "Camera: " << gCameraPos.x << ", " << gCameraPos.y << ", " << gCameraPos.z
		<< "  Light: " << ::lightPos.x << ", " << ::lightPos.y << ", " << ::lightPos.z
		<< " Lin: " << g_vec_lights[0].attenLinear
		<< " Quad: " << g_vec_lights[0].attenQuad;
	glfwSetWindowTitle(window, ssTitle.str().c_str());

	return;
}
