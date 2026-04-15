#include "equalizer.hpp"
#include <cstdio>
#include <math.h>

void equalizer::Equalizer::StereoToMono()
{
  std::vector< int16_t > monoData(audioData_.size() / 2);
  for (size_t i = 0; i < audioData_.size(); i += 2)
  {
    int16_t left = audioData_[i];
    int16_t right = audioData_[i + 1];
    int32_t sum = static_cast<int32_t>(left) + right;
    monoData[i / 2] =  sum / 2;
  }
  audioData_ = monoData;
  header_.numChannels_ = 1;
  uint32_t tmpSize = header_.subchunk2Size_ / 2;
  header_.chunkSize_ -= tmpSize;
  header_.subchunk2Size_ /= 2;
  header_.blockAlign_ = header_.numChannels_ * header_.bitsPerSample_ / 8;
  header_.byteRate_ = header_.sampleRate_ * header_.numChannels_ * header_.bitsPerSample_ / 8;
}

void equalizer::Equalizer::openFile(const std::string& filename)
{
  WavHeader header;
  std::vector< int16_t > audioData;
  header.readWavFile(filename, audioData);
  header_ = header;
  audioData_ = audioData;
  changedAudioData_ = audioData_;
}

void equalizer::Equalizer::saveFile(const std::string& filename)
{
  header_.saveWav(filename, changedAudioData_);
}

void equalizer::Equalizer::renameFile(const std::string& oldName, const std::string& newName)
{
  if (oldName.substr(oldName.size() - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Wrong file format");
  }

  if (newName.substr(newName.size() - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Can't save to this file because file has wrong format");
  }

  if (std::rename(oldName.c_str(), newName.c_str()) != 0) {
    throw std::logic_error("no file with this name");
  }
}


void equalizer::Equalizer::showInfoAboutFile(std::ostream& out) const
{
  header_.showInfo(out);
}

std::vector< float > equalizer::Equalizer::convert()
{
    std::vector< float > samples(audioData_.size());
    for (size_t i = 0; i < audioData_.size(); ++i)
    {
      samples[i] = audioData_[i] / 32768.0f;
    }
    return samples;
}

void equalizer::Equalizer::inversion()
{
  std::vector< int16_t > inversedData(audioData_.size());
  for (size_t i = 0; i < audioData_.size(); ++i)
  if (changedAudioData_[i] == -32768)
  {
    inversedData[i] = 32767;
  }
  else
  {
    inversedData[i] = -changedAudioData_[i];
  }
  changedAudioData_ = inversedData;
}

void equalizer::Equalizer::reverse()
{
  std::vector< int16_t > reversedData(audioData_.size());
  for (size_t i = 0; i < audioData_.size(); ++i)
  {
    reversedData[i] = changedAudioData_[changedAudioData_.size() - i - 1];
  }
  changedAudioData_ = reversedData;
}

void equalizer::Equalizer::cutFromLeft(const float& cutSize)
{
  leftCut_ = cutSize;
}

void equalizer::Equalizer::cutFromRight(const float& cutSize)
{
  RightCut_ = cutSize;
}

void equalizer::Equalizer::changeMuteStatus() noexcept
{
  isMuted_ = !isMuted_;
}

void equalizer::Equalizer::changeVolume(const float& lowFreqGain, const float& midFreqGain, const float& highFreqGain)
{
  std::vector< float > convertedData = convert();
  std::vector< int16_t > changedAudioData(audioData_.size());
  equalizer::BiquadFilter lbf(equalizer::FilterTypeEnum::bq_type_lowpass, 200.0 / header_.sampleRate_, 0.707, 0);
  equalizer::BiquadFilter hbf(equalizer::FilterTypeEnum::bq_type_highpass, 3000.0 / header_.sampleRate_, 0.707, 0);
  gainLow_ += lowFreqGain;
  gainMid_ += midFreqGain;
  gainHigh_ += highFreqGain;
  for (size_t i = 0; i < audioData_.size(); ++i)
  {
    float lowFreqSample = lbf.process(convertedData[i]);
    float highFreqSample = hbf.process(convertedData[i]);
    float midFreqSample = (convertedData[i] - lowFreqSample - highFreqSample);

    lowFreqSample *= gainLow_;
    highFreqSample *= gainMid_;
    midFreqSample *= gainHigh_;

    float resSample = (lowFreqSample + highFreqSample + midFreqSample) * 32768.0f;
    changedAudioData[i] = resSample > 32767 ? 32767 : resSample;
  }
  changedAudioData_ = changedAudioData;
}
