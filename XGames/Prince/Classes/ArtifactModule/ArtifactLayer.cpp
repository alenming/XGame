#include "ArtifactLayer.h"
#include "Widget/LayerCommHeadInfo.h"
#include "ArtifactModel.h"
#include "ToolModule/ToolModel.h"
#include "Temp/CustomTips.h"


//神器激活最大条件数
const int MaxCondition = 5;
//神器最多属性数量
const int MaxAttribute = 5;
//激活条件文本宽度最大值
const float MaxDesWidth = 270.0f;
//动态改变激活条件文本的纵坐标
const float DesPosY = 88.0f;

//洗练石ID
const int RefineStoreID = 10008;


ArtifactLayer::ArtifactLayer()
{
	mCurArtifact = eORDER_FIRST;
	mCurRefineMethod = eMETHOD_COMMON;
	m_bIsNeedResetRefine = false;
	m_bIsSaveData = false;
	ArtifactModel::getInstance(true)->addObserver(this);
}

ArtifactLayer::~ArtifactLayer( void )
{
	ArtifactModel::getInstance()->removeObserver(this);
}

bool ArtifactLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	playEnterAni();

	return true;
}

void ArtifactLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		m_pArtifactController.sendGetArtifactMainMsg();
		this->getScheduler()->unschedule("ArtifactLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "ArtifactLayer::sendMsg");
}

void ArtifactLayer::onExit()
{
	Layer::onExit();
}

void ArtifactLayer::update( float dt )
{
	this->unscheduleUpdate();

	Director::getInstance()->replaceScene(MainCityScene::create());
}

void ArtifactLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIArtifacts/UIArtifacts.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");

			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_ARTIFACTS);
	this->addChild(_layerCommHeadInfo);

	Layout* parentPnl = (Layout*)m_ui.pRoot->getChildByName("Pnl_shenqi");
	m_ui.imgNameBg = (ImageView*)parentPnl->getChildByName("Img_Name");

	m_ui.PnlBorn = (Layout*)parentPnl->getChildByName("Pnl_Born");
	m_ui.vecPnlJewel.clear();
	//神器宝石
	for(int i=0; i<MaxCondition; i++)
	{
		Layout* pnlJewel = (Layout*)m_ui.PnlBorn->getChildByName("Pnl_1000" + _TO_STR(i+1));
		m_ui.vecPnlJewel.push_back(pnlJewel);
		//添加宝石和遮罩
		for(int j=0; j<MaxCondition; j++)
		{
			ImageView* jewel = ImageView::create("Image/UIArtifacts/image/img_baoshi_" + _TO_STR(j+1) + ".png");
			ImageView* zhezhao = ImageView::create("Image/UIArtifacts/image/img_baoshi_zhezhao.png");

			ImageView* jewelBg = (ImageView*)pnlJewel->getChildByName("Img_1000" + _TO_STR(j+1));
			jewelBg->addChild(jewel);
			jewel->setPosition(jewelBg->getContentSize()/2);
			jewelBg->addChild(zhezhao);
			zhezhao->setPosition(jewelBg->getContentSize()/2);
			zhezhao->setName("Image_ZheZhao");
		}
	}
	m_ui.PnlArtifactList = (Layout*)parentPnl->getChildByName("Pnl_List");
	
	Layout* PnlArtifact = (Layout*)m_ui.PnlArtifactList->getChildByName("Pnl_Icon");
	Button* btnArtifact = (Button*)PnlArtifact->getChildByName("Btn_Artifact");
	btnArtifact->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_FIRST_ARTIFACT));
	m_ui.vecPnlArtifact.clear();
	m_ui.vecPnlArtifact.push_back(PnlArtifact);
	//克隆剩下的
	for(int i=1; i<ArtifactModel::getInstance()->getAllArtifact().size(); i++)
	{
		Layout* artifact = (Layout*)PnlArtifact->clone();
		m_ui.PnlArtifactList->addChild(artifact);
		artifact->setPosition(Point(PnlArtifact->getPositionX() + (PnlArtifact->getContentSize().width + 5)*i, PnlArtifact->getPositionY()));
		Button* btn = (Button*)artifact->getChildByName("Btn_Artifact");
		btn->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_FIRST_ARTIFACT+i));
		m_ui.vecPnlArtifact.push_back(artifact);
	}

	m_ui.PnlDetails = (Layout*)parentPnl->getChildByName("Pnl_Main");
	m_ui.imgConditionBg = (ImageView*)m_ui.PnlDetails->getChildByName("Img_di1");
	m_ui.imgConditionBg->setVisible(false);
	m_ui.btnActive = (Button*)m_ui.imgConditionBg->getChildByName("Btn_Activation");
	m_ui.btnActive->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_ACTIVE));

	Layout* PnlCondition = (Layout*)m_ui.imgConditionBg->getChildByName("Pnl_Require");
	m_ui.vecPnlCondition.clear();
	m_ui.vecPnlCondition.push_back(PnlCondition);
	//克隆剩下的
	for(int i=1; i<MaxCondition; i++)
	{
		Layout* condition = (Layout*)PnlCondition->clone();
		m_ui.imgConditionBg->addChild(condition);
		condition->setPosition(Point(PnlCondition->getPositionX(), PnlCondition->getPositionY() - (PnlCondition->getContentSize().height + 13)*i));
		m_ui.vecPnlCondition.push_back(condition);
	}

	m_ui.imgFightAttriBg = (ImageView*)m_ui.PnlDetails->getChildByName("Img_di2");
	m_ui.imgRefineAll = (ImageView*)m_ui.PnlDetails->getChildByName("Img_di3");

	//洗练相关
	Layout* PnlRefine = (Layout*)m_ui.imgFightAttriBg->getChildByName("Pnl_Refine");
	m_ui.btnCommon = (Button*)PnlRefine->getChildByName("Btn_Normal");
	m_ui.btnCommon->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_NORMAL));
	m_ui.btnHigh = (Button*)PnlRefine->getChildByName("Btn_High");
	m_ui.btnHigh->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_ADVANCE));
	m_ui.txtCostStore = (Text*)PnlRefine->getChildByName("Lab_Cost1");
	m_ui.txtCostMoney = (Text*)PnlRefine->getChildByName("Lab_Cost2");
	m_ui.txtCostStore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCostMoney->setFontName(FONT_FZZHENGHEI);
	m_ui.imgCoin = (ImageView*)PnlRefine->getChildByName("Img_Coin");
	m_ui.btnRefineOne = (Button*)PnlRefine->getChildByName("Btn_One");
	m_ui.btnRefineOne->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_REFINEONE));
	m_ui.btnRefineTen = (Button*)PnlRefine->getChildByName("Btn_Ten");
	m_ui.btnRefineTen->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_REFINETEN));
	m_ui.btnCancel = (Button*)PnlRefine->getChildByName("Btn_Cancel");
	m_ui.btnCancel->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_CANCEL));
	m_ui.btnSave = (Button*)PnlRefine->getChildByName("Btn_Save");
	m_ui.btnSave->addTouchEventListener(CC_CALLBACK_2(ArtifactLayer::onBtnClick, this, BTN_SAVE));

	m_ui.pnlFightAttri = (Layout*)m_ui.PnlDetails->getChildByName("Pnl_AttriList");
	m_ui.vecPnlAttri.clear();
	for(int i=0; i < MaxAttribute; i++)
	{
		Layout* PnlAttri = (Layout*)m_ui.pnlFightAttri->getChildByName("Pnl_Attribute" + _TO_STR(i+1));
		m_ui.vecPnlAttri.push_back(PnlAttri);
	}
}

