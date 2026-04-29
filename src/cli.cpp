#include "cli.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <limits>
#include <filesystem>

using cli = equalizer::cliEqualizer;
void cli::printBanner(std::ostream& out)
{
  out << R"(
███████╗ ██████╗ ██╗   ██╗ █████╗ ██╗     ██╗███████╗███████╗██████╗
██╔════╝██╔═══██╗██║   ██║██╔══██╗██║     ██║╚══███╔╝██╔════╝██╔══██╗
█████╗  ██║   ██║██║   ██║███████║██║     ██║  ███╔╝ █████╗  ██████╔╝
██╔══╝  ██║▄▄ ██║██║   ██║██╔══██║██║     ██║ ███╔╝  ██╔══╝  ██╔══██╗
███████╗╚██████╔╝╚██████╔╝██║  ██║███████╗██║███████╗███████╗██║  ██║
╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝╚══════╝╚══════╝╚═╝  ╚═╝

                        CLI AUDIO EQUALIZER
                   TYPE help TO VIEW ALL COMMANDS

)";
}

void cli::success(std::ostream& out, const std::string& text)
{
  out << "[OK] " << text << '\n';
}

std::string cli::error(const std::string& text)
{
  return "[ERROR] " + text + '\n';
}

std::string cli::warning(const std::string& text)
{
  return "[WARN] " + text + '\n';
}


void cli::load(std::istream& in, std::ostream& out, const std::vector< std::string >& params)
{
  if (params.size() != 2)
  {
    throw std::invalid_argument(cli::warning("Usage: load <file>"));
  }
  std::string name = params[1];
  if (name == fileName)
  {
    return;
  }
  if (!isSaved)
  {
    out << cli::warning("You have unsaved file: " + fileName);
    out << "Do you want to save it (y/n): ";
    std::string ans;
    in >> ans;
    std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
    while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
    {
      out << cli::error("Wrong input");
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes")
    {
      std::string newFileName;
      out << "Enter new name of the file: ";
      in >> newFileName;
      in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      equalizer.saveFile(newFileName);
      cliEqualizer::success(out, "Track saved");
    }
  }
  try
  {
    equalizer.openFile(name);
  }
  catch(const std::exception& e)
  {
    throw std::invalid_argument(cli::error(e.what()));
  }

  fileName = name;
  isSaved = true;
  isLoaded = true;
  cliEqualizer::success(out, "Track loaded");
}

void cli::save(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() == 1)
  {
    equalizer.saveFile(fileName);
  } else
  {
    for (size_t i = 1; i < params.size(); ++i)
    {
      equalizer.saveFile(params[i]);
    }
  }
  isSaved = true;
  cliEqualizer::success(out, "Track saved");
}

void cli::rename(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() != 2)
  {
    throw std::invalid_argument(cli::warning("Usage: rename <file>"));
  }
  std::string name = params[1];
  if (fileName == name)
  {
    return;
  }
  equalizer.renameFile(fileName, name);
  fileName = name;
  cliEqualizer::success(out, "Track renamed");

}

void cli::getInfo(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() == 1)
  {
    out << "Info about file\n";
    out << "name: " << fileName << '\n';
    equalizer.showInfoAboutFile(out);

    return;
  } else
  {
    if (params[1] == "-f")
    {
      std::string tmp = fileName.substr(0, fileName.size() - 4) + "_info.txt";
      std::string res = tmp;
      if (params.size() == 3)
      {
        size_t pos = fileName.find_last_of('/');
        if (pos != std::string::npos)
        {
          res = tmp.substr(pos, tmp.size());
        }
        res = params[2] + res;
      }
      std::ofstream outfile(res);
      if (!outfile)
      {
        throw std::invalid_argument(cli::error("Pathspec did not match any files"));
      }
      outfile << "Info about file\n";
      outfile << "name: " << fileName << '\n';
      equalizer.showInfoAboutFile(outfile);
      cli::success(out, "Saved to file: " + res);
      outfile.close();
      return;
    }
    else
    {
      throw std::invalid_argument(cli::warning("Unknown option: " + params[1]));
    }
  }
}

void cli::mute(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() > 1)
  {
    throw std::invalid_argument(cli::error("Usage: mute <no-parameters>"));
  }
  if (!equalizer.isMuted_)
  {
  equalizer.changeMuteStatus();
  isSaved = false;
  }
  cliEqualizer::success(out, "Track muted");
}

void cli::unmute(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() > 1)
  {
    throw std::invalid_argument(cli::error("Usage: unmute <no-parameters>"));
  }

  if (equalizer.isMuted_)
  {
    equalizer.changeMuteStatus();
    isSaved = false;
  }
  cliEqualizer::success(out, "Track unmuted");
}

