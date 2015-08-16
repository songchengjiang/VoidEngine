#ifndef _CONVERTER_FACTORY_
#define _CONVERTER_FACTORY_
#include <string>
#include "Converter.h"

class ConverterFactory
{
public:
	~ConverterFactory();
	static ConverterFactory* instance();

	Converter* createConverter(const std::string &type);

private:

	ConverterFactory();
};

#endif