#include "LayerManualSkill.h"
#include "FightModel.h"
#include "CocoStudio.h"
#include "SceneFight.h"
#include "../Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "LayerFighter.h"
#include "Common/Common.h"
#include "LayerFightInfo.h"

using namespace cocos2d;
using namespace cocos2d::ui;


const int SKILL_ITEM_COUNT = 6;


LayerManualSkill::LayerManualSkill()
	: m_pParent(nullptr)
	, mContainerNode(nullptr)
{

}

LayerManualSkill::~LayerManualSkill()
{

}

bool LayerManualSkill::init()
{
	if (!Layer::init())
	{
		return false;
	}

	if (!initUI())
	{
		return false;
	}

	return true; 
}

void LayerManualSkill::onEnter()
{
	Layer::onEnter();
	m_pParent = dynamic_cast<SceneFight*>(this->getParent());

	//与LayerFightInfo进入动画同步
	Point bottomPoint = mContainerNode->getPosition();
	auto bottomAction = EaseSineOut::create(MoveTo::create(0.13f, bottomPoint));
	mContainerNode->setPosition(bottomPoint + Point(0, -150));
	mContainerNode->runAction(bottomAction);
}

void LayerManualSkill::onExit()
{
	Layer::onExit();
}

bool LayerManualSkill::initUI()
{
	initSkill();
	return true;
}

void LayerManualSkill::initSkill()
{

	const int spaceX = 15;
	//所有技能图标的容器
	if (mContainerNode)
	{
		mContainerNode->removeFromParent();
	}
	mContainerNode = Node::create();
	Node* container = mContainerNode;

	mVecManualSkillItem.clear();

	Vector<Fighter*>* pHeroList = FightModel::getInstance()->getHeroList();
	vector<int> vecIndex;

	//统计主动技能数量
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = FightModel::getInstance()->getHeroByIndex(i);
		if (fighter && fighter->getActiveSkill())
		{
			vecIndex.push_back(fighter->getIndex());
		}
	}

	//空占位，技能图标数量保持5个
	if (vecIndex.size() < SKILL_ITEM_COUNT)
	{
		int emptyCount = SKILL_ITEM_COUNT - vecIndex.size();
		for (int i = 0; i < emptyCount; i++)
		{
			vecIndex.push_back(-1);
		}
	}

	if (vecIndex.size() > 0)
	{
		Layout* root = nullptr;
		//主动技能组件初始化
		for (size_t i = 0; i < vecIndex.size(); i++)
		{
			root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
				"Image/UInewfight/UInewfight_manuskill.ExportJson"));
			int fighterIndex = vecIndex[i];
			UIManualSkillItem* manualSkillItem = UIManualSkillItem::create(root);
			manualSkillItem->setPositionX((manualSkillItem->getContentSize().width + spaceX) * i);
			int yy = 0;
			if (i == 0 || i == 5)
			{
				yy = 27;
			}
			else if (i == 1 || i == 4)
			{
				yy = 16;
			}
			else
			{
				yy = 12;
			}
			manualSkillItem->setPositionY(yy);
			manualSkillItem->mFighterIndex = fighterIndex;
			Fighter* fighter = FightModel::getInstance()->getHeroByIndex(fighterIndex);

			if (fighter && fighter->getActiveSkill()) //该技能位不为空
			{
				manualSkillItem->mBtnSkill->setTouchEnabled(true);
				manualSkillItem->mBtnSkill->addTouchEventListener(CC_CALLBACK_2(LayerManualSkill::onBtnSkill, this, manualSkillItem));
				manualSkillItem->mTxtHeroName->setString(STR_UTF8(fighter->getName()));
				setTextAddStroke(manualSkillItem->mTxtHeroName, Color3B(0x82, 0x2b, 0x2b), 3);
				manualSkillItem->mImgDisable->setVisible(false);
				manualSkillItem->mSkillIcon = Sprite::create(
					ResourceUtils::getSmallIconPath(fighter->getActiveSkill()->getSkillResId()));
				manualSkillItem->mSkillIcon->setAnchorPoint(Point(Point::ANCHOR_BOTTOM_LEFT));
				manualSkillItem->mSkillPanel->addChild(manualSkillItem->mSkillIcon);
				manualSkillItem->mSkillId = fighter->getActiveSkill()->getSkillId();

			}
			else //该技能位为空
			{
				manualSkillItem->mTxtHeroName->setVisible(false);
				manualSkillItem->mImgDisable->setVisible(true);
				manualSkillItem->mTxtRedSp->setVisible(false);
				manualSkillItem->mBtnSkill->setTouchEnabled(false);
				manualSkillItem->mTxtSP->setVisible(false);
				manualSkillItem->mTxtRedSp->setVisible(false);
				manualSkillItem->mLightAni->setVisible(false);
				manualSkillItem->mSkillCDAni->setVisible(false);
			}
			container->addChild(manualSkillItem);
			mVecManualSkillItem.push_back(manualSkillItem);
		}

		if (root != nullptr)
		{
			container->setContentSize(Size(root->getContentSize().width * vecIndex.size() + spaceX * (vecIndex.size() - 1),
				root->getContentSize().height));
			container->setAnchorPoint(Point(0.5f, 0.0f));
			Size layerSize = this->getContentSize();
			container->setPosition(Point(layerSize.width / 2, 0));
			this->addChild(container);
		}
	}

	updateUI();
}

