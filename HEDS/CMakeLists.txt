#config glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Dependencies/GLFW)
include_directories(Dependencies/GLFW/include)
LIST(APPEND LIBS glfw ${GLFW_LIBRARIES})

# config glad
set(GLAD_SRC Dependencies/GLAD/src/glad.c)
if(NOT WIN32)
    LIST(APPEND LIBS dl)
endif()
include_directories(Dependencies/GLAD/include)

# config glm
include_directories(Dependencies/GLM)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ConstructSolid ConstructSolidSrc)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/HalfEdgeClass HalfEdgeClassSrc)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Renderer RendererSrc)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/poly2tri/common Poly2triCommonSrc)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/poly2tri/sweep Poly2triSweepSrc)


set(SOURCE_FILES
    Application.cpp
    ${ConstructSolidSrc}
    ${HalfEdgeClassSrc}
    ${RendererSrc}
    ${GLAD_SRC}
    poly2tri/poly2tri.h
    ${Poly2triCommonSrc}
    ${Poly2triSweepSrc}
)
add_executable(HEDS ${SOURCE_FILES})

target_link_libraries(HEDS ${LIBS})
