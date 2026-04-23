#include "cli.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
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
  out << "[OK] " << text;
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
  if (params.size() == 1)
  {
    throw std::invalid_argument(cli::error("No file to load"));
  }
  if (params.size() > 2)
  {
    throw std::invalid_argument(cli::warning("Load function takes only one parametr"));
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
      std::vector< std::string > tmpParams = {0, newFileName};
      cli::save(in, out, tmpParams);
      return;
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
  cliEqualizer::success(out, "Track loaded\n");
}

void cli::save(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("No track to save"));
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
  cliEqualizer::success(out, "Track saved\n");

}

void cli::rename(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded || params.size() == 1)
  {
    throw std::invalid_argument(cli::error("Can't find file to rename"));
  }
  if (params.size() > 2)
  {
    throw std::invalid_argument(cli::warning("Rename function takes only one parametr"));
  }
  std::string name = params[1];
  if (fileName == name)
  {
    return;
  }
  equalizer.renameFile(fileName, name);
  fileName = name;
  cliEqualizer::success(out, "Track renamed\n");

}

void cli::getInfo(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find file to get info about"));
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
        throw std::invalid_argument("can't find directory with this path <" + res + ">");
      }
      outfile << "<Info about file\n";
      outfile << "name: " << fileName << '\n';
      equalizer.showInfoAboutFile(outfile);
      cli::success(out, "Saved to file: " + fileName);
      outfile.close();
      return;
    }
    else
    {
      throw std::invalid_argument(cli::warning("Unknown parametr: " + params[1]));
    }
  }
}

void cli::mute(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to mute"));
  }
  if (!equalizer.isMuted_)
  {
  equalizer.changeMuteStatus();
  isSaved = false;
  }
  cliEqualizer::success(out, "Track muted\n");
}

void cli::unmute(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to unmute"));
  }

  if (equalizer.isMuted_)
  {
    equalizer.changeMuteStatus();
    isSaved = false;
  }
  cliEqualizer::success(out, "Track unmuted\n");
}

void cli::reverse(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to reverse"));
  }

  equalizer.reverse();
  isSaved = false;
  cliEqualizer::success(out, "Track reversed\n");
}

void cli::inverse(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to inverse"));
  }
  equalizer.inversion();
  isSaved = false;
  cliEqualizer::success(out, "Track inversed\n");
}

void cli::fromStereoToMono(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to convert"));
  }

  if (equalizer.header_.numChannels_ == 1)
  {
    throw std::invalid_argument(cli::warning("Track is already in mono format"));
  }
  equalizer.StereoToMono();
  isSaved = false;

  cliEqualizer::success(out, "Track was converted to mono\n");
}

void cli::changeVolume(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument(cli::error("Can't find track to change volume"));
  }

  if (params.size() == 1)
  {
    throw std::invalid_argument(cli::warning("No parametrs found. Use --help for more information"));
  }

  if (params[1] == "--help")
  {
    out << R"(
VOLUME COMMAND

Usage:
volume <gain> [-l] [-m] [-h]

Description:
  Changes volume of the track.

Arguments:
  <gain>        Volume change in percent (-200 to 200)

Options:
  -l            Apply to low frequencies
  -m            Apply to mid frequencies
  -h            Apply to high frequencies

Behavior:
  If no flags are specified, volume is applied to all frequencies.

Examples:
  volume 50           Increase overall volume by 50%
  volume -30          Decrease overall volume by 30%
  volume 20 -l        Boost low frequencies
  volume 10 -m -h     Boost mid and high frequencies
)";
    return;
  }

  float gain = 0.0;
  try
  {
    gain = std::stof(params[1]);
  }
  catch(...)
  {
    throw std::invalid_argument(cli::error("First parametr must be integer"));
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
      throw std::invalid_argument(cli::error("Unknown parametr: " + params[i]));
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
    cliEqualizer::success(out, "Track's volume was changed\n");
    return;
  }

  if (lowsFlag)
  {
    equalizer.changeVolume(gain / 100, 0.0, 0.0);
  }
  if (midFlag)
  {
    equalizer.changeVolume(0.0, gain / 100, 0.0);
  }
  if (highFlag)
  {
    equalizer.changeVolume(0.0, 0.0, gain / 100);
  }
    cliEqualizer::success(out, "Track's volume was changed\n");
}

void cli::help(std::istream& in, std::ostream& out, const std::vector< std::string >& params)
{
  out << "AVAILABLE COMMANDS\n";
  out << "load <file>                   - load audio file\n";
  out << "save [-f file]                - save audio\n";
  out << "rename <name>                 - rename file\n";
  out << "info [-f [path]]              - show or export info\n";
  out << "mute / unmute                 - toggle sound\n";
  out << "reverse                       - reverse audio\n";
  out << "inverse                       - invert waveform\n";
  out << "convert                       - convert stereo to mono\n";
  out << "volume <gain> [-l] [-m] [-h]\n    - change volume";
  out << "exit                          - exit program\n";
}


void cli::exit(std::istream& in, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isSaved)
  {
    out << "<YOU HAVE AN UNSAVED TRACK>\n";
    out << "Do you want to save it (y/n): ";
    std::string ans;
    in >> ans;
    std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
    while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
    {
      out << "<WRONG INPUT>\n";
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes")
    {
      std::string newFileName;
      out << "Enter new name of the file: ";
      in >> newFileName;
      std::vector< std::string > tmpParams = {0, newFileName};
      cli::save(in, out, tmpParams);
      return;
    }
  }
}









