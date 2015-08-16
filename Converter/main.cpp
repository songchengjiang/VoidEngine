#include "ConverterFactory.h"
#include "Argument.h"
#include <iostream>

int main(int argc, char **argv)
{
	Argument argument(argc, argv);

	auto args = argument.getArgs();

	for (auto &iter : args) {
		auto modelArgs = iter.second;
		if (iter.first == "-m" && !modelArgs.empty()) {
			auto modelConv = ConverterFactory::instance()->createConverter("MODEL");
			std::string modelFile = modelArgs[0];
			std::string outFile;
			if (1 < modelArgs.size()) outFile = modelArgs[1];
			modelConv->convert(modelFile, outFile);
		}
	}

	return 0;
}