#pragma once
#include <pain.h>

#include "CelestialBodies.h"

namespace Game {
reg::Entity create(pain::Scene &scene, int cameraWidth, int cameraHeight,
                   pain::Application *app);

class Script : public pain::WorldObject {
 public:
  Script(reg::Entity, pain::Scene &, reg::Entity bodyA, reg::Entity bodyB,
         bool isRunning);
  void onCreate();
  void onUpdate(pain::DeltaTime deltaTime);
  float m_constantG = 1.f;
  int m_timeDivision = 50;

 private:
  // parameters
  bool m_isRunning;

  // non-parameters
  reg::Entity m_bodyA;
  reg::Entity m_bodyB;
};

}  // namespace Game
