#ifndef _MODEL_CONVERTER_
#define _MODEL_CONVERTER_
#include <string>
#include "Converter.h"

struct aiNode;
struct aiMesh;
struct aiScene;
class ModelConverter : public Converter
{
public:
	ModelConverter();
	virtual ~ModelConverter();

	virtual void convert(const std::string &filePath, const std::string &outFile) override;

private:

};

#endif