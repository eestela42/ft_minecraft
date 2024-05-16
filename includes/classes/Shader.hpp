#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
  
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <vector>

typedef struct vertexAttribute
{
	u_int location;
	u_int size;
	GLenum type;
} t_vertexAttribute;

class Shader
{
	public:
		/** @brief Compiles the shader program found at the path.*/ 
		Shader(const std::string &folderPath);
		~Shader();
		void Use();

		void SetBool(const std::string &name, bool value) const;  
		void SetInt(const std::string &name, int value) const;   
		void SetFloat(const std::string &name, float value) const;
		void SetFloat4(const std::string &name, float value1, float value2, float value3, float value4) const;
		void Setmat4(const std::string &name, glm::mat4 value) const;
		//we need all set functions for the uniforms

		std::vector<t_vertexAttribute> &GetVertexAttributes();
		static Shader *GetActiveShader();
	private:
		unsigned int ID; // the program ID
		static Shader *activeShader;
		std::vector<t_vertexAttribute> attributes;
			
		u_int CompileSingleShader(const char *path, GLenum type, std::string sType);
		void parseAttributes(const char *path);
		void CheckCompileErrors(unsigned int shader, std::string type);
};
  
#endif