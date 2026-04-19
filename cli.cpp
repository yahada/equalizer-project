#include "cli.hpp"
#include <algorithm>
#include <cctype>
void equalizer::cliEqualizer::load(std::istream& in, std::ostream& out, const std::string& name)
{
  if (name == fileName)
  {
    return;
  }

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
      equalizer::cliEqualizer::save(in, out, newFileName);
      return;
    }
  }
  equalizer.openFile(name);
  fileName = name;
  isSaved = true;
  isLoaded = true;
  out << "<TRACK WITH NAME " << name << " WAS SUCCESSFULLY LOADED>\n";
}

void equalizer::cliEqualizer::save(std::istream&, std::ostream& out, const std::string& name)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO SAVE. PLEASE LOAD IT FIRST>\n";
    return;
  }
  equalizer.saveFile(name.empty() ? fileName : name);
  isSaved = true;
  out << "<THE TRACK WAS SUCCESSFULLY SAVED>\n";
}

void equalizer::cliEqualizer::rename(std::istream&, std::ostream& out, const std::string& name)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO RENAME. PLEASE LOAD IT FIRST>\n";
    return;
  }

  if (fileName == name)
  {
    return;
  }
  equalizer.renameFile(fileName, name);
  fileName = name;
  out << "<THE FILE WAS SUCCESSFULLY RENAMED>\n";
}

void equalizer::cliEqualizer::getInfo(std::istream&, std::ostream& out)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO SHOW INFO ABOUT. PLEASE LOAD IT FIRST>\n";
    return;
  }
  out << "<INFO ABOUT FILE>\n";
  equalizer.showInfoAboutFile(out);
}

void equalizer::cliEqualizer::mute(std::istream&, std::ostream& out)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO MUTE. PLEASE LOAD IT FIRST>\n";
    return;
  }
  if (!equalizer.getMuteStatus())
  {
  equalizer.changeMuteStatus();
  isSaved = false;

  }
  out << "<THE TRACK WAS SUCCESSFULLY MUTED>\n";

}

void equalizer::cliEqualizer::unmute(std::istream&, std::ostream& out)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO MUTE. PLEASE LOAD IT FIRST>\n";
    return;
  }

  if (equalizer.getMuteStatus())
  {
    equalizer.changeMuteStatus();
    isSaved = false;
  }
  out << "<THE TRACK WAS SUCCESSFULLY UNMUTED>\n";
}

void equalizer::cliEqualizer::reverse(std::istream&, std::ostream& out)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO REVERSE. PLEASE LOAD IT FIRST>\n";
    return;
  }

  equalizer.reverse();
  isSaved = false;
  out << "<THE TRACK WAS SUCCESSFULLY REVERSED>\n";
}

void equalizer::cliEqualizer::inverse(std::istream&, std::ostream& out)
{
  if (!isLoaded)
  {
    out << "<THERE'S NO TRACK TO INVERSED. PLEASE LOAD IT FIRST>\n";
    return;
  }
  equalizer.inversion();
  isSaved = false;
  out << "<THE TRACK WAS SUCCESSFULLY INVERSED>\n";
}

void equalizer::cliEqualizer::exit(std::istream& in, std::ostream& out)
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
      equalizer::cliEqualizer::save(in, out, newFileName);
      return;
    }
  } else
  {
    return;
  }
}









