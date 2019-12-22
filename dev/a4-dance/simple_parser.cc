#include "simple_parser.h"
#include <cctype>
#include <sstream>

SimpleParser::SimpleParser(std::istream *i) {
  in_ = i;
}

bool SimpleParser::Expect(std::string s) {
  SwallowWhitespace();
  if (s.size() == 0) {
    return true;
  }
  std::string swallowed;
  char c;
  in_->get(c);
  swallowed += c;
  while ((*in_) && swallowed.size() < s.size() && swallowed == s.substr(0, swallowed.size())) {
    in_->get(c);
    swallowed += c;
  }
  if (swallowed == s) {
    return true;
  }
  else {
    std::string::reverse_iterator it;
    for (it = swallowed.rbegin(); it != swallowed.rend(); ++it) {
      in_->putback(*it);
    }
    return false;
  }
}

bool SimpleParser::Peek(std::string s) {
  SwallowWhitespace();
  bool retval = false;
  if (s.size() == 0) {
    return true;
  }
  std::string swallowed;
  char c;
  in_->get(c);
  swallowed += c;
  while ((*in_) && swallowed.size() < s.size() && swallowed == s.substr(0, swallowed.size())) {
    in_->get(c);
    swallowed += c;
  }
  if (swallowed == s) {
    retval = true;
  }
  else {
    retval = false;
  }
  std::string::reverse_iterator it;
  for (it = swallowed.rbegin(); it != swallowed.rend(); ++it) {
    in_->putback(*it);
  }
  return retval;
}

void SimpleParser::SwallowWhitespace() {
  char c;
  in_->get(c);
  while ((*in_) && std::isspace(c)) {
    in_->get(c);
  }
  in_->putback(c);
}

void SimpleParser::SwallowLine() {
  std::string placeholder;
  std::getline(*in_, placeholder);
  SwallowWhitespace();
}

bool SimpleParser::ReadFloat(float &f) {
  SwallowWhitespace();
  std::string accum;
  char c;
  in_->get(c);
  while ((*in_) && FloatChar(c)) {
    accum += c;
    in_->get(c);
  }
  in_->putback(c);
  std::stringstream ss(accum);
  ss >> f;
  return true;
}

bool SimpleParser::ReadInt(int &i) {
  SwallowWhitespace();
  std::string accum;
  char c;
  in_->get(c);
  while ((*in_) && IntChar(c)) {
    accum += c;
    in_->get(c);
  }
  in_->putback(c);
  std::stringstream ss(accum);
  ss >> i;
  return true;
  
}

bool SimpleParser::ReadToken(std::string &s) {
  SwallowWhitespace();
  std::string accum;
  char c;
  in_->get(c);
  while ((*in_) && TokenChar(c)) {
    accum += c;
    in_->get(c);
  }
  in_->putback(c);
  s = accum;
  return true;
}

bool SimpleParser::Good() {
  return (*in_).good();
}

bool SimpleParser::ReadLine(std::string &line) {
  getline(*in_, line);
  return true;
}

bool SimpleParser::UpcomingInt() {
  SwallowWhitespace();
  char c;
  in_->get(c);
  in_->putback(c);
  return IntChar(c);
}

bool SimpleParser::FloatChar(char c) {
  return ( c == 'e'
          || (c >= '0' && c <= '9')
          || c == '.'
          || c == '+'
          || c == '-'
          );
}

bool SimpleParser::IntChar(char c) {
  return ( (c >= '0' && c <= '9') || c == '-');
}

bool SimpleParser::TokenChar(char c) {
  return !std::isspace(c);
}
