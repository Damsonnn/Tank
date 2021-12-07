#ifndef STATICMODEL_H
#define STATICMODEL_H

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

class StaticModel {
private:
	GLuint texture;
	vector<vector<glm::vec4>> vertices;
	vector<vector<glm::vec4>> normals;
	vector<vector<glm::vec2>> texCoords;
	vector<vector<unsigned int>> indices;
	void processMesh(aiMesh* mesh);
	void processNode(aiNode* node, const aiScene* scene);
public:
	StaticModel(const char* modelPath, const char* texturePath);
	~StaticModel();
	
	void load(const char* modelPath, const char* texturePath);
	void draw(ShaderProgram* sp, glm::vec3 position, float rotation_x, float rotation_y, glm::vec3 scale);
};

#endif

