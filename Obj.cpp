#include "Obj.h"
#include "Window.h"

Obj::Obj(const char *filepath) {
	toWorld = glm::mat4(1.0f);
	parse(filepath);
	max = glm::vec3(0, 0, 0);
	min = glm::vec3(0, 0, 0);
	centerOffset = findCenter(max, min);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//Vertices in VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//Normals in VBO2
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Obj::~Obj(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);
}

void Obj::bunnySetup() {
	//reposition and scale bunny
	this->setScale(1);
	
	//set to a gummy lookin material 
	this->obj_mode = 1;			//v shiny, no diffuse
	this->m_ambi = glm::vec3(0.01f, 0.10f, 0.20f);
	this->m_diff = glm::vec3(0.01f, 0.51f, 0.51f);
	this->m_spec = glm::vec3(0.77f, 0.77f, 0.77f);
	this->shininess = 128.0f;
}

void Obj::bearSetup() {
	//reposition and scale bear
	this->setScale(2);

	//set to a rocky material 
	this->obj_mode = 2;			//only diffuse, no shiny
	this->m_ambi = glm::vec3(0.21f, 0.13f, 0.05f);
	this->m_diff = glm::vec3(0.71f, 0.43f, 0.18f);
	this->m_spec = glm::vec3(0.39f, 0.27f, 0.17f);
	this->shininess = 1.0f;
}

void Obj::dragonSetup() {
	//reposition and scale dragon
	this->setScale(3);

	//set to a shiny ruby material 
	this->obj_mode = 3;			//diffuse and shiny
	this->m_ambi = glm::vec3(0.30f, 0.01f, 0.01f);
	this->m_diff = glm::vec3(0.61f, 0.04f, 0.04f);
	this->m_spec = glm::vec3(0.73f, 0.63f, 0.63f);
	this->shininess = 128.0f;
}

//returns vec3 of object's center
glm::vec3 Obj::findCenter(glm::vec3 max, glm::vec3 min) {
	//find center offset
	for (int i = 0; i < vertices.size(); ++i) {
		//find max values
		if (vertices[i].x > max.x) {
			max.x = vertices[i].x;
		}
		if (vertices[i].y > max.y) {
			max.y = vertices[i].y;
		}
		if (vertices[i].z > max.z) {
			max.z = vertices[i].z;
		}
		//find min values
		if (vertices[i].x < min.x) {
			min.x = vertices[i].x;
		}
		if (vertices[i].y < min.y) {
			min.y = vertices[i].y;
		}
		if (vertices[i].z < min.z) {
			min.z = vertices[i].z;
		}
	}
	//return model center offset
	return glm::vec3(-(max.x + min.x) / 2, -(max.y + min.y) / 2, -(max.z + min.z) / 2);
}

void Obj::parse(const char *filepath) {
	// Populate the face indices, vertices, and normals vectors with the Obj data
	FILE* filePointer;		// file pointer
	float x, y, z;			// vertex and normal coordinates
	float r, g, b;		    // vertex color
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
				fscanf(filePointer, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				//scale position according to constructor size
				this->vertices.push_back(glm::vec3(x, y, z));
			}

			//put normals in the array
			else if ((c1 == 'v') && (c2 == 'n')) {
				fscanf(filePointer, " %f %f %f", &x, &y, &z);
				this->normals.push_back(glm::vec3(x, y, z));
			}

			//put faces in the array
			else if ((c1 == 'f') && (c2 == ' ')) {
				fscanf(filePointer, "%u//%u %u//%u %u//%u", &v1, &n1, &v2, &n2, &v3, &n3);
				indices.push_back(--v1);
				indices.push_back(--v2);
				indices.push_back(--v3);
			}
		}
	}

	//print helpful line for Debugging and then close file
	fprintf(stdout, "%s  parsed\t%i\tvertices\t%i\tnormals\n\t\t\t%i\tindices\t\t%i\tfaces\n",
		filepath, (int)vertices.size(), (int)normals.size(), (int)indices.size(), (int)indices.size() / 3);
	fclose(filePointer);
}

void Obj::draw(GLuint shaderProgram) {
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 view = Window::V * this->toWorld;
	
	glUniform1i(glGetUniformLocation(shaderProgram, "material.obj_mode"), this->obj_mode);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), m_ambi.x, m_ambi.y, m_ambi.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), m_diff.x, m_diff.y, m_diff.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), m_spec.x, m_spec.y, m_spec.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), this->shininess);

	uView = glGetUniformLocation(shaderProgram, "view");
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &this->toWorld[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//method to update objects
void Obj::ObjUpdate() {
	this->toWorld = this->toWorld;
}

//function to scale and center objects
void Obj::setScale(int model) {
	//translate to origin
	this->toWorld = glm::translate(glm::mat4(1.0f), this->centerOffset)*this->toWorld;

	//set scale of BUNNY
	if (model == 1) {
		this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * this->toWorld;
	}
	//set scale of BEAR
	else if (model == 2) {
		this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f)) * this->toWorld;
	}
	//set scale of DRAGON
	else if (model == 3) {
		this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * this->toWorld;
	}
}

void Obj::fullReset() {
	this->toWorld = glm::mat4(1.0f);
}