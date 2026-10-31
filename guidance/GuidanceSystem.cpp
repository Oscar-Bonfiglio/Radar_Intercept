//  guidance/GuidanceSystem.cpp
//
//  Proportional Navigation (PN) implementation.

#include "GuidanceSystem.h"
#include <cmath>

GuidanceSystem::GuidanceSystem(float navConstant)
    : navConstant_(navConstant) {}

Vector2 GuidanceSystem::calculate(const Vector2& missilePos,
                                  const Vector2& missileVel,
                                  const Vector2& targetPos,
                                  const Vector2& prevTargetPos,
                                  float dt) const
{
    // Line-of-sight (LOS) vector
    Vector2 los = targetPos - missilePos;
    float   losDist = los.length();

    if (losDist < 1e-3f) {
        return missileVel.normalized();
    }

    Vector2 losUnit = los / losDist;

    // LOS rotation rate (λ̇)
    Vector2 prevLos = prevTargetPos - missilePos;

    float losAngle     = std::atan2(los.y,     los.x);
    float prevLosAngle = std::atan2(prevLos.y, prevLos.x);

    float losRate = 0.f;
    if (dt > 1e-6f) {
        float deltaAngle = losAngle - prevLosAngle;

        // Wrap to [-π, π]
        while (deltaAngle >  M_PI) deltaAngle -= 2.f * M_PI;
        while (deltaAngle < -M_PI) deltaAngle += 2.f * M_PI;

        losRate = deltaAngle / dt;
    }

    // Closing velocity (positive when closing)
    float closingSpeed = -missileVel.dot(losUnit);

    // PN commanded acceleration
    float aCmdMag = navConstant_ * std::abs(closingSpeed) * losRate;

    // Perpendicular to LOS (rotate LOS 90°)
    Vector2 losPerp = { -losUnit.y, losUnit.x };

    // Blend: mostly point at target, plus PN correction
    Vector2 desiredDir = losUnit + losPerp * (aCmdMag * 0.05f);

    return desiredDir.normalized();
}
