#pragma once

#include <algorithm>
#include <cmath>

namespace umas::input
{

// Blend a linear response with a cubic curve. Values inside the deadzone are
// zero; the remaining range is remapped so full stick still gives full output.
inline double shapeStickAxis(double value, double maxAbs, double deadzoneAbs,
                             double expo = 0.65)
{
    if (maxAbs <= 0.0)
        return 0.0;

    const double normalized = std::clamp(value / maxAbs, -1.0, 1.0);
    const double deadzone = std::clamp(deadzoneAbs / maxAbs, 0.0, 0.99);
    const double magnitude = std::abs(normalized);
    if (magnitude <= deadzone)
        return 0.0;

    const double remapped = (magnitude - deadzone) / (1.0 - deadzone);
    const double clampedExpo = std::clamp(expo, 0.0, 1.0);
    const double shaped = (1.0 - clampedExpo) * remapped +
                          clampedExpo * remapped * remapped * remapped;
    return std::copysign(shaped * maxAbs, normalized);
}

} // namespace umas::input
