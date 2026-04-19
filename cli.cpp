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
    while (ans != "y" || ans != "yes" || ans != "n" || ans != "no" || ans != "")
    {
      out << "<WRONG INPUT>\n";
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes" || ans == "")
    {
      std::string newFileName;
      in >> newFileName;
      equalizer::cliEqualizer::save(in, out, newFileName);
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






