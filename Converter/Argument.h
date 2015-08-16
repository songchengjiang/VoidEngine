#ifndef _ARGUMENT_
#define _ARGUMENT_
#include <vector>
#include <string>

typedef std::vector< std::pair<std::string, std::vector<std::string>> > ArgumentList;
class Argument
{
public:

	Argument(int argc, char **argv);
	~Argument();

	const ArgumentList& getArgs() { return _args; };

private:
	
	ArgumentList _args;
};
#endif
