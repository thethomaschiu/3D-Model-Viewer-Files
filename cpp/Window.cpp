#include "Window.h"

const char* window_title = "GLFW Starter Project";
Cube * cube;
Obj * curObj;
Obj * bunny;
Obj * bear;
Obj * dragon;
//LIGHTS
Light * curLight;
Light * direct;
Light * point;
Light * spot;


GLint shaderProgram, toonShader;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

double Window::prev_x, Window::prev_y = 0;
double Window::cur_x, Window::cur_y = 0;
bool Window::rotate, Window::translate, Window::rotLight, Window::transLight = false;
int Window::lightMode = 2;
int Window::renderMode = 1;
bool Window::lightControl = false;

void Window::initialize_objects() {
	//cube = new Cube();
	//show cube outline to test for size of other models
	//cube->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)) * cube->toWorld;
	bunny = new Obj("bunny.obj");
	bunny->bunnySetup();
	bear = new Obj("bear.obj");
	bear->bearSetup();
	dragon = new Obj("dragon.obj");
	dragon->dragonSetup();

	//Init lights
	direct = new Light("", 1);
	point = new Light("sphere.obj", 2);
	spot = new Light("cone.obj", 3);

	curObj = bunny;
	curLight = point;

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	toonShader = LoadShaders("../toonvert.vert", "../toonfrag.frag");
	//shaderProgram = toonShader;
}

void Window::clean_up() {
	//objects
	delete(cube);
	delete(bunny);
	delete(bear);
	delete(dragon);
	//lights
	delete(direct);
	delete(point);
	delete(spot);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height) {
	// Initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height) {
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback() {
	// Call the update function the cube
	//cube->update();
	curObj->ObjUpdate();
	if (curLight == direct) {
		curLight->Lposition = glm::vec3(curLight->LtoWorld[3][0], curLight->LtoWorld[3][1], curLight->LtoWorld[3][2]);
		curLight->light_mode = lightMode;
	}
	else {
		curLight->updateLight();
	}
}

void Window::display_callback(GLFWwindow* window) {
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Use the shader of programID
	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "lightObj"), 0);

	// Render the cube
	//cube->draw(shaderProgram);
	curObj->draw(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "lightObj"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "renderMode"), renderMode);

	if (renderMode == 2) {
		if (lightMode == 1) {
			glUniform1i(glGetUniformLocation(shaderProgram, "light.mode"), 1);
			curLight->drawDirectLight(shaderProgram);
		}
		else if (lightMode == 2) {
			glUniform1i(glGetUniformLocation(shaderProgram, "light.mode"), 2);
			curLight->drawPointLight(shaderProgram);
		}
		else if (lightMode == 3) {
			glUniform1i(glGetUniformLocation(shaderProgram, "light.mode"), 3);
			curLight->drawSpotLight(shaderProgram);
		}
	}

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Check for a key press
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE) {
			// Close the window. This causes the program to also terminate.
			fprintf(stdout, "ESC pressed\n");
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		//Press F1 to switch to bunny
		else if (key == GLFW_KEY_F1) {
			fprintf(stdout, "F1 pressed\n");
			bunny->fullReset();
			bunny->setScale(1);
			curObj = bunny;
		}
		//Press F2 to switch to bear
		else if (key == GLFW_KEY_F2) {
			fprintf(stdout, "F2 pressed\n");
			bear->fullReset();
			bear->setScale(2);
			curObj = bear;
		}
		//Press F3 to switch to dragon
		else if (key == GLFW_KEY_F3) {
			fprintf(stdout, "F3 pressed\n");
			dragon->fullReset();
			dragon->setScale(3);
			curObj = dragon;
		}

		//Press 0 to switch back to model control
		else if (key == GLFW_KEY_0) {
			fprintf(stdout, "0 pressed\n");
			lightControl = false;
		}
		//Press 1 to switch to directional light
		else if (key == GLFW_KEY_1) {
			fprintf(stdout, "1 pressed\n");
			lightMode = 1;
			lightControl = true;
			curLight = direct;
		}
		//Press 2 to switch to point light
		else if (key == GLFW_KEY_2) {
			fprintf(stdout, "2 pressed\n");
			lightMode = 2;
			lightControl = true;
			curLight = point;
		}
		//Press 3 to switch to spotlight
		else if (key == GLFW_KEY_3) {
			fprintf(stdout, "3 pressed\n");
			lightMode = 3;
			lightControl = true;
			curLight = spot;
		}

		//Press r to reset model
		if (key == GLFW_KEY_R) {
			fprintf(stdout, "reset model\n");
			curObj->fullReset();
			curLight->resetLightDefault();
			//reset scale and pos depending on curObj
			if (curObj == bunny) {
				curObj->setScale(1);
			}
			else if (curObj == bear) {
				curObj->setScale(2);
			}
			else if (curObj == dragon) {
				curObj->setScale(3);
			}
		}

		//Press s/S to scale down/up
		if (key == GLFW_KEY_S) {
			//scale up
			if (mods & GLFW_MOD_SHIFT) {
				fprintf(stdout, "Scaling up\n");
				curObj->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f, 1.2f, 1.2f)) * curObj->toWorld;
			}
			//scale down
			else {
				fprintf(stdout, "scaling down\n");
				curObj->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f)) * curObj->toWorld;
			}
		}

		//Press n to switch rendering mode
		if (key == GLFW_KEY_N) {
			//Switch render mode
			if (renderMode == 1) {
				renderMode = 2;
				
			}
			else {
				renderMode = 1;
				if (lightControl) {
					lightControl = false;
				}
			}
			lightMode = curLight->light_mode;
			fprintf(stdout, "switching rendering mode to %i\n", renderMode);
		}

		//Press w/W to shrink/Grow spotlight
		if (key == GLFW_KEY_W) {
			//Grow spot
			if (mods & GLFW_MOD_SHIFT) {
				fprintf(stdout, "Grow spotlight\n");
				curLight->cutoff -= 0.05f;
			}
			//shrink spot
			else {
				fprintf(stdout, "shrink spotlight\n");
				curLight->cutoff += 0.05f;
			}
		}

		//Press e/E to blur/Sharpen spotlight edge
		if (key == GLFW_KEY_E) {
			//Sharpen
			if (mods & GLFW_MOD_SHIFT) {
				fprintf(stdout, "Sharpen edge\n");
				curLight->exponent -= 0.5f;
			}
			//shrink spot
			else {
				fprintf(stdout, "blur edge\n");
				curLight->exponent += 0.5f;
			}
		}
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	//check for clicks
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		//translate obj or light
		!lightControl ? translate = true : transLight = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &prev_x, &prev_y);
		//control obj or light
		!lightControl ? rotate = true : rotLight = true;
	}
	else {
		translate = false;
		rotate = false;
		rotLight = false;
		transLight = false;
	}
}

