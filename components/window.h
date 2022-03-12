#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

class Window {
    public:
    
    struct WindowSize
    {
        int x;
        int y;
    };

    Window() = default;

    void create()  {
        window = glfwCreateWindow(size.x, size.y, "Hello", nullptr, nullptr);
        if (window == nullptr)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // glfwSetFramebufferSizeCallback(window, &(this->framebuffer_size_callback));
    }

    void on_resize(WindowSize windowSize) {
        this->size = windowSize;
        glViewport(0, 0, windowSize.x, windowSize.y);
    }

    bool running() {
        return !glfwWindowShouldClose(window);
    }

    [[nodiscard]] const WindowSize& get_size() const { return size; }

    GLFWwindow* get_glfwwindow() { return window; }

private:
    GLFWwindow *window{};
    
    WindowSize size = {800, 800};

};

#endif