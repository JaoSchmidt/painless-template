#include "CelestialBodies.h"

#include <pain.h>

#include <cmath>

#include "GUI/ImGuiDebugRegistry.h"
#include "Game.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace Star {

reg::Entity create(pain::Scene &scene, pain::Texture &pTexture,
                   const std::string &name, const glm::vec3 &initialPosition,
                   float mass, const glm::vec3 &initialVelocity,
                   const glm::vec3 &cor) {
  reg::Entity entity = scene.createEntity();
  scene.createComponents(            //
      entity,                        //
      pain::Transform3dComponent{},  //
      pain::MeshComponent{}, pain::NativeScriptComponent{});

  pain::Scene::emplaceScript<Star::Script>(entity, scene, name, mass,
                                           initialPosition, initialVelocity);
  return entity;
};
Script::Script(reg::Entity entity, pain::Scene &scene, const std::string &name,
               float mass, const glm::vec3 &position, const glm::vec3 &velocity)
    : pain::WorldObject(entity, scene),
      m_name(name),
      m_mass(mass),
      m_acceleration(glm::vec3(0.f)),
      m_initialVelocity(velocity),
      m_initialPosition(position) {
  getComponent<pain::Transform3dComponent>().m_position = position;
}

void Script::resetStats() {
  getComponent<pain::Transform3dComponent>().m_position = m_initialPosition;
  m_velocityDir = m_initialVelocity;
  m_acceleration = glm::vec3(0.f);
  getComponent<pain::ParticleSprayComponent>().clear();
}

void Script::setInitialValues(const std::string &name, float mass,
                              const glm::vec3 &position,
                              const glm::vec3 &velocity) {
  m_name = name;
  setInitialValues(mass, position, velocity);
}
void Script::setInitialValues(float mass, const glm::vec3 &position,
                              const glm::vec3 &velocity) {
  m_mass = mass;
  m_acceleration = glm::vec3(0.f);
  m_initialVelocity = velocity;
  m_initialPosition = position;
  getComponent<pain::Transform3dComponent>().m_position = position;
  m_velocityDir = velocity;
}

void Script::onCreate() {
  IMGUI_PLOG([this] {
    const pain::Transform3dComponent &tc =
        getComponent<pain::Transform3dComponent>();
    const std::string label = "Star " + m_name + " Info";

    ImGui::PushID(label.c_str());
    ImGui::SeparatorText(label.c_str());

    // Log stuff
    ImGui::Text("Reset Options");
    ImGui::Text("Coordinates (%.2f,%.2f,%2f)", TP_VEC3(tc.m_position));
    ImGui::Text("Velocity (%.2f,%.2f,%2f)", TP_VEC3(m_velocityDir));
    ImGui::Text("Acceleration %.2f", glm::length(m_acceleration));
    // Reset stuff
    ImGui::Text("Reset Options:");
    const std::string labelMass = "Star " + m_name + " Info";
    ImGui::InputFloat("Mass", &m_mass, 2.f, 1.0f, "%f");
    ImGui::InputFloat3("Initial Velocity", &m_initialVelocity.x);
    ImGui::InputFloat3("Initial Position", &m_initialPosition.x);
    ImGui::PopID();
  });
}

// ------------------------------------------------------------------//
// Modeling
// ------------------------------------------------------------------//

// brutal Force precision attempt

void Script::addForce(const reg::Entity otherStar,
                      const Star::Script &otherStarController,
                      float deltaTime) {
  const glm::vec3 &otherStarPos =
      getComponent<pain::Transform3dComponent>(otherStar).m_position;
  const glm::vec3 &pos = getComponent<pain::Transform3dComponent>().m_position;

  const float G = m_constantG;
  const auto relativePos = otherStarPos - pos;

  const float acceleration =
      otherStarController.m_mass * G / glm::length2(relativePos);
  m_acceleration = acceleration * glm::normalize(relativePos);

  m_velocityDir += m_acceleration * deltaTime;
  pain::Transform3dComponent &tc = getComponent<pain::Transform3dComponent>();
  tc.m_position += m_velocityDir * deltaTime;

  // potential energy (unrelated, for log only)
  m_potentialEnergy =
      otherStarController.m_mass * m_mass * G / glm::length(relativePos);
}

