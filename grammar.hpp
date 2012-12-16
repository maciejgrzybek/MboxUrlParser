#include <string>
#include <vector>

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/qi_and_predicate.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/fusion/tuple.hpp>

#include "tlds.hpp"

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

        notURL = char_;//(!URL >> char_);

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
              tlds_
            );

        domain =
            (
              +( +(char_("a-zA-Z_\\-")) >> char_(".") ) >>
              tld >> &~(char_("a-zA-Z_\\-"))
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

        start =
            (
                  +(URL | notURL)
//                *(*notURL >> URL [phx::push_back(_val,_1)] >> *notURL)
            )
        ;
    }

    qi::rule<Iterator, std::vector<std::string>()> start;
    qi::rule<Iterator, std::string()> URL;
    qi::rule<Iterator, void()> notURL;
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

    tlds tlds_;
};

