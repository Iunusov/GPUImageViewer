#include "SDLUtils.hpp"

#include "Config.hpp"

namespace SDLUtils {
bool loadImageTilesToGPU(SDL_Renderer *renderer, const std::string &filePath,
                         std::vector<Tile> &tiles, int &imgW,
                         int &imgH) noexcept {
  SDL_Surface *surface = IMG_Load(filePath.c_str());
  if (!surface) {
    SDL_Log("IMG_Load failed: %s", filePath.c_str());
    return false;
  }

  imgW = surface->w;
  imgH = surface->h;

  for (int y = 0; y < imgH; y += TILE_SIZE) {
    for (int x = 0; x < imgW; x += TILE_SIZE) {
      SDL_Rect srcRect = {x, y, TILE_SIZE, TILE_SIZE};
      if (x + TILE_SIZE > imgW) {
        srcRect.w = imgW - x;
      }
      if (y + TILE_SIZE > imgH) {
        srcRect.h = imgH - y;
      }
      if (srcRect.w <= 0 || srcRect.h <= 0) {
        continue;
      }

      SDL_Surface *tileSurface =
          SDL_CreateSurface(srcRect.w, srcRect.h, SDL_PIXELFORMAT_RGBA32);

      if (!tileSurface) {
        SDL_Log("SDL_CreateSurface failed: %s", filePath.c_str());
        SDL_DestroySurface(tileSurface);
        SDL_DestroySurface(surface);
        return false;
      }

      SDL_BlitSurface(surface, &srcRect, tileSurface, nullptr);
      Tile tile = {SDL_CreateTextureFromSurface(renderer, tileSurface),
                   {(float)srcRect.x, (float)srcRect.y, (float)srcRect.w,
                    (float)srcRect.h}};
      SDL_DestroySurface(tileSurface);
      tiles.emplace_back(std::move(tile));
    }
  }

  SDL_DestroySurface(surface);
  return true;
}
void getPrimaryDisplayResolution(size_t &w, size_t &h) noexcept {
  auto dmode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
  w = dmode->w;
  h = dmode->h;
}
} // namespace SDLUtils