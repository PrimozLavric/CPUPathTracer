//
// Created by primoz on 23.7.2019.
//

#ifndef CPUPATHTRACER_IMAGE_SAMPLER_HPP
#define CPUPATHTRACER_IMAGE_SAMPLER_HPP

#include <vector>

struct PixelSample {
  PixelSample(size_t pixelX, size_t pixelY, float x, float y);

  struct {
    size_t x;
    size_t y;
  } pixel;

  struct {
    float x;
    float y;
  } sample;
};

class ImageSampler {
 public:
  ImageSampler(size_t width, size_t height);

  virtual std::vector<PixelSample> generateSamples() const;

  virtual ~ImageSampler() = default;

 private:
  /**
   * Width in pixels.
   */
  size_t width_;

  /**
   * Height in pixels.
   */
  size_t height_;
};

#endif // CPUPATHTRACER_IMAGE_SAMPLER_HPP
