#include "VideoContextSDL.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <unordered_map>

#include "Math.hpp"
#include <cmath>
#include <fstream>

#include "Config.hpp"

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

IVideoContext *VideoContextSDL::instance = nullptr;

void VideoContextSDL::Create(Window data) noexcept {
  instance = new VideoContextSDL(data);
}

VideoContextSDL::~VideoContextSDL() noexcept {
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void VideoContextSDL::clear() noexcept { SDL_RenderClear(rend); }

void VideoContextSDL::delay(size_t ms) const noexcept {
  int vsync{0};
  SDL_GetRenderVSync(rend, &vsync);
  if (vsync) {
    return;
  }
  SDL_Delay((uint32_t)ms);
}

void VideoContextSDL::present() noexcept { SDL_RenderPresent(rend); }

void VideoContextSDL::setup() noexcept {
  std::string renderer_name;
  std::string vsync_option;
  std::string fps_option;
  std::string antialiasing_option;

  auto renderer_name_file =
      std::ifstream(std::string(SDL_GetBasePath()) + "renderer.txt");

  std::getline(renderer_name_file, renderer_name);
  std::getline(renderer_name_file, vsync_option);
  std::getline(renderer_name_file, fps_option);
  std::getline(renderer_name_file, antialiasing_option);

  if (renderer_name.size() && renderer_name[0] == '#') {
    renderer_name = "";
  }

  SDL_Log(0);
  SDL_Log("-------------------------------------------------");
  SDL_Log("VideoContextSDL::setup()");
  SDL_Log("-------------------------------------------------");
  SDL_Log("%s", SDL_GetRevision());
  SDL_Log(0);
  SDL_Log("Available Drivers:");
  const auto numdrivers{SDL_GetNumRenderDrivers()};
  std::string drivers;

  for (auto i(0); i < numdrivers; ++i) {
    drivers += SDL_GetRenderDriver(i);
    drivers += " ";
  }

  SDL_Log("%s", drivers.c_str());

  if (renderer_name.size()) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, renderer_name.c_str());
  }

  if (vsync_option == "1") {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  }

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    SDL_Log("SDL_Init failed");
    return;
  }
  SDL_Log(0);
  SDL_Log("Current Driver:");
  std::string driver{SDL_GetCurrentVideoDriver()};
  SDL_Log("%s", driver.c_str());

  SDL_PropertiesID props = SDL_CreateProperties();
#ifdef _WIN32
  if (std::string(SDL_GetCurrentVideoDriver()) == "windows") {
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER,
                           mainWindow);
  }
#endif

#ifdef __linux__
  if (std::string(SDL_GetCurrentVideoDriver()) == "x11") {
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER,
                          mainWindow);
  }
#endif
#ifdef __APPLE__
  if (std::string(SDL_GetCurrentVideoDriver()) == "cocoa") {
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER,
                           mainWindow);
  }
#endif
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN,
                         true);

  if (renderer_name.find("opengl") != std::string::npos) {
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
  }

  if (renderer_name == "metal") {
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, true);
  }

  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN,
                         true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);
  SDL_Log(0);
  if ((renderer_name.find("opengl") != std::string::npos) &&
      antialiasing_option == "1") {
    SDL_Log("SDL_GL_MULTISAMPLEBUFFERS=1; SDL_GL_MULTISAMPLESAMPLES=4");
    SDL_Log(0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  }

  win = SDL_CreateWindowWithProperties(props);
  if (win == nullptr) {
    SDL_Log("SDL_CreateWindow failed");
    SDL_Quit();
    return;
  }

  auto dmode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(win));

  m_fps = dmode->refresh_rate;

  rend =
      SDL_CreateRenderer(win, renderer_name.size() ? renderer_name.c_str() : 0);

  if (rend == nullptr) {
    SDL_Log("SDL_CreateRenderer failed");
    SDL_Quit();
    return;
  }

  SDL_Log("max texture size: %ld", getMaxTextureSize());
  if (vsync_option == "1") {
    SDL_SetRenderVSync(rend, SDL_RENDERER_VSYNC_ADAPTIVE);
  }

  SDL_Log(0);
  SDL_Log("Renderer:");
  SDL_Log("%s", SDL_GetRendererName(rend));
  SDL_Log(0);
  SDL_GetCurrentRenderOutputSize(rend, &w, &h);
  SDL_Log("%dx%d", w, h);
  SDL_Log(0);
  SDL_Log("VSync: ");
  int vsync{0};
  SDL_GetRenderVSync(rend, &vsync);

  if (vsync) {
    SDL_Log("on");
  } else {
    SDL_Log("off");
  }

  if (fps_option.size() && !vsync) {
    m_fps = (float)round(std::stoi(fps_option));
  }

  SDL_Log("FPS: %f", m_fps);

  SDL_ShowWindow(win);
  SDL_Log("-------------------------------------------------");
  SDL_Log("VideoContextSDL::setup() finished successfully");
  SDL_Log("-------------------------------------------------");
  SDL_Log(0);
  SDL_Log(0);
}

void VideoContextSDL::load(const std::string &path) noexcept {
  if (!rend) {
    return;
  }
  std::tuple<std::string, int, int, std::vector<SDLUtils::Tile>> result;
  std::get<0>(result) = path;
  if (SDLUtils::loadImageTilesToGPU(rend, std::get<0>(result),
                                    std::get<3>(result), std::get<1>(result),
                                    std::get<2>(result))) {
    mImages.emplace_back(std::move(result));
  }
}

void VideoContextSDL::draw() noexcept {
  // std::vector<std::tuple<std::string,int,int,std::vector<SDLUtils::Tile>>>
  // mImages{};
  double paddingY = 0.0;
  bool first = true;
  for (const auto &image : mImages) {
    const double imageHeight = static_cast<double>(std::get<2>(image));
    double offsetY = first ? 0.0 : paddingY + imageHeight / 2.0;
    for (const auto &tile : std::get<3>(image)) {
      SDL_FRect rect = {(float)Math::CalculateScreenPosition(
                            (double)tile.dest.x, (double)std::get<1>(image),
                            cameraPosition.x, (double)w, m_scale),
                        (float)Math::CalculateScreenPosition(
                            (double)tile.dest.y + offsetY,
                            (double)std::get<2>(image), cameraPosition.y,
                            (double)h, m_scale),
                        tile.dest.w, tile.dest.h};

      // rect.x=0; rect.y=0;
      if (rect.x + rect.w < 0 || rect.x > w / m_scale || rect.y + rect.h < 0 ||
          rect.y > h / m_scale) {
        continue;
      }

      SDL_RenderTextureRotated(rend, tile.texture, NULL, &rect, 0, nullptr,
                               SDL_FLIP_NONE);
    }
    paddingY += (first ? imageHeight / 2.0 : imageHeight) + PADDING_PX;
    first = false;
  }
}

size_t VideoContextSDL::getMaxTextureSize() const noexcept {
  SDL_PropertiesID props = SDL_GetRendererProperties(rend);
  auto size = SDL_GetNumberProperty(
      props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
  return size > 0 ? size : 0;
}
