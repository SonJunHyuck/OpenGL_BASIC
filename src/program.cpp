#include "program.h"

Program::~Program()
{
    if (m_program)
    {
        glDeleteProgram(m_program);
    }
}

ProgramUPtr Program::Create(const std::vector<ShaderPtr> &shaders)
{
    // alive program memory in this scope
    auto program = ProgramUPtr(new Program());

    if (!program->Link(shaders))
        return nullptr;

    return std::move(program);
}

ProgramUPtr Program::Create(const std::string &vertShaderFilename, const std::string &fragShaderFilename)
{
    ShaderPtr vs = Shader::CreateFromFile(vertShaderFilename, GL_VERTEX_SHADER);
    ShaderPtr fs = Shader::CreateFromFile(fragShaderFilename, GL_FRAGMENT_SHADER);
    
    if (!vs || !fs)
        return nullptr;
    
    return std::move(Create({vs, fs}));
}

bool Program::Link(const std::vector<ShaderPtr> &shaders)
{
    // create program and get program id
    m_program = glCreateProgram();

    for (auto &shader : shaders)
    {
        // attach to program with shader id
        glAttachShader(m_program, shader->Get());
    }
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }

    return true;
}

void Program::Use() const
{
    glUseProgram(m_program);
}

void Program::SetUniform(const std::string& name, int value) const 
{
  auto loc = glGetUniformLocation(m_program, name.c_str());
  glUniform1i(loc, value);
}

void Program::SetUniform(const std::string &name, float value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string &name, const glm::vec2 &value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(value));
}
void Program::SetUniform(const std::string &name, const glm::vec3 &value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}
void Program::SetUniform(const std::string &name, const glm::vec4 &value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const 
{
  auto loc = glGetUniformLocation(m_program, name.c_str());
  
  // Location, Mat 몇개?, is_transpose, mat4의 첫 번째 주소값
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}