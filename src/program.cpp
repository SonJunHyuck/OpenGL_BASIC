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