#include <sstream>
#include "commandline.h"
#include "twainsave_opensource.h"
#include "twainsave_verinfo.h"

CommandLine::CommandLine(std::istream& in)
{
	std::string cmd;
	argv_str.push_back(TWAINSAVE_VERINFO_ORIGINALFILENAME);
	while (std::getline(in, cmd))
	{
		std::string arg;
		std::istringstream iss(cmd);
		while (iss >> arg)
			argv_str.push_back(arg);
	}
}

std::vector<char*>& CommandLine::get_arguments()
{
	argV.clear();
	for (size_t i = 0; i < argv_str.size(); ++i)
		argV.push_back(const_cast<char*>(argv_str[i].c_str()));
	return argV;
}

