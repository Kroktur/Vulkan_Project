#pragma once
#include <GLFW/glfw3.h>


class InputManager
{
public:
    void Initialize(GLFWwindow* window);
    void Update();

    bool IsKeyDown(int key);
    bool IsKeyPressed(int key);
    bool IsKeyReleased(int key);

    bool IsMouseDown(int button);
    bool IsMousePressed(int button);
    bool IsMouseReleased(int button);

    void GetMousePosition(double& x, double& y);

private:
    GLFWwindow* m_window;

    bool m_currentKeys[1024]  = { false };
    bool m_previousKeys[1024] = { false };

    bool m_currentMouse[8]  = { false };
    bool m_previousMouse[8] = { false };

    double m_mouseX = 0.0f;
    double m_mouseY = 0.0f;
};