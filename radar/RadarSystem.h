#pragma once

//  radar/RadarSystem.h
//  Simulates a radar sweep that detects Aircraft within range.
//  The sweep rotates at a fixed RPM; a target is "painted" only
//  when the beam passes over it.

#include "../physics/Vector2.h"
#include <vector>

struct RadarContact {
    int     entityID;
    Vector2 position;
    float   speed;
    bool    isFresh;   // true if the sweep passed over it this frame
};

class RadarSystem {
public:
    // range: detection radius in world units (pixels)
    // sweepRPM: beam rotations per minute
    explicit RadarSystem(float range = 400.f, float sweepRPM = 10.f);

    // Update the sweep angle; dt in seconds.
    void update(float dt);

    // Scan a list of candidate targets.
    // Only returns contacts within range AND inside the sweep beam.
    void scan(const std::vector<RadarContact>& candidates);

    // Accessors
    float getSweepAngle() const { return sweepAngle_; }  // radians
    float getRange()      const { return range_; }
    const std::vector<RadarContact>& getContacts() const { return contacts_; }

    void setOrigin(const Vector2& o) { origin_ = o; }
    Vector2 getOrigin() const { return origin_; }

private:
    Vector2 origin_;
    float   range_;
    float   sweepAngle_ = 0.f;   // current angle of the radar beam
    float   sweepSpeed_;         // radians per second
    float   beamWidth_ = 0.15f;  // radians (approx 8.6 degrees)

    std::vector<RadarContact> contacts_;
};
