#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

GLuint createShader(const char *filename, const char *type);
GLuint createProgram(GLuint vert, GLuint frag);

#endif
