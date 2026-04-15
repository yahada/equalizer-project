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

float equalizer::BiquadFilter::process(const float& input)
{
  float out = input * inputCurr_ + tmp1_;
  tmp1_ = (input * inputPrev1_) + tmp2_ - (outputPrev1_ * out);
  tmp2_ = (input * inputPrev2_) - (outputPrev2_ * out);
  return out;
}

void equalizer::BiquadFilter::calcBiquad()
{
  double norm = 0;
  double V = pow(10, fabs(peakGain_) / 20.0);
  double K = tan(M_PI * fc_);
  switch (filterType_)
  {
  case bq_type_lowpass:
    norm = 1 / (1 + K / q_ + K * K);
    inputCurr_ = K * K * norm;
    inputPrev1_ = 2 * inputCurr_;
    inputPrev2_ = inputCurr_;
    outputPrev1_ = 2 * (K * K - 1) * norm;
    outputPrev2_ = (1 - K / q_ + K * K) * norm;
    break;

  case bq_type_highpass:
    norm = 1 / (1 + K / q_ + K * K);
    inputCurr_ = 1 * norm;
    inputPrev1_ = -2 * inputCurr_;
    inputPrev2_ = inputCurr_;
    outputPrev1_ = 2 * (K * K - 1) * norm;
    outputPrev2_ = (1 - K / q_ + K * K) * norm;
    break;

  case bq_type_bandpass:
    norm = 1 / (1 + K / q_ + K * K);
    inputCurr_ = K / q_ * norm;
    inputPrev1_ = 0;
    inputPrev2_ = -inputCurr_;
    outputPrev1_ = 2 * (K * K - 1) * norm;
    outputPrev2_ = (1 - K / q_ + K * K) * norm;
    break;

  case bq_type_notch:
    norm = 1 / (1 + K / q_ + K * K);
    inputCurr_ = (1 + K * K) * norm;
    inputPrev1_ = 2 * (K * K - 1) * norm;
    inputPrev2_ = inputCurr_;
    outputPrev1_ = inputPrev1_;
    outputPrev2_ = (1 - K / q_ + K * K) * norm;
    break;

  case bq_type_peak:
    if (peakGain_ >= 0)
    {
      norm = 1 / (1 + 1 / q_ * K + K * K);
      inputCurr_ = (1 + V / q_ * K + K * K) * norm;
      inputPrev1_ = 2 * (K * K - 1) * norm;
      inputPrev2_ = (1 - V / q_ * K + K * K) * norm;
      outputPrev1_ = inputPrev1_;
      outputPrev2_ = (1 - 1 / q_ * K + K * K) * norm;
    }
    else
    {
      norm = 1 / (1 + V / q_ * K + K * K);
      inputCurr_ = (1 + 1 / q_ * K + K * K) * norm;
      inputPrev1_ = 2 * (K * K - 1) * norm;
      inputPrev2_ = (1 - 1 / q_ * K + K * K) * norm;
      outputPrev1_ = inputPrev1_;
      outputPrev2_ = (1 - V / q_ * K + K * K) * norm;
    }
    break;
  case bq_type_lowshelf:
    if (peakGain_ >= 0)
    {
      norm = 1 / (1 + sqrt(2) * K + K * K);
      inputCurr_ = (1 + sqrt(2 * V) * K + V * K * K) * norm;
      inputPrev1_ = 2 * (V * K * K - 1) * norm;
      inputPrev2_ = (1 - sqrt(2 * V) * K + V * K * K) * norm;
      outputPrev1_ = 2 * (K * K - 1) * norm;
      outputPrev2_ = (1 - sqrt(2) * K + K * K) * norm;
    }
    else
    {
      norm = 1 / (1 + sqrt(2 * V) * K + V * K * K);
      inputCurr_ = (1 + sqrt(2) * K + K * K) * norm;
      inputPrev1_ = 2 * (K * K - 1) * norm;
      inputPrev2_ = (1 - sqrt(2) * K + K * K) * norm;
      outputPrev1_ = 2 * (V * K * K - 1) * norm;
      outputPrev2_ = (1 - sqrt(2 * V) * K + V * K * K) * norm;
    }
    break;
  case bq_type_highshelf:
    if (peakGain_ >= 0)
    {
      norm = 1 / (1 + sqrt(2) * K + K * K);
      inputCurr_ = (V + sqrt(2 * V) * K + K * K) * norm;
      inputPrev1_ = 2 * (K * K - V) * norm;
      inputPrev2_ = (V - sqrt(2 * V) * K + K * K) * norm;
      outputPrev1_ = 2 * (K * K - 1) * norm;
      outputPrev2_ = (1 - sqrt(2) * K + K * K) * norm;
    }
    else
    {
      norm = 1 / (V + sqrt(2 * V) * K + K * K);
      inputCurr_ = (1 + sqrt(2) * K + K * K) * norm;
      inputPrev1_ = 2 * (K * K - 1) * norm;
      inputPrev2_ = (1 - sqrt(2) * K + K * K) * norm;
      outputPrev1_ = 2 * (K * K - V) * norm;
      outputPrev2_ = (V - sqrt(2 * V) * K + K * K) * norm;
    }
    break;
  }
  return;
}