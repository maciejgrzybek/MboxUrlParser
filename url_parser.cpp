#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <boost/program_options.hpp>

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QtCore/QFile>

#include "grammar.hpp"

namespace po = boost::program_options;

namespace phx = boost::phoenix;

int main(int argc, char* argv[])
{

po::options_description desc("Allowed options");
desc.add_options()
    ("help,h", "this what you read")
    ("input-file,i",po::value<std::string>(),"file to read mbox from")
    ("output-file,o",po::value<std::string>(),"file to write URLs to")
;

po::variables_map vm;
po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
po::store(parsed, vm);
po::notify(vm);

std::vector<std::string> unregistered = po::collect_unrecognized(parsed.options, po::include_positional);

if (vm.count("help") > 0 || unregistered.size() > 0)
{
  if (unregistered.size() > 0)
  {
    std::cout << "Unknown option(s): " << std::endl;
    for (auto& i : unregistered)
      std::cout << i << std::endl;

    std::cout << std::endl;
  }
  std::cout << desc << std::endl;
  return 1;
}

QFile* fileIn = NULL;
QFile* fileOut = NULL;
QTextStream* qtin = NULL;
QTextStream* qtout = NULL;
if (vm.count("input-file") > 0)
{
  std::string inputFile = vm["input-file"].as<std::string>();
  fileIn = new QFile(QString(inputFile.c_str()));
  fileIn->open(QIODevice::ReadOnly);
  qtin = new QTextStream(fileIn);
}
else
{
  qtin = new QTextStream(stdin);
}
if (vm.count("output-file") > 0)
{
  std::string outputFile = vm["output-file"].as<std::string>();
  fileOut = new QFile(QString(outputFile.c_str()));
  fileOut->open(QIODevice::WriteOnly); 
  qtout = new QTextStream(fileOut);
}
else
{
  qtout = new QTextStream(stdout);
}

size_t line = 0;

unsigned result;
myGrammar<std::string::iterator> grammar;

while (!qtin->atEnd())
{
  ++line;
  QString str_in = qtin->readLine();

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
      *qtout << line << " : " << QString(i.c_str()) << endl;
    }
  }
  else
  {
    // parsing failed, no action needed
  }
}

delete qtin;
delete fileIn;
delete qtout;
delete fileOut;

return 0;
}

