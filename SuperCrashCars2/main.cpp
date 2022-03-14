﻿#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

#include "InputManager.h"
#include "InputController.h"

#include "Camera.h"
#include "Skybox.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"
#include "PowerUp.h"

#include "ImguiManager.h"
#include "AudioManager.h"

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Shaders
	auto defaultShader = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	auto depthShader = std::make_shared<ShaderProgram>("shaders/simpleDepth.vert", "shaders/simpleDepth.frag");
	Utils::instance().shader = defaultShader;


	// Lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(600.0f, 300.0f, 0.0f);

	// Skybox
	Skybox skybox;

	// Anti-Aliasing (Not working)
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);

	// Camera
	bool cameraToggle = false;
	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera menuCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	playerCamera.setPitch(-30.0f);

	// Physx
	PhysicsManager pm = PhysicsManager(1.5f/60.0f);
	PVehicle player = PVehicle(2, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 30.0f, 0.0f));
	PVehicle enemy = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -10.0f));
	PVehicle enemy2 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -20.0f));
	PVehicle enemy3 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -30.0f));
	PVehicle enemy4 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -40.0f));

	PowerUp powerUp1 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(-20.0f, 20.0f, -30.0f));
	PowerUp powerUp2 = PowerUp(pm, Model("models/powerups/boost/turbo.obj"), PowerUpType::eBOOST, PxVec3(163.64, 77.42f + 5.0f, -325.07f));
	PowerUp powerUp3 = PowerUp(pm, Model("models/powerups/health_star/health.obj"), PowerUpType::eHEALTH, PxVec3(-20.0f, 20.0f, -50.0f));
	
	std::vector<PVehicle*> vehicleList;
	std::vector<PowerUp*> powerUps;
	vehicleList.push_back(&player);
	vehicleList.push_back(&enemy);
	vehicleList.push_back(&enemy2);
	vehicleList.push_back(&enemy3);
	vehicleList.push_back(&enemy4);
	powerUps.push_back(&powerUp1);
	powerUps.push_back(&powerUp2);
	powerUps.push_back(&powerUp3);

	// AI toggle
	bool ai_ON = true;
	
	// Controller
	InputController controller1, controller2, controller3, controller4;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1 = InputController(GLFW_JOYSTICK_1);
	if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
		Log::info("Controller 2 connected");
		controller2 = InputController(GLFW_JOYSTICK_2);
	}
	// ImGui 
	ImguiManager imgui(window);

	// Audio 
	AudioManager::get().init();

	// Menu
	GameManager::get().initMenu();

	// Shadows
	const unsigned int SHADOW_WIDTH = 2048 * 4, SHADOW_HEIGHT = 2048 * 4;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!window.shouldClose() && !GameManager::get().quitGame) {
		
		// always update the time and poll events
		Time::update();
		glfwPollEvents();
		glEnable(GL_DEPTH_TEST);

		// main switch to decide what screen to display
		switch (GameManager::get().screen){
		case Screen::eMAINMENU:
			if (Time::shouldSimulate) {
				Time::startSimTimer();

				// read inputs 
				if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(false, player);
				
				Time::simulatePhysics(); // not technically physics but we reset the bool + timer here
			}
			if (Time::shouldRender) { // render at 60fps even in menu
				Time::startRenderTimer();
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glFrontFace(GL_CW);
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Utils::instance().shader->use();
				Utils::instance().shader->setVector4("lightColor", lightColor);
				Utils::instance().shader->setVector3("lightPos", lightPos); // later we will change this to the actual light position, leave as this for now
				Utils::instance().shader->setVector3("camPos", menuCamera.getPosition());


				skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// imGUI section
				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();


				glDisable(GL_FRAMEBUFFER_SRGB);
				window.swapBuffers();
				Time::renderFrame(); // turn off the render flag and stop timer
			}
			break;
		case Screen::eLOADING: // not used rn 
			// if we ever have a loading screen (maybe)
			break;
		case Screen::ePLAYING:
			// simulate when unpaused, otherwise just grab the inputs.
			if (Time::shouldSimulate) {

				if (GameManager::get().paused) { // paused, read the inputs using the menu function
					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(false, player);
				} 
				else {
					Time::startSimTimer();

#pragma region inputs

					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(true, player);

					if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(player.vehicleParams.k_throttle);
					if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) {
						player.handbrake();
						Time::resetStats();
					}
					if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS)) player.jump();
					if (inputManager->onKeyAction(GLFW_KEY_F, GLFW_PRESS)) player.boost();
					if (inputManager->onKeyAction(GLFW_KEY_R, GLFW_PRESS) || GameManager::get().startFlag == true) {
						player.reset();
						enemy.reset();
						Time::resetStats();
						GameManager::get().startFlag = false;

					}

#pragma endregion

					AudioManager::get().setListenerPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec(), player.getUpVec());

					// applying collisions in main thread instead of collision thread

					for (PVehicle* carPtr : vehicleList) {
						if (carPtr->vehicleAttr.collided) {
							carPtr->getRigidDynamic()->addForce((carPtr->vehicleAttr.forceToAdd), PxForceMode::eIMPULSE);
							carPtr->vehicleAttr.collided = false;
							AudioManager::get().playSound(SFX_CAR_HIT, Utils::instance().pxToGlmVec3(carPtr->getPosition()), 0.5f);
							Log::debug("Player coeff: {}", player.vehicleAttr.collisionCoefficient);
							Log::debug("Enemy coeff: {}", enemy.vehicleAttr.collisionCoefficient);
						}

						if (carPtr->vehicleAttr.reachedTarget && carPtr->carid == 1) {
							int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
							if (vehicleList[rndIndex] != carPtr) {
								carPtr->vehicleAttr.reachedTarget = false;
								carPtr->chaseVehicle(*vehicleList[rndIndex]);
							}
						}
						
						// update car state
						carPtr->updateState();

					}


					// handling triggers of powerUps in the main thrad instead of the collision thread
					for (PowerUp* powerUpPtr : powerUps) {
						if (powerUpPtr->triggered) {
							powerUpPtr->triggered = false;
							AudioManager::get().playSound(SFX_ITEM_COLLECT, Utils::instance().pxToGlmVec3(powerUpPtr->getPosition()), 0.65f);
							powerUpPtr->destroy();
						}
					}

					if (ai_ON) {
						for (int i = 0; i < vehicleList.size(); i++) {
							if (vehicleList[i]->carid != 1) continue;
							PVehicle* targetVehicle = (PVehicle*)vehicleList[i]->vehicleAttr.targetVehicle;
							if (targetVehicle) vehicleList[i]->chaseVehicle(*targetVehicle);
							else {
								int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
								if (vehicleList[rndIndex] != vehicleList[i]) {
									vehicleList[i]->chaseVehicle(*vehicleList[rndIndex]);
								}
							}
						}
					}

					pm.simulate();
					for (PVehicle* vehicle : vehicleList) vehicle->updatePhysics();
					Time::simulatePhysics();
				}
			}
			if (Time::shouldRender) {
					Time::startRenderTimer();

					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);
					glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					#pragma region Shadow map

					glCullFace(GL_FRONT);
					glFrontFace(GL_CCW);

					// 1. render depth of scene to texture (from light's perspective)
					// --------------------------------------------------------------
					glm::mat4 lightProjection, lightView;
					glm::mat4 lightSpaceMatrix;
					float near_plane = 1.0f, far_plane = 7.5f;
					//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
					lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 0.1f, 1500.0f);
					lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
					lightSpaceMatrix = lightProjection * lightView;
					// render scene from light's point of view
					

					Utils::instance().shader = depthShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					//Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());


					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);
					glActiveTexture(GL_TEXTURE0);
	
					pm.drawGround();
					for (PVehicle* vehicle : vehicleList) vehicle->render();
					for (PowerUp* powerUp : powerUps) powerUp->render();

					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					// reset viewport
					glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);
					#pragma endregion 


					Utils::instance().shader = defaultShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					//Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());


					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, depthMap);


					playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());
					pm.drawGround();
					for (PVehicle* vehicle : vehicleList) vehicle->render();
					for (PowerUp* powerUp : powerUps) powerUp->render();
					skybox.draw(playerCamera.getPerspMat(), glm::mat4(glm::mat3(playerCamera.getViewMat())));

					if (GameManager::get().paused) {
						// if game is paused, we will render an overlay.
						// render the PAUSE MENU HERE
					}

					// imgui
					imgui.initFrame();
					imgui.renderStats(player);
					//imgui.renderSliders(player, enemy);
					imgui.renderMenu(ai_ON);
					imgui.renderDamageHUD(vehicleList);
					imgui.renderPlayerHUD(player);
					imgui.endFrame();

					glDisable(GL_FRAMEBUFFER_SRGB);
					window.swapBuffers();

					Time::renderFrame(); // turn off the render flag and stop timer

				}
			break;
		case Screen::eGAMEOVER:
			// gome over not implemented yet
			break;
		}
	}

	player.free();
	enemy.free();
	pm.free();
	imgui.freeImgui();

	glfwTerminate();
	return 0;
}

void Render() 
{

}
