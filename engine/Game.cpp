
// ================================================================
//  engine/Game.cpp
// ================================================================
 
#include "Game.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
 
// Construction
Game::Game(int screenW, int screenH)
    : renderer_("Missile Sim", screenW, screenH)
    , guidance_(3.5f)
    , radar_(300.f, 8.f)          // 300px range, 8 RPM
    , debugOverlay_(true)
    , player_(0,
              {(float)screenW * 0.5f, (float)screenH * 0.75f},
              /*maxSpeed=*/350.f, /*thrust=*/9000.f, /*missiles=*/15)
    , screenW_(screenW)
    , screenH_(screenH)
    , prevEnemyPos_({(float)screenW * 0.5f, (float)screenH * 0.25f})
{
    //Position radar at the player's starting location
    radar_.setOrigin(player_.getPosition());
    spawnEnemy({ (float) screenW * 0.3f, (float)screenH * 0.25f });
    spawnEnemy( { (float) screenW * 0.7f, (float)screenH * 0.15f });
}
 
// Main loop
 
void Game::run() {
    if (!renderer_.isOk()) {
        std::cerr << "Renderer failed to initialise. Exiting.\n";
        return;
    }
 
    Uint32 lastTick = SDL_GetTicks();
 
    while (running_) {
        Uint32 now = SDL_GetTicks();
        float  dt  = std::min((now - lastTick) / 1000.f, 0.05f); // cap at 50 ms
        lastTick   = now;
 
        handleEvents();
        update(dt);
        checkCollisions();
        render();
    }
}
 
// Users Input  
 
void Game::handleEvents() {
    SDL_Event event;
    input_.firePressed = false;
    input_.spawnPressed = false;
 
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
 
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running_ = false;             break;
                    case SDLK_UP:     input_.up    = true;          break;
                    case SDLK_DOWN:   input_.down  = true;          break;
                    case SDLK_LEFT:   input_.left  = true;          break;
                    case SDLK_RIGHT:  input_.right = true;          break;
                    case SDLK_SPACE:  input_.firePressed = true;    break;
                    case SDLK_e:  input_.spawnPressed = true;       break;
                    case SDLK_d:      debugOverlay_.toggle();       break;
                    default: break;
                }
                break;
 
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:    input_.up    = false; break;
                    case SDLK_DOWN:  input_.down  = false; break;
                    case SDLK_LEFT:  input_.left  = false; break;
                    case SDLK_RIGHT: input_.right = false; break;
                    default: break;
                }
                break;
        }
    }
}
 
void Game::update(float dt) {
    simTime_ += dt;
    if (playerHitCooldown_ > 0.f)
        playerHitCooldown_ = std::max(0.f, playerHitCooldown_ - dt);
    //Enemy Spawn
    if(input_.spawnPressed) {
        float random_x = (float)(rand() % screenW_);
        float random_y = (float)(rand() % screenH_);
        spawnEnemy({ random_x, random_y });
    }
 
    //Player input
    static Vector2 smoothDir = {};
    Vector2 rawDir = {};
    if (input_.up)    rawDir.y -= 1.f;
    if (input_.down)  rawDir.y += 1.f;
    if (input_.left)  rawDir.x -= 1.f;
    if (input_.right) rawDir.x += 1.f;
    if (rawDir.lengthSq() > 0.f) rawDir = rawDir.normalized();
    
    float turnRate = 0.01f;
    smoothDir = smoothDir + (rawDir - smoothDir ) * turnRate;
    player_.update(dt, smoothDir);
 
    //Wrap player position at screen edges
    Vector2 pp = player_.getPosition();
    if (pp.x < 0)        pp.x += screenW_;
    if (pp.x > screenW_) pp.x -= screenW_;
    if (pp.y < 0)        pp.y += screenH_;
    if (pp.y > screenH_) pp.y -= screenH_;
    player_.getPosition();
 
    //Fire missile at nearest enemy
    if (input_.firePressed && !enemies_.empty()) {
        // Find nearest alive enemy
        Aircraft* target = nullptr;
        float     bestDist = 1e9f;
        for (auto& e : enemies_) {
            if (!e->isAlive()) continue;
            float d = player_.getPosition().distanceTo(e->getPosition());
            if (d < bestDist) { bestDist = d; target = e.get(); }
        }
        if (target) {
            player_.fireMissile(target->getPosition());
        }
    }
 
    //Enemy simple AI: drift toward player
    for (auto& enemy : enemies_) {
        if (!enemy->isAlive()) continue;
        Vector2 toPlayer = (player_.getPosition() - enemy->getPosition()).normalized();
        enemy->update(dt, toPlayer * 0.3f); // lazy drift
    }
 
    //Update player missiles with PN guidance
    for (auto& missile : player_.getMissiles()) {
        if (!missile->isActive()) continue;
 
        //Pick first alive enemy as target
        Aircraft* target = nullptr;
        for (auto& e : enemies_) {
            if (e->isAlive()) { target = e.get(); break; }
        }
 
        Vector2 steer = {};
        if (target) {
            steer = guidance_.calculate(
                missile->getPosition(),
                missile->getVelocity(),
                target->getPosition(),
                prevEnemyPos_,
                dt
            );
            prevEnemyPos_ = target->getPosition();
        }
        missile->update(dt, steer);
    }
 
    // ── Radar update + scan ───────────────────────────────────────
    radar_.setOrigin(player_.getPosition());
    radar_.update(dt);
 
    std::vector<RadarContact> candidates;
    for (auto& e : enemies_) {
        if (!e->isAlive()) continue;
        candidates.push_back({ e->getId(), e->getPosition(),
                                e->getVelocity().length(), false });
    }
    radar_.scan(candidates);
 
    // ── Debug info ────────────────────────────────────────────────
    buildDebugInfo();
}
 
