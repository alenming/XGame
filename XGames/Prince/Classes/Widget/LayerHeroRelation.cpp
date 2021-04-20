#include "LayerHeroRelation.h"
#include "ToolModule/UIToolHeader.h"
#include "HeroModule/HeroModel.h"
#include "DataManager/DataManager.h"
#include "HeroRelationModule/HeroRelationModel.h"
#include "Common/Common.h"
#include "SmithyModule/SmithyDef.h"
#include "SmithyModule/SmithyModel.h"
#include "GuideCenter/GuideManager.h"


LayerHeroRelation::LayerHeroRelation()
{
	m_iFrontAdvLv = 0;
	m_bIsEquipCL = false;
	m_call = nullptr;
}

LayerHeroRelation::~LayerHeroRelation( void )
{

}

LayerHeroRelation* LayerHeroRelation::create( int heroId, bool isAdv, closeViewCallback call, bool isEquipCL, int cl_ToolId, EQUIP_ITEM_TYPE CL_type )
{
	LayerHeroRelation* _layer = new LayerHeroRelation();
	if (_layer->init(heroId, isAdv, call, isEquipCL, cl_ToolId, CL_type))
	{
		_layer->autorelease();
		return _layer;
	}
	else
	{
		delete _layer;
		return nullptr;
	}
}

bool LayerHeroRelation::init( int heroId, bool isAdv, closeViewCallback call, bool isEquipCL, int cl_ToolId, EQUIP_ITEM_TYPE CL_type )
{
	if (!ModalLayer::init())
	{
		return false;
	}

	m_iTempId = heroId;
	m_bIsAdv = isAdv;
	m_bIsEquipCL = isEquipCL;
	m_iCLToolId = cl_ToolId;
	m_clType = CL_type;
	m_call = call;

	initUI();

	return true;
}

void LayerHeroRelation::onEnter()
{
	ModalLayer::onEnter();

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	m_ui.pRoot->getChildByName("IMG_TapToSkip")->runAction(RepeatForever::create(seq));
	
	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_HERO_FATE_ACTIVE);
}

void LayerHeroRelation::onExit()
{
	ModalLayer::onExit();
}

void LayerHeroRelation::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIArray/UIArray_FateUp.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);

	m_ui.pImgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_fateUp"));
	m_ui.pPnlContent = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_content"));
	m_ui.pPnlContent->setOpacity(0);
	m_ui.pPnlTips = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_tips"));
	m_ui.pPnlSingleHead = dynamic_cast<Layout*>(m_ui.pPnlContent->getChildByName("Pnl_headIcon"));
	m_ui.pPnlSingleHead->setVisible(false);
	m_ui.pPnlDoubleHead = dynamic_cast<Layout*>(m_ui.pPnlContent->getChildByName("Pnl_headUp"));
	m_ui.pPnlDoubleHead->setVisible(false);
	m_ui.pPnlAttri_1 = dynamic_cast<Layout*>(m_ui.pPnlContent->getChildByName("Pnl_fateItem1"));
	m_ui.pPnlAttri_1->setVisible(false);
	m_ui.pPnlAttri_2 = dynamic_cast<Layout*>(m_ui.pPnlContent->getChildByName("Pnl_fateItem2"));
	m_ui.pPnlAttri_2->setVisible(false);
	m_ui.pPnlAttri_3 = dynamic_cast<Layout*>(m_ui.pPnlContent->getChildByName("Pnl_fateItem3"));
	m_ui.pPnlAttri_3->setVisible(false);
	m_ui.pPnlMask = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_mask"));

	m_ui.pLabOperateTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlDoubleHead, "Lab_adv"));
	m_ui.pLabOperateTips->setFontName(FONT_FZZHENGHEI);

	auto touchCall = [this](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");

			if (m_call)
			{
				m_call();
			}
			dispose();
		}
	};
	
	m_ui.pPnlMask->addTouchEventListener(touchCall);

	playBgAni();
	updateUI();
}

void LayerHeroRelation::aniAniCallBack(Armature *armature, MovementEventType movementType, const string& movementID, const string& nextAniName)
{
	if (movementType == COMPLETE)
	{
		armature->getAnimation()->play(nextAniName);
		armature->getAnimation()->setMovementEventCallFunc(nullptr);
	}
}


