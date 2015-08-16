#include "Argument.h"

Argument::Argument(int argc, char **argv)
{
	char *currentArgName = nullptr;
	std::vector<std::string> args;
	for (int n = 0; n < argc; ++n) {
		char *arg = argv[n];
		if (arg[0] == '-') {
			if (currentArgName != nullptr)
				_args.push_back(std::make_pair(currentArgName, args));
			currentArgName = arg;
			args.clear();
			continue;
		}
		args.push_back(arg);
	}

	if (currentArgName != nullptr)
		_args.push_back(std::make_pair(currentArgName, args));
}

Argument::~Argument()
{

}