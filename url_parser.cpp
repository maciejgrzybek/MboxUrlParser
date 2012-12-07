#include <iostream>
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>

using namespace boost::spirit;

namespace phx = boost::phoenix;

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
              ( lit("http") >> -lit("s") ) | lit("ftp")
            );

        URL =
            (
              -( proto >> lit("://") ) >>
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

int main(void)
{

size_t line = 0;

std::string str;
unsigned result;
myGrammar<std::string::iterator> grammar;
while (std::getline(std::cin, str))
{
  ++line;

  /*if (str.empty() || str[0] == 'q' || str[0] == 'Q')
    break;*/

  if (str.empty())
    continue;

  std::string::iterator strbegin = str.begin();
  std::vector<std::string> p;

  bool r = qi::parse(strbegin, str.end(), grammar, p);
/*  bool r = qi::phrase_parse(strbegin, str.end(),
      "(" >> qi::double_ >> ", " >> qi::double_ >> ")",
      qi::char_("a-zA-Z"),
      p
      );*/
  
  if (r && strbegin == str.end())
  {
//    std::cout << "Parsing succeeded!" << std::endl;
    for (auto& i : p)
    {
      std::cout << line << " : " << i << std::endl;
    }
  }
  else
  {
/*    std::string rest(strbegin,str.end());
    std::cout << "Parsing failed!" << std::endl;
    std::cout << "Stopped at: " << rest << std::endl;*/
  }
}

return 0;
}

