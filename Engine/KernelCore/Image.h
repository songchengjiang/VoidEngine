#ifndef _VE_IMAGE_
#define _VE_IMAGE_
#include "Prerequisites.h"

class VE_EXPORT veImage
{
public:
	veImage(const std::string &filePath);
	~veImage();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	bool loadFromFile(const std::string &filePath);

private:

};

#endif