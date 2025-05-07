#pragma once

#include <SDL.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>

#include "Coord.hpp"
#include "CppHacks.hpp"
#include "IVideoContext.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <iostream>

class VideoContextSDL final : public IVideoContext {
public:
private:
  static IVideoContext *instance;
  SDL_Renderer *rend = nullptr;
  float m_scale{1};
  double m_angle{};
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
  size_t getMaxTextureSize() const noexcept override;

  void setScale(float scale) noexcept override {
    m_scale = scale;
    SDL_SetRenderScale(rend, m_scale, m_scale);
    SDL_Rect viewport{0, 0, (int)((float)w / m_scale),
                      (int)((float)h / m_scale)};
    SDL_SetRenderViewport(rend, &viewport);
  }
  void setAngle(double angle) noexcept override { m_angle = angle; }

  size_t getWidth() const noexcept override { return w; }
  size_t getHeigt() const noexcept override { return h; }
  float getFps() const noexcept override { return m_fps; }
  INLINE void clear() noexcept override;
  INLINE void delay(size_t ms) const noexcept override;
  void present() noexcept override;
  INLINE void draw(const std::string &obj) noexcept override;
  void setup() noexcept override;
  static void getPrimaryDisplayResolution(size_t &w, size_t &h) noexcept {
    auto dmode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    w = dmode->w;
    h = dmode->h;
  }
};
