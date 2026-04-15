#include "equalizer.hpp"
#include "filter.hpp"
int main()
{
  equalizer::Equalizer eq;
  eq.openFile("sample-12s.wav");
  equalizer::BiquadFilter bf(equalizer::FilterTypeEnum::bq_type_lowpass, 200.0 / eq.header_.sampleRate_, 1.0, -3.0);
  for (size_t i = 0; i < eq.audioData_.size(); ++i)
  {
    eq.audioData_[i] = bf.process(eq.audioData_[i]);
  }
  eq.saveFile("new.wav");
}
