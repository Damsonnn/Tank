#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include "shaderprogram.h"
#include "tank.h"
#include "staticmodel.h"

float fly_speed = 0;
float aspectRatio = 1;
float tank_speed = 0;
float tank_speed_x = 0;
float tower_speed_x = 0;
float turret_speed_y = 0;
bool shoot = false;

glm::vec3 position = glm::vec3(-3,1,0);
float horizontalAngle = 3.14;
float verticalAngle = 3.14/2*3;

float mouseSpeed = 0.005f;

ShaderProgram *sp;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

// obliczanie kierunku modelu
glm::vec3 calcDir(float tank_x) {
	glm::vec4 dir = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::rotate(glm::mat4(1.0f), tank_x, glm::vec3(0, 1, 0));
	dir = M * dir;
	return glm::vec3(dir);
}

//obsługa klawiatury
void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) fly_speed = 4;
		if (key == GLFW_KEY_S) fly_speed = -4;
		if (key == GLFW_KEY_LEFT) tank_speed_x = 1;
		if (key == GLFW_KEY_RIGHT) tank_speed_x = -1;
		if (key == GLFW_KEY_UP) tank_speed = 1;
		if (key == GLFW_KEY_DOWN) tank_speed = -1;
		if (key == GLFW_KEY_J) tower_speed_x = 1;
		if (key == GLFW_KEY_L) tower_speed_x = -1;
		if (key == GLFW_KEY_K) turret_speed_y = 0.5;
		if (key == GLFW_KEY_I) turret_speed_y = -0.5;
		if (key == GLFW_KEY_SPACE) shoot = true;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) fly_speed = 0;
		if (key == GLFW_KEY_S) fly_speed = 0;
		if (key == GLFW_KEY_LEFT) tank_speed_x = 0;
		if (key == GLFW_KEY_RIGHT) tank_speed_x = 0;
		if (key == GLFW_KEY_UP) tank_speed = 0;
		if (key == GLFW_KEY_DOWN) tank_speed = 0;
		if (key == GLFW_KEY_J) tower_speed_x = 0;
		if (key == GLFW_KEY_L) tower_speed_x = 0;
		if (key == GLFW_KEY_K) turret_speed_y = 0;
		if (key == GLFW_KEY_I) turret_speed_y = 0;
		if (key == GLFW_KEY_SPACE) shoot = false;
	}
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

float my_clamp(float num, float low, float high) {
	if (num < low) return low;
	else if (num > high) return high;
	else return num;
}

//operacje inicjujące
void initOpenGLProgram(GLFWwindow* window) {

	glClearColor(0.192f,0.29f,0.545f,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	sp = new ShaderProgram("vshader.glsl", NULL, "fshader.glsl");
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
}

//tworzenie podłogi + wysłanie atrybutów
void drawScene(GLFWwindow* window, glm::vec3 direction, StaticModel *ground) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f)); 
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 50.0f); 

	sp->use();
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

	glUniform4f(sp->u("lp1"), 3, 1, 0, 1);
	glUniform4f(sp->u("lp2"), -3.5, 1.5, -3.5, 1);

	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			ground->draw(sp, glm::vec3(10 - i*2, 0, 10 - j*2),0,0, glm::vec3(1, 1, 1));
		}
	}
}


int main(void)
{
	GLFWwindow* window; 

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) { 
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	

	window = glfwCreateWindow(900, 900, "OpenGL", NULL, NULL);  

	if (!window) 
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK) { 
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	//wczytanie modeli
	initOpenGLProgram(window);
	StaticModel* bullet = new StaticModel("bomb.obj", "bomb.png");
	StaticModel* house = new StaticModel("cottage.obj", "cottage_diffuse.png");
	StaticModel* ground = new StaticModel("cobblestone.obj", "cobblestone.png");
	StaticModel* lamp = new StaticModel("lamp.obj", "lamp.png");
	StaticModel* tent = new StaticModel("tent.obj", "tent.png");
	StaticModel* plane = new StaticModel("plane.obj", "plane.png");
	Tank* tank = new Tank;

	double xpos, ypos;
	float ttk = 0;
	float bullet_angle_y;
	float bullet_angle_x;
	float bullet_speed = 5;
	float plane_speed = 4;
	float plane_speed_x = 0.2;
	float plane_angle = 0;
	float tank_angle = 0;
	float tower_angle = 0;
	float turret_angle = 0;
	glm::vec3 tank_position = glm::vec3(0, 0, 0);
	glm::vec3 plane_position = glm::vec3(0,7,0);
	glm::vec3 bullet_direction;
	glm::vec3 bullet_position;

	glfwSetTime(0); 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) 
	{	
		//obliczanie kierunku i pozycji kamery
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, 900 / 2, 900 / 2);
		horizontalAngle += mouseSpeed * float(900 / 2 - xpos);
		verticalAngle += mouseSpeed * float(900 / 2 - ypos);
		glm::vec3 direction(-cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), -cos(verticalAngle) * cos(horizontalAngle));
		position += (float)(fly_speed * glfwGetTime()) * direction;

		//obliczanie pozycji czołgu i jego elementów
		tower_angle += tower_speed_x * glfwGetTime();

		tank_angle += tank_speed_x * glfwGetTime();
		tank_position += (float)(tank_speed * glfwGetTime()) * calcDir(tank_angle);
		
		turret_angle = my_clamp(turret_angle + turret_speed_y * glfwGetTime(),-0.27,0.04);
		
		//obliczanie pozycji samolotu
		plane_angle += plane_speed_x * glfwGetTime();
		plane_position += (float)(plane_speed * glfwGetTime()) * calcDir(plane_angle);
		
		drawScene(window, direction, ground); 

		//obliczanie pozycji pocisku i jego rysowanie
		if (ttk > 0) {
			bullet_position += (float)(bullet_speed * glfwGetTime()) * bullet_direction;
			bullet->draw(sp, bullet_position, bullet_angle_y, bullet_angle_x, glm::vec3(1, 1, 1));
			ttk = my_clamp(ttk - glfwGetTime(), 0, 10);
		}
		else if (shoot) {
			bullet_position = tank_position;
			bullet_angle_y = turret_angle;
			bullet_angle_x = tank_angle + tower_angle;
			bullet_direction = glm::vec3(cos(bullet_angle_y) * sin(bullet_angle_x), -sin(bullet_angle_y), cos(bullet_angle_y) * cos(bullet_angle_x));
			ttk = 2;
		}
		
		//rysowanie reszty modeli
		tank->draw(sp, tank_angle, tank_position, tower_angle, turret_angle);
		house->draw(sp, glm::vec3(5, 0, 0), 0, 0, glm::vec3(0.4,0.4,0.4));
		lamp->draw(sp, glm::vec3(-4, 0, -4), 0, 3.14*3/4, glm::vec3(0.5, 0.5, 0.5));
		tent->draw(sp, glm::vec3(0, 0, 6), 0, 0, glm::vec3(1.2, 1.2, 1.2));
		tent->draw(sp, glm::vec3(5, 0, 5), 0, 3.14 * 5 / 4, glm::vec3(1.2, 1.2, 1.2));
		plane->draw(sp, plane_position, 0, plane_angle, glm::vec3(0.5, 0.5, 0.5));

		glfwSetTime(0); 
		glfwSwapBuffers(window); 
		glfwPollEvents(); 
	}

	freeOpenGLProgram(window);
	delete tank;
	delete ground;
	delete house;
	delete bullet;
	delete lamp;
	delete tent;
	delete plane;

	glfwDestroyWindow(window); 
	glfwTerminate(); 
	exit(EXIT_SUCCESS);
}
