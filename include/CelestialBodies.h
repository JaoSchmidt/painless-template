#pragma once

#include <pain.h>

#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Star {

reg::Entity create(pain::Scene &scene,                                  //
                   pain::Texture &pTexture,                             //
                   const std::string &name, const glm::vec3 &position,  //
                   float mass, const glm::vec3 &initialVelocity,        //
                   const glm::vec3 &cor = glm::vec3(1.f)                //
);

class Script : public pain::WorldObject {
 public:
  Script(reg::Entity entity, pain::Scene &scene, const std::string &name,
         float mass, const glm::vec3 &position, const glm::vec3 &velocity);
  void onCreate();
  void onUpdate(pain::DeltaTime deltaTimeSec);
  void setInitialValues(float mass, const glm::vec3 &position,
                        const glm::vec3 &velocity);
  void setInitialValues(const std::string &name, float mass,
                        const glm::vec3 &position, const glm::vec3 &velocity);
  void resetStats();
  std::string m_name;
  void addForce(const reg::Entity star, const Script &starController,
                float deltaTime);

  float getMotionEnergyFromOrigin() {
    return m_mass * glm::length2(m_velocityDir) / 2;
  };
  float getPotentialEnergy() { return m_potentialEnergy; }
  void potentialEnergy(const Script *otherStar);

 private:
  glm::vec3 kVel(const glm::vec3 &velocity, const float deltaTime,
                 const glm::vec3 &acceleration);
  glm::vec3 kAcc(const Script *otherStar);

  float m_mass;
  glm::vec3 m_acceleration = glm::vec3(0.f);
  glm::vec3 m_velocityDir = glm::vec3(0.f);
  glm::vec3 m_initialVelocity;
  glm::vec3 m_initialPosition;

  // energy
  float m_potentialEnergy;

  // particles
  pain::DeltaTime m_timeSinceLastEmission = 0;
  pain::DeltaTime m_emissionInterval = pain::DeltaTime::oneSecond() * 0.02;

  constexpr static float m_constantG = 1.f;
};

}  // namespace Star
