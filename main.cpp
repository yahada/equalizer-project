#include "wav_header.hpp"
#include "filter.hpp"
int main()
{
  equalizer::WavHeader header;
  std::vector< int16_t > audioData;
  try
  {
    header.readWavFile("file_example_WAV_2MG.wav", audioData);
    header.showInfo(std::cout);
    equalizer::Filter filter(header);
    std::vector< float > newAD = filter.convert(audioData);
    equalizer::LowPass lpL(filter.countAlpha(200));
    equalizer::LowPass lpR(filter.countAlpha(200));

    equalizer::HighPass hpL(filter.countAlpha(4000));
    equalizer::HighPass hpR(filter.countAlpha(4000));


    std::vector<float> lows_(newAD.size());
    std::vector<float> highs_(newAD.size());

    for (size_t i = 0; i < newAD.size(); i += 2)
    {
        lows_[i] = lpL.proccess(newAD[i]);

        lows_[i + 1] = lpR.proccess(newAD[i + 1]);

        highs_[i] = hpL.process(newAD[i]);
        highs_[i + 1] = hpL.process(newAD[i + 1]);
    }

    std::vector< int16_t > lows(audioData.size());
    for (size_t i = 0; i < lows_.size(); ++i) {
      float s = lows_[i];

      if (s > 1.0f) s = 1.0f;
      if (s < -1.0f) s = -1.0f;

      lows[i] = static_cast<int16_t>(s * 32767);
    }

    std::vector< int16_t > highs(audioData.size());
    for (size_t i = 0; i < highs_.size(); ++i) {
      float s = highs_[i];

      if (s > 1.0f) s = 1.0f;
      if (s < -1.0f) s = -1.0f;

      highs[i] = static_cast<int16_t>(s * 32767);
    }


    header.saveWav("low.wav", lows);
    header.saveWav("highs.wav", highs);

  }
  catch(const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
