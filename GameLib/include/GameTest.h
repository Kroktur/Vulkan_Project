#pragma once
#include "InputManager.h"
#include <GLFW/glfw3.h>
#include <iostream>

void GameToto()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "GameToto", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    InputManager input;
    input.Initialize(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        input.Update();

        if (input.IsKeyPressed(Key::A))
            std::cout << "A key was pressed!\n";

        if (input.IsKeyReleased(Key::A))
            std::cout << "A key was released!\n";

        if (input.IsKeyPressed(Key::Num1))
            std::cout << "Num1 key was pressed!\n";

        if (input.IsKeyReleased(Key::Num1))
            std::cout << "Num1 key was released!\n";

        if (input.IsKeyPressed(SpecialKey::Space))
            std::cout << "Space key was pressed!\n";

        if (input.IsKeyReleased(SpecialKey::Space))
            std::cout << "Space key was released!\n";

        if (input.IsMouseDown(Mouse::Left))
            std::cout << "Left mouse button is being held down!\n";

        if (input.IsMouseDown(Mouse::Right))
            std::cout << "Right mouse button is being held down!\n";

        if (input.IsKeyDown(SpecialKey::Escape))
            glfwSetWindowShouldClose(window, true);
        /*double mouseX, mouseY;
        input.GetMousePosition(mouseX, mouseY);
        std::cout << "Mouse Position: (" << mouseX << ", " << mouseY << ")\n";*/
    }

    glfwTerminate();
}