void ArtifactLayer::updateUI()
{
	ArtifactModel* pInstane = ArtifactModel::getInstance();
	ArtifactParam* Artifact = pInstane->getArtifactByOrder(mCurArtifact);

	//神器名字
	ImageView* imgArtifactName = (ImageView*)m_ui.imgNameBg->getChildByName("ArtifactName");
	if(imgArtifactName == nullptr)
	{
		imgArtifactName = ImageView::create();
		imgArtifactName->setName("ArtifactName");
		m_ui.imgNameBg->addChild(imgArtifactName);
		imgArtifactName->setPosition(m_ui.imgNameBg->getContentSize()/2);
		imgArtifactName->setAnchorPoint(Vec2(0.5, 0.4));
	}	
	imgArtifactName->loadTexture("Image/UIArtifacts/image/img_name1000" + _TO_STR(Artifact->getCommonAttribute().m_iOrderNum) + ".png");

	//神器图片
	ImageView* imgArtifact = (ImageView*)m_ui.PnlBorn->getChildByName("ImgArtifact");
	if(imgArtifact == nullptr)
	{
		imgArtifact = ImageView::create();
		imgArtifact->setName("ImgArtifact");
		m_ui.PnlBorn->addChild(imgArtifact);
		imgArtifact->setZOrder(-1);
		imgArtifact->setPosition(m_ui.PnlBorn->getContentSize()/2);
		imgArtifact->setAnchorPoint(Vec2(0.5, 0.55));
	}
	imgArtifact->loadTexture("Image/UIArtifacts/image/img_shenqi1000" + _TO_STR(Artifact->getCommonAttribute().m_iOrderNum) + ".png");

	//未激活的置灰
	setImageGray(imgArtifact, !Artifact->getCommonAttribute().m_bIsActived);

	//神器宝石
	for(int i=0; i<m_ui.vecPnlJewel.size(); i++)
	{
		m_ui.vecPnlJewel.at(i)->setVisible(false);
		if(i+1 == mCurArtifact)
		{
			if(Artifact->isActived())
			{
				m_ui.vecPnlJewel.at(i)->setVisible(false);
			}
			else
			{
				m_ui.vecPnlJewel.at(i)->setVisible(true);
				for(int j=0; j<MaxCondition; j++)
				{
					ImageView* imgJewelBg = (ImageView*)m_ui.vecPnlJewel.at(i)->getChildByName("Img_1000" + _TO_STR(j+1));
					vector<sCondition> condition = Artifact->getActiveCondition();
					if(condition.at(j).m_iDoneNum >= condition.at(j).m_iGoalNum)
					{
						imgJewelBg->getChildByName("Image_ZheZhao")->setVisible(false);
					}
				}
			}
		}
	}

	//动画
	doArtifactAni();

	//神器列表状态
	updateListState();

	//神器激活状态和洗练状态
	if(Artifact->isActived())
	{
		m_ui.imgConditionBg->setVisible(false);
		m_ui.pnlFightAttri->setVisible(true);

		if(Artifact->isRefineAll())
		{
			m_ui.imgFightAttriBg->setVisible(false);
			m_ui.imgRefineAll->setVisible(true);
			updateRefineData();
		}
		else
		{
			m_ui.imgFightAttriBg->setVisible(true);
			m_ui.imgRefineAll->setVisible(false);

			updateRefineStates();
		}	
	}
	else
	{
		m_ui.imgConditionBg->setVisible(true);
		m_ui.pnlFightAttri->setVisible(false);
		m_ui.imgFightAttriBg->setVisible(false);
		m_ui.imgRefineAll->setVisible(false);

		updateActiveStates();
	}
	
}


void ArtifactLayer::doArtifactAni()
{
	ArtifactParam* Artifact = ArtifactModel::getInstance()->getArtifactByOrder(mCurArtifact);

	//神器本身的动画要被停止并归中坐标
	ImageView* imgArtifact = (ImageView*)m_ui.PnlBorn->getChildByName("ImgArtifact");
	imgArtifact->stopActionByTag(999);
	imgArtifact->setPosition(m_ui.PnlBorn->getContentSize()/2);

	//神器动画, 未激活时无动画
	if(!Artifact->isActived())
	{
		//有的话，移除
		if(m_ui.PnlBorn->getChildByName("Ani_Common"))
		{
			m_ui.PnlBorn->getChildByName("Ani_Common")->removeFromParentAndCleanup(true);
		}

		//有后置和前置动画的话，移除
		if(m_ui.PnlBorn->getChildByName("Ani_Back"))
		{
			m_ui.PnlBorn->getChildByName("Ani_Back")->removeFromParentAndCleanup(true);
		}
		if(m_ui.PnlBorn->getChildByName("Ani_Front"))
		{
			m_ui.PnlBorn->getChildByName("Ani_Front")->removeFromParentAndCleanup(true);
		}
	}
	else
	{
		//有的话，先移除
		if(!m_ui.PnlBorn->getChildByName("Ani_Common"))
		{
			playCommonBGAni();
		}

		playArtifactAni();
	}
}