// ── Collisions ────────────────────────────────────────────────────
 
void Game::checkCollisions() {
    for (auto& missile : player_.getMissiles()) {
        if (!missile->isActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->isAlive()) continue;

            float dist = missile->getPosition()
                             .distanceTo(enemy->getPosition());

            if (dist < Missile::IMPACT_THRESHOLD + Aircraft::COLLISION_RADIUS) {
                missile->detonate();
                enemy->takeDamage(missile->getDamage());
            }
        }
    }

    if (!player_.isAlive() || playerHitCooldown_ > 0.f) return;

    const float contactDist = Aircraft::COLLISION_RADIUS * 2.f;
    for (auto& enemy : enemies_) {
        if (!enemy->isAlive()) continue;

        float dist = player_.getPosition().distanceTo(enemy->getPosition());
        if (dist < contactDist) {
            player_.takeDamage(30.f);
            playerHitCooldown_ = 0.6f;
            break;
        }
    }
}
 
// ── Render ────────────────────────────────────────────────────────
 
void Game::render() {
    renderer_.beginFrame();
 
    // Radar (drawn first, in the background)
    std::vector<Vector2> contactPositions;
    for (const auto& c : radar_.getContacts())
        contactPositions.push_back(c.position);
 
    renderer_.drawRadar(player_.getPosition(),
                        radar_.getRange(),
                        radar_.getSweepAngle(),
                        contactPositions);
 
    // Enemies
    for (const auto& enemy : enemies_) {
        if (!enemy->isAlive()) continue;
        renderer_.drawAircraft(enemy->getPosition(), enemy->getHeading(),
                                enemy->getHealth(), enemy->getMaxHealth(),
                                /*isPlayer=*/false);
    }
 
    // Player missiles
    for (const auto& missile : player_.getMissiles()) {
        if (!missile->isActive()) continue;
        renderer_.drawMissile(missile->getPosition(), missile->getVelocity());
    }
 
    // Player aircraft (drawn last so it's on top)
    renderer_.drawAircraft(player_.getPosition(), player_.getHeading(),
                            player_.getHealth(), player_.getMaxHealth(),
                            /*isPlayer=*/true);
 
    // HUD overlay
    debugOverlay_.render(renderer_);
 
    renderer_.endFrame();
}
 
// Helpers
 
void Game::spawnEnemy(const Vector2& pos) {
    int id = static_cast<int>(enemies_.size()) + 1;
    auto e = std::make_unique<Aircraft>(id, pos, 150.f, 5000.f, 0);
    enemies_.push_back(std::move(e));
}
 
void Game::buildDebugInfo() {
    debugOverlay_.clear();
 
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1);
 
    ss.str(""); ss << "Time: " << simTime_ << "s";
    debugOverlay_.addLine(ss.str());
 
    ss.str(""); ss << "Player pos: " << player_.getPosition().toString();
    debugOverlay_.addLine(ss.str());
 
    ss.str(""); ss << "Player spd: " << player_.getVelocity().length();
    debugOverlay_.addLine(ss.str());
 
    ss.str(""); ss << "Missiles:   " << player_.getMissileCount();
    debugOverlay_.addLine(ss.str());
 
    ss.str(""); ss << "Radar contacts: " << radar_.getContacts().size();
    debugOverlay_.addLine(ss.str());

    ss.str(""); ss << "Enemies: " << enemies_.size();
    debugOverlay_.addLine(ss.str());

 
    for (size_t i = 0; i < enemies_.size(); ++i) {
        ss.str("");
        ss << "Enemy#" << i << " HP: "
           << enemies_[i]->getHealth()
           << (enemies_[i]->isAlive() ? "" : " [DEAD]");
        debugOverlay_.addLine(ss.str());
    }
 
    debugOverlay_.addLine("D=toggle debug | SPACE=fire | Arrows=move | Esc=Quit | E=Spawn");
}