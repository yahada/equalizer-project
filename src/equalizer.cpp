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

void equalizer::Equalizer::getSettings(std::ostream& out) const
{
  out << "Muted status: " << (isMuted_ ? "Muted" : "Unmuted") << '\n';
  out << "Low frequencies volume: " << gainLow_ * 100 << "%\n";
  out << "Mid frequencies volume: " << gainMid_ * 100 << "%\n";
  out << "High frequencies volume: " << gainHigh_ * 100 << "%\n";
  out << "Cut from the left: " << leftCut_ << "sec\n";
  out << "Cut from the right: " << rightCut_ << "sec\n";
}

void equalizer::Equalizer::loadSettings(bool mutedStatus, float gainLow, float gainMid, float gainHigh, float cutLeft, float cutRight)
{
  isMuted_ = mutedStatus;
  gainLow_ = gainLow;
  gainMid_ = gainMid;
  gainHigh_ = gainHigh;
  leftCut_ = cutLeft;
  rightCut_ = cutRight;
}

void equalizer::Equalizer::openFile(const std::string& filename)
{
  WavHeader header;
  std::vector< int16_t > audioData;
  header.readWavFile(filename, audioData);
  header_ = header;
  audioData_ = audioData;
  changedAudioData_ = audioData_;

  isMuted_ = false;
  leftCut_ = 0.0f;
  rightCut_ = 0.0f;
  gainLow_ = 1.0f;
  gainMid_ = 1.0f;
  gainHigh_ = 1.0f;
}

void equalizer::Equalizer::saveFile(const std::string& filename)
{
  if (isMuted_)
  {
    std::vector< int16_t > tmpAudioData(changedAudioData_.size(), 0);
    header_.saveWav(filename, tmpAudioData);
    return;
  }
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
  rightCut_ = cutSize;
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
  if (gainLow_ > 2.0)
  {
    gainLow_ = 2.0;
  } else if (gainLow_ < 0)
  {
    gainLow_ = 0.0;
  }
  gainMid_ += midFreqGain;
  if (gainMid_ > 2.0)
  {
    gainMid_ = 2.0;
  } else if (gainMid_ < 0)
  {
    gainMid_ = 0.0;
  }
  gainHigh_ += highFreqGain;
  if (gainHigh_ > 2.0)
  {
    gainHigh_ = 2.0;
  } else if (gainHigh_ < 0)
  {
    gainHigh_ = 0.0;
  }
  for (size_t i = 0; i < audioData_.size(); ++i)
  {
    float lowFreqSample = lbf.process(convertedData[i]);
    float highFreqSample = hbf.process(convertedData[i]);
    float midFreqSample = (convertedData[i] - lowFreqSample - highFreqSample);

    lowFreqSample *= gainLow_;
    highFreqSample *= gainHigh_;
    midFreqSample *= gainMid_;

    float resSample = (lowFreqSample + highFreqSample + midFreqSample) * 32768.0f;
    if (resSample > 32767) resSample = 32767;
    if (resSample < -32768) resSample = -32768;

    changedAudioData[i] = static_cast< int16_t >(resSample);
  }
  changedAudioData_ = changedAudioData;
}

void equalizer::Equalizer::reset()
{
  gainLow_ = 1.0;
  gainMid_ = 1.0;
  gainHigh_ = 1.0;

  leftCut_ = 0.0;
  rightCut_ = 0.0;

  isMuted_ = false;
}