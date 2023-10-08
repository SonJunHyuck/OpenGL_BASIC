#include "shader.h"

Shader::~Shader() 
{
  if (m_shader) 
  {
    glDeleteShader(m_shader);  // delete shader object
  }
}

ShaderUPtr Shader::CreateFromFile(const std::string &filename, GLenum shaderType)
{
    ShaderUPtr shader = ShaderUPtr(new Shader());

    // 로드 실패 시, shader 메모리 자동 해제 <- unique_ptr이니까
    if (!shader->LoadFile(filename, shaderType))
        return nullptr;

    // 로드 성공 시, shader 메모리 이전
    return std::move(shader);
}

bool Shader::LoadFile(const std::string &filename, GLenum shaderType)
{
    // optional이니까 값이 있을 수도, 없을 수도 -> has_value로 체크
    auto result = LoadTextFile(filename);

    if (!result.has_value())
        return false;

    // std::string code = result.value(); 로 받으면, 용량이 너무 커짐
    std::string& code = result.value();
    const char* codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();

    // create and compile shader
    m_shader = glCreateShader(shaderType);  // (SHADER) create shader object
    glShaderSource(m_shader, 1, (const GLchar *const *)&codePtr, &codeLength);  // (SHADER) object <- code 
    glCompileShader(m_shader);  // (SHADER) compile

    // check compile error
    int success = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);  // (SHADER) get shader info
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);  // get error info
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }

    return true;
}