void ArtifactLayer::playCommonBGAni()
{
	// 加载 动画文件;
	string strFileName;
	strFileName.assign("Image/AniSQ_CIRCLE/AniSQ_CIRCLE.ExportJson");

	string aniName = "Animation1";

	ArmatureDataManager::getInstance()->addArmatureFileInfo(strFileName.c_str());
	Armature* ani = Armature::create("AniSQ_CIRCLE");
	ani->getAnimation()->play(aniName);
	ani->setPosition(Vec2(m_ui.PnlBorn->getContentSize() / 2));
	ani->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.PnlBorn->addChild(ani);
	ani->setLocalZOrder(-3);
	ani->setName("Ani_Common");
	ani->getAnimation()->setSpeedScale(0.5f);
}

void ArtifactLayer::playArtifactAni()
{
	//有的话，先移除
	if(m_ui.PnlBorn->getChildByName("Ani_Back"))
	{
		m_ui.PnlBorn->getChildByName("Ani_Back")->removeFromParentAndCleanup(true);
	}
	if(m_ui.PnlBorn->getChildByName("Ani_Front"))
	{
		m_ui.PnlBorn->getChildByName("Ani_Front")->removeFromParentAndCleanup(true);
	}

	//镜子动画坐标
	Point pos(0, 0);
	pos = m_ui.PnlBorn->getChildByName("Ani_Common")->getPosition();

	string aniName = "Animation1";

	// 后置动画文件;
	string strNameBack;
	strNameBack.assign("AniSQ_1000" + _TO_STR(mCurArtifact) + "_Back");
	string strFileNameBack;
	strFileNameBack.assign("Image/" + strNameBack + "/" + strNameBack + ".ExportJson");
	ArmatureDataManager::getInstance()->addArmatureFileInfo(strFileNameBack.c_str());

	Armature* aniBack = Armature::create(strNameBack);
	aniBack->getAnimation()->play(aniName);
	aniBack->setPosition(pos);

	m_ui.PnlBorn->addChild(aniBack);
	aniBack->setLocalZOrder(-2);
	aniBack->setName("Ani_Back");

	// 前置动画文件
	string strNameFront;
	strNameFront.assign("AniSQ_1000" + _TO_STR(mCurArtifact) + "_Front");
	string strFileNameFront;
	strFileNameFront.assign("Image/" + strNameFront + "/" + strNameFront + ".ExportJson");
	ArmatureDataManager::getInstance()->addArmatureFileInfo(strFileNameFront.c_str());

	Armature* aniFront = Armature::create(strNameFront);
	aniFront->getAnimation()->play(aniName);
	aniFront->setPosition(pos);

	m_ui.PnlBorn->addChild(aniFront);
	aniFront->setLocalZOrder(0);
	aniFront->setName("Ani_Front");

	//神器本身的上下漂浮
	ImageView* imgArtifact = (ImageView*)m_ui.PnlBorn->getChildByName("ImgArtifact");
	ActionInterval* moveUp = MoveBy::create(1.5f, Vec2(0, 6));
	ActionInterval* moveDn = MoveBy::create(1.5f, Vec2(0, -6));
	ActionInterval* act = Sequence::create(moveUp, moveDn, moveDn, moveUp, nullptr);
	ActionInterval* repeatAct = RepeatForever::create(act);
	repeatAct->setTag(999);
	imgArtifact->runAction(repeatAct);
}


void ArtifactLayer::playEnterAni()
{
	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.5f;

	// 切入特效;
	m_ui.PnlDetails->setPositionX(m_ui.PnlDetails->getPositionX() + shiftPosX);
	m_ui.PnlDetails->runAction(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(-shiftPosX, 0.0f))));
}

