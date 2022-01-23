#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Time.h"
#include <iostream>
#include <cmath>
#include <string>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputManager.h"
#include "Camera.h"
#include "GLMesh.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"


/*
	TODO:
		- Calculate vehicle acceleration.
		- Create better player camera. (find a way to get the car's angle).
		- Learn about convex hulls.
		- Create differnt colliders for each new car. (after learning convex hulls).
*/

//************************************************************************************************//

/* This is the test code for input controller mapping
 * 
 * Xbox:
 * Button[0]--A, Button[1]--B, Button[2]--X, Button[3]--Y, Button[4]--LB, Button[5]--RB, 
 * Button[6]--BACK(Left-Middle), Button[7]--START(Right-Middle), Button[8]--LSB, Button[9]--RSB, 
 * Button[10]--UP, Button[11]--RIGHT, Button[12]--Down, Button[13]--LEFT.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis(up is -1 for myself), axes[2]--Right Stick X Axis, 
 * axes[3]--Right Trigger, axes[4]--Left Trigger, axes[5]--Right Stick Y Axis(up is -1 for myself).
 * 
 *
 * PS4:
 * Button[0]--Square, Button[1]--X, Button[2]--O, Button[3]--Triangle, Button[4]--L1, Button[5]--R1, 
 * Button[6]--L2, Button[7]--R2, Button[8]--SHARE, Button[9]--OPT, 
 * Button[10]--LSB, Button[11]--RSB, Button[12]--PlayStation, Button[13]--Touchpad,
 * Button[14]--UP, Button[15]--RIGHT, Button[16]--Down, Button[17]--LEFT.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis, axes[2]--Right Stick X Axis,
 * axes[3]--Left Trigger, axes[4]--Right Trigger, axes[5]--Right Stick Y Axis(up is -1 for myself).
 * 
 * 
 * NS:
 * Button[0]--B, Button[1]--A, Button[2]--Y, Button[3]--X, Button[4]--L, Button[5]--R, 
 * Button[6]--ZL, Button[7]--ZR, Button[8]--'-', Button[9]--'+', Button[10]--LSB, Button[11]--RSB, 
 * Button[12]--Home, Button[13]--ScreenShot,Button[14]--???, Button[15]--???, Button[16]--Up, 
 * Button[17]--Righ, Button[18]--Down, Button[19]--Left.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis, 
 * axes[2]--Right Stick X Axis, axes[3]--Left Trigger.
 */
void testInput(const float* axes, const unsigned char* buttons) {
	if (GLFW_PRESS == buttons[0])
	{
		std::cout << "0" << std::endl;
	}
	else if (GLFW_PRESS == buttons[1])
	{
		std::cout << "1" << std::endl;
	}
	else if (GLFW_PRESS == buttons[2])
	{
		std::cout << "2" << std::endl;
	}
	else if (GLFW_PRESS == buttons[3])
	{
		std::cout << "3" << std::endl;
	}
	else if (GLFW_PRESS == buttons[4])
	{
		std::cout << "4" << std::endl;
	}
	else if (GLFW_PRESS == buttons[5])
	{
		std::cout << "5" << std::endl;
	}
	else if (GLFW_PRESS == buttons[6])
	{
		std::cout << "6" << std::endl;
	}
	else if (GLFW_PRESS == buttons[7])
	{
		std::cout << "7" << std::endl;
	}
	else if (GLFW_PRESS == buttons[8])
	{
		std::cout << "8" << std::endl;
	}
	else if (GLFW_PRESS == buttons[9])
	{
		std::cout << "9" << std::endl;
	}
	else if (GLFW_PRESS == buttons[10])
	{
		std::cout << "10" << std::endl;
	}
	else if (GLFW_PRESS == buttons[11])
	{
		std::cout << "11" << std::endl;
	}
	else if (GLFW_PRESS == buttons[12])
	{
		std::cout << "12" << std::endl;
	}
	else if (GLFW_PRESS == buttons[13])
	{
		std::cout << "13" << std::endl;
	}
	else if (GLFW_PRESS == buttons[14])
	{
		std::cout << "14" << std::endl;
	}
	else if (GLFW_PRESS == buttons[15])
	{
		std::cout << "15" << std::endl;
	}
	else if (GLFW_PRESS == buttons[16])
	{
		std::cout << "16" << std::endl;
	}
	else if (GLFW_PRESS == buttons[17])
	{
		std::cout << "17" << std::endl;
	}
	else if (GLFW_PRESS == buttons[18])
	{
		std::cout << "18" << std::endl;
	}
	else if (GLFW_PRESS == buttons[19])
	{
		std::cout << "19" << std::endl;
	}
	
	//std::cout << "Left Stick X Axis: " << axes[0] << std::endl;
	//std::cout << "Left Stick Y Axis: " << axes[1] << std::endl;
	//std::cout << "Right Stick X Axis: " << axes[2] << std::endl;
	//std::cout << "Right Stick Y Axis: " << axes[5] << std::endl;
	//std::cout << "Right Trigger/R2: " << axes[4] << std::endl;
	//std::cout << "Left Trigger/L2: " << axes[3] << std::endl;
}

