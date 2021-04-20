#include "PopReward.h"
#include "GuideCenter/GuideManager.h"
#include "LoveModule/LoveHeader.h"


const int goldSysId = 11001;
const int coinSysId = 11002;

PopReward::PopReward(void)
	: m_pRoot(nullptr)
	, m_imgBg(nullptr)
	, _callBackFunc(nullptr)
	, m_nCurIndex(0)
	, m_imgContinue(nullptr)
	, m_nCurPartCount(4)
	, m_bIsEnableContinue(false)
{
	m_vcReward.clear();
}


PopReward::~PopReward(void)
{
}


void PopReward::show( const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc/* = nullptr*/ )
{
	PopReward::create(vcReward, callBackFunc);
}


PopReward* PopReward::create( const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc/* = nullptr*/ )
{
	PopReward* pop = new PopReward;
	if (nullptr != pop && pop->init(vcReward, callBackFunc))
	{
		pop->autorelease();
		return pop;
	}

	CC_SAFE_DELETE(pop);
	return nullptr;
}

void PopReward::onEnter()
{
	LayerColor::onEnter();
	SoundUtils::playSoundEffect("jiangli");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_COMMON_REWARD);
}

bool PopReward::init( const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc/* = nullptr*/ )
{
	if (!LayerColor::init())
	{
		return false;
	}

	m_vcReward = vcReward;
	_callBackFunc = callBackFunc;

	this->setColor(Color3B::BLACK);
	this->setOpacity(210);

	initUI();

	return true;
}

void PopReward::initUI()
{
	// Root;
	m_pRoot = Layout::create();
	m_pRoot->setContentSize(Director::getInstance()->getWinSize());
	m_pRoot->setTouchEnabled(true);
	this->addChild(m_pRoot);

	// 背景图;
	m_imgBg = ImageView::create("Image/UITask/img_tanchu1_di.png");
	m_imgBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_imgBg->setPosition(Vec2(m_pRoot->getContentSize().width/2, m_pRoot->getContentSize().height/2));
	m_pRoot->addChild(m_imgBg);
	m_imgBg->setVisible(false);
	ImageView* imgLineTop = ImageView::create("Image/UITask/img_tanchu1_xian.png");
	imgLineTop->setPosition(Vec2(m_imgBg->getContentSize().width/2.0f,
		m_imgBg->getContentSize().height - 2.0f));
	m_imgBg->addChild(imgLineTop);
	ImageView* imgLineBottom = dynamic_cast<ImageView*>(imgLineTop->clone());
	imgLineBottom->setRotationSkewY(180.0f);
	imgLineBottom->setPosition(Vec2(m_imgBg->getContentSize().width/2.0f, 2.0f));
	m_imgBg->addChild(imgLineBottom);

	// 内容层;
	m_pnlContent = Layout::create();
	m_pnlContent->setContentSize(m_imgBg->getContentSize());
	m_pnlContent->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pnlContent->setPosition(m_imgBg->getPosition());
	m_pRoot->addChild(m_pnlContent);
	m_imgBg->setScale(0.1f);

	// 显示标题光背景;
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");
	Armature* light = Armature::create("Anitupo1");

	Bone*  bone = light->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = light->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_gongxihuode.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_gongxihuode.png");
		bone1->addDisplay(_newSkin1, 0);
	}
	light->getAnimation()->play(aniName2);
	light->setScale(0.6f);
	light->setPosition(m_pRoot->getContentSize().width/2, m_pRoot->getContentSize().height/2+100.0f);
	m_pRoot->addChild(light);

	auto callbackfuc = CallFunc::create([=]()
	{
		// 底图动画;
		m_imgBg->setVisible(true);
		Sequence* seq_bg = Sequence::create(EaseSineOut::create(ScaleTo::create(0.25f, 1.03f)),
		CallFuncN::create([=](Node* pSender)
		{
			// 开始播放物品动画;
			showItemAnimation(pSender, 0);
		}),EaseSineOut::create(ScaleTo::create(0.08f, 1.05f)),ScaleTo::create(0.08f, 1.05f),	
			// 1.05f的缩放，持续5帧;
			ScaleTo::create(0.08f, 1.0f),nullptr);
		m_imgBg->runAction(seq_bg);
	});

	m_pRoot->runAction(callbackfuc);

	Director::getInstance()->getRunningScene()->addChild(this, 98);
	this->setPosition(Director::getInstance()->getRunningScene()->getPosition());
}

void PopReward::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParentAndCleanup(true);
}

