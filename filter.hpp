#ifndef FILTER
#define FILTER
#include "wav_header.hpp"

namespace equalizer {
  enum {
    bq_type_lowpass = 0,
    bq_type_highpass,
    bq_type_bandpass,
    bq_type_notch,
    bq_type_peak,
    bq_type_lowshelf,
    bq_type_highshelf
  };

  class Biquad {
  public:
    Biquad();
    Biquad(const int& filterType, const double& fc, const double& q, const double& peakGainDB);
    void setType(const int& filterType);
    void setQ(const double& q);
    void setFc(const double& fc);
    void setPeakGain(const double&  peakGainDB);
    void setBiquad(const int& filterType, const double& fc, const double& q, const double& peakGainDB);
    float process(const float& input);
  private:
    void calcBiquad();
    int filterType_;
    double inputCurr, inputPrev1_, inputPrev2_, outputPrev1_, outputPrev2_;
    double Fc_, Q_, peakGain_;
    double tmp1_, tmp2_;
  };
}
#endif