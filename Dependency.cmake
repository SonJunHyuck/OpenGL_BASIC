# ExternalProject 관련 명령어 셋 추가
# CMake에서 추가적인 기능을 위해 사용 (ExternalProject_Add를 사용하기 위함)
include(ExternalProject)

# Dependency 관련 변수 설정 (directories 설정)
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)  # PROJECT_BINARY_DIR : build/
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)     # build/install/include
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# spdlog: fast logger library (refer to cout)
# 적혀있는 주소에서 다운받아서 cmake를 실행
ExternalProject_Add(
    dep_spdlog              # build 후 나오는 이름 (변수 같은 것)
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"          # version (해당 repo에서 사용할 branch 또는 tag를 가져옴)
    GIT_SHALLOW 1           # GIT_SHALLOW를 활성화 한다. (SHALLOW는 가장 최신의 것만을 다운받겠다.)

    # update_step, patch_step을 생략("")하겠다.
    UPDATE_COMMAND ""       # 라이브러리가 최신화되면 업데이트 하겠다.
    PATCH_COMMAND ""        # 내가 이 라이브러리를 고쳐 쓸 경우

    # CMAKE에 인자를 넣겠다. (변수명 : CMAKE_INSTALL_PREFIX) -> "라이브러리 빌드 다 했을 때, 어디에 넣을래?"
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DSPDLOG_BUILD_EXAMPLE=OFF

    TEST_COMMAND ""         # test step 생략
)

# Dependency 리스트에 dep_spdlog추가
set(DEP_LIST ${DEP_LIST} dep_spdlog)

# $<$<CONFIG:Debug>:d> 는 debug모드로 컴파일 하는 경우에는 d, 아니면 아무것도 붙이지 말라 (mac에선 지움)
if(APPLE OR MINGW)
    set(DEP_LIBS ${DEP_LIBS} spdlog)
else()
    set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)
endif()


# glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.2"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
    )

# Dependency 리스트에 dep_glfw추가
set(DEP_LIST ${DEP_LIST} dep_glfw)

if(APPLE OR MINGW)
    set(DEP_LIBS ${DEP_LIBS} glfw3)
else()
    set(DEP_LIBS ${DEP_LIBS} glfw3$<$<CONFIG:Debug>:d>)
endif()

# glad
ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
    GIT_TAG "v0.1.34"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    )

# Dependency 리스트에 dep_glfw추가
set(DEP_LIST ${DEP_LIST} dep_glad)

if(APPLE OR MINGW)
    set(DEP_LIBS ${DEP_LIBS} glad)
else()
    set(DEP_LIBS ${DEP_LIBS} glad$<$<CONFIG:Debug>:d>)
endif()


# stb
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    # install -> only stb_image.h (-E copy : windows, linux, macOS anything)
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h
    )

set(DEP_LIST ${DEP_LIST} dep_stb)
# because don't need build -> DEP_LIBS X

# glm
ExternalProject_Add(
  dep_glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "0.9.9.8"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
    ${DEP_INSTALL_DIR}/include/glm
  )
set(DEP_LIST ${DEP_LIST} dep_glm)
# because don't need build -> DEP_LIBS X

# imgui (내부 프로젝트)
add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)  # 헤더 고칠 일 없으니까, cpp만 넣기
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
add_dependencies(imgui ${DEP_LIST})
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)