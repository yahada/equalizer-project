#ifndef FILTER
#define FILTER
#include "wav_header.hpp"

namespace equalizer {
  enum FilterTypeEnum {
    bq_type_lowpass,
    bq_type_highpass,
  };
  class BiquadFilter {
  public:
    BiquadFilter();
    BiquadFilter(const FilterTypeEnum& filterType, const double& fc, const double& q, const double& peakGain);
    void setFilterType(const FilterTypeEnum& filterType);
    void setQ(const double& q);
    void setFc(const double& fc);
    void setPeakGain(const double&  peakGainDB);
    void setBiquad(const FilterTypeEnum& filterType, const double& fc, const double& q, const double& peakGain);
    float process(const float& input);
  private:
    void calcBiquad();
    FilterTypeEnum filterType_;
    double inputCurr_, inputPrev1_, inputPrev2_, outputPrev1_, outputPrev2_;
    double fc_, q_, peakGain_;
    double tmp1_, tmp2_;
  };
}
#endif