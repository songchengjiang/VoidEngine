#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Material.h"
#include "KernelCore/Texture.h"
#include "KernelCore/TextureManager.h"
#include "KernelCore/FrameBufferObject.h"
#include "KernelCore/Camera.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterCAMERA : public veFileReaderWriter
{
public:
	veFileReaderWriterCAMERA()
		: _camera(nullptr)
	{};
	~veFileReaderWriterCAMERA(){};

	virtual void* readFile(const std::string &filePath){
		std::string buffer = veFile::instance()->readFileToBuffer(filePath);
		_doucument.Parse(buffer.c_str());
		readCamera();
		return _camera;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
		return true;
	}

private:

	void readCamera() {
		_camera = new veCamera;
		if (_doucument.HasMember(NAME_KEY.c_str())) {
			_camera->setName(_doucument[NAME_KEY.c_str()].GetString());
		}

		if (_doucument.HasMember(VIEWPORT_KEY.c_str())) {
			const Value &val = _doucument[VIEWPORT_KEY.c_str()];
			readViewport(val);
		}

		if (_doucument.HasMember(PROJECTION_KEY.c_str())) {
			const Value &val = _doucument[PROJECTION_KEY.c_str()];
			readProjection(val);
		}

		if (_doucument.HasMember(LOOKAT_KEY.c_str())) {
			const Value &val = _doucument[LOOKAT_KEY.c_str()];
			readLookat(val);
		}

		if (_doucument.HasMember(FBO_KEY.c_str())) {
			const Value &val = _doucument[FBO_KEY.c_str()];
			readfbo(val);
		}
	}

	void readViewport(const Value &viewportVal) {
		veViewport vp = {viewportVal[0].GetInt(), viewportVal[1].GetInt(), viewportVal[2].GetInt(), viewportVal[3].GetInt()};
		_camera->setViewport(vp);
	}

	void readProjection(const Value &projectionVal) {
		if (projectionVal[TYPE_KEY.c_str()].GetString() == PERSPECTIVE_KEY) {
			float fov = 30.0f;
			float aspRadio = (float)_camera->getViewport().width / (float)_camera->getViewport().height;
			float zNear = 1.0f;
			float zFar = 1000.0f;

			if (projectionVal.HasMember(FOV_KEY.c_str())) {
				fov = projectionVal[FOV_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(ASPECTRATIO_KEY.c_str())) {
				aspRadio = projectionVal[ASPECTRATIO_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(ZNEAR_KEY.c_str())) {
				zNear = projectionVal[ZNEAR_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(ZFAR_KEY.c_str())) {
				zFar = projectionVal[ZFAR_KEY.c_str()].GetDouble();
			}

			_camera->setProjectionMatrixAsPerspective(fov, aspRadio, zNear, zFar);
		}
		else if (projectionVal[TYPE_KEY.c_str()].GetString() == ORTHO_KEY) {
			float aspRadio = (float)_camera->getViewport().width / (float)_camera->getViewport().height;
			float left = -aspRadio;
			float right = aspRadio;
			float bottom = -1.0;
			float top = 1.0;
			float zNear = 1.0f;
			float zFar = 1000.0f;

			if (projectionVal.HasMember(LEFT_KEY.c_str())) {
				left = projectionVal[LEFT_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(RIGHT_KEY.c_str())) {
				right = projectionVal[RIGHT_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(BOTTOM_KEY.c_str())) {
				bottom = projectionVal[BOTTOM_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(TOP_KEY.c_str())) {
				top = projectionVal[TOP_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(ZNEAR_KEY.c_str())) {
				zNear = projectionVal[ZNEAR_KEY.c_str()].GetDouble();
			}

			if (projectionVal.HasMember(ZFAR_KEY.c_str())) {
				zFar = projectionVal[ZFAR_KEY.c_str()].GetDouble();
			}
			_camera->setProjectionMatrixAsOrtho(left, right, bottom, top, zNear, zFar);
		}
	}

	void readLookat(const Value &lookatVal) {
		veVec3 eye(0.0f, 0.0f, 1.0f);
		veVec3 center(0.0f);
		veVec3 up = veVec3::UNIT_Y;
		if (lookatVal.HasMember(EYE_KEY.c_str())) {
			const Value & val = lookatVal[EYE_KEY.c_str()];
			if (val.Size() == 3) {
				eye = veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble());
			}
		}
		if (lookatVal.HasMember(CENTER_KEY.c_str())) {
			const Value & val = lookatVal[CENTER_KEY.c_str()];
			if (val.Size() == 3) {
				center = veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble());
			}
		}
		if (lookatVal.HasMember(UP_KEY.c_str())) {
			const Value & val = lookatVal[UP_KEY.c_str()];
			if (val.Size() == 3) {
				up = veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble());
			}
		}
		_camera->setViewMatrixAslookAt(eye, center, up);
	}

	void readfbo(const Value &fboVal) {
		auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_camera->getName());
		for (unsigned int i = 0; i < fboVal.Size(); ++i) {
			const Value &attachmentVal = fboVal[i];
			GLenum attachment = GL_COLOR_ATTACHMENT0;
			veTexture *texture = nullptr;
			int width = _camera->getViewport().width;
			int height = _camera->getViewport().height;
			GLuint internalFormat = GL_RGBA32F;
			if (attachmentVal.HasMember(ATTACHMENT_KEY.c_str())) {
				attachment = getFrameBufferObjectAttach(attachmentVal[ATTACHMENT_KEY.c_str()].GetString());
			}

			if (attachmentVal.HasMember(SOURCE_KEY.c_str())) {
				std::string name = attachmentVal[SOURCE_KEY.c_str()].GetString();
				texture = veTextureManager::instance()->getOrCreateTexture(name, veTexture::TEXTURE_2D);
			}

			if (!texture) continue;

			if (attachmentVal.HasMember(SOURCE_KEY.c_str())) {
				std::string name = attachmentVal[SOURCE_KEY.c_str()].GetString();
				texture = veTextureManager::instance()->getOrCreateTexture(name, veTexture::TEXTURE_2D);
			}

			if (attachmentVal.HasMember(WIDTH_KEY.c_str())) {
				width = attachmentVal[WIDTH_KEY.c_str()].GetUint();
			}

			if (attachmentVal.HasMember(HEIGHT_KEY.c_str())) {
				height = attachmentVal[HEIGHT_KEY.c_str()].GetUint();
			}

			if (attachmentVal.HasMember(FORMAT_KEY.c_str())) {
				const char* format = attachmentVal[FORMAT_KEY.c_str()].GetString();
				if (strcmp(format, "RGB32F") == 0) {
					internalFormat = GL_RGB32F;
				}
				else if (strcmp(format, "RGBA32F") == 0) {
					internalFormat = GL_RGBA32F;
				}
				else if (strcmp(format, "RGB8") == 0) {
					internalFormat = GL_RGB8;
				}
				else if (strcmp(format, "RGBA8") == 0) {
					internalFormat = GL_RGBA8;
				}
			}

			texture->storage(internalFormat, width, height);
			fbo->attach(attachment, texture);
		}
		_camera->setFrameBufferObject(fbo);
	}


	GLenum getFrameBufferObjectAttach(const char* str) {
		if (strncmp(COLOR_KEY.c_str(), str, COLOR_KEY.size()) == 0) {
			return GL_COLOR_ATTACHMENT0 + atoi(&str[COLOR_KEY.size()]);
		}
		else if (strcmp(DEPTH_KEY.c_str(), str) == 0) {
			return GL_DEPTH_ATTACHMENT;
		}
		else if (strcmp(STENCIAL_KEY.c_str(), str) == 0) {
			return GL_STENCIL_ATTACHMENT;
		}
		return 0;
	}

private:

	Document _doucument;
	veCamera *_camera;
};

VE_READERWRITER_REG("vecamera", veFileReaderWriterCAMERA);