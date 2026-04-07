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
    HighPass(float alpha);
    float process(float x);
  private:
      LowPass lp_;
  };
}



#endif