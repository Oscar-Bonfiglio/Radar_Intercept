#pragma once

//  physics/PhysicsBody.h
//  A generic rigid-body component used by Aircraft and Missile.
//  Handles position, velocity, acceleration, drag, and Euler
//  integration each tick.

#include "Vector2.h"

class PhysicsBody {
public:
    // Construction
    explicit PhysicsBody(float mass = 1.f, float drag = 0.01f);
    
    // Per-frame update
    void update(float dt);

    // Force application 
    void applyForce(const Vector2& force);

    // Instant velocity change
    void applyImpulse(const Vector2& impulse);

    // Accessors
    Vector2 getPosition()     const { return position_; }
    Vector2 getVelocity()     const { return velocity_; }
    Vector2 getAcceleration() const { return acceleration_; }
    float   getMass()         const { return mass_; }
    float   getSpeed()        const { return velocity_.length(); }

    void setPosition(const Vector2& p) { position_ = p; }
    void setVelocity(const Vector2& v) { velocity_ = v; }

private:
    Vector2 position_;
    Vector2 velocity_;
    Vector2 acceleration_;

    float mass_;  // kg
    float drag_;  // linear drag coefficient

    Vector2 accumulatedForce_;
};