void LayerManualSkill::onBtnSkill( Ref* ref, cocos2d::ui::Widget::TouchEventType type, UIManualSkillItem* skillItem)
{
	if (!skillItem)
	{
		return;
	}

	if (Widget::TouchEventType::BEGAN == type)
	{
		if (!mContainerNode->getChildByName("tipSkill"))
		{
			mContainerNode->removeChildByName("tipSkill");
		}
		SkillTips*	 tipSkill = SkillTips::create();
		mContainerNode->addChild(tipSkill,9999,"tipSkill");

		tipSkill->setSkill(skillItem->mSkillId);
		tipSkill->setPosition(skillItem->getPosition() + Point(-60, 170));
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (mContainerNode->getChildByName("tipSkill"))
		{
			mContainerNode->removeChildByName("tipSkill");
		}

		if (skillItem->mSkillEnable)
		{
			Fighter* pFighter = FightModel::getInstance()->getFighter(FightStateMachine::MY_SIDE, skillItem->mFighterIndex);
			if (!pFighter->isAlive())
			{
				return;
			}

			ActiveSkill* pSkill = pFighter->getActiveSkill();
			if (!pSkill || !pSkill->isActiveSkill())
			{
				return;
			}

			pSkill->launchSkill();
			m_pParent->getLayerFightInfo()->updateUI();
			updateUI(true);
			skillItem->mLightAni->setVisible(true);
			skillItem->mLightAni->getAnimation()->play("touch");
			skillItem->setSkillEnable(false);
		}
	}
	else if (Widget::TouchEventType::CANCELED == type)
	{
		if (mContainerNode->getChildByName("tipSkill"))
		{
			mContainerNode->removeChildByName("tipSkill");
		}
	}
}

