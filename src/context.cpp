#include "context.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{
    float vertices[] = {
         0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top right, red
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right, green
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left, blue
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  // top left, yellow
    };
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // VAO gen -> VAO bind -> VBO gen -> VBO bind -> 
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 24);
    // (0) attr : pos, (1) attr : color
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);  // 하나의 vertex와 다른 vertex 사이의 byte 크기 만큼 차이
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 3);  // color attribute는 float * 3만큼 뒤에서 시작
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

    // ========= Create =========
    //ShaderPtr vertShader = Shader::CreateFromFile("../shader/simple.vs", GL_VERTEX_SHADER);
    //ShaderPtr fragShader = Shader::CreateFromFile("../shader/simple.fs", GL_FRAGMENT_SHADER);
    ShaderPtr vertShader = Shader::CreateFromFile("../shader/per_vertex_color.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("../shader/per_vertex_color.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    // ========== Attach & Link =========
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    // ======== Uniform ========
    // auto loc = glGetUniformLocation(m_program->Get(), "color");  // Get Uniform handle
    // m_program->Use();
    // glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    // "state setting function" (save to OpenGL context)
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    return true;
}

void Context::Render()
{   
    glClear(GL_COLOR_BUFFER_BIT);
 
    m_program->Use();

    // primitive, count, index_type, pointer/offset
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // primitive, offset, count
    // // glDrawArrays(GL_TRIANGLES, 0, 6);

    // glClear(GL_COLOR_BUFFER_BIT);

    // static float time = 0.0f;
    // float t = sinf(time) * 0.5f + 0.5f;
    // auto loc = glGetUniformLocation(m_program->Get(), "color");
    // m_program->Use();
    // glUniform4f(loc, t * t, 2.0f * t * (1.0f - t), (1.0f - t) * (1.0f - t), 1.0f);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // time += 0.016f;
}