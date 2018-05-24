#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "Obj.h"
#include "Light.h"
#include "shader.h"

class Window {
public:
	//VARIABLES
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static double prev_x, prev_y;
	static double cur_x, cur_y;
	static bool rotate, translate, rotLight, transLight;
	static int lightMode;
	static int renderMode;
	static bool lightControl;


	//FUNCTIONS
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


	static glm::vec3 trackballMap(double coordX, double coordY);
};

#endif