//function to map trackball movement, returns a glm::vec3
glm::vec3 Window::trackballMap(double coordX, double coordY) {
	glm::vec3 v;
	float d;
	v.x = (float)(2.0f*coordX - width) / width;
	v.y = (float)(height - 2.0f*coordY) / height;
	v.z = 0.0f;
	d = glm::length(v);
	d = (d < 1.0f) ? d : 1.0f;
	v.z = (float)sqrt(1.001f - d*d);
	glm::normalize(v);
	return v;
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	//map prev_pos
	glm::vec3 vPrev = trackballMap(prev_x, prev_y);
	//save (x,y) = cur_pos
	glfwGetCursorPos(window, &cur_x, &cur_y);
	//map cur_pos
	glm::vec3 vCur = trackballMap(cur_x, cur_y);
	//calculate rotation
	glm::vec3 direction = glm::cross(vPrev, vCur);

	//TRANSLATION
	glm::vec3 move = glm::vec3((float)-(prev_x - cur_x) / 30, (float)(prev_y - cur_y) / 30, 0);
	if (translate) {
		curObj->toWorld = glm::translate(glm::mat4(1.0f), move)*curObj->toWorld;
	}
	else if (transLight) {
		curLight->LtoWorld = glm::translate(glm::mat4(1.0f), move)*curLight->LtoWorld;
	}

	//ROTATION
	if (rotate) {
		curObj->toWorld = glm::rotate(glm::mat4(1.0f), 0.08f, direction)*curObj->toWorld;
	}
	//roatation of light
	else if (rotLight) {
		//special case for directional light
		if (lightMode == 1) {
			curLight->Ldirection = glm::vec3(curLight->LtoWorld[0][0], curLight->LtoWorld[1][1], curLight->LtoWorld[2][2]);
		}
		curLight->LtoWorld = glm::rotate(glm::mat4(1.0f), 0.08f, direction)*curLight->LtoWorld;
	}

	//set prev_pos = cur_pos
	prev_x = cur_x;
	prev_y = cur_y;
}

//Scrolling mouse wheel translates objects forward or backward.
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//object forward/back if we are not controling light
	if (!lightControl) {
		if (yoffset > 0) {
			curObj->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1.1f))*curObj->toWorld;
		}
		else if (yoffset < 0) {
			curObj->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.1f))*curObj->toWorld;
		}
	}
	//when controling light scroll wheel moves object towards/away from center
	else {
		if (curLight == point) {
			if (yoffset < 0) {
				curLight->LtoWorld = curLight->LtoWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1.1f));
			}
			else if (yoffset > 0) {
				curLight->LtoWorld = curLight->LtoWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.1f));
			}
		}
		else {
			if (yoffset < 0) {
				curLight->LtoWorld = curLight->LtoWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.1f, 0));
			}
			else if (yoffset > 0) {
				curLight->LtoWorld = curLight->LtoWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.1f, 0));
			}
		}
	}
}
