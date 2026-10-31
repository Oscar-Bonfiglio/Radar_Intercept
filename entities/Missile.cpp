// ================================================================
//  entities/Missile.cpp
// ================================================================
 
#include "Missile.h"
#include <iostream>
 
Missile::Missile(int id, const Vector2& spawnPos,
                 const Vector2& initialVelocity,
                 float fuel, float thrust, float damage)
    : id_(id)
    , body_(40.f, 0.005f)    // 40 kg, low drag
    , fuel_(fuel)
    , thrust_(thrust)
    , damage_(damage)
{
    body_.setPosition(spawnPos);
    body_.setVelocity(initialVelocity);
}
 
void Missile::update(float dt, const Vector2& steerDir) {
    if (state_ != State::Active) return;
 
    if (fuel_ > 0.f) {
        // Apply thrust in the steered direction
        body_.applyForce(steerDir * thrust_);
 
        fuel_ -= burnRate_ * dt;
        if (fuel_ <= 0.f) {
            fuel_  = 0.f;
            state_ = State::OutOfFuel;
            std::cout << "[Missile #" << id_ << "] Out of fuel.\n";
        }
    }
 
    body_.update(dt);
}
 
void Missile::detonate() {
    if (state_ == State::Exploded) return;
    state_ = State::Exploded;
    std::cout << "[Missile #" << id_ << "] DETONATED at "
              << body_.getPosition().toString() << "\n";
}
 