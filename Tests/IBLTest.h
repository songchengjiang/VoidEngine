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
        
        float roughness = 0.0f;
        for (float x = -15.0; x <= 15.0; x += 5) {
            float fresnel = 0.0f;
            for (float y = -15.0; y <= 15.0; y += 5) {
                auto IBLMats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/IBL.vemtl", "IBL"));
                veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere-0"));
                veTransformer *transer = new veTransformer;
                entity->addComponent(transer);
                transer->setPosition(veVec3(x, y, 0.0f));
                transer->setScale(veVec3(2.0f));
                root->addChild(entity);
                
                IBLMats->getMaterial(0)->getTechnique(0)->getPass(0)->getUniform("u_roughness")->setValue(roughness);
                IBLMats->getMaterial(0)->getTechnique(0)->getPass(0)->getUniform("u_fresnel")->setValue(fresnel);
                setRenderableObjectMat(entity, IBLMats->getMaterial(0));
                fresnel += 1.0f / 6.0f;
            }
            roughness += 1.0f / 6.0f;
        }
        

		{
            veNode *lightNode = _sceneManager->createNode("IBNode");
            veIBLight *ibLight = static_cast<veIBLight *>(_sceneManager->createLight(veLight::IB, "ibLight"));
            lightNode->addComponent(ibLight);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			//point->addComponent(new LightUpdater(15.0f, 0.0f));
			ibLight->setIntensity(1.0f);
            
            veTexture *diffLighting = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/bolonga_irradiance.ktx", "DiffLighting"));
            diffLighting->setWrapMode(veTexture::REPEAT);
            diffLighting->setFilterMode(veTexture::LINEAR);
            veTexture *specLighting = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/bolonga_radiance.ktx", "SpecLighting"));
            specLighting->setWrapMode(veTexture::REPEAT);
            specLighting->setFilterMode(veTexture::LINEAR_MIP_MAP_LINEAR);
            ibLight->setDiffuseLightingTexture(diffLighting);
            ibLight->setSpecularLightingTexture(specLighting);
			root->addChild(lightNode);
		}
        
        {
            auto skyBox = _sceneManager->createSkyBox("skybox");
            _sceneManager->setSkyBox(skyBox);
            veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-bolonga.vemtl", "skybox-mats"));
            skyBox->setMaterial(materials->getMaterial(0));
        }

		_sceneManager->getRootNode()->addChild(root);
		_sceneManager->setAmbientColor(veVec3(0.0f));
	};
	~IBLTest() {};

protected:


};

#endif