#ifndef _LIGHT_H_
#define _LIGHT_H_

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Light{
public:
	Light(const char *filepath, int);
	~Light();

	std::vector<unsigned int> lindices;
	std::vector<glm::vec3> lvertices;
	std::vector<glm::vec3> lnormals;

	glm::mat4 LtoWorld;

	GLuint lVBO, lVAO, lEBO, lVBO2;

	int light_mode;

	glm::vec3 Lcolor;
	glm::vec3 Lposition;
	glm::vec3 Ldirection;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float cons_att;
	float line_att;
	float quad_att;

	float cutoff;
	float exponent;

	void parseLight(const char *filepath);
	void updateLight();
	void drawDirectLight(GLuint);
	void drawPointLight(GLuint);
	void drawSpotLight(GLuint);
	void resetLightDefault();
};
#endif