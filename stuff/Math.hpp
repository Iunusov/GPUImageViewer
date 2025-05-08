#pragma once
#include <numbers>
#include <type_traits>

namespace Math {
constexpr double CalculateScreenPosition(const double obj_coord,
                                         const double obj_dimension,
                                         const double camera_coord,
                                         const double screen_resolution,
                                         const double zoom_value) noexcept {
  return obj_coord - obj_dimension / 2.0 - camera_coord +
         (screen_resolution / 2.0) / zoom_value;
}

static_assert(CalculateScreenPosition(-500.0, 80.0, 0.0, 1080.0, 1.0) == 0.0,
              "");
static_assert(CalculateScreenPosition(0.0, 120.0, 0.0, 1920.0, 1.0) == 900.0,
              "");
} // namespace Math
