#pragma once
#include <string>
#include <vector>
struct Coord;

class IVideoContext {
public:
  virtual void setup() noexcept = 0;
  virtual void draw() noexcept = 0;
  virtual void load(const std::string &obj) noexcept = 0;
  virtual void present() noexcept = 0;
  virtual void clear() noexcept = 0;
  virtual void setCamera(const Coord &) noexcept = 0;
  virtual void setScale(float) noexcept = 0;
  virtual void setAngle(double) noexcept = 0;
  virtual size_t getWidth() const noexcept = 0;
  virtual size_t getHeigt() const noexcept = 0;
  virtual float getFps() const noexcept = 0;
  virtual void delay(size_t) const noexcept = 0;
  virtual size_t getMaxTextureSize() const noexcept = 0;
  virtual ~IVideoContext() noexcept = default;
  virtual std::vector<float> getCameraPoints() const noexcept = 0;
};
