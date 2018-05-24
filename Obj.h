#ifndef _OBJ_H_
#define _OBJ_H_

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Obj{
public:
	Obj(const char* filepath);
	~Obj();

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	glm::mat4 toWorld;
	glm::vec3 max;
	glm::vec3 min;
	glm::vec3 centerOffset;

	//material stuff
	int obj_mode;
	glm::vec3 m_ambi;
	glm::vec3 m_diff;
	glm::vec3 m_spec;
	float shininess;

	GLuint VBO, VAO, EBO, VBO2;
	GLuint uModel, uView, uProjection;

	void parse(const char* filepath);
	void bunnySetup();
	void bearSetup();
	void dragonSetup();
	glm::vec3 findCenter(glm::vec3, glm::vec3);
	void draw(GLuint);
	void ObjUpdate();
	void setScale(int);
	void fullReset();
};

#endif