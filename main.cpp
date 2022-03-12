#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "components/window.h"
#include "components/texture.h"
#include "components/renderer.h"
#include "components/ubo.h"
#include "components/ssbo.h"

#include "components/compute_shader.h"
#include "components/vert_frag_shader.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

using namespace std;

Window simWindow;
Texture texture;
void pick_file(char path[MAX_PATH]) {
    OPENFILENAME ofn;
    ZeroMemory( &ofn,      sizeof( ofn ) );
    ZeroMemory(path, MAX_PATH);
    ofn.lStructSize  = sizeof( ofn );
    ofn.hwndOwner    = nullptr;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter  = "Images\0*.png;*.jpg\0Any File\0*.*\0";
    ofn.lpstrFile    = path;
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrTitle   = "Valaszd ki a kepet";
    ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if(GetOpenFileNameA(&ofn)) {
        std::cout << path << endl;
    } else
    {
        // All this stuff below is to tell you exactly how you messed up above.
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError())
        {
            case CDERR_DIALOGFAILURE   : std::cout << "CDERR_DIALOGFAILURE\n";   break;
            case CDERR_FINDRESFAILURE  : std::cout << "CDERR_FINDRESFAILURE\n";  break;
            case CDERR_INITIALIZATION  : std::cout << "CDERR_INITIALIZATION\n";  break;
            case CDERR_LOADRESFAILURE  : std::cout << "CDERR_LOADRESFAILURE\n";  break;
            case CDERR_LOADSTRFAILURE  : std::cout << "CDERR_LOADSTRFAILURE\n";  break;
            case CDERR_LOCKRESFAILURE  : std::cout << "CDERR_LOCKRESFAILURE\n";  break;
            case CDERR_MEMALLOCFAILURE : std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
            case CDERR_MEMLOCKFAILURE  : std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
            case CDERR_NOHINSTANCE     : std::cout << "CDERR_NOHINSTANCE\n";     break;
            case CDERR_NOHOOK          : std::cout << "CDERR_NOHOOK\n";          break;
            case CDERR_NOTEMPLATE      : std::cout << "CDERR_NOTEMPLATE\n";      break;
            case CDERR_STRUCTSIZE      : std::cout << "CDERR_STRUCTSIZE\n";      break;
            case FNERR_BUFFERTOOSMALL  : std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
            case FNERR_INVALIDFILENAME : std::cout << "FNERR_INVALIDFILENAME\n"; break;
            case FNERR_SUBCLASSFAILURE : std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
            default                    : std::cout << "You cancelled.\n";
        }
    }
}

struct Data
{
    int region;
    int regionCount;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glfwSetWindowAspectRatio(window, texture.width, texture.height);
}

void imgui_setup()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(simWindow.get_glfwwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void imgui_newframe()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

int region = 0;
int regionCount = 3;
static float tresholds[100000];
int imgui_render()
{
    int state = 0;
    ImGui::Begin("Controls cucc"); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("Vajon mukodik?"); // Display some text (you can use a format strings too)

    ImGui::InputInt("Hany resz nxn", &regionCount);
    ImGui::InputInt("Jelenlegi resz", &region);

    if(regionCount < 0) regionCount = 0;
    if(region < 0) region = 0;
    else if(region >= regionCount * regionCount) region = regionCount * regionCount - 1;

    ImGui::SliderFloat("Mi folott", &tresholds[region], 0.F, 3.F);

    if (ImGui::Button("Save image"))
    {
        state = 2;
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    return state;
}

int main()
{
    char path[MAX_PATH];
    pick_file(path);
    if(path[0] == 0) {
        return -1;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    simWindow.create();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    simWindow.on_resize(simWindow.get_size());
    glfwSetFramebufferSizeCallback(simWindow.get_glfwwindow(), framebuffer_size_callback);

    VertFragShader vf("shaders/vertex.vert", "shaders/frag.frag");
    glUseProgram(vf.program);

    Renderer::init();
    ComputeShader("shaders/copy.comp");

    int width, height, channels;
    unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
    if (img == nullptr)
    {
        std::cout << "ERROR loading image" << endl;
        return -1;
    }

    texture.create(width, height, channels, 0);

    texture.load(img);

    glfwSetWindowSize(simWindow.get_glfwwindow(), width, height);
    simWindow.on_resize({width, height});

    stbi_image_free(img);

    imgui_setup();
    vf.bind_ubo("Rects", 5);
    vf.bind_ubo("Tresholds", 6);
    vf.bind_ubo("WindowSize", 4);

    Texture saveTexture{};
    saveTexture.create(width, height, 1);
    saveTexture.bind();
    saveTexture.bind_as_image();

    ComputeShader saveShader("shaders/compute.comp");
    saveShader.bind_ubo("WindowSize", 4);
    saveShader.bind_ubo("Rects", 5);
    saveShader.bind_ubo("Tresholds", 6);

    texture.bind();
    texture.bind_as_image();

    for(float & treshold : tresholds) {
            treshold = 3;
    }

    auto size = simWindow.get_size();
    Ubo windowSize(sizeof(int) * 2, &size);
    windowSize.bind(4);
    while (simWindow.running())
    {
        imgui_newframe();
        int state = imgui_render();
        Data data = {region, regionCount};
        Ssbo rectSSBO(sizeof(Data), &data, 5);
        rectSSBO.bind();

        Ssbo tresholdSSBO(sizeof(tresholds), tresholds, 6);
        tresholdSSBO.bind();

        if (state == 1)
        {
        }
        else if (state == 2)
        {
            saveTexture.bind();
            saveShader.dispatch(width / 8 + 1, height / 8 + 1, 1);
            auto *img_data = new unsigned char[width * height * channels];
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
            stbi_write_png("res.png", width, height, 4, img_data, 4 * width);
            delete[] img_data;
            texture.bind();
        }
        ImGui::Render();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            glUseProgram(vf.program);
            
        }
        Renderer::render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        glfwSwapBuffers(simWindow.get_glfwwindow());
        glfwPollEvents();
    }
    std::cout << endl;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(simWindow.get_glfwwindow());
    glfwTerminate();
    return 0;
}
