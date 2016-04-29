#include "ConverterFactory.h"
#include "Argument.h"
#include <iostream>

int main(int argc, char **argv)
{
	Argument argument(argc, argv);

	auto args = argument.getArgs();

	for (auto &iter : args) {
		auto converter = ConverterFactory::instance()->createConverter(iter.first);
		auto args = iter.second;
		if (converter && !args.empty()) {
			std::string inFile = args[0];
			std::string outFile;
			if (1 < args.size()) outFile = args[1];
			converter->convert(inFile, outFile);
			delete converter;
		}
	}

	return 0;
}