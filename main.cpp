#include "equalizer.hpp"
#include "filter.hpp"
int main()
{
  equalizer::Equalizer eq;
  eq.openFile("file_example_WAV_1MG.wav");
  eq.changeVolume(-1, 0, -1);
  eq.changeVolume(2, 0, 2);



  eq.saveFile("new.wav");
}
