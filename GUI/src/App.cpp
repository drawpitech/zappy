/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"

#include "Models/Animations/Animation.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Renderer/Renderer.hpp"

App::App() {
    m_renderer = std::make_unique<Renderer>();
}

App::~App() {
}

void App::run() {
    std::shared_ptr<Renderer::Scene> scene = std::make_shared<Renderer::Scene>();
    scene->staticMeshes.emplace_back("../assets/SponzaPBR/Sponza.gltf");

    // Animated mesh example
    const std::shared_ptr<SkeletalMesh> danMesh = std::make_shared<SkeletalMesh>("../assets/Dancing_Twerk/Dancing Twerk.dae");
    const std::shared_ptr<Animation> idleAnim = std::make_shared<Animation>("../assets/Dancing_Twerk/Dancing Twerk.dae", danMesh);
    scene->animatedMeshes.push_back(std::make_shared<Renderer::AnimatedMesh>(danMesh, Animator(idleAnim)));

    while (!m_renderer->shouldStop())
        m_renderer->render(scene);
}
