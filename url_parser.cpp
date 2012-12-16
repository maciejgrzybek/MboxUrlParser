#include <iostream>
#include <algorithm>

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/program_options.hpp>

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>

using namespace boost::spirit;

namespace phx = boost::phoenix;
namespace po = boost::program_options;

template <typename Iterator>
struct myGrammar : qi::grammar<Iterator, std::vector<std::string>()>
{
    myGrammar() : myGrammar::base_type(start)
    {
        using qi::eps;
        using qi::lit;
        using qi::_val;
        using qi::_1;
        using ascii::char_;

        notURL = (!URL >> char_);

        username =
            (
              +((qi::alnum | qi::punct) - char_(":@"))
            );

        password =
            (
              +((qi::alnum | qi::punct) - char_("@"))
            );

        auth_string =
            (
              username >> -(char_(":") >> password) >> char_("@")
            );

        tld =
            (
              lit("info") |
              lit("com") |
              lit("org") |
              lit("net") |
              lit("biz") |
              lit("eu") |
              lit("pl") |
              lit("us") |
              lit("uk") |
              lit("ch") |
              lit("tv") |
              lit("fm")
            );

        domain =
            (
              +( +( char_("a-zA-Z") | char_("-") ) >> char_(".") ) >>
              tld
            );

        port %=
            (
              lit(":") >> uint_
            );

        key   =  +( qi::alnum | qi::punct );
        value =  *( qi::alnum | qi::punct );

        pair =
            (
              key >> -(char_("=") >> value)
            );

        params =
            (
              char_("?") >>
              pair >>
              *(char_("&") >> pair)
            );


        path =
            (
              char_("/") >>
              *(+( char_("a-zA-Z_0-9,.") | char_("-") ) >>
              -char_("/")) >>
              -params
            );

        proto =
            (
              ( qi::string("http") >> -qi::string("s") ) | qi::string("ftp")
            );

        URL =
            (
              -( proto >> qi::string("://") ) >>
              -hold[auth_string] >>
              domain >>
              -port >>
              -path
            );

        start = // eps             [_val = 0] >>
            (
//                *(*(!URL >> omit[notURL]) >> URL)
                *(*notURL >> URL [phx::push_back(_val,_1)] >> *notURL)
            )
        ;
    }

    qi::rule<Iterator, std::vector<std::string>()> start;
    qi::rule<Iterator, std::string()> URL;
    qi::rule<Iterator, std::string()> notURL;
    qi::rule<Iterator, std::string()> separator;
    qi::rule<Iterator, std::string()> username;
    qi::rule<Iterator, std::string()> password;
    qi::rule<Iterator, std::string()> auth_string;
    qi::rule<Iterator, std::string()> tld;
    qi::rule<Iterator, std::string()> domain;
    qi::rule<Iterator, unsigned()> port;
    qi::rule<Iterator, std::string()> proto;
    qi::rule<Iterator, std::string()> path;
    qi::rule<Iterator, std::string()> params;
    qi::rule<Iterator, std::string()> pair;
    qi::rule<Iterator, std::string()> key;
    qi::rule<Iterator, std::string()> value;
};

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

  bool r = qi::parse(begin, end, grammar, p);
  
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

