#ifndef _CONVERTER_
#define _CONVERTER_
#include <string>

class Converter
{
public:
	Converter();
	virtual ~Converter();

	virtual void convert(const std::string &filePath, const std::string &outFile) = 0;

private:

};

#endif