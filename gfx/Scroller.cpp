#include "Scroller.hpp"
#include "VideoContextSDL.hpp"
#include <SDL.h>

#include "Config.hpp"
#include "Coord.hpp"

namespace {
constexpr const float ZERO_FLOAT{0.0f};
constexpr const float ONE_FLOAT{1.0f};

constexpr auto P4 = [](float a) { return a * a * a * a; };
} // namespace

Coord Scroller::GetCameraPos() const noexcept { return m_position; }

float Scroller::getScale() const noexcept { return P4(m_scale); }
double Scroller::getAngle() const noexcept { return m_angle; }

void Scroller::execute() noexcept {
  // Get the next event
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      escape_key_pressed = true;
      return;
    } else if (event.type == SDL_EVENT_KEY_DOWN ||
               event.type == SDL_EVENT_KEY_UP) {
      ts = std::chrono::steady_clock::now();
      switch (event.key.key) {
      case SDLK_UP:
        m_v_direction =
            event.type == SDL_EVENT_KEY_DOWN ? -ONE_FLOAT : ZERO_FLOAT;
        break;
      case SDLK_DOWN:
        m_v_direction =
            event.type == SDL_EVENT_KEY_DOWN ? ONE_FLOAT : ZERO_FLOAT;
        break;
      case SDLK_LEFT:
        m_h_direction =
            event.type == SDL_EVENT_KEY_DOWN ? -ONE_FLOAT : ZERO_FLOAT;
        break;
      case SDLK_RIGHT:
        m_h_direction =
            event.type == SDL_EVENT_KEY_DOWN ? ONE_FLOAT : ZERO_FLOAT;
        break;
      case SDLK_R:
        if (event.type == SDL_EVENT_KEY_DOWN) {
          if (m_angle >= 360.0f) {
            m_angle = ZERO_FLOAT;
          }
          m_angle += 90.0f;
        }
        break;
      case SDLK_ESCAPE:
        escape_key_pressed = true;
        break;
      case SDLK_SPACE:
        m_scale = ONE_FLOAT;
        m_position.y = ZERO_FLOAT;
        m_position.x = ZERO_FLOAT;
        m_angle = ZERO_FLOAT;
        break;
      default:
        break;
      }
    }

    else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
      ts = std::chrono::steady_clock::now();
      if (m_wheel_y > ZERO_FLOAT && event.wheel.y > ZERO_FLOAT ||
          m_wheel_y < ZERO_FLOAT && event.wheel.y < ZERO_FLOAT) {
        m_zoom_direction = event.wheel.y > ZERO_FLOAT ? ONE_FLOAT : -ONE_FLOAT;
      }
      m_wheel_y = event.wheel.y;
    }
  }

  const float new_scale{m_scale + m_time_per_frame_ms * m_zoom_direction *
                                      SCALE_STEP / 100.0f};
  m_scale = new_scale > 0.0f ? new_scale : m_scale;
  m_position.y +=
      (m_time_per_frame_ms * m_v_direction * SCALE_STEP * 10.0f) / getScale();
  m_position.x +=
      (m_time_per_frame_ms * m_h_direction * SCALE_STEP * 10.0f) / getScale();

  float const step{0.1f};
  m_zoom_direction += m_zoom_direction > ZERO_FLOAT ? -step : step;
  if (m_zoom_direction <= step && m_zoom_direction >= -step) {
    m_zoom_direction = ZERO_FLOAT;
  }
}