void LayerHeroRelation::updateUI()
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
	if(heroParam == nullptr)
	{
		CCLOG("获取英雄出错");
		return;
	}

	if(m_bIsAdv)
	{
		m_ui.pLabOperateTips->setString(DataManager::getInstance()->searchCommonTexdtById(20003));
		m_ui.pImgTitle->loadTexture("Image/Icon/Fate/img_yuanfentisheng.png");
		m_ui.pPnlSingleHead->setVisible(false);
		m_ui.pPnlDoubleHead->setVisible(true);
		UIToolHeader* leftIcon = UIToolHeader::create(m_iTempId);
		UIToolHeader* rightIcon = UIToolHeader::create(m_iTempId);

		Layout* pnlLeftIcon = dynamic_cast<Layout*>(m_ui.pPnlDoubleHead->getChildByName("Pnl_leftHead"));
		Layout* pnlRightIcon = dynamic_cast<Layout*>(m_ui.pPnlDoubleHead->getChildByName("Pnl_rightHead"));

		pnlLeftIcon->removeAllChildren();
		pnlRightIcon->removeAllChildren();
		
		if(leftIcon != nullptr)
		{
			leftIcon->setPosition(pnlLeftIcon->getContentSize()/2);
			pnlLeftIcon->addChild(leftIcon);
		}
		if(rightIcon != nullptr)
		{
			rightIcon->setPosition(pnlRightIcon->getContentSize()/2);
			pnlRightIcon->addChild(rightIcon);
		}

		//如果是绿色以上(含绿色)品质(qua >= 2),需要设置角标
		if(heroParam->mQuality == 2)
		{
			rightIcon->setQuaCornerIcon(m_iTempId, heroParam->mAdvLvl);
		}
		else if(heroParam->mQuality > 2)
		{
			leftIcon->setQuaCornerIcon(m_iTempId, heroParam->mAdvLvl-1);
			rightIcon->setQuaCornerIcon(m_iTempId, heroParam->mAdvLvl);
		}

		leftIcon->setStarsNum(heroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
		leftIcon->setLvlWithoutBg(heroParam->mLevel);

		rightIcon->setStarsNum(heroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
		rightIcon->setLvlWithoutBg(heroParam->mLevel);

		updateFateInfo();
	}
	else if(m_bIsEquipCL)
	{
		m_ui.pLabOperateTips->setString(DataManager::getInstance()->searchCommonTexdtById(20004));
		m_ui.pImgTitle->loadTexture("Image/Icon/Fate/img_huodeyuanfen.png");
		m_ui.pPnlSingleHead->setVisible(false);
		m_ui.pPnlDoubleHead->setVisible(true);

		//淬炼数据
		CL_Param m_paramCL;
		m_paramCL.clear();
		SmithyModel::getInstance()->getCLParam(m_iCLToolId, m_clType, m_paramCL);

		//淬炼前道具资源ID
		int equipIconId = 0;
		if(m_clType == EQUIP_ITEM_TYPE_WEAPON)
		{
			equipIconId = m_iTempId*100 + WEAPON_ICON_BASE_ID;
		}
		else if(m_clType == EQUIP_ITEM_TYPE_HORSE)
		{
			equipIconId = m_iTempId*100 + HORSE_ICON_BASE_ID;
		}
		else if(m_clType == EQUIP_ITEM_TYPE_BOOK)
		{
			equipIconId = m_iTempId*100 + BOOK_ICON_BASE_ID;
		}
		UIToolHeader* leftIcon = nullptr;
		if(equipIconId != 0)
		{
			//查淬炼表
			RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(equipIconId);
			if(_row != nullptr)
			{
				int iconResId = _row->getIntData("icon");
				leftIcon = UIToolHeader::create(ResourceUtils::getSmallIconPath(iconResId));
			}
		}
		
		UIToolHeader* rightIcon = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquIcon));

		Layout* pnlLeftIcon = dynamic_cast<Layout*>(m_ui.pPnlDoubleHead->getChildByName("Pnl_leftHead"));
		Layout* pnlRightIcon = dynamic_cast<Layout*>(m_ui.pPnlDoubleHead->getChildByName("Pnl_rightHead"));

		pnlLeftIcon->removeAllChildren();
		pnlRightIcon->removeAllChildren();

		if(leftIcon != nullptr)
		{
			leftIcon->setPosition(pnlLeftIcon->getContentSize()/2);
			pnlLeftIcon->addChild(leftIcon);
		}
		if(rightIcon != nullptr)
		{
			rightIcon->setPosition(pnlRightIcon->getContentSize()/2);
			pnlRightIcon->addChild(rightIcon);
		}

		//淬炼分支
		updateViewByCL();
	}
	else 
	{
		m_ui.pImgTitle->loadTexture("Image/Icon/Fate/img_huodeyuanfen.png");
		m_ui.pPnlSingleHead->setVisible(true);
		m_ui.pPnlDoubleHead->setVisible(false);

		UIToolHeader* headIcon = UIToolHeader::create(m_iTempId);
		if(headIcon != nullptr)
		{
			headIcon->setPosition(m_ui.pPnlSingleHead->getContentSize()/2);
			m_ui.pPnlSingleHead->addChild(headIcon);
		}

		headIcon->setStarsNum(heroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
		headIcon->setLvlWithoutBg(heroParam->mLevel);

		updateFateInfo();
	}

	m_ui.pPnlContent->runAction(FadeIn::create(1.0f));
}

