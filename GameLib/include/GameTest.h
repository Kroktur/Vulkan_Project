#pragma once
#include "InputManager.h"
#include "SAT.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

void TestInput()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "GameToto", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    KGR::InputManager input;
    input.Initialize(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        input.Update();

        if (input.IsKeyPressed(KGR::Key::A ))
            std::cout << "A key was pressed!\n";
      
        if (input.IsKeyReleased(KGR::Key::A))
            std::cout << "A key was released!\n";

        if (input.IsKeyPressed(KGR::Key::Num1))
            std::cout << "Num1 key was pressed!\n";

        if (input.IsKeyReleased(KGR::Key::Num1))
            std::cout << "Num1 key was released!\n";

        if (input.IsKeyPressed(KGR::SpecialKey::Space))
            std::cout << "Space key was pressed!\n";

        if (input.IsKeyReleased(KGR::SpecialKey::Space))
            std::cout << "Space key was released!\n";

        if (input.IsMouseDown(KGR::Mouse::Left))
            std::cout << "Left mouse button is being held down!\n";

        if (input.IsMouseDown(KGR::Mouse::Right))
            std::cout << "Right mouse button is being held down!\n";

        if (input.IsKeyDown(KGR::SpecialKey::Escape))
            glfwSetWindowShouldClose(window, true);
        /*double mouseX, mouseY;
        input.GetMousePosition(mouseX, mouseY);
        std::cout << "Mouse Position: (" << mouseX << ", " << mouseY << ")\n";*/
    }

    glfwTerminate();
}

void TestCollision1()
{
    KGR::OBB3D box1(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    );

    float angle = glm::radians(45.0f);
    glm::vec3 axisX = glm::normalize(glm::vec3(std::cos(angle), 0, std::sin(angle)));
    glm::vec3 axisY = glm::vec3(0, 1, 0);
    glm::vec3 axisZ = glm::normalize(glm::cross(axisX, axisY));

    KGR::OBB3D box2(
        glm::vec3(1.5f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        axisX,
        axisY,
        axisZ
    );

    KGR::Collision3D result = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(box1, box2);

    if (result.IsColliding())
    {
        std::cout << "Collision detected!\n";
        std::cout << "Penetration: " << result.GetPenetration() << "\n";
        std::cout << "Normal: "
            << result.GetCollisionNormal().x << ", "
            << result.GetCollisionNormal().y << ", "
            << result.GetCollisionNormal().z << "\n";
    }
    else
    {
        std::cout << "No collision.\n";
    }
}

void TestCollision2()
{
    KGR::OBB3D box1(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    );

    float angle = glm::radians(45.0f);
    glm::vec3 axisX = glm::normalize(glm::vec3(std::cos(angle), 0, std::sin(angle)));
    glm::vec3 axisY = glm::vec3(0, 1, 0);
    glm::vec3 axisZ = glm::normalize(glm::cross(axisX, axisY));

    KGR::OBB3D box2(
        glm::vec3(5.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        axisX,
        axisY,
        axisZ
    );

    KGR::Collision3D result = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(box1, box2);

    if (result.IsColliding())
    {
        std::cout << "Collision detected!\n";
        std::cout << "Penetration: " << result.GetPenetration() << "\n";
        std::cout << "Normal: "
            << result.GetCollisionNormal().x << ", "
            << result.GetCollisionNormal().y << ", "
            << result.GetCollisionNormal().z << "\n";
    }
    else
    {
        std::cout << "No collision.\n";
    }
}

void TestCollision3()
{
    KGR::OBB3D box1(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    );

    float angle = glm::radians(45.0f);
    glm::vec3 axisX = glm::normalize(glm::vec3(std::cos(angle), 0, std::sin(angle)));
    glm::vec3 axisY = glm::vec3(0, 1, 0);
    glm::vec3 axisZ = glm::normalize(glm::cross(axisX, axisY));

    KGR::OBB3D box2(
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        axisX,
        axisY,
        axisZ
    );


    KGR::Collision3D result = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(box1, box2);

    if (result.IsColliding())
    {
        std::cout << "Collision detected!\n";
        std::cout << "Penetration: " << result.GetPenetration() << "\n";
        std::cout << "Normal: "
            << result.GetCollisionNormal().x << ", "
            << result.GetCollisionNormal().y << ", "
            << result.GetCollisionNormal().z << "\n";
    }
    else
    {
        std::cout << "No collision.\n";
    }
}
