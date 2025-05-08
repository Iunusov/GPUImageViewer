#include <SDL.h>

#include "Config.hpp"
#include "Coord.hpp"
#include "Scroller.hpp"
#include "VideoContextSDL.hpp"

namespace {
constexpr const float ZERO_FLOAT{0.0f};
constexpr const float ONE_FLOAT{1.0f};

constexpr auto P4 = [](float a) { return a * a * a * a; };
} // namespace

Coord Scroller::GetCameraPos() const noexcept { return m_position; }

float Scroller::getScale() const noexcept { return P4(m_scale); }
double Scroller::getAngle() const noexcept { return m_angle; }

void Scroller::execute() noexcept {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT: {
      escape_key_pressed = true;
      return;
    }
    case SDL_EVENT_KEY_UP:
    case SDL_EVENT_KEY_DOWN: {
      ts = std::chrono::steady_clock::now();
      const bool is_key_down = (event.type == SDL_EVENT_KEY_DOWN);
      switch (event.key.key) {
      case SDLK_UP: {
        m_v_direction = is_key_down ? -ONE_FLOAT : ZERO_FLOAT;
        break;
      }
      case SDLK_DOWN: {
        m_v_direction = is_key_down ? ONE_FLOAT : ZERO_FLOAT;
        break;
      }
      case SDLK_LEFT: {
        m_h_direction = is_key_down ? -ONE_FLOAT : ZERO_FLOAT;
        break;
      }
      case SDLK_RIGHT: {
        m_h_direction = is_key_down ? ONE_FLOAT : ZERO_FLOAT;
        break;
      }
      case SDLK_R: {
        if (is_key_down) {
          if (m_angle >= 360.0f) {
            m_angle = ZERO_FLOAT;
          }
          m_angle += 90.0f;
        }
        break;
      }
      case SDLK_ESCAPE: {
        escape_key_pressed = true;
        break;
      }
      case SDLK_SPACE: {
        if (!is_key_down) {
          break;
        }
        m_angle = ZERO_FLOAT;
        camIndex++;
        if (camIndex == fixedCamPoints.size()) {
          camIndex = 0;
          m_position = {ZERO_FLOAT, ZERO_FLOAT};
          m_angle = ZERO_FLOAT;
          break;
        }
        // m_scale = ONE_FLOAT;

        float sum{camIndex * PADDING_PX};

        for (size_t i(0); i <= camIndex; ++i) {
          if (i == 0 || i == camIndex) {
            sum += (fixedCamPoints[i] / 2);
          } else {
            sum += (fixedCamPoints[i]);
          }
        }
        m_position = {ZERO_FLOAT, sum};

        break;
      }
      default:
        break;
      }
      break;
    }

    case SDL_EVENT_MOUSE_WHEEL: {
      ts = std::chrono::steady_clock::now();
      if ((m_wheel_y > ZERO_FLOAT && event.wheel.y > ZERO_FLOAT) ||
          (m_wheel_y < ZERO_FLOAT && event.wheel.y < ZERO_FLOAT)) {
        m_zoom_direction = event.wheel.y > ZERO_FLOAT ? ONE_FLOAT : -ONE_FLOAT;
      }
      m_wheel_y = event.wheel.y;
      break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      if (mousedown) {
        ts = std::chrono::steady_clock::now();
        m_position.y -= (event.motion.yrel * 10.0f * SCALE_STEP) / getScale();
        m_position.x -= (event.motion.xrel * 10.0f * SCALE_STEP) / getScale();
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      mousedown = true;
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      mousedown = false;
      break;
    }
    default:
      break;
    }
  }

  const float new_scale{m_scale + m_time_per_frame_ms * m_zoom_direction *
                                      SCALE_STEP / 100.0f};
  m_scale = new_scale > 0.0f ? new_scale : m_scale;
  m_position.y +=
      (m_time_per_frame_ms * m_v_direction * SCALE_STEP * 10.0f) / getScale();
  m_position.x +=
      (m_time_per_frame_ms * m_h_direction * SCALE_STEP * 10.0f) / getScale();

  const float step{0.1f};
  m_zoom_direction += m_zoom_direction > ZERO_FLOAT ? -step : step;
  if (m_zoom_direction <= step && m_zoom_direction >= -step) {
    m_zoom_direction = ZERO_FLOAT;
  }
}
