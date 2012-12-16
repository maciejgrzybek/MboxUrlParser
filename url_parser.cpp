#include <iostream>
#include <algorithm>

#include <boost/program_options.hpp>

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>

#include "grammar.hpp"

namespace po = boost::program_options;

namespace phx = boost::phoenix;

int main(int argc, char* argv[])
{

po::options_description desc("Allowed options");
desc.add_options()
    ("help", "this is what you read")
    ("omit-new-lines", "skips new line characters")
;

po::variables_map vm;
po::store(po::parse_command_line(argc, argv, desc), vm);
po::notify(vm);

if (vm.count("help") > 0)
{
  std::cout << desc << std::endl;
  return 1;
}

bool omit = false;

if (vm.count("omit-new-lines") > 0)
{
  omit = true;
}

size_t line = 0;

unsigned result;
myGrammar<std::string::iterator> grammar;

QTextStream qtin(stdin);
while (!qtin.atEnd())
{
  ++line;
  QString str_in = qtin.readLine();

  if (str_in.isEmpty())
    continue;

  QString decoded = QUrl::fromPercentEncoding(str_in.toUtf8());

/*
  if (omit)
    str_in.erase(std::remove(str_in.begin(), str_in.end(), '\n'), str_in.end());
*/

  std::string str = decoded.toStdString(); // toStdString() strips all non-ASCII chars

  std::string::iterator begin = str.begin();
  std::string::iterator end = str.end();

  std::vector<std::string> p;

  bool r = qi::phrase_parse(begin, end,
                +(grammar.URL | grammar.notURL), // not using grammar.start(), because of inlining
                qi::char_("\"'<>,;"),
                p);

  //bool r = qi::parse(begin, end, grammar, p);
  
  if (r && begin == end)
  {
    for (auto& i : p)
    {
      std::cout << line << " : " << i << std::endl;
    }
  }
  else
  {
    // parsing failed, no action needed
  }
}

return 0;
}

