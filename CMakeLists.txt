cmake_minimum_required(VERSION 3.20)
project(robi_new)

set(CMAKE_CXX_STANDARD 17)

include_directories(components)
include_directories(include)
include_directories(include/glad)
include_directories(include/GLFW)
include_directories(include/imgui)
include_directories(include/KHR)

link_directories(lib)

add_executable(robi_new
        components/compute_shader.h
        components/renderer.h
        components/ssbo.h
        components/texture.h
        components/ubo.h
        components/vert_frag_shader.h
        components/window.h
        include/glad/glad.h
        include/GLFW/glfw3.h
        include/GLFW/glfw3native.h
        include/imgui/imconfig.h
        include/imgui/imgui.h
        include/imgui/imgui_impl_glfw.h
        include/imgui/imgui_impl_opengl3.h
        include/imgui/imgui_internal.h
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h
        include/KHR/khrplatform.h
        include/stb_image.h
        include/stb_image_write.h
        glad.cpp
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        main.cpp)

target_link_libraries(robi_new glfw3)
target_link_libraries(robi_new imm32)
target_link_libraries(robi_new comdlg32)
find_package(OpenGL REQUIRED)
target_link_libraries(robi_new OpenGL::GL)

add_custom_command(TARGET robi_new PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:robi_new>/shaders)
add_custom_command(TARGET robi_new POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:robi_new>/res)
