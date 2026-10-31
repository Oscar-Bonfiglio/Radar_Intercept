#pragma once

//  guidance/GuidanceSystem.h
//  Computes the steering direction for a missile each tick.
//
//  Implements Proportional Navigation (PN):
//    The missile steers to null the line-of-sight rotation rate.
//    This is how real homing missiles work.
//
//  Usage:
//    GuidanceSystem gs;
//    Vector2 steerDir = gs.calculate(missilePos, missileVel, targetPos);
//    missile.update(dt, steerDir);

#include "../physics/Vector2.h"

class GuidanceSystem { 
public: 
    // Navigation constant (3–5 is typical for PN guidance)
    explicit GuidanceSystem(float navConstant = 3.f);

    // Returns the desired unit steering direction for this tick.
    // missileVel is the current missile velocity vector.
    // prevTargetPos should be the target's position last frame
    //   (pass same as targetPos on first call).
    Vector2 calculate(const Vector2& missilePos,
                      const Vector2& missileVel,
                      const Vector2& targetPos, 
                      const Vector2& prevTargetPos,
                      float dt) const; 

    void setNavConstant(float n) { navConstant_ = n; }
    float getNavConstant() const { return navConstant_; }

private:
    float navConstant_; 
};