void LayerHeroRelation::updateFateInfo()
{
	//非淬炼分支
	if(!m_bIsEquipCL)
	{
		//获取受关联的英雄ID列表
		vector<sFateHeroOrder> vFateHeroId;

		//获取缘分ID列表
		vector<int> vFatedId;

		//缘分属性
		vector<sFateAttri> vFatedAttri;

		HeroRelationModel::getInstance()->getFatedHeroId(m_iTempId, vFateHeroId);
		HeroRelationModel::getInstance()->getFatedIdList(vFatedId);
		HeroRelationModel::getInstance()->getFatedAttri(vFatedAttri);

		//影响英雄
		if(vFatedAttri.size() == 0)
		{
			m_ui.pPnlAttri_1->setVisible(false);
			m_ui.pPnlAttri_2->setVisible(false);
			m_ui.pPnlAttri_3->setVisible(false);
		}
		else if(vFatedAttri.size() == 1)
		{
			m_ui.pPnlAttri_1->setVisible(true);
			m_ui.pPnlAttri_2->setVisible(false);
			m_ui.pPnlAttri_3->setVisible(false);
		}
		else if(vFatedAttri.size() == 2)
		{
			m_ui.pPnlAttri_1->setVisible(true);
			m_ui.pPnlAttri_2->setVisible(true);
			m_ui.pPnlAttri_3->setVisible(false);
		}
		else if(vFatedAttri.size() >= 3)
		{
			m_ui.pPnlAttri_1->setVisible(true);
			m_ui.pPnlAttri_2->setVisible(true);
			m_ui.pPnlAttri_3->setVisible(true);
		}

		//更新缘分前需先解析上一等级缘分属性
		if(vFatedAttri.size() > 0)
		{
			calculateFrontAdvLv();
		}

		updateFateDetails();
	}
}