void ArtifactLayer::updateListState()
{
	ArtifactModel* pInstane = ArtifactModel::getInstance();

	for(int i=0; i<m_ui.vecPnlArtifact.size(); i++)
	{
		ArtifactParam* Artifact = pInstane->getArtifactByOrder(i+1);
		if(!Artifact)
		{
			continue;
		}
		ImageView* imgActive = (ImageView*)m_ui.vecPnlArtifact.at(i)->getChildByName("Img_Inactive");
		ImageView* imgUnknown = (ImageView*)m_ui.vecPnlArtifact.at(i)->getChildByName("Img_Unknown");
		ImageView* imgZheZhao = (ImageView*)m_ui.vecPnlArtifact.at(i)->getChildByName("Img_zhezhao");
		ImageView* imgArtifact = (ImageView*)m_ui.vecPnlArtifact.at(i)->getChildByName("Img_Artifact");
		Button* btnArtifact = (Button*)m_ui.vecPnlArtifact.at(i)->getChildByName("Btn_Artifact");
		
		btnArtifact->setHighlighted(false);
		btnArtifact->setEnabled(true);

		//第一个神器默认选中，并且默认可见，若已激活则下一个神器可见
		if(i == 0)
		{
			imgActive->setVisible(!Artifact->isActived());
			imgUnknown->setVisible(false);
			imgZheZhao->setVisible(imgActive->isVisible());
		}
		else
		{
			//是否开放还需要参考前一个神器的状态
			ArtifactParam* ArtifactOb = pInstane->getArtifactByOrder(i);
			if(Artifact->isActived())
			{
				imgActive->setVisible(false);
				imgUnknown->setVisible(false);
			}
			else
			{
				imgActive->setVisible(ArtifactOb->isActived());
				imgUnknown->setVisible(!ArtifactOb->isActived());
			}
			
			imgZheZhao->setVisible(imgActive->isVisible());
		}
		
		imgArtifact->loadTexture("Image/UIArtifacts/image/img_shenqi1000" + _TO_STR(i+1) + ".png");

		if(mCurArtifact == i+1)
		{
			btnArtifact->setHighlighted(true);
			btnArtifact->setEnabled(false);
		}
	}
}

void ArtifactLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	Button* btn = (Button*)ref;
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case BTN_ACTIVE:
			{
				if(isCanActive())
				{
					//先记录正要激活的神器ID
					ArtifactModel::getInstance()->setCurArtifactID(mCurArtifact);

					//发送激活神器协议
					m_pArtifactController.sendActiveArtifactMsg(10+mCurArtifact);
				}
			}
			break;
		case BTN_REFINEONE:
		case BTN_REFINETEN:
			{
				if(isCanRefine())
				{
					//先记录正要洗练的神器ID
					ArtifactModel::getInstance()->setCurArtifactID(mCurArtifact);

					//发送洗练协议
					int arctId = 10+mCurArtifact;
					int times = nWidgetName == BTN_REFINEONE ? 1 : 10;
					int refType = mCurRefineMethod == eMETHOD_COMMON ? 1 : 2;
					//如果是高级洗练，记录下洗练方式给talkingdata使用
					if(mCurRefineMethod == eMETHOD_ADVANCE)
					{
						ArtifactModel::getInstance()->setRefineType(times);
					}
					else
					{
						ArtifactModel::getInstance()->setRefineType(0);
					}
					m_pArtifactController.sendRefineArtifactMsg(arctId, times, refType);
				}
			}
			break;
		case BTN_CANCEL:
			{
				//取消洗练结果，重置按钮为洗练态
				ArtifactModel::getInstance()->resetCancelRefine();
				m_bIsNeedResetRefine = false;
				updateRefineStates();
			}
			break;
		case BTN_SAVE:
			{
				//先记录正要保存的神器ID
				ArtifactModel::getInstance()->setCurArtifactID(mCurArtifact);

				//发送保存洗练协议
				m_pArtifactController.sendSaveRefineMsg(10+mCurArtifact);
			}
			break;
		case BTN_NORMAL:
			{
				ImageView* selFlag = (ImageView*)btn->getChildByName("Img_Sel");
				if(selFlag->isVisible())
				{
					return;
				}
				else
				{
					selFlag->setVisible(true);
					mCurRefineMethod = eMETHOD_COMMON;
					m_ui.btnHigh->getChildByName("Img_Sel")->setVisible(false);
					m_ui.imgCoin->loadTexture("Image/Icon/global/img_yinbi.png");
				}	

				setRefineCost();
			}
			break;
		case BTN_ADVANCE:
			{
				ImageView* selFlag = (ImageView*)btn->getChildByName("Img_Sel");
				if(selFlag->isVisible())
				{
					return;
				}
				else
				{
					selFlag->setVisible(true);
					mCurRefineMethod = eMETHOD_ADVANCE;
					m_ui.btnCommon->getChildByName("Img_Sel")->setVisible(false);
					m_ui.imgCoin->loadTexture("Image/Icon/global/img_yuanbao.png");
				}

				setRefineCost();
			}
			break;
		case BTN_FIRST_ARTIFACT:
		case BTN_SECOND_ARTIFACT:
		case BTN_THIRD_ARTIFACT:
		case BTN_FOURTH_ARTIFACT:
		case BTN_FIFTH_ARTIFACT:
			{
				int order = nWidgetName - (int)BTN_FIRST_ARTIFACT + 1;
				if(order == mCurArtifact)
				{
					return;		//当前已经选中
				}
				if(nWidgetName != BTN_FIRST_ARTIFACT)
				{
					ArtifactParam* artifact1 = ArtifactModel::getInstance()->getArtifactByOrder(order);
					
					if(!artifact1->isActived())
					{
						ArtifactParam* artifact2 = ArtifactModel::getInstance()->getArtifactByOrder(order-1);
						if(!artifact2->isActived())
						{
							return;		//如果上一个还未激活，则这个点击不能响应
						}
					}
				}
				
				ArtifactModel::getInstance()->resetCancelRefine();
				m_bIsNeedResetRefine = false;
				mCurArtifact = (eArtifactOrder)order;
				mCurRefineMethod = eMETHOD_COMMON;		//洗练方式重置
				
				playEnterAni();
				updateUI();
			}		
			break;
		default:
			break;
		}
	}
}

