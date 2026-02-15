#include "Game.h"

#include <memory>

#include "CelestialBodies.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "imgui.h"
namespace Game {

reg::Entity create(pain::Scene &scene, int cameraWidth, int cameraHeight,
                   pain::Application *app) {
  reg::Entity cam =
      pain::Dummy3dCamera::create(scene, cameraWidth, cameraHeight, 80.0f);
  app->set3dRendererCamera(cam, cameraWidth, cameraHeight);

  pain::Texture &texture = pain::TextureManager::createTexture(
      "./resources/default/textures/defaultTexture.png");
  reg::Entity bodyA = Star::create(  //
      scene, texture,                //
      "Blue", glm::vec3(0.6f, 0.f, 0.f), 2.f, glm::vec3(0.f, -1.f, 0.f),
      glm::vec3(0.1f, 1.f, 1.f));
  reg::Entity bodyB = Star::create(  //
      scene, texture,                //
      "Red", glm::vec3(-0.6f, 0.f, 0.f), 2.f, glm::vec3(0.f, 1.f, 0.f),
      glm::vec3(1.0f, 0.1f, 0.1f));

  bool isRunning = true;
  pain::Scene::emplaceScript<Game::Script>(cam, scene, bodyA, bodyB, isRunning);
  return scene.getEntity();
}

Script::Script(reg::Entity entity, pain::Scene &scene, reg::Entity bodyA,
               reg::Entity bodyB, bool isRunning)
    : pain::WorldObject(entity, scene),
      m_isRunning(isRunning),
      m_bodyA(bodyA),
      m_bodyB(bodyB) {};

void Script::onCreate() {
  IMGUI_PLOG([this] {
    Star::Script &controllerA = getNativeScript<Star::Script>(m_bodyA);
    Star::Script &controllerB = getNativeScript<Star::Script>(m_bodyB);
    const float potentialEnergy = controllerA.getPotentialEnergy();

    ImGui::SeparatorText("System Controller");
    ImGui::InputFloat("Consant G", &m_constantG);
    ImGui::Text("Energy = m1*v1²/2 + m2*v2²/2 - G(m1m2)/r");
    ImGui::Text("%f = %f + %f - %f",
                controllerA.getMotionEnergyFromOrigin() +
                    controllerB.getMotionEnergyFromOrigin() - potentialEnergy,
                controllerA.getMotionEnergyFromOrigin(),
                controllerB.getMotionEnergyFromOrigin(), potentialEnergy);
    if (ImGui::Button("Reset System")) {
      controllerA.resetStats();
      controllerB.resetStats();
    }
    ImGui::SeparatorText("Interesting Staring Parameters");
    if (ImGui::Button("OperaGX logo")) {
      controllerA.setInitialValues(2.f, glm::vec3(0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, -1.f, 0.f));
      controllerA.resetStats();
      controllerB.setInitialValues(2.f, glm::vec3(-0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, 1.f, 0.f));
      controllerB.resetStats();
    }
    if (ImGui::Button("Cartoon eye")) {
      controllerA.setInitialValues(4.f, glm::vec3(0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, -2.f, 0.f));
      controllerA.resetStats();
      controllerB.setInitialValues(8.f, glm::vec3(-0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, 1.f, 0.f));
      controllerB.resetStats();
    }
    if (ImGui::Button("Sun earth")) {
      controllerA.setInitialValues(4.f, glm::vec3(0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, -10.f, 0.f));
      controllerA.resetStats();
      controllerB.setInitialValues(6000.f, glm::vec3(-20.f, 0.f, 0.f),
                                   glm::vec3(0.f, 0.f, 0.f));
      controllerB.resetStats();
    }
    if (ImGui::Button("Elliptic orbits")) {
      controllerA.setInitialValues(2.f, glm::vec3(0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, -1.5f, 0.f));
      controllerA.resetStats();
      controllerB.setInitialValues(3.f, glm::vec3(-0.6f, 0.f, 0.f),
                                   glm::vec3(0.f, 1.f, 0.f));
      controllerB.resetStats();
    }
  });
}

void Script::onUpdate(pain::DeltaTime deltaTime) {
  Star::Script &controllerA = getNativeScript<Star::Script>(m_bodyA);
  Star::Script &controllerB = getNativeScript<Star::Script>(m_bodyB);
  for (int i = 0; i < m_timeDivision; i++) {
    controllerA.addForce(m_bodyB, controllerB,
                         deltaTime.getSecondsf() / m_timeDivision);
    controllerB.addForce(m_bodyA, controllerA,
                         deltaTime.getSecondsf() / m_timeDivision);
  }
  // controllerA->addForce(m_bodyB.get(), controllerB, deltaTime);
  // controllerB->addForce(m_bodyA.get(), controllerA, deltaTime);
}
}  // namespace Game
