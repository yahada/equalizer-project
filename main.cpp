#include "equalizer.hpp"
int main()
{
  equalizer::Equalizer eq;
  eq.openFile("file_example_WAV_2MG.wav");
  eq.reverse();
  eq.saveFile("inversed_file.wav");
}
