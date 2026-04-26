#include "equalizer.hpp"
#include <algorithm>
#include <cstdio>
#include <math.h>
#include <stdexcept>

void equalizer::Equalizer::refreshHeaderSizes() noexcept
{
  header_.subchunk2Size_ = static_cast< uint32_t >(changedAudioData_.size() * sizeof(int16_t));
  header_.blockAlign_ = header_.numChannels_ * header_.bitsPerSample_ / 8;
  header_.byteRate_ = header_.sampleRate_ * header_.blockAlign_;
  header_.chunkSize_ = 36 + header_.subchunk2Size_;
}

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
  changedAudioData_ = monoData;
  header_.numChannels_ = 1;
  refreshHeaderSizes();
}

bool equalizer::Equalizer::getUiStatus() const noexcept
{
  return uiStatus;
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

void equalizer::Equalizer::resetChanges()
{
  changedAudioData_ = audioData_;
  isMuted_ = false;
  gainLow_ = 1.0f;
  gainMid_ = 1.0f;
  gainHigh_ = 1.0f;
}

void equalizer::Equalizer::cutSegment(float positionRatio, float durationSeconds)
{
  if (header_.numChannels_ == 0 || header_.sampleRate_ == 0 || changedAudioData_.empty())
  {
    throw std::logic_error("No audio loaded");
  }

  const float clampedRatio = std::max(0.0f, std::min(1.0f, positionRatio));
  const std::size_t channels = header_.numChannels_;
  const std::size_t totalFrames = changedAudioData_.size() / channels;
  if (totalFrames == 0)
  {
    throw std::logic_error("Audio file is empty");
  }

  std::size_t framesToCut = static_cast< std::size_t >(durationSeconds * header_.sampleRate_);
  framesToCut = std::max<std::size_t>(1, std::min<std::size_t>(framesToCut, totalFrames));

  const std::size_t centerFrame = static_cast< std::size_t >(clampedRatio * static_cast<float>(totalFrames - 1));
  std::size_t startFrame = centerFrame > framesToCut / 2 ? centerFrame - framesToCut / 2 : 0;
  if (startFrame + framesToCut > totalFrames)
  {
    startFrame = totalFrames - framesToCut;
  }
  const std::size_t endFrame = startFrame + framesToCut;

  const std::size_t startSample = startFrame * channels;
  const std::size_t endSample = endFrame * channels;

  changedAudioData_.erase(changedAudioData_.begin() + startSample, changedAudioData_.begin() + endSample);
  audioData_ = changedAudioData_;

  gainLow_ = 1.0f;
  gainMid_ = 1.0f;
  gainHigh_ = 1.0f;
  refreshHeaderSizes();
}

void equalizer::Equalizer::trimToRange(float startRatio, float endRatio)
{
  if (header_.numChannels_ == 0 || header_.sampleRate_ == 0 || changedAudioData_.empty())
  {
    throw std::logic_error("No audio loaded");
  }

  const float start = std::max(0.0f, std::min(1.0f, startRatio));
  const float end = std::max(start, std::min(1.0f, endRatio));
  const std::size_t channels = header_.numChannels_;
  const std::size_t totalFrames = changedAudioData_.size() / channels;
  if (totalFrames == 0)
  {
    throw std::logic_error("Audio file is empty");
  }

  std::size_t startFrame = static_cast< std::size_t >(start * static_cast< float >(totalFrames - 1));
  std::size_t endFrame = static_cast< std::size_t >(end * static_cast< float >(totalFrames - 1));
  if (endFrame <= startFrame)
  {
    endFrame = std::min(totalFrames, startFrame + 1);
  }

  const std::size_t startSample = startFrame * channels;
  const std::size_t endSample = std::min(changedAudioData_.size(), endFrame * channels);

  std::vector< int16_t > trimmed(changedAudioData_.begin() + startSample, changedAudioData_.begin() + endSample);
  changedAudioData_ = trimmed;
  audioData_ = changedAudioData_;

  gainLow_ = 1.0f;
  gainMid_ = 1.0f;
  gainHigh_ = 1.0f;
  refreshHeaderSizes();
}

const std::vector< int16_t >& equalizer::Equalizer::processedAudioData() const noexcept
{
  return changedAudioData_;
}

uint32_t equalizer::Equalizer::sampleRate() const noexcept
{
  return header_.sampleRate_;
}

uint16_t equalizer::Equalizer::numChannels() const noexcept
{
  return header_.numChannels_;
}

float equalizer::Equalizer::durationSeconds() const noexcept
{
  if (header_.sampleRate_ == 0 || header_.numChannels_ == 0)
  {
    return 0.0f;
  }
  return static_cast< float >(changedAudioData_.size()) / static_cast< float >(header_.sampleRate_ * header_.numChannels_);
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
    resSample = std::max(-32768.0f, std::min(32767.0f, resSample));
    changedAudioData[i] = static_cast< int16_t >(resSample);
  }
  changedAudioData_ = changedAudioData;
}