void cli::reverse(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params.size() > 1)
  {
    throw std::invalid_argument(cli::error("Usage: reverse <no-parameters>"));
  }
  equalizer.reverse();
  isSaved = false;
  cliEqualizer::success(out, "Track reversed");
}

void cli::inverse(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params.size() > 1)
  {
    throw std::invalid_argument(cli::error("Usage: inverse <no-parameters>"));
  }

  equalizer.inversion();
  isSaved = false;
  cliEqualizer::success(out, "Track inversed");
}

void cli::fromStereoToMono(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params.size() > 1)
  {
    throw std::invalid_argument(cli::error("Usage: convert <no-parameters>"));
  }

  if (equalizer.header_.numChannels_ == 1)
  {
    throw std::invalid_argument(cli::warning("Track is already mono"));
  }
  equalizer.StereoToMono();
  isSaved = false;

  cliEqualizer::success(out, "Track was converted to mono");
}

void cli::changeVolume(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params.size() == 1)
  {
    throw std::invalid_argument(cli::warning("No parameters found. Use --help for more information"));
  }

  if (params[1] == "--help")
  {
    out << "VOLUME COMMAND\n";
    out << "Usage:\n";
    out << "  volume <gain> [-l] [-m] [-h]\n";
    out << "\n";
    out << "Arguments:\n";
    out << "  <gain>        Volume change in percent [-200 to 200]\n";
    out << "\n";
    out << "Options:\n";
    out << "  -l            Apply to low frequencies\n";
    out << "  -m            Apply to mid frequencies\n";
    out << "  -h            Apply to high frequencies\n";
    out << "  reset         Reset all gains\n";
    out << "\n";
    out << "Behavior:\n";
    out << "  If no flags are specified, volume is applied to all frequencies.\n";
    return;
  }

  if (params[1] == "reset")
  {
    equalizer.gainLow_ = 1.0;
    equalizer.gainMid_ = 1.0;
    equalizer.gainHigh_ = 1.0;
    return;
  }

  float gain = 0.0;
  try
  {
    gain = std::stof(params[1]);
  }
  catch(...)
  {
    throw std::invalid_argument(cli::error("Unknown parameter: " + params[1]));
  }

  if (gain < -200.0 || gain > 200.0)
  {
    throw std::invalid_argument(cli::warning("Volume gain must be in range [-200, 200]"));
  }

  bool lowsFlag = false;
  bool midFlag = false;
  bool highFlag = false;
  for (size_t i = 2; i < params.size(); ++i)
  {
    if (params[i] != "-h" && params[i] != "-m" && params[i] != "-l")
    {
      throw std::invalid_argument(cli::error("Unknown option: " + params[i]));
    }
    switch(params[i][1])
    {
      case 'h':
        highFlag = true;
        break;
      case 'm':
        midFlag = true;
        break;
      case 'l':
        lowsFlag = true;
        break;
    }
  }
  if (!highFlag && !midFlag && !lowsFlag)
  {
    equalizer.changeVolume(gain / 100, gain / 100, gain / 100);
    cliEqualizer::success(out, "Volume updated");
    return;
  }

  float low = 0.0f;
  float mid = 0.0f;
  float high = 0.0f;

  if (lowsFlag)
  {
    low = gain / 100;
  }
  if (midFlag)
  {
    mid = gain / 100;
  }
  if (highFlag)
  {
    high = gain / 100;
  }

  equalizer.changeVolume(low, mid, high);
  isSaved = false;
  cliEqualizer::success(out, "Volume updated");
}

void cli::help(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  out << "AVAILABLE COMMANDS\n";
  out << "load <file>                   - load audio file\n";
  out << "save [-f file]                - save audio\n";
  out << "rename <name>                 - rename file\n";
  out << "info [-f [path]]              - show info export it to the file\n";
  out << "mute / unmute                 - toggle sound\n";
  out << "reverse                       - reverse audio\n";
  out << "inverse                       - invert waveform\n";
  out << "convert                       - convert stereo to mono\n";
  out << "volume <gain> [-l] [-m] [-h]  - change volume\n";
  out << "cut <duration> [-l] [-r]      - edit duration of track\n";
  out << "setting [-f]                  - show settings or export it to the file\n";
  out << "loadSet <file>                - load settings from file\n";
  out << "reset                         - reset all settings\n";
  out << "exit                          - exit program\n";
}

