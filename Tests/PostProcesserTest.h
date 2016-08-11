#ifndef _POST_PROCESSER_TEST_
#define _POST_PROCESSER_TEST_
#include "BaseTest.h"

class PostProcesserTest : public BaseTest
{
public:
	PostProcesserTest() {
		veNode *root = _sceneManager->createNode("root");

		veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(0.3f));
			root->addChild(entity);

			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachNode(entity);
		}

		//{
		//	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/grey.vemtl", "grey-mats"));
		//	auto postProcesser = _sceneManager->createPostProcesser("grey");
		//	postProcesser->setMaterialArray(mats);
        //  _mainViewer->getCamera()->addPostProcesser(postProcesser);
		//}

		//{
		//	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/bloom.vemtl", "bloom-mats"));
		//	auto postProcesser = _sceneManager->createPostProcesser("bloom");
		//	postProcesser->setMaterialArray(mats);
        //  _mainViewer->getCamera()->addPostProcesser(postProcesser);
		//}

		//{
		//	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/oldTV.vemtl", "oldTV-mats"));
		//	auto postProcesser = _sceneManager->createPostProcesser("oldTV");
		//	postProcesser->setMaterialArray(mats);
        //  _mainViewer->getCamera()->addPostProcesser(postProcesser);
		//}

		{
			auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/ASCII.vemtl", "ASCII-mats"));
			auto postProcesser = _sceneManager->createPostProcesser("oldTV");
			postProcesser->setMaterialArray(mats);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
		}

		//{
		//	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/tiling.vemtl", "tiling-mats"));
		//	auto postProcesser = _sceneManager->createPostProcesser("tiling");
		//	postProcesser->setMaterialArray(mats);
        //  _mainViewer->getCamera()->addPostProcesser(postProcesser);
		//}

		_sceneManager->getRootNode()->addChild(root);
	};
	~PostProcesserTest() {};

protected:


};

#endif