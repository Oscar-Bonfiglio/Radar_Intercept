// ================================================================
//  entities/Aircraft.cpp
// ================================================================
 
#include "Aircraft.h"
#include <iostream>
#include <cmath>
 
Aircraft::Aircraft(int id, const Vector2& spawnPos,
                   float maxSpeed, float engineThrust,
                   int maxMissiles)
    : id_(id)
    , body_(800.f, 0.02f)    // 800 kg airframe, moderate drag
    , maxSpeed_(maxSpeed)
    , engineThrust_(engineThrust)
    , health_(maxHealth_)
    , maxMissiles_(maxMissiles)
{
    body_.setPosition(spawnPos);
}
 
void Aircraft::update(float dt, const Vector2& inputDir) {
    if (state_ != State::Flying) return;
 
    // Apply thrust in the input direction
    if (inputDir.lengthSq() > 0.f) {
        body_.applyForce(inputDir.normalized() * engineThrust_);
    }
 
    body_.update(dt);
 
    // Clamp to max speed
    if (body_.getSpeed() > maxSpeed_) {
        body_.setVelocity(body_.getVelocity().normalized() * maxSpeed_);
    }
}
 
bool Aircraft::takeDamage(float amount) {
    if (state_ == State::Destroyed) return true;
 
    health_ -= amount;
    std::cout << "[Aircraft #" << id_ << "] Hit! Health: " << health_ << "\n";
 
    if (health_ <= 0.f) {
        health_ = 0.f;
        state_  = State::Destroyed;
        std::cout << "[Aircraft #" << id_ << "] DESTROYED.\n";
        return true;
    }
    return false;
}
 
Missile* Aircraft::fireMissile(const Vector2& targetPos) {
    if ((int)missiles_.size() >= maxMissiles_) {
        std::cout << "[Aircraft #" << id_ << "] No missiles left.\n";
        return nullptr;
    }
 
    // Initial velocity inherits aircraft velocity plus forward kick
    Vector2 heading  = body_.getVelocity().normalized();
    Vector2 initVel  = body_.getVelocity() + heading * 120.f;
 
    auto missile = std::make_unique<Missile>(
        nextMissileId_++,
        body_.getPosition(),
        initVel,
        /*fuel=*/60.f, /*thrust=*/4000.f, /*damage=*/50.f
    );
 
    Missile* ptr = missile.get();
    missiles_.push_back(std::move(missile));
 
    std::cout << "[Aircraft #" << id_ << "] Fired missile toward "
              << targetPos.toString() << "\n";
    return ptr;
}