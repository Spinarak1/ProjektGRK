
#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <GL/gl.h>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "Keyboard.h"
#include "Fish.h"

GLuint programColor;
GLuint programTexture;
Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model booble;
obj::Model fishModel;
obj::Model GoldFish;
obj::Model rockModel;
obj::Model seaSurfaceModel;
obj::Model seabedModel;

GLuint textureFish;
GLuint textureRock;
GLuint textureSeabed;
GLuint textureGoldFish;
GLuint textureBuble;

glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir; 
glm::vec3 cameraSide; 


glm::mat4 cameraMatrix, perspectiveMatrix;

glm::quat cameraRotation = glm::quat(1, 0, 0, 0);

float cameraAnglePitch = 0;
float cameraAngleRoll = 0;
float cameraAngleYaw = 0;


glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));


float boobleHeight = -30.0f;
int fishPositionX[30], fishPositionY[15], fishPositionZ[15];
float fishRotation[15];


float fishFlowingPositionZ = 0;

glm::mat4 createCameraMatrix()
{
	glm::quat rotX = glm::angleAxis(cameraAnglePitch, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis(cameraAngleYaw, glm::vec3(0, 1, 0));
	glm::quat rotZ = glm::angleAxis(cameraAngleRoll, glm::vec3(0, 0, 1));

	glm::quat cameraRotationChange = rotX * rotY * rotZ;

	cameraRotation = cameraRotationChange * cameraRotation;

	cameraAnglePitch = 0;
	cameraAngleYaw = 0;
	cameraAngleRoll = 0;

	cameraDir = glm::inverse(cameraRotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(cameraRotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, cameraRotation);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color, float alpha)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(program, "objectAlpha"), alpha);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float sin = sinf(time*0.4) / 2;
	if (keyboardGetMode())
	{
		keyboardUpdateCamera(&cameraPos, &cameraDir, &cameraSide, &cameraAnglePitch, &cameraAngleYaw, &cameraAngleRoll);
	}
	else
	{
		cameraRotation = glm::angleAxis(0.05f * time, glm::vec3(0, 1, 0));

		cameraPos.x = 16 * sinf(-time * 0.5f);
		cameraPos.y = 0;
		cameraPos.z = 16 * cosf(-time * 0.5f);
	}

	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f/255.0f, 62.0f/255.0f, 139.0f/255.0f, 1.0f);

	if (keyboardGetMode())
	{
		glm::mat4 GoldenFishInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(175.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.005f));
		glm::mat4 GoldenFishMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(cameraRotation)) * GoldenFishInitialTransformation;
		drawObjectTexture(&GoldFish, GoldenFishMatrix, textureGoldFish);
	}

	for (int i = 0; i <= 10; i++)
	{
		int x, y, z;
		float r;

		x = fishPositionX[i];
		y = fishPositionY[i];
		z = fishPositionZ[i];

		r = fishRotation[i];

		glm::quat rotation = glm::quat(1, 0, sinf(r * time * 3), 0);

		drawObjectTexture(&fishModel, glm::translate(glm::vec3(x + 15, y + (sinf(time) * 4.0f), z)) * glm::mat4_cast(glm::inverse(rotation)) * glm::scale(glm::vec3(2.35f)), textureFish);
	}

		if (fishFlowingPositionZ > 120)
			fishFlowingPositionZ = 0;
		else
			fishFlowingPositionZ += 0.04f;

		drawObjectTexture(&fishModel, glm::translate(glm::vec3(-2, 3, -10 + fishFlowingPositionZ)) * glm::rotate(glm::radians(sinf(7.0f * time) * 4.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.35f)), textureFish);

	for (int i = 0; i <= 25; i++)
	{
		drawObjectTexture(&rockModel, glm::translate(glm::vec3(sinf(i) * 10, -6, -30 + i * 1.0f)), textureRock);
	}

	drawObjectColor(&seaSurfaceModel, glm::translate(glm::vec3(0, 10, 0)) * glm::scale(glm::vec3(4.0f)) , glm::vec3(0.1f), 0.01f);
	drawObjectTexture(&seabedModel, glm::translate(glm::vec3(0, -10, 0)) * glm::scale(glm::vec3(1.2f)), textureSeabed);
	if (boobleHeight < 30)
	{
		drawObjectTexture(&booble, glm::translate(glm::vec3(10.1f*sin - 0.5, -4.1f + boobleHeight, 30.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(9.2f*sin, -3.2f + boobleHeight, 32.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-8.0f*sin, boobleHeight + 3.9f, 43.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-7.9f*sin, boobleHeight, 43.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(13.2f*sin, boobleHeight + 0.2f, 54.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(16.0f*sin, boobleHeight + 8.9f, -32.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(12.1f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(16.2f*sin, boobleHeight + 5.2f, -21.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(26.0f*sin, boobleHeight + 2.9f, 54.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(17.1f*sin, boobleHeight, 21.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-4.2f*sin, boobleHeight + 1.2f, 34.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(5.0f*sin, boobleHeight + 6.9f, 33.0f)*sin)*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(12.1f*sin, boobleHeight + 3.0f, 37.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(35.2f*sin, boobleHeight + 0.3f, 39.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-34.0f*sin, boobleHeight + 0.5f, -23.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-6.1f*sin, -0.1f + boobleHeight, -53.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-34.2f*sin, -0.2f + boobleHeight, -25.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(12.0f*sin, boobleHeight + 0.9f, -1.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-9.9f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-12.2f*sin, boobleHeight + 0.2f, -3.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(7.0f*sin, boobleHeight + 0.9f, -3.3f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-28.1f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(13.2f*sin, boobleHeight + 0.2f, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(10.0f*sin, boobleHeight + 0.9f, 30.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(11.1f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-15.2f*sin, boobleHeight + 2.2f, -14.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-16.0f*sin, boobleHeight + 5.9f, 53.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-17.1f*sin, boobleHeight + 3.0f, 2.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(15.2f*sin, boobleHeight + 2.2f, 36.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-18.0f*sin, boobleHeight + 6.9f, 24.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(15.1f*sin, -0.1f + boobleHeight, -23.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-14.2f*sin, -0.2f + boobleHeight, -11.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-3.0f*sin, boobleHeight + 7.9f, 53.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(6.9f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-4.2f*sin, boobleHeight + 2.2f, 54.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-14.0f*sin, boobleHeight + 0.9f, 32.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(2.1f*sin, boobleHeight, 13.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-14.2f*sin, boobleHeight + 1.2f, 16.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-9.0f*sin, boobleHeight + 0.5f, -32.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-43.1f*sin, boobleHeight, -53.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-34.2f*sin, boobleHeight + 3.2f, 2.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-54.0f*sin, boobleHeight - 0.9f, -1.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-65.1f*sin, boobleHeight + 0.0f, 2.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(15.2f*sin, boobleHeight + 0.2f, 35.0f*sin))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(18.0f*sin, boobleHeight + 0.9f, 20.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(5.1f*sin, -0.1f + boobleHeight, 21.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(3.2f*sin, -0.2f + boobleHeight, 22.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(13.0f*sin, boobleHeight + 0.2f, 14.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(34.9f*sin, boobleHeight, 12.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(26.2f*sin, boobleHeight - 4.5f, 29.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-43.0f*sin, boobleHeight + 0.9f, 23.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-4.1f*sin, boobleHeight, 30.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(4.2f*sin, boobleHeight - 4.2f, 11.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(5.0f*sin, boobleHeight + 3.9f, 18.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-15.1f*sin, boobleHeight, -27.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-3.2f*sin, boobleHeight - 0.1f, -23.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(-16.0f*sin, boobleHeight - 3.9f, -17.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(0.1f*sin, boobleHeight - 32.f, -15.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(7.2f*sin, boobleHeight + 0.2f, -13.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		drawObjectTexture(&booble, glm::translate(glm::vec3(5.0f*sin, boobleHeight + 0.9f, 5.0f))*glm::scale(glm::vec3(0.15f)), textureBuble);
		boobleHeight += 0.14f;
	}
	else {
		boobleHeight = 0;
	}

	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	GoldFish = obj::loadModelFromFile("models/GoldenFish.obj");
	fishModel = obj::loadModelFromFile("models/fish.obj");
	booble = obj::loadModelFromFile("models/booblees.obj");
	rockModel = obj::loadModelFromFile("models/rock.obj");
	seaSurfaceModel = obj::loadModelFromFile("models/flat.obj");
	seabedModel = obj::loadModelFromFile("models/flat.obj");

	textureFish = Core::LoadTexture("textures/fish.png");
	textureRock = Core::LoadTexture("textures/rock.png");
	textureSeabed = Core::LoadTexture("textures/seabed.png");
	textureGoldFish = Core::LoadTexture("textures/GoldenFish.png");
	textureBuble = Core::LoadTexture("textures/boob.png");
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(1366, 768);
	glutCreateWindow("Akwarium");
	glewInit();

	init();
	fishRandPosition(fishPositionX, fishPositionY, fishPositionZ, fishRotation);


	glutKeyboardFunc(keyboardPressed);
	glutKeyboardUpFunc(keyboardUp);

	glutSpecialFunc(keyboardSpecialPressed);
	glutSpecialUpFunc(keyboardSpecialUp);

	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
