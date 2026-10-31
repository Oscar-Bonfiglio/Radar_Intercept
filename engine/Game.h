#pragma once
 
// ================================================================
//  engine/Game.h
//  Top-level simulation coordinator.
//  Owns all subsystems, runs the main loop, dispatches input,
//  updates entities, and checks collisions.
// ================================================================
 
#include "Renderer.h"
#include "../entities/Aircraft.h"
#include "../guidance/GuidanceSystem.h"
#include "../radar/RadarSystem.h"
#include "../ui/DebugOverlay.h"
 
#include <vector>
#include <memory>
 
class Game {
public:
    Game(int screenW, int screenH);
 
    // Runs until the window is closed.  Blocks.
    void run();
 
private:
    // ── Subsystems ───────────────────────────────────────────────
    Renderer       renderer_;
    GuidanceSystem guidance_;
    RadarSystem    radar_;
    DebugOverlay   debugOverlay_;
 
    // ── Entities ─────────────────────────────────────────────────
    Aircraft player_;
    std::vector<std::unique_ptr<Aircraft>> enemies_;
 
    // ── Simulation state ─────────────────────────────────────────
    bool  running_      = true;
    float simTime_      = 0.f;      // total elapsed seconds
    int   screenW_, screenH_;
 
    // Previous target positions (needed by PN guidance)
    // Simple: we track one enemy target per missile
    Vector2 prevEnemyPos_;
    float playerHitCooldown_ = 0.f;
 
    // ── Loop phases ──────────────────────────────────────────────
    void handleEvents();
    void update(float dt);
    void checkCollisions();
    void render();
 
    // ── Helpers ──────────────────────────────────────────────────
    void spawnEnemy(const Vector2& pos);
    void buildDebugInfo();
 
    // Input state
    struct InputState {
        bool up = false, down = false, left = false, right = false;
        bool firePressed = false,  spawnPressed = false;
    } input_;
};
 