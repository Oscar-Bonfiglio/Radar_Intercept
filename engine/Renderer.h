#pragma once
 
// ================================================================
//  engine/Renderer.h
//  Thin SDL2 wrapper.  Knows how to draw primitives, aircraft
//  sprites (triangles), missile trails, and the radar display.
// ================================================================
 
#include <SDL2/SDL.h>
#include "../physics/Vector2.h"
#include <string>
#include <vector>
 
class Renderer {
public:
    // ── Construction ─────────────────────────────────────────────
    Renderer(const std::string& title, int width, int height);
    ~Renderer();
 
    // Non-copyable
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
 
    // ── Frame lifecycle ──────────────────────────────────────────
    void beginFrame();    // clear screen
    void endFrame();      // SDL_RenderPresent
 
    // ── Primitives ───────────────────────────────────────────────
    void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void drawLine(const Vector2& a, const Vector2& b);
    void drawCircle(const Vector2& centre, float radius);
    void drawFilledCircle(const Vector2& centre, float radius);
    void drawText(const std::string& text, const Vector2& pos); // simple pixel text stub
 
    // ── Entity drawing helpers ───────────────────────────────────
    // Aircraft: small triangle pointing in heading direction
    void drawAircraft(const Vector2& pos, float headingRad,
                      float health, float maxHealth,
                      bool isPlayer = false);
 
    // Missile: small dot with a short velocity trail
    void drawMissile(const Vector2& pos, const Vector2& velocity);
 
    // Radar: sweep circle + beam line + contact blips
    void drawRadar(const Vector2& origin, float range,
                   float sweepAngle,
                   const std::vector<Vector2>& contacts);
 
    // ── Utilities ────────────────────────────────────────────────
    int getWidth()  const { return width_;  }
    int getHeight() const { return height_; }
    bool isOk()     const { return window_ && renderer_; }
 
private:
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    int width_, height_;
 
    // Internal helpers
    void drawTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3);
};
 