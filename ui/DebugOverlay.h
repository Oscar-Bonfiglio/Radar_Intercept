#pragma once

//  ui/DebugOverlay.h
//  Collects debug strings each frame and renders them as an
//  on-screen HUD. Separate from Renderer so it can be toggled
//  off without touching rendering logic.

#include "../engine/Renderer.h"
#include <string>
#include <vector>

class DebugOverlay {
public:
    explicit DebugOverlay(bool visible = true);

    // Toggle with a keypress
    void toggle() { visible_ = !visible_; }
    bool isVisible() const { return visible_; }

    // Call each frame before render() to queue lines
    void clear();
    void addLine(const std::string& line);

    // Draws queued lines in the top-left corner using Renderer
    void render(Renderer& renderer) const;

private:
    bool visible_;
    std::vector<std::string> lines_;

    static constexpr int MARGIN  = 8;
    static constexpr int LINE_H  = 12;
};
