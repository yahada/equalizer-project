#ifndef FILTER
#define FILTER
#include "wav_header.hpp"

namespace equalizer {
  class LowPass {
  public:
    LowPass(float alpha);
    float proccess(float x);
  private:
    float alpha_;
    float lastY_;
  };

  class HighPass {
  public:
    HighPass(float alpha):
      lp_(alpha)
    {}

    float process(float x)
    {
      return x - lp_.proccess(x);
    }

  private:
      LowPass lp_;
  };

  class Filter {
  public:
    Filter(WavHeader header);
    float countAlpha(float cutoff);
    std::vector< float > convert(std::vector< int16_t >);
  private:
    WavHeader header_;
  };
}



#endif