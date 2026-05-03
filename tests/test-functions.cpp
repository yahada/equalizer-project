#define BOOST_TEST_MODULE FunctionTests
#include <boost/test/included/unit_test.hpp>
#include <string>
#include "../src/equalizer.hpp"
#include <sstream>

bool approximatelyEqual(const std::vector<int16_t>& a, const std::vector<int16_t>& b, int tolerance)
{
  if (a.size() != b.size()) return false;

  for (size_t i = 0; i < a.size(); ++i)
  {
    if (std::abs(a[i] - b[i]) > tolerance)
    {
      return false;
    }
  }
  return true;
}

BOOST_AUTO_TEST_SUITE(FileWorkFunctions)
BOOST_AUTO_TEST_CASE(file_opening_success)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  BOOST_CHECK(eq.numChannels() == 2);
  BOOST_CHECK(eq.sampleRate() == 44100);
  BOOST_CHECK(!eq.processedAudioData().empty());
}

BOOST_AUTO_TEST_CASE(file_opening_fail_no_file)
{
  equalizer::Equalizer eq;
  try
  {
    eq.openFile("failDir/fail.wav");
    BOOST_FAIL("Expected exception not thrown");
  }
  catch(const std::exception& e)
  {
    BOOST_CHECK(std::string(e.what()).find("Cannot open file") != std::string::npos);
  }
}

BOOST_AUTO_TEST_CASE(file_opening_fail_wrong_format)
{
  equalizer::Equalizer eq;
  try
  {
    eq.openFile("failDir/fail.notwav");
    BOOST_FAIL("Expected exception not thrown");
  }
  catch(const std::exception& e)
  {
    BOOST_CHECK(std::string(e.what()).find("Wrong file format") != std::string::npos);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AudioFunctions)
BOOST_AUTO_TEST_CASE(reverse)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  std::vector< int16_t > originalData = eq.processedAudioData();
  eq.reverse();
  std::vector< int16_t > reversedData = eq.processedAudioData();
  std::vector< int16_t > tmpData(reversedData.size());
  for (size_t i = 0; i < reversedData.size(); ++i)
  {
    tmpData[i] = reversedData[reversedData.size() - i - 1];
  }
  BOOST_CHECK(originalData == tmpData);
}

BOOST_AUTO_TEST_CASE(inverse)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  std::vector< int16_t > originalData = eq.processedAudioData();
  eq.inversion();
  std::vector< int16_t > inversedData = eq.processedAudioData();
  std::vector< int16_t > tmpData(inversedData.size());
  for (size_t i = 0; i < inversedData.size(); ++i)
  {
    tmpData[i] = -inversedData[i];
  }
  BOOST_CHECK(originalData == tmpData);
}

BOOST_AUTO_TEST_CASE(StereoToMonoSuccess)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  std::vector< int16_t > originalData = eq.processedAudioData();
  eq.StereoToMono();
  std::vector< int16_t > monoData(originalData.size() / 2);
  for (size_t i = 0; i < originalData.size(); i += 2)
  {
    int16_t left = originalData[i];
    int16_t right = originalData[i + 1];
    int32_t sum = static_cast<int32_t>(left) + right;
    monoData[i / 2] =  sum / 2;
  }
  std::vector< int16_t > changedData = eq.processedAudioData();
  BOOST_CHECK(monoData == changedData);
  BOOST_CHECK(eq.numChannels() == 1);
}

BOOST_AUTO_TEST_CASE(changeVolume)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  std::vector< int16_t > originalData = eq.processedAudioData();
  eq.changeVolume(1.0, -1.0, -1.0);
  std::vector< int16_t > prossedFirst = eq.processedAudioData();
  BOOST_CHECK(prossedFirst != originalData);
  eq.changeVolume(-1.0, 1.0, 1.0);
  auto prossedSecond = eq.processedAudioData();
  BOOST_CHECK(approximatelyEqual(originalData, prossedSecond, 1));
}

BOOST_AUTO_TEST_CASE(resetChanges)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  std::stringstream settings_before_changes;
  eq.getSettings(settings_before_changes);
  eq.cutLeft(1);
  eq.cutRight(1);
  eq.changeMuteStatus();
  std::stringstream settings_after_changes;
  eq.getSettings(settings_after_changes);
  BOOST_CHECK(settings_before_changes.str() != settings_after_changes.str());
  eq.resetChanges();
  std::stringstream settings_after_reset;
  eq.getSettings(settings_after_reset);
  BOOST_CHECK(settings_before_changes.str() == settings_after_reset.str());
}

BOOST_AUTO_TEST_CASE(changeDuration)
{
  equalizer::Equalizer eq;
  eq.openFile("samples/file_example_WAV_1MG.wav");
  float dur = eq.durationSeconds();
  eq.changeDuration(1.0, 1.0);
  BOOST_CHECK_CLOSE(dur - 2.0, eq.durationSeconds(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
