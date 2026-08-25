#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>
#include <vector>
#include <istream>

class CommandLine
{
	std::vector<char*> argV;
	std::vector<std::string> argv_str;

public:
	CommandLine(std::istream& in);
	std::vector<char*>& get_arguments();
};
#endif