void XboxInput(PVehicle& p1, const float* axes, const unsigned char* buttons) {
	//Xbox con code
	float throttle = 1.0f;
	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (axes[4] != -1)
	{
		p1.reverse(throttle * 0.5f);
	}
	if (axes[5] != -1)
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();

	return;
}

void PS4Input(PVehicle& p1, const float* axes, const unsigned char* buttons) {
	
	float throttle = 1.0f;
	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (axes[3] != -1)
	{
		p1.reverse(throttle * 0.5f);
	}
	if (axes[4] != -1)
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();
}

void NSInput(PVehicle& p1, const float* axes, const unsigned char* buttons) {
	float throttle = 1.0f;
	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (GLFW_PRESS == buttons[6])
	{
		p1.reverse(throttle * 0.5f);
	}
	if (GLFW_PRESS == buttons[7])
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();
}
//************************************************************************************************//

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	Utils::instance().shader = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	Camera editorCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));

	GLMesh obstacleMesh(GL_FILL), ball(GL_FILL);
	obstacleMesh.createCube(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ball.createSphere(1.0f, 25, glm::vec3(1.0f, 0.0f, 0.0f));
	Model ground = Model("models/ground/ground.obj");
	ground.translate(glm::vec3(0.0f, -1.2f, 0.0f));
	ground.scale(glm::vec3(2.0f, 1.0f, 2.0f));

	// Physx
	float throttle = 1.0f;
	PhysicsManager pm = PhysicsManager(1.0f/60.0f);
	PVehicle player = PVehicle(pm, VehicleType::eTOYOTA);
	PVehicle enemy = PVehicle(pm, VehicleType::eJEEP, PxVec3(5.0f, 0.0f, 0.0f));
	PDyanmic obstacle_d = PDyanmic(pm, PxSphereGeometry(1), PxVec3(-20.0f, 20.0f, -10.0f));
	PStatic obstacle_s = PStatic(pm, PxBoxGeometry(1.0f, 1.0f, 1.0f), PxVec3(-20.0f, 0.0f, -20.0f));

	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	playerCamera.setPitch(-30.0f);

	bool cameraToggle = false;
	int flag = 0;
	while (!window.shouldClose()) {

		Time::update();

		pm.simulate();
		player.update();
		enemy.update();
		glfwPollEvents();

		Utils::instance().shader->use();

//************************************************************************************************//		
		if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
			const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			if (flag == 0)
			{
				std::cout << name << " connected." << std::endl;
				flag = 1;
			}
			int countA;
			int axesCount;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			int buttonCount;
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
			const char* Xname = "Xbox Controller";
			const char* Pname = "PS4 Controller";
			
			//XboxInput(player, axes, buttons);
			//PS4Input(player, axes, buttons);
			//NSInput(player, axes, buttons);
			testInput(axes, buttons);
		}
//************************************************************************************************//

		if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_REPEAT))
			editorCamera.handleRotation(inputManager->getMousePosition().x, inputManager->getMousePosition().y);
		else if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE))
			editorCamera.resetLastPos();

		if (inputManager->onKeyAction(GLFW_KEY_W, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_W);
		if (inputManager->onKeyAction(GLFW_KEY_A, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_A);
		if (inputManager->onKeyAction(GLFW_KEY_S, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_S);
		if (inputManager->onKeyAction(GLFW_KEY_D, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_D);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_SHIFT);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT_CONTROL, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_CONTROL);

		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) player.handbrake();

		if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS) && Time::interval(2.0f)) 
			player.getRigidDynamic()->addForce(PxVec3(0.0, 15000.0, 0.0), PxForceMode::eIMPULSE);

		if (inputManager->onKeyAction(GLFW_KEY_C, GLFW_PRESS) && Time::interval(1.0f))
			cameraToggle = !cameraToggle;

		if (abs(player.getPosition().z) >= 101.0f || abs(player.getPosition().x) >= 101.0) player.removePhysics();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (cameraToggle) 
			editorCamera.render();
		else {
			playerCamera.setPosition(glm::vec3(player.getPosition().x, player.getPosition().y + 6.0f, player.getPosition().z + 12.0f));
			playerCamera.render();
		}

		ground.draw();
		player.render();
		enemy.render();
		obstacle_d.render(ball);
		obstacle_s.render(obstacleMesh);

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();

	}

	player.free();
	enemy.free();
	obstacle_d.free();
	obstacle_s.free();
	pm.free();

	glfwTerminate();
	return 0;
}
