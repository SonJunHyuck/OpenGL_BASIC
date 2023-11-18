#pragma once

#include <memory>
#include <string>
#include <optional>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ## : 은 klass랑 변수이름(UPtr, Ptr, WPtr)을 붙여줌
#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

// 메모리 할당 안돼있다. -> 비어있다. -> 에러 캐치 가능
// std::string* LoadTextFile(const std::string& filename);

// optional : 포인터의 메모리 누수 위험 방지 장치
std::optional<std::string> LoadTextFile(const std::string& filename);
