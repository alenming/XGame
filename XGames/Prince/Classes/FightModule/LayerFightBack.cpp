#include "LayerFightBack.h"
#include "Utils/ResourceUtils.h"


LayerFightBack::LayerFightBack()
	: mBackImg(nullptr)
	, mGroundImg(nullptr)
	, mGroundFrontImg(nullptr)
	, mGroundBackImg(nullptr)
	, mMask(nullptr)
	, mGroundFrontImg2(nullptr)
	, mFightBgId(-1)
	, mFrontParent(nullptr)
{

}

LayerFightBack::~LayerFightBack()
{

}

LayerFightBack* LayerFightBack::create(int fightBgId, Node* frontParent)
{
	LayerFightBack* layerFightBack = new LayerFightBack();
	if (layerFightBack && layerFightBack->init(fightBgId, frontParent))
	{
		layerFightBack->autorelease();
		return layerFightBack;
	}
	else
	{
		delete layerFightBack;
		layerFightBack = nullptr;
		return nullptr;
	}
}

bool LayerFightBack::init(int fightBgId, Node* frontParent)
{
	if (!Layer::init())
	{
		return false;
	}

	mFrontParent = frontParent;
	Size winSize = Director::getInstance()->getWinSize();

	//±³¾°Í¼
	mBackImg = Sprite::create();
	this->addChild(mBackImg);
	
	//µØ°åÍ¼
	mGroundImg = Sprite::create();
	this->addChild(mGroundImg);

	//µØ°åºó¾°Í¼1
	mGroundBackImg = Sprite::create();
	this->addChild(mGroundBackImg);
	
	//µØ°åÇ°¾°Í¼
	mGroundFrontImg = Sprite::create();
	this->addChild(mGroundFrontImg);

	if (mFrontParent)
	{
		mGroundFrontImg2 = Sprite::create();
		mFrontParent->addChild(mGroundFrontImg2, 999);
	}

	change(fightBgId);

	//ÕÚ¸ÇÃÉ°å
	mMask = Sprite::create("Image/Fight/b.png");
	mMask->setAnchorPoint(Point(0.5f, 0.5f));
	mMask->setPosition(Point(winSize.width / 2.0f, winSize.height / 2.0f));
	mMask->setScale(MASK_START_SCALE);
	mMask->setOpacity(0);
	this->addChild(mMask);

	return true;
}

void LayerFightBack::changeWithFade(int fightBgId, const ChangeCallBack& callBack)
{
	auto fadeOut = FadeOut::create(CHANGE_FADE_DURATION);
	auto changeCallBack = CallFunc::create(CC_CALLBACK_0(LayerFightBack::change, this, fightBgId));
	auto fadeIn = FadeIn::create(CHANGE_FADE_DURATION);
	auto endCallBack = CallFuncN::create(
		[callBack](Node* node)
		{
			node->removeFromParent();
			if (callBack != nullptr)
				callBack();
		}
	);

	auto layerColor = LayerColor::create(Color4B::BLACK);
	layerColor->setOpacity(0.0f);
	this->addChild(layerColor);
	layerColor->runAction(Sequence::create(fadeIn, changeCallBack, fadeOut, endCallBack, nullptr));

}

void LayerFightBack::change(int fightBgId)
{
	mFightBgId = fightBgId;

	Size winSize = Director::getInstance()->getWinSize();
	string path = "";
	Size groundImgSize;

	//±³¾°Í¼
	path = ResourceUtils::getFightBgPath(mFightBgId) + BG_NAME_B2;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		mBackImg->setTexture(path);
		mBackImg->setAnchorPoint(Point(0.5f, 1.0f));
		mBackImg->setPosition(Point(winSize.width / 2, winSize.height + 45));
		mBackImg->setScale(BACK_IMG_START_SCALE);
		mBackImg->setVisible(true);
	}
	else
	{
		mBackImg->setVisible(false);
	}

	//µØ°åÍ¼
	path = ResourceUtils::getFightBgPath(mFightBgId) + BG_NAME_B1;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		mGroundImg->setTexture(path);
		mGroundImg->setAnchorPoint(Point(0.5, 1.0f));
		groundImgSize = mGroundImg->getContentSize();
		mGroundImg->setPosition(Point(winSize.width / 2, groundImgSize.height));
		mGroundImg->setVisible(true);
	}
	else
	{
		mGroundImg->setVisible(false);
	}

	//µØ°åºó¾°Í¼1
	path = ResourceUtils::getFightBgPath(mFightBgId) + BG_NAME_F2;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		mGroundBackImg->setTexture(path);
		mGroundBackImg->setAnchorPoint(Point(0.5f, 0.0f));
		mGroundBackImg->setPosition(winSize.width / 2.0f, groundImgSize.height);
		mGroundBackImg->setVisible(true);
	}
	else
	{
		mGroundBackImg->setVisible(false);
	}

	//µØ°åÇ°¾°Í¼
	path = ResourceUtils::getFightBgPath(mFightBgId) + BG_NAME_F1;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		mGroundFrontImg->setTexture(path);
		mGroundFrontImg->setAnchorPoint(Point(0.5f, 0.0f));
		mGroundFrontImg->setPosition(winSize.width / 2.0f, groundImgSize.height);
		mGroundFrontImg->setVisible(true);
	}
	else
	{
		mGroundFrontImg->setVisible(false);
	}

	//×îÇ°¾°
	path = ResourceUtils::getFightBgPath(mFightBgId) + BG_NAME_F3;
	if (mGroundFrontImg2 && FileUtils::getInstance()->isFileExist(path))
	{
		mGroundFrontImg2->setTexture(path);
		mGroundFrontImg2->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
		mGroundFrontImg2->setPosition(this->getContentSize().width / 2.0f, mGroundFrontImg2->getContentSize().height);
		BlendFunc blendFunc = {GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA}; //±ê×¼ÕýÆ¬µþµ×
		mGroundFrontImg2->setBlendFunc(blendFunc);
		mGroundFrontImg2->setVisible(true);
	}
	else
	{
		if (mGroundFrontImg2)
		{
			mGroundFrontImg2->setVisible(false);
		}
	}
}