void LayerManualSkill::updateUI(bool playTouchAni)
{
	for (int i = 0; i < mVecManualSkillItem.size(); ++i)
	{
		UIManualSkillItem* manualSkillItem = mVecManualSkillItem.at(i);

		Fighter* pFighter = FightModel::getInstance()->getFighter(FightStateMachine::MY_SIDE, manualSkillItem->mFighterIndex);
		if (!pFighter) continue;
		
		ActiveSkill* pSkill = pFighter->getActiveSkill();
		if (!pSkill || !pSkill->isActiveSkill())
		{
			CCLOG("ActiveSkill[%d] error in updateUISkill", manualSkillItem->mFighterIndex);
			continue;
		}
		
		bool bIsAlive = pFighter->isAlive();
		int nSP = pSkill->getSpCost();
		
		if (pSkill->isSpEnough())
		{
			manualSkillItem->mTxtSP->setVisible(true);
			manualSkillItem->mTxtSP->setString(TO_STR(nSP));
			manualSkillItem->mTxtRedSp->setVisible(false);
		}
		else
		{
			manualSkillItem->mTxtRedSp->setVisible(true);
			manualSkillItem->mTxtRedSp->setString(TO_STR(nSP));
			manualSkillItem->mTxtSP->setVisible(false);
		}
		
		if (!bIsAlive)
		{
			manualSkillItem->setSkillIconGray(true);
		}
		else
		{
			manualSkillItem->setSkillIconGray(false);
		}

		manualSkillItem->mLastCD = manualSkillItem->mCurrentCD;
		manualSkillItem->mCurrentCD = pSkill->getCurrentCD();

		bool bIsReady = pSkill->isReady();
		bool bIsActive = pSkill->isActived();
		bool noBan = pFighter->getActiveSkill()->onJudgeStart();

		bool isEnable = bIsReady && bIsAlive && noBan;
		//是否显示遮罩
		manualSkillItem->mImgMask->setVisible(!isEnable);
		//技能是否可释放
		manualSkillItem->setSkillEnable(isEnable);
		//暂时隐藏掉边框和高亮边框
		manualSkillItem->mBrightBorder->setVisible(false);//bIsReady && bIsAlive);
		manualSkillItem->mBorder->setVisible(false);
		//高亮动画
		manualSkillItem->mLightAni->setVisible(isEnable);
		//锁定标记
		manualSkillItem->mLockImg->setVisible(!noBan);
		//CD动画
		manualSkillItem->mSkillCDAni->setVisible(bIsAlive);

		if (manualSkillItem->mLastCD != manualSkillItem->mCurrentCD)
		{
			//播放完CD动画，再播放高光动画
			auto aniCallBack = [manualSkillItem](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
			{
				manualSkillItem->mLightAni->setVisible(manualSkillItem->mSkillEnable);

				if (manualSkillItem->mSkillEnable)
				{
					if (!manualSkillItem->mLightAni->getAnimation()->isPlaying())
					{
						manualSkillItem->mLightAni->getAnimation()->play("loop");
					}
				}
			};

			manualSkillItem->playCDAni(aniCallBack);
		}
		else
		{
			manualSkillItem->mLightAni->setVisible(isEnable);

			if (isEnable)
			{
				if (!manualSkillItem->mLightAni->getAnimation()->isPlaying())
				{
					manualSkillItem->mLightAni->getAnimation()->play("loop");
				}
			}
		}

	}
}

UIManualSkillItem::UIManualSkillItem()
{

}

UIManualSkillItem::~UIManualSkillItem()
{

}

UIManualSkillItem* UIManualSkillItem::create(Layout* templet)
{
	UIManualSkillItem* manualSkillItem = new UIManualSkillItem();
	manualSkillItem->setRoot(templet);
	if (manualSkillItem && manualSkillItem->init())
	{
		manualSkillItem->autorelease();
		return manualSkillItem;
	}
	else
	{
		delete manualSkillItem;
		return nullptr;
	}
}

bool UIManualSkillItem::init()
{
	if (!Node::init() || !mRoot)
	{
		return false;
	}

	mLastCD = 0;
	mCurrentCD = 0;

	this->setContentSize(mRoot->getContentSize());
	this->addChild(mRoot);
	mTxtHeroName = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(mRoot, "lab_yingxiongming"));
	mTxtHeroName->setFontName(FONT_FZZHENGHEI);
	mSkillPanel = dynamic_cast<Layout*>(mRoot->getChildByName("Panel_skill"));
	mTxtSP = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(mRoot, "Lab_nuqichongzu"));
	mTxtRedSp = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(mRoot, "Lab_nuqibuzu"));
	mTxtSP->setFontName(FONT_FZZHENGHEI);
	mTxtRedSp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(mTxtSP, Color3B(0x0c, 0x11, 0x31), 2);
	setTextAddStroke(mTxtRedSp, Color3B(0x0c, 0x11, 0x31), 2);
	mImgMask = dynamic_cast<ImageView*>(mRoot->getChildByName("img_zhezhao"));
	mImgDisable = dynamic_cast<ImageView*>(mRoot->getChildByName("img_weikaiqi"));
	mBtnSkill = dynamic_cast<Button*>(mRoot->getChildByName("Button_skill"));
	mBrightBorder = dynamic_cast<ImageView*>(mRoot->getChildByName("img_kedianji"));
	mBorder = dynamic_cast<ImageView*>(mRoot->getChildByName("img_biankuang"));
	mCDPanel = dynamic_cast<Layout*>(mRoot->getChildByName("Panel_CD"));
	mLockImg = dynamic_cast<ImageView*>(mRoot->getChildByName("Image_kongzhi"));
	mLockImg->setVisible(false);
	mBrightBorder->setVisible(false);
	
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniZhudongBtnNum/AniZhudongBtnNum0.png",
		"Image/AniZhudongBtnNum/AniZhudongBtnNum0.plist",
		"Image/AniZhudongBtnNum/AniZhudongBtnNum.ExportJson");
	mSkillCDAni = Armature::create("AniZhudongBtnNum");
	auto num1 = cocostudio::Skin::create("Image/UInewfight/cd_num/" + TO_STR(0) + ".png"); //mLastCD
	mSkillCDAni->getBone("num")->addDisplay(num1, 0);
	auto num2 = cocostudio::Skin::create("Image/UInewfight/cd_num/" + TO_STR(0) + ".png"); //mCurrentCD
	mSkillCDAni->getBone("num_Copy6")->addDisplay(num2, 0);
	mSkillCDAni->setPosition(mCDPanel->getContentSize() / 2);
	mCDPanel->addChild(mSkillCDAni);

	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniZhudongBtnLight/AniZhudongBtnLight0.png",
		"Image/AniZhudongBtnLight/AniZhudongBtnLight0.plist",
		"Image/AniZhudongBtnLight/AniZhudongBtnLight.ExportJson");
	mLightAni = Armature::create("AniZhudongBtnLight");
	mLightAni->setPosition(mBrightBorder->getPosition());
	this->addChild(mLightAni);

	mSkillIcon = nullptr;
	mSkillEnable = false;
	mFighterIndex = -1;

	return true;
}

