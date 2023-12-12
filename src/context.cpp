#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow *window)
{
    if (!m_cameraControl)
        return;

    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;
    
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    // Yaw
    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;

    // Pitch
    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}

bool Context::Init()
{
    float vertices[] = { // pos.xyz, normal.xyz, texcoord.uv
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
};

    uint32_t indices[] = {
         0,  2,  1,  2,  0,  3,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    // VAO gen -> VAO bind -> VBO gen -> VBO bind -> 
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 8 * 4 * 6);  // 8float * Square(4) * 6face
    
    // attr num , float count, GL_Float, GL_FALSE, stride, start
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);  // 하나의 vertex와 다른 vertex 사이의 byte 크기 만큼 차이
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);  // Normal Attr
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);  // texture Attri
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    // ========= Create =========
    //ShaderPtr vertShader = Shader::CreateFromFile("../shader/simple.vs", GL_VERTEX_SHADER);
    //ShaderPtr fragShader = Shader::CreateFromFile("../shader/simple.fs", GL_FRAGMENT_SHADER);
    // ShaderPtr vertShader = Shader::CreateFromFile("./shader/per_vertex_color.vs", GL_VERTEX_SHADER);
    // ShaderPtr fragShader = Shader::CreateFromFile("./shader/per_vertex_color.fs", GL_FRAGMENT_SHADER);
    // ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    // ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/lighting.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/lighting.fs", GL_FRAGMENT_SHADER);

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


    // ======== Texture ========
    auto image = Image::Load("./images/container.jpg");

    if (!image)
        return false;

    SPDLOG_INFO("image: {}x{}, {} channels", 
                    image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./images/awesomeface.png");

    if (!image2)
        return false;

    SPDLOG_INFO("image: {}x{}, {} channels", 
                    image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());

    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);  // 0번 Slot
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);  // 1번 Slot
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);  // Value -> 0 -> 0번 슬롯에 Texture 있다.
    m_program->SetUniform("tex2", 1);

    return true;
}

void Context::Render()
{
    // begin, end pair -> UI Window
    if (ImGui::Begin("my first ImGui window"))
    {
        if(ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor)))
        {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();

        ImGui::DragFloat3("Camera Pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("Camera Yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("Camera Pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();

        if(ImGui::Button("Reset Camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        ImGui::Separator();

        // ImGui : Light & Material
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
        }

        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit3("m.ambient", glm::value_ptr(m_material.ambient));
            ImGui::ColorEdit3("m.diffuse", glm::value_ptr(m_material.diffuse));
            ImGui::ColorEdit3("m.specular", glm::value_ptr(m_material.specular));
            ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f);
        }

        ImGui::Checkbox("animation", &m_animation);

        ImGui::Text("This is first text...");
    }
    ImGui::End();

    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);  // w : 1.0f -> 점, 0.0f -> 벡터 (위치 상관x -> 평행이동x)
    
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 20.0f);

    float angle = glfwGetTime() * glm::pi<float>() * 0.5f;
    auto x = sinf(angle) * 10.0f;
    auto z = cosf(angle) * 10.0f;

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    // Render LightCube
    auto lightModelTransform =
        glm::translate(glm::mat4(1.0), m_light.position) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    m_program->Use();
    m_program->SetUniform("lightPos", m_light.position);
    m_program->SetUniform("light.ambient", m_light.diffuse);
    m_program->SetUniform("material.ambient", m_material.diffuse);
    m_program->SetUniform("ambientStrength", 1.0f);
    m_program->SetUniform("transform", projection * view * lightModelTransform);
    m_program->SetUniform("modelTransform", lightModelTransform);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Render Object Cubes
    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", m_light.position);
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);
    m_program->SetUniform("material.ambient", m_material.ambient);
    m_program->SetUniform("material.diffuse", m_material.diffuse);
    m_program->SetUniform("material.specular", m_material.specular);
    m_program->SetUniform("material.shininess", m_material.shininess);

    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        auto rotateAngle = glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i);
        model = glm::rotate(model, m_animation ? rotateAngle : 0.0f,
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

}