void Script::onUpdate(pain::DeltaTime deltaTimeSec) {
  m_timeSinceLastEmission += deltaTimeSec;
}

// LeapFrog attempt

// void StarController::addForce(const Star *otherStar,
//                               const StarController *otherStarController,
//                               float deltaTime)
// {
//   const glm::vec3 &otherStarPos =
//       otherStar->getComponent<pain::TransformComponent>().m_position;
//   const glm::vec3 &pos = getComponent<pain::TransformComponent>().m_position;
//   pain::MovementComponent &mc = getComponent<pain::MovementComponent>();

//   const float G = ((Game *)m_scene)->m_constantG;
//   const auto relativePos = otherStarPos - pos;

//   mc.m_velocityDir = m_nextStepVel + m_acceleration * deltaTime / 20.f;
//   const float acceleration =
//       otherStarController->m_mass * G / glm::length2(relativePos);
//   m_acceleration = acceleration * glm::normalize(relativePos);

//   // potential energy (unrelated, for log only)
//   m_potentialEnergy =
//       otherStarController->m_mass * m_mass * G / glm::length(relativePos);
// }

// void StarController::onUpdate(double deltaTimeSec)
// {
//   const pain::MovementComponent &mc =
//   getComponent<pain::MovementComponent>(); m_nextStepVel =
//       mc.m_velocityDir + m_acceleration * (float)deltaTimeSec / 20.f;
//   m_timeSinceLastEmission += deltaTimeSec;
// }

// ------------------------------------------------------------------//
// Runge Kutta Method (RK4) attempt
// void StarController::potentialEnergy(const StarController *otherStar)
// {
//   const glm::vec3 &pos = getComponent<pain::TransformComponent>().m_position;
//   const glm::vec3 relativePos =
//       otherStar->getComponent<pain::TransformComponent>().m_position - pos;
//   const float G = ((Game *)m_scene)->m_constantG;

//   m_potentialEnergy = otherStar->m_mass * m_mass * G /
//   glm::length(relativePos);
// }

// glm::vec3 StarController::kAcc(const StarController *otherStar)
// {
//   const glm::vec3 &pos = getComponent<pain::TransformComponent>().m_position;
//   const glm::vec3 relativePos =
//       otherStar->getComponent<pain::TransformComponent>().m_position - pos;
//   const float G = ((Game *)m_scene)->m_constantG;

//   float gravity = G * otherStar->m_mass / glm::length2(relativePos);
//   return gravity * glm::normalize(relativePos); // Acceleration due to
//   gravity
// }

// glm::vec3 StarController::kVel(const glm::vec3 &velocity, const float
// deltaTime,
//                                const glm::vec3 &acceleration)
// {
//   return velocity + acceleration * deltaTime;
// }

// void StarController::addForce(const Star *otherStar,
//                               const StarController *otherStarController,
//                               float deltaTime)
// {
//   pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
//   m_acceleration = kAcc(otherStarController);

//   glm::vec3 kVelA = kVel(m_velocity, deltaTime, m_acceleration);
//   glm::vec3 kVelB = kVel(m_velocity, deltaTime * 0.5f, m_acceleration);
//   glm::vec3 kVelC = kVel(m_velocity, deltaTime * 0.5f, m_acceleration);
//   glm::vec3 kVelD = kVel(m_velocity, deltaTime, m_acceleration);

//   m_velocity = (kVelA + 2.0f * (kVelB + kVelC) + kVelD) / 6.0f;
//   tc.m_position = tc.m_position + m_velocity * deltaTime;

//   // potential energy (unrelated, for log only)
//   potentialEnergy(otherStarController);
// }

// void StarController::onUpdate(double deltaTimeSec)
// {
//   // pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
//   // mc.m_velocityDir += m_acceleration * (float)deltaTimeSec;
//   m_timeSinceLastEmission += deltaTimeSec;
// }

}  // namespace Star
