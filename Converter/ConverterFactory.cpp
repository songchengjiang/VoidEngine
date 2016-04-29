#include "ConverterFactory.h"
#include "ModelConverter.h"
#include "TextureConverter.h"

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
	if (type == "-m") {
		return new ModelConverter;
	}
	else if (type == "-t") {
		return new TextureConverter;
	}
	return nullptr;
}
