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
    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();

    // ========== Create & Attach & Link ========= (create shader func has covered program clas)
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get());

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
    if (!m_textureProgram)
      return false;
    SPDLOG_INFO("program id: {}", m_textureProgram->Get());

    // ======== Uniform ========
    // auto loc = glGetUniformLocation(m_program->Get(), "color");  // Get Uniform handle
    // m_program->Use();
    // glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    // "state setting function" (save to OpenGL context)
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);


    // ======== Texture ========
    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./images/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./images/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./images/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./images/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

    m_windowTexture = Texture::CreateFromImage(Image::Load("./images/blending_transparent_window.png").get());

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
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 5.0f, 0.0f ,180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));

            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        ImGui::Checkbox("animation", &m_animation);

        ImGui::Text("This is first text...");
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);  // w : 1.0f -> 점, 0.0f -> 벡터 (위치 상관x -> 평행이동x)
    
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 300.0f);
    
    float angle = glfwGetTime() * glm::pi<float>() * 0.5f;
    auto x = sinf(angle) * 10.0f;
    auto z = cosf(angle) * 10.0f;

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if (m_flashLightMode)
    {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else
    {
        // Render LightCube
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    // Render Model
    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
                                              cosf(glm::radians(m_light.cutoff[0])),
                                              cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // glEnable(GL_STENCIL_TEST);  // Stencile TEST 켜기, buffer들은 모두 0으로 초기화
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);  //
    // glStencilMask(0xFF);  // Stencile Buffer에 작성 하겠다.
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);  
    // glStencilMask(0x00);  // Stencile Buffer에 작성을 하지 않겠다.
    // glDisable(GL_DEPTH_TEST);
    // m_simpleProgram->Use();
    // m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    // m_simpleProgram->SetUniform("transform", transform * glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
    // m_box->Draw(m_simpleProgram.get());

    // glEnable(GL_DEPTH_TEST);
    // glDisable(GL_STENCIL_TEST);
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glStencilMask(0xFF);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textureProgram->Use();
    
    glActiveTexture(GL_TEXTURE0);
    m_windowTexture->Bind();
    m_textureProgram->SetUniform("tex", 0);

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());  // 먼저 그려짐 -> Depth 3

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());  // 제일 마지막에 그려짐 -> Depth 1
}

// Depth가 작은 것이 큰 것을 가릴 수 있다.
// Depth가 큰 것은 뒤에 있다. (이후에 그려질 것에 의해 가려짐)
// 