#include <iostream>
#include <boost/spirit/home/qi.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

using namespace boost::spirit;

namespace phx = boost::phoenix;

template <typename Iterator>
struct myGrammar : qi::grammar<Iterator, std::string()>
{
    myGrammar() : myGrammar::base_type(start)
    {
        using qi::eps;
        using qi::lit;
        using qi::_val;
        using qi::_1;
        using ascii::char_;

        notURL = +(!URL >> char_);

        separator = qi::eol | qi::space; // FIXME check this and add correct values

        tld =
            (
              lit("com") | lit("pl") | lit("us") | lit("uk") | lit("ch")
            );

        domain =
            (
              +( +char_("a-zA-Z") >> char_(".") ) >>
              tld
            );

        port %=
            (
              lit(":") >> uint_
            );

        key   =  qi::char_("a-zA-Z_\\-") >> *qi::char_("a-zA-Z_0-9\\-");
        value = +qi::char_("a-zA-Z_0-9\\-@.");

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
              domain >>
              -port >>
              -path
            );

        start = // eps             [_val = 0] >>
            (
                //qi::omit[*char_("a-zA-Z")] >>
                //+int_/*char_("a-zA-Z]")*/ >> url [_val = _1] >> +int_
//                *(*(!URL >> omit[notURL]) >> URL)
                *(*notURL >> URL [_val = _1])
            )
        ;
    }

    qi::rule<Iterator, std::string()> start;
    qi::rule<Iterator, std::string()> URL;
    qi::rule<Iterator, std::string()> notURL;
    qi::rule<Iterator, std::string()> separator;
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
  std::string p;

  bool r = qi::parse(strbegin, str.end(), grammar, p);
/*  bool r = qi::phrase_parse(strbegin, str.end(),
      "(" >> qi::double_ >> ", " >> qi::double_ >> ")",
      qi::char_("a-zA-Z"),
      p
      );*/
  
  if (r && strbegin == str.end())
  {
//    std::cout << "Parsing succeeded!" << std::endl;
    std::cout << line << " : " << p << std::endl;
  }
  else
  {
/*    std::string rest(strbegin,str.end());
    std::cout << "Parsing failed!" << std::endl;
    std::cout << "Stopped at: " << rest << std::endl;*/
  }
}

std::cout << "Bye :)" << std::endl;

return 0;
}

