//  radar/RadarSystem.cpp

#include "RadarSystem.h"
#include <cmath>

static constexpr float TWO_PI = 2.f * static_cast<float>(M_PI);

RadarSystem::RadarSystem(float range, float sweepRpm)
    : origin_(0.f, 0.f),
      range_(range),
      sweepSpeed_(sweepRpm * TWO_PI / 60.f)   // RPM → rad/s
{}

// Update sweep angle
void RadarSystem::update(float dt) {
    sweepAngle_ += sweepSpeed_ * dt;

    // Wrap to [0, 2π)
    if (sweepAngle_ >= TWO_PI) sweepAngle_ -= TWO_PI;
    if (sweepAngle_ < 0.f)     sweepAngle_ += TWO_PI;

    // Mark all contacts as not fresh this frame
    for (auto& c : contacts_) {
        c.isFresh = false;
    }
}

void RadarSystem::scan(const std::vector<RadarContact>& candidates) {
    for (const auto& candidate : candidates) {

        // Distance check
        Vector2 delta = candidate.position - origin_;
        float dist = delta.length();
        if (dist > range_) continue;

        // Angle of target relative to radar origin
        float angle = std::atan2(delta.y, delta.x);
        if (angle < 0.f) angle += TWO_PI;

        // Angular difference between beam and target
        float diff = std::abs(angle - sweepAngle_);
        if (diff > static_cast<float>(M_PI))
            diff = TWO_PI - diff;

        // Inside beam?
        if (diff <= beamWidth_ * 0.5f) {

            bool found = false;

            // Update existing contact
            for (auto& c : contacts_) {
                if (c.entityID == candidate.entityID) {
                    c = candidate;
                    c.isFresh = true;
                    found = true;
                    break;
                }
            }

            // Add new contact
            if (!found) {
                RadarContact fresh = candidate;
                fresh.isFresh = true;
                contacts_.push_back(fresh);
            }
        }
    }
}
