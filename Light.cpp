#include "Light.h"
#include "Window.h"

Light::Light(const char *filepath, int type){
	LtoWorld = glm::mat4(1.0f);
	
	Lcolor = glm::vec3(1.0f);
	
	//DIRECTIONAL LIGHT
	if (type == 1) {
		ambient = glm::vec3(2.0f);
		diffuse = glm::vec3(1.0f);
		specular = glm::vec3(1.0f);
		resetLightDefault();
	}
	else{
		parseLight(filepath);

		//POINT LIGHT
		if (type == 2) {
			Lposition = glm::vec3(1.0f);
			ambient = glm::vec3(2.0f);
			diffuse = glm::vec3(1.0f);
			specular = glm::vec3(1.0f);
			cons_att = 1.0f;
			line_att = 0.09f;
			quad_att = 0.032f;
			resetLightDefault();
		}

		//SPOTLIGHT
		else if (type == 3) {
			Lposition = glm::vec3(1.0f);
			ambient = glm::vec3(2.0f);
			diffuse = glm::vec3(1.0f);
			specular = glm::vec3(1.0f);
			cons_att = 1.0f;
			line_att = 0.10f;
			quad_att = 0.050f;
			cutoff = 0.1f;
			exponent = 3.0f;
			resetLightDefault();
		}

		glGenVertexArrays(1, &lVAO);
		glGenBuffers(1, &lVBO);
		glGenBuffers(1, &lVBO2);
		glGenBuffers(1, &lEBO);

		glBindVertexArray(lVAO);
		//Vertices
		glBindBuffer(GL_ARRAY_BUFFER, lVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*lvertices.size(), lvertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//Normals
		glBindBuffer(GL_ARRAY_BUFFER, lVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*lnormals.size(), lnormals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*lindices.size(), lindices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

Light::~Light(){
	glDeleteVertexArrays(1, &lVAO);
	glDeleteBuffers(1, &lVBO);
	glDeleteBuffers(1, &lVBO2);
	glDeleteBuffers(1, &lEBO);
}
void Light::parseLight(const char *filepath) {
	// Populate the face indices, vertices, and normals vectors with the Obj data
	FILE* filePointer;		// file pointer
	float x, y, z;			// vertex and normal coordinates
	unsigned int v1, v2, v3;//indices
	unsigned int n1, n2, n3;
	int c1, c2;			    // characters read from file

	filePointer = fopen(filepath, "rb");

	//make sure file was opened
	if (filePointer == NULL) {
		fprintf(stderr, "File not opened.\n");
		return;
	}
	else {
		c1 = fgetc(filePointer);

		while (c1 != EOF) {
			c1 = fgetc(filePointer);

			//skip comment lines
			if (c1 == '#') {
				while (c1 > 31) {
					c1 = fgetc(filePointer);
				}
			}

			//get second character if c1 is an actual character and not a comment line
			if (c1 > 31 && c1 != '#') {
				c2 = fgetc(filePointer);
			}

			//put vertices in the array
			if ((c1 == 'v') && (c2 == ' ')) {
				fscanf(filePointer, "%f %f %f", &x, &y, &z);
				//scale position according to constructor size
				this->lvertices.push_back(glm::vec3(x, y, z));
			}

			//put normals in the array
			else if ((c1 == 'v') && (c2 == 'n')) {
				fscanf(filePointer, " %f %f %f", &x, &y, &z);
				this->lnormals.push_back(glm::vec3(x, y, z));
			}

			//put faces in the array
			else if ((c1 == 'f') && (c2 == ' ')) {
				fscanf(filePointer, "%u//%u %u//%u %u//%u", &v1, &n1, &v2, &n2, &v3, &n3);
				lindices.push_back(--v1);
				lindices.push_back(--v2);
				lindices.push_back(--v3);
			}
		}
	}

	//print helpful line for Debugging and then close file
	fprintf(stdout, "%s  parsed\t%i\tvertices\t%i\tnormals\n\t\t\t%i\tindices\t\t%i\tfaces\n",
		filepath, (int)lvertices.size(), (int)lnormals.size(), (int)lindices.size(), (int)lindices.size() / 3);
	fclose(filePointer);
}

void Light::updateLight() {
	this->Lposition = glm::vec3(LtoWorld[3][0], LtoWorld[3][1], LtoWorld[3][2]);
	this->Ldirection = glm::vec3(-Lposition.x, -Lposition.y, -Lposition.z);
	this->light_mode = Window::lightMode;
}

void Light::drawDirectLight(GLuint shaderProgram) {

	glm::mat4 view = Window::V * this->LtoWorld;

	glUniform3f(glGetUniformLocation(shaderProgram, "light.direction"), Ldirection.x, Ldirection.y, Ldirection.z);

	glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), specular.x, specular.y, specular.z);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->LtoWorld[0][0]);
}

void Light::drawPointLight(GLuint shaderProgram) {
	
	glm::mat4 view = Window::V * this->LtoWorld;
	
	glUniform3f(glGetUniformLocation(shaderProgram, "light.color"), Lcolor.x, Lcolor.y, Lcolor.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), Lposition.x, Lposition.y, Lposition.z);
	
	glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), cons_att);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), line_att);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), quad_att);

	glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), specular.x, specular.y, specular.z);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->LtoWorld[0][0]);

	glBindVertexArray(lVAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)lindices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Light::drawSpotLight(GLuint shaderProgram) {

	glm::mat4 view = Window::V * this->LtoWorld;

	glUniform3f(glGetUniformLocation(shaderProgram, "light.color"), Lcolor.x, Lcolor.y, Lcolor.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), Lposition.x, Lposition.y, Lposition.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.direction"), Ldirection.x, Ldirection.y, Ldirection.z);

	glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), specular.x, specular.y, specular.z);

	glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), cons_att);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), line_att);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), quad_att);

	glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), cutoff);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.exponent"), exponent);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->LtoWorld[0][0]);

	glBindVertexArray(lVAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)lindices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Light::resetLightDefault() {
	LtoWorld = glm::mat4(1.0f);
	Ldirection = glm::vec3(0.0f, 0.0, -1.0f);
	Lposition = glm::vec3(0.0f, 1.0f, 0.0f);
	LtoWorld = glm::translate(LtoWorld, Lposition);
	LtoWorld = glm::scale(LtoWorld, glm::vec3(0.3f));
}

 