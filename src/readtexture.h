#ifndef READTEXTURE_H
#define READTEXTURE_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "lodepng.h"

GLuint readTexture(const char* filename);
#endif 