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

  class Filter {
  public:
    float countAlpha(float cutoff);


  private:
    WavHeader header;
    std::vector< float > convert(std::vector< int16_t >);
  };
}



#endif