#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

namespace equalizer
{
  struct WavHeader
  {
    char chunkID_[4];
    uint32_t chunkSize_;
    char format_[4];

    char subchunk1ID_[4];
    uint32_t subchunkSize_;
    uint16_t audioFormat_;
    uint16_t numChannels_;
    uint32_t sampleRate_;
    uint32_t byteRate_;
    uint16_t blockAlign_;
    uint16_t bitsPerSample_;

    char subchunk2ID_[4];
    uint32_t subchunk2Size_;
  };

  bool checkCorrectnessOfHeader(const WavHeader& header, std::string& errorMsg)
  {

    if (std::string(header.chunkID_, 4) != "RIFF")
    {
      errorMsg = "Not a valid RIFF file";
      return false;
    }

    if (std::string(header.format_, 4) != "WAVE")
    {
      errorMsg = "Not a valid WAVE file";
      return false;
    }

    if (std::string(header.subchunk1ID_, 3) == "fmt")
    {
      errorMsg = "Missing <<fmt>> sub chunck in input file";
      return false;
    }

    if (std::string(header.subchunk2ID_, 4) == "data")
    {
      errorMsg = "Missing <<data>> sub chunck in input file";
      return false;
    }
  }

  void readWavFile(const std::string& filename, WavHeader& header, std::vector< int16_t >& audioData)
  {
    size_t size = filename.size();
    if (filename.substr(size - 4, 4) != ".wav")
    {
      throw std::invalid_argument("Wrong file format");
    }

    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
      throw std::invalid_argument("File opening trouble");
    }

    file.read(reinterpret_cast< char* >(&header), sizeof(WavHeader));

    std::string errMsg;
    if (!checkCorrectnessOfHeader(header, errMsg))
    {
      throw std::invalid_argument(errMsg);
    }

    audioData.resize(header.subchunk2Size_ / sizeof(int16_t));

    file.read(reinterpret_cast<char*>(audioData.data()), header.subchunk2Size_);
  }

  void saveWav(const std::string& filename, const WavHeader& header, const std::vector< int16_t > audioData)
  {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
      std::cerr << "Problems with creation file" << filename << "\n";
      throw;
    }
    file.write(reinterpret_cast< const char* >(&header), sizeof(WavHeader));
    file.write(reinterpret_cast< const char* >(audioData.data()), audioData.size() * sizeof(int16_t));
  }

  void showInfo(WavHeader& header)
  {
    std::cout << "ChunkID: " << header.chunkID_ << '\n';
    std::cout << "ChunkSize: " << header.chunkSize_ << '\n';
    std::cout << "Format: " << header.format_ << '\n';

    std::cout << "Subchunk1ID: " << header.subchunk1ID_ << '\n';
    std::cout << "Subchunk1Size: " << header.subchunkSize_ << '\n';
    std::cout << "AudioFormat: " << header.audioFormat_ << '\n';
    std::cout << "NumChannels: " << header.numChannels_ << '\n';
    std::cout << "SampleRate: " << header.sampleRate_ << '\n';
    std::cout << "ByteRate: " << header.byteRate_ << '\n';
    std::cout << "BlockAlign: " << header.blockAlign_ << '\n';
    std::cout << "BitsPerSample: " << header.bitsPerSample_ << '\n';

    std::cout << "Subchunk2ID: " << header.subchunk2ID_ << '\n';
    std::cout << "Subchunk2Size: " << header.subchunk2Size_ << '\n';
  }

}

int main()
{
  equalizer::WavHeader header;
  std::vector< int16_t > audioData;
  try
  {
    equalizer::readWavFile("file_example_WAV_1MG.wav", header, audioData);
    equalizer::saveWav("new_file.wav", header, audioData);
  }
  catch(const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
