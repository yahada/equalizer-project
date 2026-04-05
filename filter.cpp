#include "filter.hpp"
#include <math.h>
namespace equalizer
{
  Filter::Filter(WavHeader header):
    header_(header)
  {}


  std::vector< float > Filter::convert(std::vector< int16_t > AudioDataRaw)
  {
    std::vector< float > samples(AudioDataRaw.size());
    for (size_t i = 0; i < AudioDataRaw.size(); ++i)
    {
      samples[i] = AudioDataRaw[i] / 32768.0f;
    }
    return samples;
  }

  float Filter::countAlpha(float cutoff)
  {
    float dt = 1.0f / header_.sampleRate_;
    float rc = 1.0f / (2 * M_PI * cutoff);
    return dt / (rc + dt);
  }


  LowPass::LowPass(float alpha):
    alpha_(alpha),
    lastY_(0)
  {}


  float LowPass::proccess(float x)
  {
    float y = (alpha_ * x) + ((1 - alpha_) * lastY_);
    lastY_ = y;
    return y;
  }
}

