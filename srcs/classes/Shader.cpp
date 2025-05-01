#include <classes/Shader.hpp>

#define VRTX 1 << 0
#define FGMT 1 << 1
#define GMTR 1 << 2
#define CMPT 1 << 3

Shader *Shader::activeShader = NULL;
Shader::~Shader()
{
	
}

Shader::Shader(const std::string &folderPath) {
	u_char activeShaders = 0;
	ID = glCreateProgram();
	//check if path = file
	

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			const std::string& filePath = entry.path().string();
			const std::string& fileName = entry.path().filename().string();

			if (fileName.find("vertex_shader") != std::string::npos) {
				GLuint vertexShader = CompileSingleShader(filePath.c_str(), GL_VERTEX_SHADER, "VERTEX");
				glAttachShader(ID, vertexShader);
				glDeleteShader(vertexShader);
				activeShaders |= VRTX;
				parseAttributes(filePath.c_str());
			} else if (fileName.find("fragment_shader") != std::string::npos) {
				GLuint fragmentShader = CompileSingleShader(filePath.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");
				glAttachShader(ID, fragmentShader);
				glDeleteShader(fragmentShader);
				activeShaders |= FGMT;
			} else if (fileName.find("geometry_shader") != std::string::npos) {
				GLuint geometryShader = CompileSingleShader(filePath.c_str(), GL_GEOMETRY_SHADER, "GEOMETRY");
				glAttachShader(ID, geometryShader);
				glDeleteShader(geometryShader);
				activeShaders |= GMTR;
			}
			else if (fileName.find("compute_shader") != std::string::npos) {
				GLuint computeShader = CompileSingleShader(filePath.c_str(), GL_COMPUTE_SHADER, "COMPUTE");
				glAttachShader(ID, computeShader);
				glDeleteShader(computeShader);
				activeShaders |= CMPT;
				break;
			}	
		}
	}
	if ((!(activeShaders & VRTX) || !(activeShaders & FGMT)) && !(activeShaders & CMPT)) {
		assert(!"Shader::Shader Minimum shader requirement not met");
	}
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");
}

u_int Shader::CompileSingleShader(const char *path, GLenum type, std::string sType) {
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

	try 
    {
		shaderFile.open(path);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}

	const char * cShaderCode = shaderCode.c_str();
	u_int shader = glCreateShader(type);
	glShaderSource(shader, 1, &cShaderCode, NULL);
	glCompileShader(shader);
	CheckCompileErrors(shader, sType);

	return shader;
}

void Shader::parseAttributes(const char *path) {
    std::ifstream file(path);

	if (!file.is_open())
    {
        std::cout << "Error opening file : " << path << std::endl;
        assert(0);
    }
	// std::cout << "Vertex Shader at " << path << std::endl;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string token;

        if (line.find("//") != std::string::npos) {continue;}

		if (lineStream >> token && token == "layout") {
			t_vertexAttribute attribute;
			std::string typeName;
			while (lineStream >> token && token != "=") {}
			lineStream >> attribute.location;

			lineStream >> token;
			lineStream >> token;
			lineStream >> token;

			typeName = token;
			if (typeName == "int") {
				attribute.type = GL_INT;
				attribute.size = 1;
				// std::cout << "Int, size " << attribute.size << std::endl;
			}
			else if (typeName == "float") {
				attribute.type = GL_FLOAT;
				attribute.size = 1;
				// std::cout << "Float, size " << attribute.size << std::endl;
			}
			else if (typeName.find("vec") != std::string::npos) {
				attribute.type = GL_FLOAT;
				attribute.size = typeName[3] - '0';
				// std::cout << "Float, size " << attribute.size << std::endl;
			}
			else {
				std::cout << "Type not supported by current codebase : " << typeName << std::endl;
				continue ;
				// assert(0);
			}
			attributes.push_back(attribute);
		}
    }
}

void Shader::Use() { 
	if (activeShader != this) {
		glUseProgram(ID);
		activeShader = this;
	}
}
void Shader::SetBool(const std::string &name, bool value) const {         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::SetInt(const std::string &name, int value) const { 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::SetFloat(const std::string &name, float value) const { 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat4(const std::string &name, float value1, float value2, float value3, float value4) const { 
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value1 , value2, value3, value4); 
}
void Shader::Setmat4(const std::string &name, glm::mat4 value) const { 
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::SetTexture(const std::string &name, GLuint texture_id, int texture_num) const {
    // Convert texture_num (GL_TEXTURE0, GL_TEXTURE1, etc.) to texture unit index (0, 1, etc.)
    int texture_unit = texture_num - GL_TEXTURE0;
    
    // Activate the texture unit
    glActiveTexture(texture_num);
    
    // Bind the texture to the active texture unit
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    // Set the uniform to refer to the correct texture unit
    glUniform1i(glGetUniformLocation(ID, name.c_str()), texture_unit);
}

std::vector<t_vertexAttribute> &Shader::GetVertexAttributes() {
	return attributes;
}

Shader *Shader::GetActiveShader() {
	return activeShader;
}

void Shader::CheckCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				assert(0);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				assert(0);
            }
        }
    }