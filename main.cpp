#include "wav_header.hpp"

int main()
{
  equalizer::WavHeader header;
  std::vector< int16_t > audioData;
  try
  {
    header.readWavFile("file_example_WAV_2MG.wav", audioData);
    header.showInfo(std::cout);
    header.saveWav("new_file.wav", audioData);
  }
  catch(const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
