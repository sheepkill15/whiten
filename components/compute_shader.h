#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class ComputeShader {
public:
    explicit ComputeShader(const char* file) {
        ifstream compShader(file);
        std::string computeShaderSource((std::istreambuf_iterator<char>(compShader)),
        std::istreambuf_iterator<char>());
        const char* csData = computeShaderSource.c_str();
        GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute_shader, 1, &csData, nullptr);
        glCompileShader(compute_shader);
        {
            GLint maxLength = 0;
	        glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &maxLength);
            int success;
            vector<GLchar> infoLog(maxLength);
            glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(compute_shader, maxLength, nullptr, &infoLog[0]);
                std::cout << "Compute shader compilation failed\n";
                for(char log : infoLog) {
                    cout << log;
                }
                cout << endl;
            }
        }
    
        GLuint compute_program = glCreateProgram(); 
        glAttachShader(compute_program, compute_shader);
        glLinkProgram(compute_program);
        {
            GLint maxLength = 0;
	        glGetProgramiv(compute_program, GL_INFO_LOG_LENGTH, &maxLength);
            int success;
            vector<GLchar> infoLog(maxLength);
            glGetProgramiv(compute_shader, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(compute_shader, maxLength, nullptr, &infoLog[0]);
                std::cout << "Compute shader linking failed\n";
                for(char log : infoLog) {
                    cout << log;
                }
                cout << endl;
            }
        }
        glDeleteShader(compute_shader);
        program = compute_program;
    }

    void use() const {
        glUseProgram(program);
    }

    void dispatch(int x, int y, int z) const {
        use();
        glDispatchCompute(x, y, z);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void set_uniform(GLint location, double value) const {
        use();
        glUniform1d(location, value);
    }
    void set_uniform(GLint location, int value) const {
        use();
        glUniform1i(location, value);
    }

    void bind_ubo(const char* name, int slot = 0) const {
        unsigned int size_index = glGetUniformBlockIndex(program, name);
        glUniformBlockBinding(program, size_index, slot);
    }

    GLuint program;
};

#endif