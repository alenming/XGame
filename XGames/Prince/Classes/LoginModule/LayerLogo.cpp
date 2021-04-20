#include "LayerLogo.h"
#include "SceneLogin.h"
#include "SceneLogo.h"
#include "Communication/HttpPublisher.h"
#include "Utils/SystemUtils.h"

using namespace cocos2d;
using namespace std;

const float LOGO_DURATION = 2.0f;
const float LOGO_FADE_DURATION = 1.0f;

LayerLogo::LayerLogo()
{

}

LayerLogo::~LayerLogo()
{

}

bool LayerLogo::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size winSize = Director::getInstance()->getWinSize();

	Sprite* sprite = Sprite::create(SystemUtils::getLogoImg());

	sprite->setPosition(Point(winSize.width/2, winSize.height/2));
	this->addChild(sprite);

	CallFunc* call = CallFunc::create(CC_CALLBACK_0(LayerLogo::finish, this));
	FiniteTimeAction* action = Sequence::create(DelayTime::create(LOGO_DURATION), call, NULL);
	this->runAction(action);

	return true;
}

void LayerLogo::onEnter()
{
	Layer::onEnter();
	HttpPublisher::getInstance()->sendOpenGameMsg();
}

void LayerLogo::finish()
{
	Director::getInstance()->replaceScene(TransitionFade::create(LOGO_FADE_DURATION, SceneLogin::create()));
}
