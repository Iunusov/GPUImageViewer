#pragma once

#include <SDL.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>

#include "Coord.hpp"
#include "CppHacks.hpp"
#include "IVideoContext.hpp"

class VideoContextSDL final : public IVideoContext {
private:
  static IVideoContext *instance;
  SDL_Renderer *rend = nullptr;
  float m_scale{1};
  Coord cameraPosition;
  SDL_Window *win = nullptr;
  int w{};
  int h{};
  float m_fps = 100.0f;
  Window mainWindow{};

public:
  static void Create(Window) noexcept;
  static IVideoContext *GetInstance() noexcept { return instance; }
  VideoContextSDL(Window data) { mainWindow = data; }

  ~VideoContextSDL() noexcept override;

  void setCamera(const Coord &pos) noexcept override { cameraPosition = pos; }

  void setScale(float scale) noexcept override {
    m_scale = scale;
    SDL_SetRenderScale(rend, m_scale, m_scale);
    SDL_Rect viewport{0, 0, (int)(w / m_scale), (int)(h / m_scale)};
    SDL_SetRenderViewport(rend, &viewport);
  }

  int getWidth() const noexcept override { return w; }
  int getHeigt() const noexcept override { return h; }
  float getFps() const noexcept override { return m_fps; }
  INLINE void clear() noexcept override;
  INLINE void delay(size_t ms) const noexcept override;
  void present() noexcept override;
  INLINE void draw(const std::string &obj) noexcept override;
  void setup() noexcept override;
  static void getPrimaryDisplayResolution(int &w, int &h) noexcept {
    auto dmode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    w = dmode->w;
    h = dmode->h;
  }
};
