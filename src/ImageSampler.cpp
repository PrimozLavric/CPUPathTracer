
#include "ImageSampler.hpp"
#include <chrono>
#include <random>

PixelSample::PixelSample(size_t pixelX, size_t pixelY, float sampleX, float sampleY)
  : pixel({pixelX, pixelY}), sample({sampleX, sampleY}) {}

ImageSampler::ImageSampler(size_t width, size_t height) : width_(width), height_(height) {}

std::vector<PixelSample> ImageSampler::generateSamples() const {
  static thread_local std::mt19937 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

  std::vector<PixelSample> samples;
  samples.reserve(width_ * height_);

  // Generate single random sample for each pixel.
  for (size_t i = 0u; i < width_; i++) {
    for (size_t j = 0u; j < height_; j++) {
      samples.emplace_back(i, j, static_cast<float>(i) + distribution(generator),
                           static_cast<float>(j) + distribution(generator));
    }
  }

  return samples;
}
