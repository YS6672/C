cmake_minimum_required(VERSION 3.1.0)
project(UoNModelLoader VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include(CTest)
enable_testing()

# Include Class Directories
include_directories(
    ${PROJECT_SOURCE_DIR}/Model ${PROJECT_SOURCE_DIR}/Cell
    ${PROJECT_SOURCE_DIR}/Material ${PROJECT_SOURCE_DIR}/Vector
    ${PROJECT_SOURCE_DIR}/ModelRenderer)

get_property(
    dirs
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY INCLUDE_DIRECTORIES)
set(UONMODELLOADER_INCLUDE_DIRS
    ${dirs}
    PARENT_SCOPE)

# Include Source Files
message(STATUS ${PROJECT_NAME})
add_library(
    ${PROJECT_NAME}
    # Model Class
    ${PROJECT_SOURCE_DIR}/Model/Model.cpp
    ${PROJECT_SOURCE_DIR}/Model/Model.hpp
    # Model Renderer
    ${PROJECT_SOURCE_DIR}/ModelRenderer/ModelRenderer.cpp
    ${PROJECT_SOURCE_DIR}/ModelRenderer/ModelRenderer.hpp
    # Material Class
    ${PROJECT_SOURCE_DIR}/Material/Material.cpp
    ${PROJECT_SOURCE_DIR}/Material/Material.hpp
    # Cell Class
    ${PROJECT_SOURCE_DIR}/Cell/Cell.cpp
    ${PROJECT_SOURCE_DIR}/Cell/Cell.hpp
    # Vector Class
    ${PROJECT_SOURCE_DIR}/Vector/Vector.cpp
    ${PROJECT_SOURCE_DIR}/Vector/Vector.hpp)

if(DEFINED BUILD_MODELRENDERER)
    # Include OpenGL
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

    target_sources(
        ${PROJECT_NAME}
        PRIVATE # Model Renderer
                ${PROJECT_SOURCE_DIR}/ModelRenderer/ModelRenderer.cpp
                ${PROJECT_SOURCE_DIR}/ModelRenderer/ModelRenderer.hpp)

    # Model Renderer Test
    add_executable(ModelRendererTest tests/ModelRendererTest.cpp)
    target_link_libraries(ModelRendererTest ${PROJECT_NAME} ${OPENGL_LIBRARIES}
                          ${GLUT_LIBRARIES})
    add_test(
        NAME ModelRendererTester
        COMMAND ModelRendererTest
        WORKING_DIRECTORY ${UNIT_TEST_PATH})
endif()

target_link_libraries(${PROJECT_NAME} ${math} ${OPENGL_LIBRARIES}
                      ${GLUT_LIBRARIES})

# if(NOT BUILD_SHARED_LIBS)

# add_definitions(-D ${PROJECT_NAME}_STATIC) endif()

# Add Tests
set(UNIT_TEST_PATH ${PROJECT_SOURCE_DIR}/tests)
# Model Class Test
add_executable(ModelClassTest tests/ModelClassTest.cpp)
target_link_libraries(ModelClassTest ${PROJECT_NAME})
add_test(
    NAME ModelClassTester
    COMMAND ModelClassTest
    WORKING_DIRECTORY ${UNIT_TEST_PATH})
# 3D Vector Class Test
add_executable(VectorClassTest tests/VectorClassTest.cpp)
target_link_libraries(VectorClassTest ${PROJECT_NAME})
add_test(
    NAME VectorClassTester
    COMMAND VectorClassTest
    WORKING_DIRECTORY ${UNIT_TEST_PATH})
# Cell Class Test
add_executable(CellClassTest tests/CellClassTest.cpp)
target_link_libraries(CellClassTest ${PROJECT_NAME})
add_test(
    NAME CellClassTester
    COMMAND CellClassTest
    WORKING_DIRECTORY ${UNIT_TEST_PATH})
