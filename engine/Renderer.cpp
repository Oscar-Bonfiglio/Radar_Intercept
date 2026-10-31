// ================================================================
//  engine/Renderer.cpp
// ================================================================

#include "Renderer.h"
#include <cmath>
#include <iostream>

// ── Construction / destruction ────────────────────────────────────

Renderer::Renderer(const std::string& title, int width, int height)
    : width_(width), height_(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return;
    }

    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return;
    }

    renderer_ = SDL_CreateRenderer(
        window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
    }
}

Renderer::~Renderer() {
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_)   SDL_DestroyWindow(window_);
    SDL_Quit();
}

// ── Frame lifecycle ───────────────────────────────────────────────

void Renderer::beginFrame() {
    // Dark sky background
    SDL_SetRenderDrawColor(renderer_, 10, 10, 20, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::endFrame() {
    SDL_RenderPresent(renderer_);
}

// ── Primitives ────────────────────────────────────────────────────

void Renderer::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}

void Renderer::drawLine(const Vector2& a, const Vector2& b) {
    SDL_RenderDrawLine(renderer_,
        static_cast<int>(a.x), static_cast<int>(a.y),
        static_cast<int>(b.x), static_cast<int>(b.y));
}

void Renderer::drawCircle(const Vector2& centre, float radius) {
    // Midpoint circle algorithm
    int x0 = static_cast<int>(centre.x);
    int y0 = static_cast<int>(centre.y);
    int r  = static_cast<int>(radius);
    int x = r, y = 0, err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(renderer_, x0+x, y0+y);
        SDL_RenderDrawPoint(renderer_, x0+y, y0+x);
        SDL_RenderDrawPoint(renderer_, x0-y, y0+x);
        SDL_RenderDrawPoint(renderer_, x0-x, y0+y);
        SDL_RenderDrawPoint(renderer_, x0-x, y0-y);
        SDL_RenderDrawPoint(renderer_, x0-y, y0-x);
        SDL_RenderDrawPoint(renderer_, x0+y, y0-x);
        SDL_RenderDrawPoint(renderer_, x0+x, y0-y);
        ++y;
        if (err <= 0) err += 2*y + 1;
        else        { --x; err += 2*(y-x) + 1; }
    }
}

void Renderer::drawFilledCircle(const Vector2& centre, float radius) {
    int r  = static_cast<int>(radius);
    int cx = static_cast<int>(centre.x);
    int cy = static_cast<int>(centre.y);
    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt((float)(r*r - dy*dy)));
        SDL_RenderDrawLine(renderer_, cx-dx, cy+dy, cx+dx, cy+dy);
    }
}

void Renderer::drawText(const std::string& /*text*/, const Vector2& /*pos*/) {
    // SDL2 without SDL_ttf can't easily render text.
    // Hook up SDL_ttf here in a later iteration, or use a bitmap font.
}

// ── Triangle helper ───────────────────────────────────────────────

void Renderer::drawTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3) {
    SDL_RenderDrawLine(renderer_,
        (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    SDL_RenderDrawLine(renderer_,
        (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y);
    SDL_RenderDrawLine(renderer_,
        (int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y);
}

// ── Entity drawing ────────────────────────────────────────────────

void Renderer::drawAircraft(const Vector2& pos, float headingRad,
                             float health, float maxHealth,
                             bool isPlayer)
{
    // Triangle: nose forward, two wings behind
    const float SIZE = 14.f;
    auto rot = [&](float lx, float ly) -> Vector2 {
        float cs = std::cos(headingRad), sn = std::sin(headingRad);
        return pos + Vector2{ lx*cs - ly*sn, lx*sn + ly*cs };
    };

    if (isPlayer)
        setColor(80, 200, 255);   // cyan for player
    else
        setColor(255, 80, 80);    // red for enemy

    drawTriangle(rot(SIZE, 0.f), rot(-SIZE*0.6f, -SIZE*0.6f),
                 rot(-SIZE*0.6f,  SIZE*0.6f));

    // Health bar — drawn below the aircraft, wider and taller for readability
    const float barW   = 44.f;
    const float barH   = 5.f;
    const float barY   = pos.y + 22.f;   // below the sprite
    float       filled = (health / maxHealth) * barW;
    if (filled < 0.f) filled = 0.f;

    // Dark background track
    setColor(40, 40, 40);
    SDL_Rect bg { (int)(pos.x - barW/2), (int)barY, (int)barW, (int)barH };
    SDL_RenderFillRect(renderer_, &bg);

    // Colour shifts green → yellow → red as health drops
    float ratio = health / maxHealth;
    Uint8 rr = (Uint8)((1.f - ratio) * 255);
    Uint8 gg = (Uint8)(ratio        * 200);
    setColor(rr, gg, 30);
    SDL_Rect fg { (int)(pos.x - barW/2), (int)barY, (int)filled, (int)barH };
    SDL_RenderFillRect(renderer_, &fg);

    // Thin white border around the bar
    setColor(160, 160, 160);
    SDL_Rect border { (int)(pos.x - barW/2) - 1, (int)barY - 1,
                      (int)barW + 2,              (int)barH + 2 };
    SDL_RenderDrawRect(renderer_, &border);
}

void Renderer::drawMissile(const Vector2& pos, const Vector2& velocity) {
    // Bright white dot
    setColor(255, 255, 180);
    drawFilledCircle(pos, 3.f);

    // Short exhaust trail in the opposite direction
    Vector2 tail = pos - velocity.normalized() * 12.f;
    setColor(255, 120, 30, 180);
    drawLine(pos, tail);
}

void Renderer::drawRadar(const Vector2& origin, float range,
                          float sweepAngle,
                          const std::vector<Vector2>& contacts)
{
    // Outer ring
    setColor(0, 80, 0, 180);
    drawCircle(origin, range);

    // Inner rings at 1/3 and 2/3
    setColor(0, 50, 0, 120);
    drawCircle(origin, range * 0.33f);
    drawCircle(origin, range * 0.66f);

    // Sweep beam line
    setColor(0, 220, 0, 200);
    Vector2 beamEnd = origin + Vector2::fromAngle(sweepAngle, range);
    drawLine(origin, beamEnd);

    // Contact blips
    setColor(0, 255, 80);
    for (const auto& c : contacts) {
        drawFilledCircle(c, 4.f);
    }
}