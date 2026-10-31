#pragma once
 
// ================================================================
//  entities/Missile.h
//  A homing missile entity.  Owns a PhysicsBody and is steered
//  each tick by GuidanceSystem.
// ================================================================
 
#include "../physics/PhysicsBody.h"
#include "../physics/Vector2.h"
#include <string>
 
class Missile {
public:
    enum class State { Active, Exploded, OutOfFuel };
 
    // ── Construction ─────────────────────────────────────────────
    Missile(int id, const Vector2& spawnPos, const Vector2& initialVelocity,
            float fuel, float thrust, float damage);
 
    // ── Simulation ───────────────────────────────────────────────
    // steerDir is the unit direction supplied by GuidanceSystem.
    void update(float dt, const Vector2& steerDir);
 
    // Trigger detonation (called by Game on confirmed hit)
    void detonate();
 
    // ── Accessors ────────────────────────────────────────────────
    int         getId()       const { return id_;                       }
    Vector2     getPosition() const { return body_.getPosition();       }
    Vector2     getVelocity() const { return body_.getVelocity();       }
    float       getFuel()     const { return fuel_;                     }
    float       getDamage()   const { return damage_;                   }
    State       getState()    const { return state_;                    }
    bool        isActive()    const { return state_ == State::Active;   }
    float       getHeading()  const { return body_.getVelocity().angle(); }
 
    static constexpr float BLAST_RADIUS     = 25.f;   // pixels
    static constexpr float IMPACT_THRESHOLD = 12.f;   // pixels
 
private:
    int          id_;
    PhysicsBody  body_;
    float        fuel_;
    float        thrust_;
    float        damage_;
    float        burnRate_ = 1.5f;   // kg/s
    State        state_    = State::Active;
};
 