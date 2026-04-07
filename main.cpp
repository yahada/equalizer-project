#include "equalizer.hpp"
int main()
{
  equalizer::Equalizer eq;
  eq.openFile("file_example_WAV_2MG.wav");
  eq.renameFile("bbbb.wav", "aaaa.wav");
}