void ArtifactLayer::updateActiveStates()
{
	ArtifactModel* pInstane = ArtifactModel::getInstance();
	ArtifactParam* Artifact = pInstane->getArtifactByOrder(mCurArtifact);

	for(int i=0; i<Artifact->getActiveCondition().size(); i++)
	{
		Layout* pnlConditon = m_ui.vecPnlCondition.at(i);
		Text* des = (Text*)pnlConditon->getChildByName("Lab_Tips");
		Text* progress = (Text*)pnlConditon->getChildByName("Lab_Progress");
		ImageView* jewel = (ImageView*)(Helper::seekWidgetByName(pnlConditon, "Img_Jewel"));
		ImageView* zhezhao = (ImageView*)(Helper::seekWidgetByName(pnlConditon, "Img_ZheZhao"));

		des->setFontName(FONT_FZZHENGHEI);
		progress->setFontName(FONT_FZZHENGHEI);

		sCondition condition = Artifact->getActiveCondition().at(i);
		des->setString(condition.m_strDes);
		des->setPositionY(DesPosY);
		Text* text = Text::create();
		text->setString(condition.m_strDes);
		text->setFontSize(des->getFontSize());
		text->setFontName(FONT_FZZHENGHEI);
		if(text->getContentSize().width < MaxDesWidth)
		{
			des->setPositionY(DesPosY - 20);
		}
		if(condition.m_iDoneNum >= condition.m_iGoalNum)
		{
			//progress->setString(STR_UTF8("已达成"));
			progress->setString(DataManager::getInstance()->searchCommonTexdtById(10004));
			progress->setColor(Color3B(0x54, 0xdd, 0x51));
			zhezhao->setVisible(false);
		}
		else
		{
			if(condition.m_bIsShowProgress)
			{
				progress->setString(_TO_STR(condition.m_iDoneNum) + "/" + _TO_STR(condition.m_iGoalNum));		
			}
			else
			{
				progress->setString("0/1");	
			}

			progress->setColor(Color3B(0x69, 0x69, 0x69));
			zhezhao->setVisible(true);
		}
		
		jewel->loadTexture("Image/UIArtifacts/image/img_baoshi_" + _TO_STR(condition.m_iType) + ".png");
	}
}

void ArtifactLayer::updateRefineStates()
{
	updateRefineData();

	//洗练配置状态
	m_ui.btnCommon->getChildByName("Img_Sel")->setVisible(mCurRefineMethod == eMETHOD_COMMON);
	m_ui.btnHigh->getChildByName("Img_Sel")->setVisible(mCurRefineMethod == eMETHOD_ADVANCE);
	
	m_ui.imgCoin->loadTexture(mCurRefineMethod == eMETHOD_COMMON ? "Image/Icon/global/img_yinbi.png" : "Image/Icon/global/img_yuanbao.png");
	
	setRefineCost();
	setBtnShowState();
}

