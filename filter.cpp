#include "filter.hpp"
#include <math.h>
namespace equalizer
{
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

  HighPass::HighPass(float alpha):
    lp_(alpha)
  {}

  float HighPass::process(float x)
  {
    return x - lp_.proccess(x);
  }
}

