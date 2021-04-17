#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <array>

namespace {
    void errorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    GLFWwindow* initialize() {
        int glfwInitRes = glfwInit();
        if (!glfwInitRes) {
            fprintf(stderr, "Unable to initialize GLFW\n");
            return nullptr;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow* window = glfwCreateWindow(1280, 720, "InitGL", nullptr, nullptr);
        if (!window) {
            fprintf(stderr, "Unable to create GLFW window\n");
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);

        int gladInitRes = gladLoadGL();
        if (!gladInitRes) {
            fprintf(stderr, "Unable to initialize glad\n");
            glfwDestroyWindow(window);
            glfwTerminate();
            return nullptr;
        }

        return window;
    }
}

// This function creates a texture and return his GL id
GLuint createTexture(unsigned width, unsigned height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    return texture;
}

// This function creates some textures in a shared context and deletes them in the main context
// This is generating some memory leaks in GPU memory in Intel Windows drivers since version 27.20.100.8783.
void testSharedContextTextures(GLFWwindow* mainWindow, GLFWwindow* sharedContextWindow) {
    glfwMakeContextCurrent(sharedContextWindow);

    // INFO: Creating more than one texture in a shared context and destroying them in the main context
    // generates a leak in Intel OpenGL drivers for Windows. Creating only one works propely. Also this is
    // working ok for old versions of the Intel driver and also for NVIDIA drivers. In other platforms like
    // Linux is working properly for Intel drivers.
    std::array<GLuint, 2> textures;
    for (auto& texture : textures)
        texture = createTexture(1024, 1024);

    glfwMakeContextCurrent(mainWindow);

    glDeleteTextures(textures.size(), textures.data());
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(errorCallback);

    GLFWwindow* window = initialize();
    if (!window) {
        return 0;
    }

    // Set the clear color to a nice green
    glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

    GLFWwindow* sharedContextWindow = glfwCreateWindow(1, 1, "", nullptr, window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        testSharedContextTextures(window, sharedContextWindow);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
