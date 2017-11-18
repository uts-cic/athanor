#ifndef atanorboost_h
#define atanorboost_h

#ifdef ATANOR_REGEX
#if defined(MAVERICK) || defined(REGEXCPP) || defined(WIN32)
#include <regex>
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;
#else
#include <boost/regex.hpp>
using boost::regex;
using boost::sregex_token_iterator;
using boost::smatch;
using boost::match_results;
using boost::wregex;
using boost::wsregex_token_iterator;
using boost::wsmatch;
#endif
#endif

#if defined(WIN32) || defined(MAVERICK) || defined(REGEXCPP)
#include <unordered_map>
#define hmap std::unordered_map
#else
using namespace boost;
#include <tr1/unordered_map>
#define hmap std::tr1::unordered_map
#endif

#endif
