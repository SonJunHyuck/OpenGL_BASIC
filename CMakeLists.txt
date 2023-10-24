# 버전 정의로 시작 cmake_minimum_required
# set(변수명 값) : 변수 설정
# ${변수명} : 변수에 들어있는 값 쓰기 
# CMAKE로 시작하는 변수는 CMAKE에서 지정돼있는 predefine된 변수
# CXX : C++


# --------------------------------------------------------
# project 설정 시작
# --------------------------------------------------------
# 3.14 최소 버전 제약
cmake_minimum_required(VERSION 3.13)

# 변수명 : PROJECT_NAME, 값 : cmake_project_example
set(PROJECT_NAME opengl_project)

# C++ 17버전 쓸것이다.
set(CMAKE_CXX_STANDARD 17)

# mac의 경우에는 framework 설정해줘야함
if(APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL")
endif()

# glfw 변수 정의
set(WINDOW_NAME "OpenGL Project")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)

# project : "여기서부터 내 프로젝트 시작!"
project(${PROJECT_NAME})

# cmake_project_example이라는 실행파일 만들거고, 뒤에 붙어있는 모든 소스파일 모아서
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/buffer.cpp src/buffer.h
    src/vertex_layout.cpp src/vertex_layout.h
)


# --------------------------------------------------------
# dependancy 시작
# --------------------------------------------------------
include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
# 실행 파일 만들 때, lib 집어넣어야 하는데, 그 실행파일이(target) 뭐야?
# target은 exe나 lib 만들 때, 지정해줘야함

# compile 할 때, "target include & lib" 지정 (PUBLIC 은 필수 정도로 해석)
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})

# 실제 lib link 하는 단계
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# #define과 똑같은 효과, 소스코드 밖에서 정의
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# Dependency들이 먼저 build 될 수 있게 관계 설정
# 리스트에 있는 타겟들 먼저 컴파일 한 후에 우리 프로젝트 컴파일 해라.
# 안적으면, 우리 프로젝트와 라이브러리를 병렬로 빌드함 -> 오류
add_dependencies(${PROJECT_NAME} ${DEP_LIST})