void PopReward::showItemAnimation( Node* pSender, int nIndex, bool bAuto /*= true*/ )
{
	if (m_vcReward.empty())
	{
		// 空弹窗也可以正常返回;
		showTouchContinue();
		return;
	}

	m_nCurIndex = nIndex;

	if (nIndex != 0 && (nIndex >= m_vcReward.size() || nIndex%4 == 0) && bAuto)
	{
		// 此处为1组(1-4个)播放结束;
		m_bIsEnableContinue = true;
		showTouchContinue();
		return;
	}

	m_bIsEnableContinue = false;
	if (!bAuto || nIndex == 0)
	{
		m_pnlContent->removeAllChildren();
		m_nCurPartCount = min((int)(m_vcReward.size() - nIndex), 4);
	}

	float fSingleWidth = 104.0f + 42.0f;
	float fStartX = -(fSingleWidth * m_nCurPartCount)/2 + fSingleWidth/2 + m_pnlContent->getContentSize().width/2.0f;
	float fStartY = m_pnlContent->getContentSize().height/2;

	UIToolHeader* _item = nullptr;
	if (m_vcReward.at(nIndex)->getType() == PRT_TOOL)
	{
		_item = UIToolHeader::create(m_vcReward.at(nIndex)->getId());
	}
	else if(m_vcReward.at(nIndex)->getType() == PRT_FRIEND_SHIP)
	{
		RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
		if (friend_ship)
		{
			_item = UIToolHeader::create(friend_ship->getIntData("value"));
		}
	}
	else if(m_vcReward.at(nIndex)->getType() == PRT_GOLD)
	{
		_item = UIToolHeader::create(goldSysId);				//解决没底图的问题
	}
	else if(m_vcReward.at(nIndex)->getType() == PRT_COIN)
	{
		_item = UIToolHeader::create(coinSysId);				//解决没底图的问题
	}
	else
	{
		_item = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_vcReward.at(nIndex)->getResId()));
	}
	_item->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_item->setScale(0.1f);
	if (m_vcReward.at(nIndex)->getId() != -1)
	{
		_item->checkChip(m_vcReward.at(nIndex)->getId());
	}
	_item->setPosition(Vec2(fStartX + nIndex%4 * fSingleWidth, fStartY));
	_item->setNumEx(m_vcReward.at(nIndex)->getCount());
	_item->showLightEff();
	m_pnlContent->addChild(_item);

	Sequence* seq_item_single = Sequence::create(
		EaseSineOut::create(ScaleTo::create(0.08f, 0.75f)),
		CallFuncN::create([=](Node* pSender){
			// 创建名称;
			Text* txtName = Text::create(m_vcReward.at(nIndex)->getName(), FONT_FZZHENGHEI, 24);
			//这个描边方法有问题
			//txtName->enableOutline(Color4B(Color3B(0x79, 0x03, 0x03)), 2);
			setTextAddStroke(txtName, Color3B(0x79, 0x03, 0x03),2);

			txtName->setPosition(Vec2(_item->getPosition().x, _item->getPosition().y - 75.0f));
			m_pnlContent->addChild(txtName);
	}),
		CallFuncN::create([=](Node* pSender){
			// 下一位;
			showItemAnimation(pSender, m_nCurIndex+1);
	}),
		EaseSineOut::create(ScaleTo::create(0.25f, 1.20f)),
		EaseSineOut::create(ScaleTo::create(0.08f, 1.20f)),		// 1.20f的缩放，持续5帧;
		EaseSineIn::create(ScaleTo::create(0.08f, 1.0f)),
		nullptr);
	_item->runAction(seq_item_single);
}

void PopReward::showTouchContinue()
{
	if (m_imgContinue != nullptr)
		return;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	m_imgContinue = ImageView::create("Image/Icon/global/img_dianjipingmu.png");
	m_imgContinue->setPosition(Vec2(m_imgBg->getPosition().x, m_imgBg->getPosition().y - 250.0f));
	m_pRoot->addChild(m_imgContinue, 3);

	ActionInterval* seq = Sequence::create(EaseSineInOut::create(FadeOut::create(1.0f)),
		EaseSineInOut::create(FadeIn::create(0.6f)),
		nullptr);
	m_imgContinue->runAction(RepeatForever::create(seq));

	// 添加触摸响应;
	m_pRoot->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("jiangli");
			if (m_nCurIndex >= m_vcReward.size())
			{
				// callback;
				if (_callBackFunc)
				{
					_callBackFunc(this, PopRewardEventType::EVENT_TYPE_END);
				}

				// 清理内存;
				for_each(m_vcReward.begin(), m_vcReward.end(), [=](PopRewardItem* rewardItem){
					delete rewardItem;
				});
				m_vcReward.clear();
				this->scheduleUpdate();
			}
			else if (m_bIsEnableContinue)
			{
				showItemAnimation((Node*)pSender, m_nCurIndex, false);
			}
		}
	});
}
