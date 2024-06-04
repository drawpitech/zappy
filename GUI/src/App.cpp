/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"

#include "Models/Animations/Animation.hpp"
#include "Models/Animations/Animator.hpp"
#include "Renderer/Renderer.hpp"

App::App() {
    m_renderer = std::make_unique<Renderer>();
}

App::~App() {
}

void App::run() {
    std::shared_ptr<Renderer::Scene> scene = std::make_shared<Renderer::Scene>();
    scene->staticMeshes.emplace_back("../assets/SponzaPBR/Sponza.gltf");
    scene->skeletalMeshes.emplace_back("../assets/dan/dan.dae");

    while (!m_renderer->shouldStop()) {
        m_renderer->render(scene);
    }
}