void UIManualSkillItem::playCDAni(const AniCallBack& callback)
{
	auto num1 = cocostudio::Skin::create("Image/UInewfight/cd_num/" + TO_STR(mLastCD) + ".png"); //mLastCD
	mSkillCDAni->getBone("num")->addDisplay(num1, 0);
	
	auto num2 = cocostudio::Skin::create("Image/UInewfight/cd_num/" + TO_STR(mCurrentCD) + ".png"); //mCurrentCD
	mSkillCDAni->getBone("num_Copy6")->addDisplay(num2, 0);

	mSkillCDAni->getAnimation()->setMovementEventCallFunc(callback);
	mSkillCDAni->getAnimation()->playWithIndex(0);
}

void UIManualSkillItem::setRoot(Layout* root)
{
	mRoot = root;
}

void UIManualSkillItem::setSkillEnable(bool enable)
{
	mSkillEnable = enable;
}

void UIManualSkillItem::setSkillIconGray(bool gray)
{
	GLProgram * p = nullptr;
	if (gray)
	{
		p = GLProgram::createWithByteArrays(
			"attribute vec4 a_position;\
			attribute vec2 a_texCoord;\
			attribute vec4 a_color;\
			varying vec4 v_fragmentColor;\
			varying vec2 v_texCoord;\
			void main()\
			{\
			gl_Position = CC_PMatrix * a_position;\
			v_fragmentColor = a_color;\
			v_texCoord = a_texCoord;\
			}", 
			"varying vec4 v_fragmentColor;\
			varying vec2 v_texCoord;\
			void main()\
			{\
			vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
			float gray = dot(v_orColor.rgb, vec3(0.299, 0.587, 0.114));\
			gl_FragColor = vec4(gray, gray, gray, v_orColor.a);\
			}");	
	}
	else
	{
		p = GLProgram::createWithByteArrays(
			"attribute vec4 a_position;\
			attribute vec2 a_texCoord;\
			attribute vec4 a_color;\
			varying vec4 v_fragmentColor;\
			varying vec2 v_texCoord;\
			void main()\
			{\
			gl_Position = CC_PMatrix * a_position;\
			v_fragmentColor = a_color;\
			v_texCoord = a_texCoord;\
			}", 
			"varying vec4 v_fragmentColor;\
			varying vec2 v_texCoord;\
			void main()\
			{\
			gl_FragColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
			}");			
	}

	if (p != nullptr)
	{
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
		p->link();
		p->updateUniforms();
		mSkillIcon->setGLProgram(p);
	}

}
