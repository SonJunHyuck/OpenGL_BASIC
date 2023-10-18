#include "context.h"  // common, shader, program 모두 포함

#include <spdlog/spdlog.h>
#include <glad/glad.h> // glfw 이전에 include!
#include <GLFW/glfw3.h>

void OnFramebufferSizeChange(GLFWwindow *window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);

    // rendering 화면 지정
    glViewport(0, 0, width, height);
}

void OnKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
                key, scancode,
                action == GLFW_PRESS ? "Pressed" : action == GLFW_RELEASE ? "Released"
                                                 : action == GLFW_REPEAT ? "Repeat"
                                                 : "Unknown",
                mods & GLFW_MOD_CONTROL ? "C" : "-",
                mods & GLFW_MOD_SHIFT ? "S" : "-",
                mods & GLFW_MOD_ALT ? "A" : "-");

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Render()
{
    // "state using function" (using OpenGL context)
    // 실제 클리어 수행 (GL_COLOR_BIT : 컬러 버퍼)
    glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
    SPDLOG_INFO("Start program!");

    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit())
    {
        const char *description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SPDLOG_INFO("Create window!");
    // GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);

    if (!window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    // window가 만들어지면서, 만들어진 context를 사용하겠다.
    glfwMakeContextCurrent(window);

    // glad를 이용한 openGL 함수 로딩 (process address를 얻어옴)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = (const char *)glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    // ========== Glad Load 끝 -> 여기서부터 GL Functions 사용 가능 ==========
    auto context = Context::Create();
    if (!context)
    {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // forced set viewport
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // setting events
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);

    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window))
    {
        context->Render();

        glfwSwapBuffers(window);

        // mouse, keyboard, window size event... 등을 수집
        glfwPollEvents();
    }

    // gl 끝나기 전에 메모리 정리 해야함
    context.reset();

    glfwTerminate();

    return 0;
}