#include "VideoContextSDL.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#include <map>
#include <string>
#include <tuple>
#include <unordered_map>

#include "Math.hpp"
#include <cmath>
#include <fstream>
#undef min
#undef max
#include <algorithm>
#include <limits>

#include "Config.hpp"

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C" {
__declspec(dllexport) size_t AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace {
struct Tile {
    SDL_Texture* texture = nullptr;
    SDL_FRect dest;
};
bool loadImageTilesToGPU(SDL_Renderer* renderer, const std::string &filePath, std::vector<Tile>& tiles, int& imgW, int& imgH) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
		SDL_Log("IMG_Load failed");
        return false;
    }

    imgW = surface->w;
    imgH = surface->h;
	
	
    for (int y = 0; y < imgH; y += TILE_SIZE) {
        for (int x = 0; x < imgW; x += TILE_SIZE) {
            SDL_Rect srcRect = {x, y, TILE_SIZE, TILE_SIZE};
            if (x + TILE_SIZE > imgW) srcRect.w = imgW - x;
            if (y + TILE_SIZE > imgH) srcRect.h = imgH - y;
			if (srcRect.w <= 0 || srcRect.h <= 0){
				
				std::cout<<x<<" "<<y<<std::endl;
			}
  
            SDL_Surface* tileSurface = SDL_CreateSurface(srcRect.w, srcRect.h, SDL_PIXELFORMAT_RGBA32 );

            if (!tileSurface) {
				SDL_Log("SDL_CreateSurface failed");
				return false;}

            SDL_BlitSurface(surface, &srcRect, tileSurface, nullptr);
            tiles.push_back({SDL_CreateTextureFromSurface(renderer, tileSurface),  {(float)srcRect.x,(float)srcRect.y,(float)srcRect.w,(float)srcRect.h }});
            SDL_DestroySurface(tileSurface);
	}
    }
	
    SDL_DestroySurface(surface);
    return true;
}

void drawTexture(SDL_Renderer *rend, const std::string &path, const double camX, const double camY,
                 const size_t w, const size_t h, const double scale){
					 static int imgW{};
					 static int imgH{};
					 static std::map<std::string, std::vector<Tile>> mCache;
					 
					 if(!imgW){
						 loadImageTilesToGPU(rend,path,mCache[path],imgW,imgH);
					 }
					 

 for(auto &tile : mCache[path]){
	 SDL_FRect rect = {(float)Math::CalculateScreenPosition((double)tile.dest.x, (double)imgW, camX, (double)w, scale),(float)Math::CalculateScreenPosition((double)tile.dest.y, (double)imgH, camY, (double)h, scale),tile.dest.w,tile.dest.h};

 
	 SDL_RenderTextureRotated(rend, tile.texture, NULL, &rect, 0, nullptr,
                           SDL_FLIP_NONE);
				
 }
}

} // namespaceimgW

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

  SDL_Log("max texture size: %d", getMaxTextureSize());
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

void VideoContextSDL::draw(const std::string &obj) noexcept {
  drawTexture(rend, obj, cameraPosition.x, cameraPosition.y, w, h,
              (double)m_scale);
}

size_t VideoContextSDL::getMaxTextureSize() const noexcept {
  SDL_PropertiesID props = SDL_GetRendererProperties(rend);
  auto size = SDL_GetNumberProperty(
      props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
  return size > 0 ? size : 0;
}
