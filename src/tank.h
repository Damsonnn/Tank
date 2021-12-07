
#ifndef TANK_H
#define TANK_H

#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "readtexture.h"
#include "shaderprogram.h"

using namespace std;

class Tank {
private:
	GLuint texture;
	vector<vector<glm::vec4>> vertices;
	vector<vector<glm::vec4>> normals;
	vector<vector<glm::vec2>> texCoords;
	vector<vector<unsigned int>> indices;
	void processMesh(aiMesh* mesh);
	void processNode(aiNode* node, const aiScene* scene);
public:
	Tank();
	~Tank();
	
	void load();
	void draw(ShaderProgram* sp, float tank_angle, glm::vec3 tank_position, float tower_angle, float turret_angle);
};

#endif
