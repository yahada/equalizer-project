#include "equalizer.hpp"
#include "filter.hpp"
int main()
{
  equalizer::Equalizer eq;
  eq.openFile("file_example_WAV_1MG.wav");
  eq.changeVolume(0, -1, -1);
  eq.saveFile("new.wav");
}