void ArtifactLayer::updateRefineData()
{
	ArtifactModel* pInstane = ArtifactModel::getInstance();
	ArtifactParam* Artifact = pInstane->getArtifactByOrder(mCurArtifact);
	vector<sFightAttri> vecAttri = Artifact->getFightAttribute();

	for(int i=0; i<m_ui.vecPnlAttri.size(); i++)
	{
		Layout* pnlFightAttri = (Layout*)m_ui.vecPnlAttri.at(i);
		ImageView* icon = (ImageView*)pnlFightAttri->getChildByName("Img_Icon");
		ImageView* jinDuMax = (ImageView*)pnlFightAttri->getChildByName("Img_jinduMax");
		Text* attriName = (Text*)pnlFightAttri->getChildByName("Lab_Attribute");	
		Text* valueChange = (Text*)pnlFightAttri->getChildByName("Lab_Value");
		Text* valueBar = (Text*)pnlFightAttri->getChildByName("Lab_Count");
		LoadingBar* progressBar = (LoadingBar*)pnlFightAttri->getChildByName("Bar_Attri");

		attriName->setFontName(FONT_FZZHENGHEI);
		valueChange->setFontName(FONT_FZZHENGHEI);
		valueBar->setFontName(FONT_FZZHENGHEI);

		jinDuMax->setVisible(false);

		icon->loadTexture(pInstane->getAttriIconPath(vecAttri.at(i).eType));
		attriName->setString(vecAttri.at(i).strChName);

		switch (vecAttri.at(i).eType)
		{
		case eTYPE_ATK:
		case eTYPE_HP:
		case eTYPE_PHYDEFENSE:
		case eTYPE_MGCDEFENSE:
			{
				int curValue = (int)vecAttri.at(i).fValue;
				int goalValue = vecAttri.at(i).fMaxValue;
				int tempValue = (int)vecAttri.at(i).fTempValue;
				valueBar->setString(_TO_STR(curValue) + "/" + _TO_STR(goalValue));
				setTextAddStroke(valueBar, Color3B(0x6a, 0x20, 0x11), 2);
				if(curValue >= goalValue)
				{
					progressBar->setPercent(100.0f);
					jinDuMax->setVisible(true);
				}
				else
				{
					progressBar->setPercent(curValue*100/goalValue);
				}

				if(m_bIsNeedResetRefine)
				{
					if(tempValue > 0)
					{
						valueChange->setString("+" + _TO_STR(tempValue));
						valueChange->setColor(Color3B(0x54, 0xdd, 0x51));
					}
					else if(tempValue < 0)
					{
						valueChange->setString(_TO_STR(tempValue));
						valueChange->setColor(Color3B(0xf4, 0x12, 0x12));
					}
					else
					{
						//valueChange->setString("");
						valueChange->setString("--");
						valueChange->setColor(Color3B(0x8F, 0x8B, 0x8D));
					}
				}
				else
				{
					valueChange->setString("");
					//valueChange->setString("--");
				}
			}	
			break;
		case eTYPE_DEFAULT:
			break;
		default:
			{
				float curValue = vecAttri.at(i).fValue;
				float goalValue = vecAttri.at(i).fMaxValue;
				float tempValue = vecAttri.at(i).fTempValue;
				valueBar->setString(_F_TO_STR(curValue*100) + "%" + "/" + _TO_STR(goalValue*100) + "%");
				setTextAddStroke(valueBar, Color3B(0x6a, 0x20, 0x11), 2);
				if(curValue < goalValue)
				{
					progressBar->setPercent(curValue*100/goalValue);
				}
				else
				{
					progressBar->setPercent(100.0f);
					jinDuMax->setVisible(true);
				}

				if(m_bIsNeedResetRefine)
				{
					if(tempValue > 0)
					{
						valueChange->setString("+" + _F_TO_STR((tempValue)*100) + "%");
						valueChange->setColor(Color3B(0x54, 0xdd, 0x51));
					}
					else if(tempValue < 0)
					{
						valueChange->setString(_F_TO_STR((tempValue)*100) + "%");
						valueChange->setColor(Color3B(0xf4, 0x12, 0x12));
					}
					else
					{
						//valueChange->setString("");
						valueChange->setString("--");
						valueChange->setColor(Color3B(0x8F, 0x8B, 0x8D));
					}
				}
				else
				{
					valueChange->setString("");
					//valueChange->setString("--");
				}
			}

			break;
		}		
	}
}