void LayerFightBack::onEnter()
{
	Layer::onEnter();
}

void LayerFightBack::onExit()
{
	Layer::onExit();
	this->unscheduleAllSelectors();
}

void LayerFightBack::start3dEffect(const EffectStartCallBack& effectStartCallBack)
{

	//ÃÉ°åÍ¸Ã÷¶È½¥±ä¶¯»­
	mMask->runAction(
		FadeIn::create(EFFECT_DURATION)
	);

	//±³¾°Í¼¶¯»­
	mBackImg->runAction(
		EaseInOut::create(
			Spawn::create(
				RotateBy::create(EFFECT_DURATION, Vec3(BACK_IMG_ROTATE_X, 0, 0)),
				ScaleTo::create(EFFECT_DURATION, BACK_IMG_END_SCALE),
				nullptr
			), EFFECT_EASE_COE
		)
	);

	//µØ°åÇ°¾°Í¼¶¯»­
	mGroundFrontImg->runAction(
		EaseInOut::create(
			RotateBy::create(EFFECT_DURATION, Vec3(GROUND_FRONT_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
		)
	);

	//µØ°å±³¾°Í¼¶¯»­

	if (mGroundBackImg)
	{
		mGroundBackImg->runAction(
			EaseInOut::create(
				Spawn::create(
				//ScaleTo::create(EFFECT_DURATION, GROUND_BACK_IMG_END_SCALE),
				RotateBy::create(EFFECT_DURATION, Vec3(GROUND_BACK_IMG_ROTATE_X, 0, 0)),
				nullptr
				), EFFECT_EASE_COE
			)
		);
	}


	//µØ°å¶¯»­
	mGroundImg->runAction(
		EaseInOut::create(
			RotateBy::create(EFFECT_DURATION, Vec3(GROUND_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
		)
	);

	//Ç°¾°¶¯»­
	if (mGroundFrontImg2)
	{
		mGroundFrontImg2->runAction(
			EaseInOut::create(
			RotateBy::create(EFFECT_DURATION, Vec3(GROUND_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
			)
		);
	}

	//»Øµ÷º¯Êý
	this->runAction(
		Sequence::create(
			CCDelayTime::create(EFFECT_DURATION),
			CallFunc::create(effectStartCallBack),
			nullptr
		)
	);
}

void LayerFightBack::return3dEffect(const EffectEndCallBack& effectEndCallBack)
{

	//ÃÉ°åÍ¸Ã÷¶È½¥±ä¶¯»­
	mMask->runAction(
		FadeOut::create(EFFECT_DURATION)
	);

	//±³¾°Í¼¶¯»­
	mBackImg->runAction(
		EaseInOut::create(
			Spawn::create(
				RotateBy::create(EFFECT_DURATION, Vec3(-BACK_IMG_ROTATE_X, 0, 0)),
				ScaleTo::create(EFFECT_DURATION, BACK_IMG_START_SCALE),
				nullptr
			), EFFECT_EASE_COE
		)
	);

	//µØ°åÇ°¾°Í¼¶¯»­
	mGroundFrontImg->runAction(
		EaseInOut::create(
			RotateBy::create(EFFECT_DURATION, Vec3(-GROUND_FRONT_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
		)
	);

	//µØ°å±³¾°Í¼¶¯»­
	if (mGroundBackImg)
	{
		mGroundBackImg->runAction(
			EaseInOut::create(
			Spawn::create(
			ScaleTo::create(EFFECT_DURATION, 1.0f),
			RotateBy::create(EFFECT_DURATION, Vec3(-GROUND_BACK_IMG_ROTATE_X, 0, 0)),
			nullptr
			), EFFECT_EASE_COE
			)
		);
	}

	//µØ°å¶¯»­
	mGroundImg->runAction(
		EaseInOut::create(
			RotateBy::create(EFFECT_DURATION, Vec3(-GROUND_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
		)
	);

	//Ç°¾°¶¯»­
	if (mGroundFrontImg2)
	{
		mGroundFrontImg2->runAction(
			EaseInOut::create(
				RotateBy::create(EFFECT_DURATION, Vec3(-GROUND_IMG_ROTATE_X, 0, 0)), EFFECT_EASE_COE
			)
		);
	}

	this->runAction(
		Sequence::create(
			CCDelayTime::create(EFFECT_DURATION),
			CallFunc::create(effectEndCallBack),
			nullptr
		)
	);
}