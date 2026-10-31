//  physics/PhysicsBody.cpp

#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(float mass, float drag)
    : mass_(mass > 0.f ? mass : 1.f),
      drag_(drag),
      position_(),
      velocity_(),
      acceleration_(),
      accumulatedForce_()
{}

void PhysicsBody::update(float dt) {
    // a = F / m
    acceleration_ = accumulatedForce_ * (1.f / mass_);

    // Semi-implicit Euler:
    // v(t+dt) = v(t) + a*dt
    velocity_ += acceleration_ * dt;

    // Linear drag
    velocity_ -= velocity_ * (drag_ * dt);

    // x(t+dt) = x(t) + v*dt
    position_ += velocity_ * dt;

    // Reset forces for next tick
    accumulatedForce_ = {};
}

void PhysicsBody::applyForce(const Vector2& force) {
    accumulatedForce_ += force;
}

// Impulse: v += J / m
void PhysicsBody::applyImpulse(const Vector2& impulse) {
    velocity_ += impulse * (1.f / mass_);
}
