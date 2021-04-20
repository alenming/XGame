#include "LayerTrailReward.h"
#include "TowerModel.h"
#include "DataManager/DataManager.h"


LayerTrailReward::LayerTrailReward(void)
{
	m_vcReward.clear();
	TowerModel::getInstance()->addObserver(this);
}


LayerTrailReward::~LayerTrailReward(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

LayerTrailReward* LayerTrailReward::create()
{
	LayerTrailReward* _layer = new LayerTrailReward;
	if (nullptr != _layer && _layer->init())
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerTrailReward::init()
{
	if (!LayerColor::initWithColor(Color4B(0x0, 0x0, 0x0, 0xC8)))
	{
		return false;
	}

	//m_mapSettlement = mapInfo;

	initUI();
	return true;
}

void LayerTrailReward::onEnter()
{
	Layer::onEnter();

	// 音效;
	//SoundUtils::playSoundEffect("quanping");
}

void LayerTrailReward::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();

	pRoot = Layout::create();
	pRoot->setContentSize(winSize);
	pRoot->setTouchEnabled(true);
	this->addChild(pRoot);

	/*
	ImageView* imgTitle = ImageView::create("Image/UINewtower/Icon/img_biaoti2_di.png");
	imgTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	imgTitle->setPosition(Vec2(pRoot->getContentSize().width/2, pRoot->getContentSize().height));
	pRoot->addChild(imgTitle);
	*/
	/*ImageView* imgTitleText = ImageView::create("Image/UINewtower/Icon/img_huodejiangli.png");
	imgTitleText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	imgTitleText->setPosition(Vec2(pRoot->getContentSize().width/2, pRoot->getContentSize().height - 70.0f));
	pRoot->addChild(imgTitleText);*/

	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anigongxihuode/Anigongxihuode.ExportJson");
	//cocostudio::Armature* light = cocostudio::Armature::create("Anigongxihuode");
	//light->setPosition(Vec2(pRoot->getContentSize().width/2.0f, pRoot->getContentSize().height - 60.0f));
	//light->getAnimation()->play("Animation1", -1, 1);
	//pRoot->addChild(light);
	
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");
	Armature* ani = Armature::create("Anitupo1");

	ani->getAnimation()->play(aniName2);

	Bone*  bone = ani->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = ani->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_gongxihuode.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_gongxihuode.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	//ani->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerHeroAdvance::aniAniCallBack, this, aniName2));
	ani->setPosition(Vec2(pRoot->getContentSize().width/2.0f, pRoot->getContentSize().height/2));
	//ani->setScale(0.8f);
	
	pRoot->addChild(ani);
	



	showTouchContinue();
}

void LayerTrailReward::showTouchContinue()
{
	Size winSize = Director::getInstance()->getWinSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	ImageView* _img = ImageView::create("Image/Icon/global/img_dianjipingmu.png");
	_img->setPosition(Vec2(winSize.width/2, 160.0f));
	this->addChild(_img);

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	_img->runAction(RepeatForever::create(seq));

	// 添加触摸响应;
	pRoot->addTouchEventListener(CC_CALLBACK_2(LayerTrailReward::onLayerTouched, this));

}

void LayerTrailReward::onLayerTouched(Ref* ref, Widget::TouchEventType type)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		this->scheduleUpdate();
	}
}

void LayerTrailReward::update( float delta )
{
	m_pParent->onLayerTrailRewardClose();

	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerTrailReward::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TRAIL_REWARD:
		{
			map<int, int> mapReward = *((map<int, int>*)(param->updateData));
			Size winSize = Director::getInstance()->getWinSize();

			float fSingleLen = 127.0f;
			float fLen = fSingleLen * mapReward.size();
			float fStartX = winSize.width/2 - fLen/2;
			float fStartY = winSize.height/2;
			unsigned int i = 0;
			for (auto iter = mapReward.begin(); iter != mapReward.end(); ++iter, ++i)
			{
				RowData* _row = DataManager::getInstance()->searchToolById(iter->first);
				if (nullptr != _row)
				{
					UIToolHeader* _item = UIToolHeader::create(ResourceUtils::getSmallIconPath(_row->getIntData("resId")));
					if (nullptr != _item)
					{
						_item->checkChip(iter->first);
						_item->setNumEx(iter->second);
						_item->setPosition(Vec2(fStartX + fSingleLen/2 + i * fSingleLen, fStartY));
						pRoot->addChild(_item);
						_item->setVisible(false);
						m_vcReward.push_back(_item);

						// 名字;
						Text* txtName = Text::create(_row->getStringData("itemName").c_str(), FONT_FZZHENGHEI, 22);
						txtName->setColor(Color3B(0xE8, 0xDE, 0xB5));
						setTextAddStroke(txtName, Color3B(0x7D, 0x3F, 0x1C), 2);
						txtName->setPosition(Point(_item->getContentSize().width/2.0f, _item->getContentSize().height/2.0f-80.0f));
						_item->addChild(txtName);
					}
				}
			}
			playItemAnimation(nullptr, 0);
		}
		break;

	default:
		break;
	}
}

void LayerTrailReward::playItemAnimation( Node* pSender, int nIndex )
{
	if (m_vcReward.empty())
		return;

	if (nIndex >= m_vcReward.size())
		return;

	m_vcReward.at(nIndex)->setScale(1.5f);
	m_vcReward.at(nIndex)->setVisible(true);

	// 获得物品飞入;
	Sequence* action = Sequence::create(EaseSineIn::create(ScaleTo::create(0.2f, 1.0f)),
		CallFuncN::create(CC_CALLBACK_1(LayerTrailReward::playItemAnimation, this, nIndex+1)), nullptr);
	m_vcReward.at(nIndex)->runAction(action);
}