void cli::settings(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }
  if (params.size() == 1)
  {
    out << "Track settings\n";
    equalizer.getSettings(out);
    return;
  } else
  {
    if (params[1] == "-f")
    {
      std::string tmp = fileName.substr(0, fileName.size() - 4) + "_settings.txt";
      std::string res = tmp;
      if (params.size() == 3)
      {
        size_t pos = fileName.find_last_of('/');
        if (pos != std::string::npos)
        {
          res = tmp.substr(pos, tmp.size());
        }
        res = params[2] + res;
      }
      std::ofstream outfile(res);
      if (!outfile)
      {
        throw std::invalid_argument(cli::error("Pathspec did not match any files"));
      }
      outfile << "Track settings\n";
      equalizer.getSettings(outfile);
      cli::success(out, "Saved to file: " + res);
      outfile.close();
      return;
    }
    else
    {
      throw std::invalid_argument(cli::warning("Unknown option: " + params[1]));
    }
  }
}

float cli::extractFloat(const std::string& line)
{
  size_t pos = line.find(':');
  if (pos == std::string::npos)
  {
    return 0.0f;
  }
  std::string numStr = line.substr(pos + 1);
  numStr.erase(std::remove(numStr.begin(), numStr.end(), '%'), numStr.end());
  size_t secPos = numStr.find("sec");
  if (secPos != std::string::npos)
  {
    numStr.erase(secPos, 3);
  }
  try
  {
    return std::stof(numStr);
  }
  catch (...)
  {
    return 0.0f;
  }
}

void cli::getSettings(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params.size() > 2)
  {
    throw std::invalid_argument(cli::warning("Usage: loadSet <file>"));
  }

  std::string filename = params[1];
  std::ifstream infile(filename);
  if (!infile.is_open())
  {
    throw std::invalid_argument(cli::error("Cannot open settings file: " + filename));
  }

  bool isMuted = false;
  float gainLow = 1.0;
  float gainMid = 1.0;
  float gainHigh = 1.0;
  float cutRight = 0.0;
  float cutLeft = 0.0;


  std::string line;
  while (std::getline(infile, line))
  {
    if (line.empty())
    {
      continue;
    }
    if (line.find("Muted status:") != std::string::npos)
    {
      isMuted = (line.find("Muted") != std::string::npos && line.find("Unmuted") == std::string::npos);
    }
    else if (line.find("Low frequencies volume:") != std::string::npos)
    {
      gainLow = extractFloat(line) / 100.0f;
    }
    else if (line.find("Mid frequencies volume:") != std::string::npos)
    {
      gainMid = extractFloat(line) / 100.0f;
    }
    else if (line.find("High frequencies volume:") != std::string::npos)
    {
      gainHigh = extractFloat(line) / 100.0f;
    }
    else if (line.find("Cut from the left:") != std::string::npos)
    {
      cutLeft = extractFloat(line);
    }
    else if (line.find("Cut from the right:") != std::string::npos)
    {
      cutRight = extractFloat(line);
    }
  }
  infile.close();

  equalizer.loadSettings(isMuted, gainLow, gainMid, gainHigh, cutLeft, cutRight);
  cli::success(out, "Settings loaded");
}

void cli::cut(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  if (params[1] == "reset")
  {
    equalizer.rightCut_ = 0.0;
    equalizer.leftCut_ = 0.0;
    return;
  }

  float cutSize = 0;
  try
  {
    cutSize = std::stof(params[1]);
  }
  catch(...)
  {
    throw std::invalid_argument(cli::error("Unknown parametr: " + params[1]));
  }

  bool leftFlag = false;
  bool rightFlag = false;
  for (size_t i = 2; i < params.size(); ++i)
  {
    if (params[i] != "-l" && params[i] != "-r")
    {
      throw std::invalid_argument(cli::error("Unknown option: " + params[i]));
    }
    if (params[i] == "-l")
    {
      leftFlag = true;
    } else
    {
      rightFlag = true;
    }
  }

  float left = 0.0f;
  float right = 0.0f;

  if (!rightFlag && !leftFlag)
  {
    left = cutSize;
    right = cutSize;
  }
  else
  {
    if (leftFlag)  left = cutSize;
    if (rightFlag) right = cutSize;
  }

  equalizer.changeDuration(left, right);
  isSaved = false;
  cli::success(out, "Duration changed");
}


void cli::exit(std::istream& in, std::ostream& out, const std::vector< std::string >&)
{
  if (!isSaved)
  {
    out << cli::warning("You have unsaved file: " + fileName);
    out << "Do you want to save it (y/n): ";
    std::string ans;
    in >> ans;
    std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
    while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
    {
      out << cli::error("Wrong input");
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes")
    {
      std::string newFileName;
      out << "Enter new name of the file: ";
      in >> newFileName;
      in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      equalizer.saveFile(newFileName);
      cliEqualizer::success(out, "Track saved");
    }
  }
}

void cli::reset(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No file loaded. Use 'load <file>' first"));
  }

  equalizer.resetChanges();
  cli::success(out, "The settings have been reseted");
  isSaved = false;
}







