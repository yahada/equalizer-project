#include "filter.hpp"
#include <math.h>

equalizer::BiquadFilter::BiquadFilter():
  filterType_(FilterTypeEnum::bq_type_lowpass),
  inputCurr_(1.0),
  inputPrev1_(0.0),
  inputPrev2_(0.0),
  outputPrev1_(0.0),
  outputPrev2_(0.0),
  fc_(0.5),
  q_(0.707),
  peakGain_(0.0),
  tmp1_(0.0),
  tmp2_(0.0)
{}

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
{}

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