bool ArtifactLayer::isCanActive()
{
	ArtifactModel* pInstane = ArtifactModel::getInstance();
	ArtifactParam* Artifact = pInstane->getArtifactByOrder(mCurArtifact);
	vector<sCondition> condition = Artifact->getActiveCondition();

	for(auto item : condition)
	{
		if(!item.m_bIsDone)
		{
			//CustomTips::show("激活条件未达成");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10005));
			return false;
		}
	}

	return true;
}

bool ArtifactLayer::isCanRefine()
{
	RowData* rowData1 = DataManager::getInstance()->searchToolSystemById(RefineCostStore);
	if(rowData1 == nullptr)
	{
		CCLOG("读取系统配置表出错!");
		return false;
	}
	
	if(ToolModel::getInstance()->searchNumByTemplateId(RefineStoreID) >= rowData1->getIntData("value"))
	{
		if(mCurRefineMethod == eMETHOD_COMMON)
		{
			RowData* rowData2 = DataManager::getInstance()->searchToolSystemById(RefineCostCoin);
			if(rowData2 == nullptr)
			{
				CCLOG("读取系统配置表出错!");
				return false;
			}
			if(MainModel::getInstance()->getMainParam()->mCoin >= rowData2->getIntData("value"))
			{
				return true;
			}
			else
			{
				//CustomTips::show("银币不足");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10006));
				return false;
			}
		}
		else
		{
			RowData* rowData2 = DataManager::getInstance()->searchToolSystemById(RefineCostGold);
			if(rowData2 == nullptr)
			{
				CCLOG("读取系统配置表出错!");
				return false;
			}
			if(MainModel::getInstance()->getMainParam()->mGold >= rowData2->getIntData("value"))
			{
				return true;
			}
			else
			{
				//CustomTips::show("金币不足");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10007));
				return false;
			}
		}
		
	}
	else
	{
		//CustomTips::show("洗练石不足");

		//获取途径
		LayerEquipSource* pSource = LayerEquipSource::create();
		this->addChild(pSource);
		pSource->showSrc(RefineStoreID);

		return false;
	}
}

void ArtifactLayer::setBtnShowState()
{
	if(m_bIsNeedResetRefine)
	{
		m_ui.btnCancel->setVisible(true);
		m_ui.btnSave->setVisible(true);
		m_ui.btnRefineOne->setVisible(false);
		m_ui.btnRefineTen->setVisible(false);
	}
	else
	{
		m_ui.btnCancel->setVisible(false);
		m_ui.btnSave->setVisible(false);
		m_ui.btnRefineOne->setVisible(true);
		m_ui.btnRefineTen->setVisible(true);
	}
}

void ArtifactLayer::setRefineCost()
{
	sRefineCost cost = ArtifactModel::getInstance()->getRefineCost();

	m_ui.txtCostStore->setString(_TO_STR(cost.m_iCostStore));

	if(mCurRefineMethod == eMETHOD_COMMON)
	{
		m_ui.txtCostMoney->setString(_TO_STR(cost.m_iCostCoin));
	}
	else
	{
		m_ui.txtCostMoney->setString(_TO_STR(cost.m_iCostGold));
	}
}


void ArtifactLayer::updateSelf( void* data )
{
	ObserverParam* obParam = (ObserverParam*) data;
	if(!obParam)
	{
		return;
	}

	switch (obParam->id)
	{
	case nMAIN_CMD_ARTIFACT_MAIN:
	case nMAIN_CMD_ARTIFACT_ACTIVE:
		updateUI();
		break;
	case nMAIN_CMD_ARTIFACT_REFINE:
		{
			m_bIsNeedResetRefine = true;
			updateRefineStates();
		}
		break;
	case nMAIN_CMD_ARTIFACT_SAVE:
		{
			m_bIsNeedResetRefine = false;
			ArtifactModel::getInstance()->resetCancelRefine();
			if(ArtifactModel::getInstance()->getArtifactByOrder(mCurArtifact)->isRefineAll())
			{
				m_ui.imgFightAttriBg->setVisible(false);
				m_ui.imgRefineAll->setVisible(true);
				updateRefineData();
				return;
			}
			else
			{
				m_ui.imgFightAttriBg->setVisible(true);
				m_ui.imgRefineAll->setVisible(false);
				updateRefineStates();
			}					
		}
		break;

	default:
		break;
	}
}



















