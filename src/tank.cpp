//identycznie jak staticmodel, inaczej oblicza jedynie pozycjê elementów

#include "tank.h"

Tank::Tank() {
	load();
}

Tank::~Tank(){
	
}

void Tank::load() {
	using namespace std;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("tank.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	cout << importer.GetErrorString() << endl;
	processNode(scene->mRootNode, scene);
	texture = readTexture("tank.png");
}
void Tank::processMesh(aiMesh *mesh) {
	vector<glm::vec4> verts;
	vector<glm::vec4> norms;
	vector<glm::vec2> texc;
	vector<unsigned int> ind;
	for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
		auto const& face = mesh->mFaces[j];

		for (int k = 0; k < 3; ++k) {

			auto const& vertex = mesh->mVertices[face.mIndices[k]];
			verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z,1));

			auto const& normal = mesh->mNormals[face.mIndices[k]];
			norms.push_back(glm::vec4(normal.x, normal.y, normal.z,0));

			if (mesh->HasTextureCoords(0)) {

				auto const& uv = mesh->mTextureCoords[0][face.mIndices[k]];
				texc.push_back(glm::vec2(uv.x, uv.y));
			}
		}
	}
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			ind.push_back(face.mIndices[j]);
		}
	}
	
	vertices.push_back(verts);
	normals.push_back(norms);
	texCoords.push_back(texc);

	indices.push_back(ind);
	verts.clear();
	norms.clear();
	texc.clear();
	ind.clear();
}
void Tank::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}




void Tank::draw(ShaderProgram *sp, float tank_angle, glm::vec3 tank_position, float tower_angle, float turret_angle) {
	for (int i = 0; i < vertices.size(); i++) {

		glm::mat4 M = glm::mat4(1.0f);
		M = glm::translate(M, tank_position);
		if (i > 3) {
			M = glm::rotate(M, tank_angle + tower_angle, glm::vec3(0, 1, 0));
			if (i == 5) M = glm::rotate(M, turret_angle, glm::vec3(1, 0, 0));
		}
		else M = glm::rotate(M, tank_angle, glm::vec3(0, 1, 0));
		
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices[i].data());

		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals[i].data());

		glEnableVertexAttribArray(sp->a("texCoords"));
		glVertexAttribPointer(sp->a("texCoords"), 2, GL_FLOAT, false, 0, texCoords[i].data());

		glUniform1i(sp->u("tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, indices[i].data());

		glDisableVertexAttribArray(sp->a("vertex"));
		glDisableVertexAttribArray(sp->a("normal"));
		glDisableVertexAttribArray(sp->a("texCoords"));
	}
}