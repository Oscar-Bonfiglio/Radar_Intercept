//  ui/DebugOverlay.cpp
//  NOTE: SDL2 doesn't render TTF text without SDL_ttf.
//  This implementation draws coloured rectangles as line placeholders
//  so the overlay is *visually present* even without a font library.
//  To add real text: link SDL2_ttf and replace the SDL_RenderFillRect
//  calls with TTF_RenderText_Solid / SDL_RenderCopy.

#include "DebugOverlay.h"
#include <SDL2/SDL.h>

DebugOverlay::DebugOverlay(bool visible)
    : visible_(visible)
{}

void DebugOverlay::clear() {
    lines_.clear();
}

void DebugOverlay::addLine(const std::string& line) {
    lines_.push_back(line);
}

void DebugOverlay::render(Renderer& renderer) const {
    if (!visible_ || lines_.empty())
        return;

    // Panel dimensions
    int panelW = 260;
    int panelH = MARGIN * 2 + static_cast<int>(lines_.size()) * LINE_H;

    // Draw semi-transparent background panel (simulated with horizontal lines)
    renderer.setColor(0, 0, 0, 200);
    for (int row = 0; row < panelH; ++row) {
        renderer.drawLine(
            { static_cast<float>(MARGIN),               static_cast<float>(MARGIN + row) },
            { static_cast<float>(MARGIN + panelW),      static_cast<float>(MARGIN + row) }
        );
    }

    // Draw each line as a bright bar (placeholder for real text)
    int y = MARGIN * 2;
    for (const auto& line : lines_) {
        float barW = std::min(static_cast<float>(line.size()) * 6.f,
                              static_cast<float>(panelW - 8));

        renderer.setColor(100, 220, 100);
        renderer.drawLine(
            { static_cast<float>(MARGIN + 4), static_cast<float>(y) },
            { static_cast<float>(MARGIN + 4) + barW, static_cast<float>(y) }
        );

        y += LINE_H;
    }
}
