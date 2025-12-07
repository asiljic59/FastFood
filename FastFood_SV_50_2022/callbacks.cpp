#include "callbacks.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (action == GLFW_PRESS)
    {
       
        if (key == GLFW_KEY_W) uY += 0.05f;
        if (key == GLFW_KEY_S) uY -= 0.05f;
        if (key == GLFW_KEY_A) uX -= 0.05f;
        if (key == GLFW_KEY_D) uX += 0.05f;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        spacePressed = true;
    }
}

