#ifndef SIMPLE_PARSER_H_
#define SIMPLE_PARSER_H_

#include <fstream>
#include <string>

/** Helper class for parsing text input. */
class SimpleParser {
public:
    SimpleParser(std::istream *i);

    bool Expect(std::string s);

    bool Peek(std::string s);

    void SwallowWhitespace();

    void SwallowLine();

    bool ReadFloat(float &f);

    bool ReadInt(int &i);

    bool ReadToken(std::string &s);

    bool Good();

    bool ReadLine(std::string &line);

    bool UpcomingInt();

protected:
    bool FloatChar(char c);

    bool IntChar(char c);

    bool TokenChar(char c);

    std::istream *in_;
};

#endif