void LayerHeroRelation::updateFateDetails()
{
	//缘分属性
	vector<sFateAttri> vFatedAttri;
	HeroRelationModel::getInstance()->getFatedAttri(vFatedAttri);

	int attriCnt = vFatedAttri.size();

	if(attriCnt == 0)
	{
		return;
	}

	for(int i=0; i<attriCnt; i++)
	{
		Layout* _parent = nullptr;
		if(i == 0)
		{
			_parent = m_ui.pPnlAttri_1;
		}
		else if(i == 1)
		{
			_parent = m_ui.pPnlAttri_2;
		}
		else
		{
			_parent = m_ui.pPnlAttri_3;
		}
		
		if(_parent == nullptr)
		{
			continue;
		}
		Layout* pnlIcon = (Layout*)_parent->getChildByName("Pnl_headIcon");
		Text*	labLevel = (Text*)_parent->getChildByName("Text_FateNameAndLevel");
		Text*	labOldAttri = (Text*)_parent->getChildByName("Text_OldProperty");
		Text*	labNewAttri = (Text*)_parent->getChildByName("Text_NewProperty");

		UIToolHeader* head = UIToolHeader::create(vFatedAttri.at(i).fateHeroId);
		pnlIcon->addChild(head);
		pnlIcon->setScale(0.6f);
		head->setPosition(Point(pnlIcon->getContentSize()/2) + Point(0, 24));

		labLevel->setString(vFatedAttri.at(i).fateName + " lv" + TO_STR(vFatedAttri.at(i).fateLevel));
		labOldAttri->setString(vFatedAttri.at(i).attriChName + "+" + TO_STR(0) + "%");
		labNewAttri->setString(vFatedAttri.at(i).attriChName + "+" + TO_STR(vFatedAttri.at(i).attriValue*100) + "%");

		labLevel->setFontName(FONT_FZZHENGHEI);
		labOldAttri->setFontName(FONT_FZZHENGHEI);
		labNewAttri->setFontName(FONT_FZZHENGHEI);

		if(m_bIsAdv && i<m_vFrontFatedAttri.size())
		{
			//此处应该利用上一等级缘分的属性，又要重新去计算缘分ID，oh，老天!
			labOldAttri->setString(m_vFrontFatedAttri.at(i).attriChName + "+" + TO_STR(m_vFrontFatedAttri.at(i).attriValue*100) + "%");
		}
	}
}

void LayerHeroRelation::calculateFrontAdvLv()
{
	HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
	if(hero == nullptr)
	{
		return;
	}
	if(hero->mAdvLvl == 1)
	{
		return;
	}

	//解析缘分觉醒等级
	vector<sFateHeroOrder> vFatedLv;
	RowData* _rowData = DataManager::getInstance()->getDTToolSystem()->searchDataById(HERO_RELATION_LIMIT_ID);
	if(_rowData != nullptr)
	{
		string fateLimit = _rowData->getStringData("value");
		vFatedLv.clear();
		vector<int> vecFateLimitData;
		StringFormat::parseCsvStringVec(fateLimit, vecFateLimitData);
		for(int i=0; i<vecFateLimitData.size(); i++)
		{
			sFateHeroOrder fateLimitData;
			fateLimitData.valueData = vecFateLimitData.at(i);
			fateLimitData.order = i+1;
			vFatedLv.push_back(fateLimitData);
		}
	}

	//查上一级缘分对应的觉醒等级
	int frontAdvLv = 0;
	for(int i=0; i<vFatedLv.size(); i++)
	{
		if(hero->mAdvLvl <= vFatedLv.at(i).valueData)
		{
			if(i > 0)
			{
				m_iFrontAdvLv = vFatedLv.at(i-1).valueData;
				break;
			}
		}
	}

	//构建上一等级的缘分属性
	m_vFrontFatedAttri.clear();

	//缘分属性
	vector<sFateAttri> vFatedAttri;
	HeroRelationModel::getInstance()->getFatedAttri(vFatedAttri);
	for(int i=0; i<vFatedAttri.size(); i++)
	{
		sFateAttri fateAttriData;
		//缘分ID
		int fateId = vFatedAttri.at(i).fateId - hero->mAdvLvl + m_iFrontAdvLv;

		//查缘分属性表
		RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(fateId);
		if(_rowData != nullptr)
		{
			fateAttriData.fateId = fateId;
			fateAttriData.fateType = _rowData->getIntData("type");
			fateAttriData.fateLevel = _rowData->getIntData("level");
			fateAttriData.fateHeroId = _rowData->getIntData("hero");
			fateAttriData.attriDes = _rowData->getStringData("dec");
			fateAttriData.attriDes2 = _rowData->getStringData("dec2");

			fateAttriData.isFateActed = true;
			fateAttriData.attriEnName = vFatedAttri.at(i).attriEnName;
			fateAttriData.attriChName = vFatedAttri.at(i).attriChName;
			fateAttriData.attriValue = _rowData->getFloatData(fateAttriData.attriEnName);
		
			m_vFrontFatedAttri.push_back(fateAttriData);
		}
	}
}

