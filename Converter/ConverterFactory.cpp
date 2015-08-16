#include "ConverterFactory.h"
#include "ModelConverter.h"

ConverterFactory::~ConverterFactory()
{

}

ConverterFactory::ConverterFactory()
{

}

ConverterFactory* ConverterFactory::instance()
{
	static ConverterFactory factory;
	return &factory;
}

Converter* ConverterFactory::createConverter(const std::string &type)
{
	if (type == "MODEL") {
		return new ModelConverter;
	}
	return nullptr;
}
