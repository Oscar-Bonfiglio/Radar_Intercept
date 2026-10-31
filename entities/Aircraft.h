#pragma once
 
// ================================================================
//  entities/Aircraft.h
//  Player-controlled (or AI) aircraft.
//  Owns a PhysicsBody, health, and a missile payload.
// ================================================================
 
#include "../physics/PhysicsBody.h"
#include "../physics/Vector2.h"
#include "Missile.h"
#include <vector>
#include <memory>
 
class Aircraft {
public:
    enum class State { Flying, Destroyed };
 
    // ── Construction ─────────────────────────────────────────────
    Aircraft(int id, const Vector2& spawnPos,
             float maxSpeed, float engineThrust,
             int maxMissiles = 4);
 
    // ── Simulation ───────────────────────────────────────────────
    // inputDir: normalised direction from player input or AI (-1..1 per axis)
    void update(float dt, const Vector2& inputDir);
 
    // Receive damage (returns true if aircraft is now destroyed)
    bool takeDamage(float amount);
 
    // Fire a missile toward a world-space target point.
    // Returns a raw pointer to the new missile (owned by this aircraft),
    // or nullptr if no ammo remains.
    Missile* fireMissile(const Vector2& targetPos);
 
    // ── Accessors ────────────────────────────────────────────────
    int     getId()       const { return id_;               }
    Vector2 getPosition() const { return body_.getPosition(); }
    Vector2 getVelocity() const { return body_.getVelocity(); }
    float   getHeading()  const { return body_.getVelocity().angle(); }
    float   getHealth()   const { return health_;           }
    float   getMaxHealth()const { return maxHealth_;        }
    int     getMissileCount() const { return static_cast<int>(missiles_.size()); }
    State   getState()    const { return state_;            }
    bool    isAlive()     const { return state_ == State::Flying; }
 
    // All missiles (including spent ones — Game filters by isActive())
    const std::vector<std::unique_ptr<Missile>>& getMissiles() const { return missiles_; }
 
    static constexpr float COLLISION_RADIUS = 18.f;  // pixels
 
private:
    int         id_;
    PhysicsBody body_;
    float       maxSpeed_;
    float       engineThrust_;
    float       health_;
    float       maxHealth_    = 100.f;
    State       state_        = State::Flying;
    int         nextMissileId_ = 0;
    int         maxMissiles_;
 
    std::vector<std::unique_ptr<Missile>> missiles_;
};