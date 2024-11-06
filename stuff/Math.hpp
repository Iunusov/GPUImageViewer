#pragma once
#include <numbers>

namespace Math {
constexpr double CalculateScreenPosition(const double obj_coord,
                                         const double obj_dimension,
                                         const double camera_coord,
                                         const double screen_resolution,
                                         const double zoom_value) noexcept {
  return obj_coord - obj_dimension / 2.0 - camera_coord +
         (screen_resolution / 2.0) / zoom_value;
}

} // namespace Math
