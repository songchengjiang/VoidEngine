#ifndef _IBL_TEST_
#define _IBL_TEST_
#include "BaseTest.h"

void setRenderableObjectMat(veNode *node, veMaterial *material)
{
    if (0 < node->getRenderableObjectCount()) {
        for (size_t i = 0; i < node->getRenderableObjectCount(); ++i){
            auto ro = node->getRenderableObject(i);
            ro->setMaterial(material);
        }
    }
    for (size_t i = 0; i < node->getChildCount(); ++i){
        veNode *child = node->getChild(i);
        setRenderableObjectMat(child, material);
    }
}

class IBLTest : public BaseTest
{
public:
	IBLTest() {
		veNode *root = _sceneManager->createNode("root");
        
        int nrRows    = 7;
        int nrColumns = 7;
        float spacing = 2.5;
        
        for (int row = 0; row < nrRows; ++row) {
            for (int col = 0; col < nrColumns; ++col) {
                auto IBLMats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/IBL.vemtl", "IBL"));
                veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere-0"));
                veTransformer *transer = new veTransformer;
                entity->addComponent(transer);
                transer->setPosition(veVec3((float)(col - (nrColumns / 2)) * spacing,
                                            (float)(row - (nrRows / 2)) * spacing,
                                            0.0f));
                transer->setScale(veVec3(1.0f));
                root->addChild(entity);
                
                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                float roughness = (float)col / (float)nrColumns;
                roughness = roughness <= 0.0? 0.025: roughness;
                IBLMats->getMaterial(0)->getTechnique(0)->getPass(0)->getUniform("u_roughness")->setValue(roughness);
                IBLMats->getMaterial(0)->getTechnique(0)->getPass(0)->getUniform("u_metallic")->setValue((float)row / (float)nrRows);
                setRenderableObjectMat(entity, IBLMats->getMaterial(0));
            }
        }
        
        {
            veNode *lightNode = _sceneManager->createNode("directionalNode");
            veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
            lightNode->addComponent(directional);
            veTransformer *lightTranser = new veTransformer;
            lightNode->addComponent(lightTranser);
            lightTranser->setPosition(veVec3(0.0f, 0.0f, 10.0f));
            directional->setIntensity(0.5f);
            
            veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
            lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
            lightNode->addChild(lightentity);
            directional->setUseSoftShadow(true);
            directional->setShadowSoftness(0.005f);
            root->addChild(lightNode);
        }
        
        {
            auto skyBox = _sceneManager->createSkyBox("skybox");
            _sceneManager->setSkyBox(skyBox);
            veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-bolonga.vemtl", "skybox-mats"));
            skyBox->setMaterial(materials->getMaterial(0));
        }
        
        {
            veTexture *irradiance = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/bolonga_irradiance.ktx", "irradiance"));
            irradiance->setWrapMode(veTexture::REPEAT);
            irradiance->setFilterMode(veTexture::LINEAR);
            veTexture *radiance = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/bolonga_radiance.ktx", "radiance"));
            radiance->setWrapMode(veTexture::REPEAT);
            radiance->setFilterMode(veTexture::LINEAR_MIP_MAP_LINEAR);
            _camera->setIrradianceTexture(irradiance);
            _camera->setRadianceTexture(radiance);
        }

		_sceneManager->getRootNode()->addChild(root);
		_sceneManager->setAmbientColor(veVec3(0.0f));
	};
	~IBLTest() {};

protected:


};

#endif