void LayerHeroRelation::playBgAni()
{
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");
	Armature* ani = Armature::create("Anitupo1");

	ani->getAnimation()->play(aniName1);

	Bone*  bone = ani->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = ani->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		if(m_bIsAdv)
		{
			//缘分提升
			Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_yuanfentisheng.png");
			bone->addDisplay(_newSkin,0);
			Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_yuanfentisheng.png");
			bone1->addDisplay(_newSkin1, 0);
		}
		else 
		{
			//获得缘分
			Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_huodeyuanfen.png");
			bone->addDisplay(_newSkin,0);
			Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_huodeyuanfen.png");
			bone1->addDisplay(_newSkin1, 0);
		}
	}

	ani->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerHeroRelation::aniAniCallBack, this, aniName2));
	ani->setPosition(Vec2(this->getContentSize() / 2) + Vec2(0, 55));
	ani->setScale(0.8f);
	m_ui.pRoot->addChild(ani);
}

void LayerHeroRelation::updateViewByCL()
{
	//影响英雄
	m_ui.pPnlAttri_1->setVisible(true);
	m_ui.pPnlAttri_2->setVisible(false);
	m_ui.pPnlAttri_3->setVisible(false);

	//缘分ID
	int fateId = 0;

	if(m_clType == EQUIP_ITEM_TYPE_BOOK)
	{
		//兵书赋予的缘分
		fateId = m_iTempId*1000 + 5*100 + 1;
	}
	else if(m_clType == EQUIP_ITEM_TYPE_HORSE)
	{
		//马书赋予的缘分
		fateId = m_iTempId*1000 + 4*100 + 1;
	}
	else if(m_clType == EQUIP_ITEM_TYPE_WEAPON)
	{
		//武器赋予的缘分
		fateId = m_iTempId*1000 + 6*100 + 1;
	}

	//查缘分表
	RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(fateId);
	if(_rowData != nullptr)
	{
		Layout* pnlIcon = (Layout*)m_ui.pPnlAttri_1->getChildByName("Pnl_headIcon");
		Text*	labLevel = (Text*)m_ui.pPnlAttri_1->getChildByName("Text_FateNameAndLevel");
		Text*	labOldAttri = (Text*)m_ui.pPnlAttri_1->getChildByName("Text_OldProperty");
		Text*	labNewAttri = (Text*)m_ui.pPnlAttri_1->getChildByName("Text_NewProperty");

		UIToolHeader* head = UIToolHeader::create(m_iTempId);
		pnlIcon->addChild(head);
		pnlIcon->setScale(0.6f);
		head->setPosition(Point(pnlIcon->getContentSize()/2) + Point(0, 24));

		labLevel->setString(_rowData->getStringData("name") + " lv" + TO_STR(1));

		//如果是武器淬炼，显示技能名称变化
		if(m_clType == EQUIP_ITEM_TYPE_WEAPON)
		{
			vector<string> vecSkillName;
			StringFormat::parseCsvStringVec(_rowData->getStringData("skill"), vecSkillName);
			labOldAttri->setString(vecSkillName.front());
			labNewAttri->setString(vecSkillName.back());
		}
		else
		{
			string attriChName = "";
			float attriVal = 0.0f;
			if(_rowData->getFloatData("hpCoe") != 0)
			{
				attriChName = DataManager::getInstance()->searchCommonTexdtById(10008);
				attriVal = _rowData->getFloatData("hpCoe");
			}
			else if(_rowData->getFloatData("atkCoe") != 0)
			{
				attriChName = DataManager::getInstance()->searchCommonTexdtById(10009);
				attriVal = _rowData->getFloatData("atkCoe");
			}
			else if(_rowData->getFloatData("pddCoe") != 0)
			{
				attriChName = DataManager::getInstance()->searchCommonTexdtById(10244);
				attriVal = _rowData->getFloatData("pddCoe");
			}

			labOldAttri->setString(attriChName + "+" + TO_STR(0) + "%");
			labNewAttri->setString(attriChName + "+" + TO_STR(attriVal*100) + "%");
		}

		labLevel->setFontName(FONT_FZZHENGHEI);
		labOldAttri->setFontName(FONT_FZZHENGHEI);
		labNewAttri->setFontName(FONT_FZZHENGHEI);
	}	
}
