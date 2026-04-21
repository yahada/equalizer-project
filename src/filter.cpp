#include "filter.hpp"
#include <math.h>

equalizer::BiquadFilter::BiquadFilter():
  filterType_(FilterTypeEnum::bq_type_lowpass),
  inputCurr_(1.0),
  inputPrev1_(0.0),
  inputPrev2_(0.0),
  outputPrev1_(0.0),
  outputPrev2_(0.0),
  fc_(0.3),
  q_(0.707),
  peakGain_(0.0),
  tmp1_(0.0),
  tmp2_(0.0)
{
  calcBiquad();
}

equalizer::BiquadFilter::BiquadFilter(const FilterTypeEnum& filterType, const double& fc, const double& q, const double& peakGain):
  filterType_(filterType),
  inputCurr_(1.0),
  inputPrev1_(0.0),
  inputPrev2_(0.0),
  outputPrev1_(0.0),
  outputPrev2_(0.0),
  fc_(fc),
  q_(q),
  peakGain_(peakGain),
  tmp1_(0.0),
  tmp2_(0.0)
{
  calcBiquad();
}

void equalizer::BiquadFilter::setFilterType(const FilterTypeEnum& filterType)
{
  filterType_ = filterType;
  calcBiquad();
}

void equalizer::BiquadFilter::setFc(const double& fc)
{
  fc_ = fc;
  calcBiquad();
}

void equalizer::BiquadFilter::setPeakGain(const double& peakGain)
{
  peakGain_ = peakGain;
  calcBiquad();
}

void equalizer::BiquadFilter::setQ(const double& q)
{
  q_ = q;
  calcBiquad();
}

void equalizer::BiquadFilter::setBiquad(const FilterTypeEnum& filterType, const double& fc, const double& q, const double& peakGain)
{
  filterType_ = filterType;
  fc_ = fc;
  q_ = q;
  peakGain_ = peakGain;
  calcBiquad();
}

float equalizer::BiquadFilter::process(const float& input)
{
  float out = input * inputCurr_ + tmp1_;
  tmp1_ = (input * inputPrev1_) + tmp2_ - (outputPrev1_ * out);
  tmp2_ = (input * inputPrev2_) - (outputPrev2_ * out);
  return out;
}

void equalizer::BiquadFilter::calcBiquad()
{
  double K = tan(M_PI * fc_);
  double norm = 1 / (1 + K / q_ + K * K);
  if (filterType_ == bq_type_lowpass)
  {
    inputCurr_ = K * K * norm;
    inputPrev1_ = 2 * inputCurr_;
  } else if (filterType_ == bq_type_highpass)
  {
    inputCurr_ = 1 * norm;
    inputPrev1_ = -2 * inputCurr_;
  } else
  {
    return;
  }
  inputPrev2_ = inputCurr_;
  outputPrev1_ = 2 * (K * K - 1) * norm;
  outputPrev2_ = (1 - K / q_ + K * K) * norm;
  return;
}