#include "UICardHead.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include "HeroModule/LayerHero.h"

const float ScaleRadio = 0.75f;

UICardHead::UICardHead(void)
{
	m_pRoot = nullptr;
	m_pBackImg = nullptr;
	m_pPro = nullptr;
	m_lastIndex = -1;

	for (int i = 0; i < SKILL_CONT; i++)
	{
		m_pSkill[i] = nullptr;
	}

	for (int i = 0; i < STAR_CONT; i++)
	{
		m_pStar[i] = nullptr;
	}
	m_pName = nullptr;   
}


UICardHead::~UICardHead(void)
{
	
}

UICardHead* UICardHead::create(int id,bool isLineup /*= false*/,bool isLimitBuy /*= false*/)
{
	//背景设置
	UICardHead* card = new UICardHead;
	if (card && card->init())
	{
		card->autorelease();
		card->initUi(id,isLineup,isLimitBuy);
		return card;
	}

	CC_SAFE_DELETE(card);
	return nullptr;
}

bool UICardHead::initUi(int id,bool isLineup,bool isLimitBuy)
{
	m_isLineup = isLineup;
	m_isLimitBuy = isLimitBuy;
	m_id = id;

	// 加载Sign UI;
	string strFileName;
	strFileName.assign("Image/UICommon/UIxianshiCard.ExportJson");
	// Root;
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->setContentSize(m_pRoot->getContentSize());
	this->addChild(m_pRoot);

	m_scaleImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_6"));

	//背景
	m_pBackImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_back"));
	
	Layout* Cardpanel = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot,"mask"));
	Cardpanel->addTouchEventListener(CC_CALLBACK_2(UICardHead::onBtnSkillPanel,this));
	//////////////////////////////////////////////////////////////////////////
	
	//名字
	m_pName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot,"Lab_RoleName"));
	m_pName->setFontName(FONT_FZZHENGHEI);
	
	//星星
	for (int i = 0; i < STAR_CONT; i++)
	{
		m_pStar[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Img_Star"+TO_STR(i+1)));
	}

	m_pPro = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Img_Type"));
	m_pPro->setScale(1.0f);

	updataUi();

	return true;
}

void UICardHead::updataUi()
{
	//////////////////////////////////////////////////////////////////////////
	//先查道具表
	RowData* pRowToolData = DataManager::getInstance()->searchToolById(m_id);
	int resId = pRowToolData->getIntData("resId");
	
	auto stencil = Sprite::create("Image/Icon/global/zise_di.png");
	auto clipper = ClippingNode::create();
	clipper->setStencil(stencil);//设置裁剪模板 //3
	clipper->setInverted(false);//设置底板可见
	clipper->setAlphaThreshold(0);//设置绘制底板的Alpha值为0
	clipper->setPosition(m_pBackImg->getPosition());
	m_scaleImg->addChild(clipper,1);//4
	auto content = Sprite::create(ResourceUtils::getHeroCompleteCardPath(resId));//被裁剪的内容
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(m_id);
	vector<float> pointVect;
	StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
	content->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
	content->setPosition(Vec2(0,-120));
	clipper->addChild(content);//5

	//差petbase表
	RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_id);
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_id);

	//名字
	if(heroParam && !m_isLimitBuy)
	{
		m_pName->setString(heroParam->getAdvName());
		//重置英雄名字颜色
		LayerHero::setNameColor(m_pName, heroParam->mQuality);
	}
	else
	{
		if(pRowPetData)
		{
			m_pName->setString(pRowPetData->getStringData("name"));
			LayerHero::setNameColor(m_pName, pRowPetData->getIntData("qua"));
		}	
	}
	setTextAddStroke(m_pName,Color3B(0x82, 0x2b, 0x2b),2);

	//星星
	int stars = 0;
	if (m_isLineup)
	{
		if (heroParam)
		{
			stars = heroParam->mHeroStars;		
		}
		else
		{
			stars = pRowPetData->getIntData("stars");
		}
	}
	else
	{
		stars = pRowPetData->getIntData("stars");
	}

	for (int i = 0; i < STAR_CONT; i++)
	{
		if (i < stars)
		{
			m_pStar[i]->setVisible(true);
		}
		else
		{
			m_pStar[i]->setVisible(false);
		}
	}

	m_pPro->setVisible(true);
	if(pRowPetData)
	{
		string imgPath;
		switch (pRowPetData->getIntData("jobType"))
		{
		case HERO_ATK: //攻击
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing0_gong.png");
				m_pPro->loadTexture(imgPath);
			}
			break;
		case HERO_DEFENSE: //防御
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing0_fang.png");
				m_pPro->loadTexture(imgPath);
			}
			break;
		case HERO_HELP: //辅助
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing0_fu.png");
				m_pPro->loadTexture(imgPath);
			}
			break;
		case HERO_FIRST: //先手
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing0_xian.png");
				m_pPro->loadTexture(imgPath);
			}
			break;

		default:
			break;
		}
	}
	
	initSkillPanel();

	initSkill(m_id);
}


void UICardHead::initSkill(int id)
{
	//技能
	for (int i = 0; i < SKILL_CONT; i++)
	{
		m_pSkill[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"skill_"+TO_STR(i+1)));
		m_pSkillButton[i] = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_pRoot,"Img_skill_boder"+TO_STR(i+1)));

		ImageView *zhezhao = dynamic_cast<ImageView*>(m_pSkillButton[i]->getChildByName("zhe"));
		Text *active = dynamic_cast<Text*>(zhezhao->getChildByName("active"));
		active->setFontName(FONT_FZZHENGHEI);
		//setTextAddStroke(active,Color3B(0xeb,0xe3,0xd9),2);
		
		if (m_isLineup)
		{
			HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_id);
			RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_id);

			//进阶条件
			vector<int> mAdvLvl;
			StringFormat::parseCsvStringVec(pRowPetData->getStringData("open"),mAdvLvl);
			if (heroParam && !heroParam->isMainHero() && heroParam->mQuality < mAdvLvl[i] && mAdvLvl[i] > 0)
			{
				zhezhao->setVisible(true);
			}
			else
			{
				zhezhao->setVisible(false);
			}
		}
		else
		{
			zhezhao->setVisible(false);
		}

		m_pSkill[i]->setVisible(false);
	}

	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_id);
	if (heroParam && heroParam->isMainHero()) //主角
	{
		if (heroParam->mSex == HERO_SEX_MALE)
		{
			setSkill(0,heroParam->mNormalSkillId,1001);
			m_pSkillButton[0]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,0,"atkID",
				heroParam->mNormalSkillId,1001,Color3B(231, 219, 169)));

			setSkill(1,heroParam->mActiveSkillId,1002);
			m_pSkillButton[1]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,1,"a_sklID",
				heroParam->mActiveSkillId,1002,Color3B(240, 119, 184)));

			setSkill(2,heroParam->mRushSkillId,1003);
			m_pSkillButton[2]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,2,"p1_sklID",
				heroParam->mRushSkillId,1003,Color3B(244, 188, 234)));

			setSkill(3,heroParam->mBuffSkillId,1004);
			m_pSkillButton[3]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,3,"p2_sklID",
				heroParam->mBuffSkillId,1004,Color3B(182, 225, 243)));

			setSkill(4,heroParam->mTriggerSkillId,1005);
			m_pSkillButton[4]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,4,"p3_sklID",
				heroParam->mTriggerSkillId,1005,Color3B(174, 231, 169)));
		}
		else if (heroParam->mSex == HERO_SEX_FEMALE)
		{
			setSkill(0,heroParam->mNormalSkillId,2001);
			m_pSkillButton[0]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,0,"atkID",
				heroParam->mNormalSkillId,2001,Color3B(231, 219, 169)));

			setSkill(1,heroParam->mActiveSkillId,2002);
			m_pSkillButton[1]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,1,"a_sklID",
				heroParam->mActiveSkillId,2002,Color3B(240, 119, 184)));

			setSkill(2,heroParam->mRushSkillId,2003);
			m_pSkillButton[2]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,2,"p1_sklID",
				heroParam->mRushSkillId,2003,Color3B(244, 188, 234)));

			setSkill(3,heroParam->mBuffSkillId,2004);
			m_pSkillButton[3]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,3,"p2_sklID",
				heroParam->mBuffSkillId,2004,Color3B(182, 225, 243)));

			setSkill(4,heroParam->mTriggerSkillId,2005);
			m_pSkillButton[4]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClickPlayer,this,4,"p3_sklID",
				heroParam->mTriggerSkillId,2005,Color3B(174, 231, 169)));
		}
	}
	else
	{
		RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_id);
		//攻击技能
		setSkill(0,pRowPetData,"atkID");
		m_pSkillButton[0]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClick,this,0,"atkID",Color3B(231, 219, 169)));

		//主动技能
		setSkill(1,pRowPetData,"a_sklID");
		m_pSkillButton[1]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClick,this,1,"a_sklID",Color3B(240, 119, 184)));

		//被动1技能
		setSkill(2,pRowPetData,"p1_sklID");
		m_pSkillButton[2]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClick,this,2,"p1_sklID",Color3B(244, 188, 234)));

		//被动2技能
		setSkill(3,pRowPetData,"p2_sklID");
		m_pSkillButton[3]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClick,this,3,"p2_sklID",Color3B(182, 225, 243)));

		//被动3技能
		setSkill(4,pRowPetData,"p3_sklID");
		m_pSkillButton[4]->addEventListener(CC_CALLBACK_2(UICardHead::onBtnClick,this,4,"p3_sklID",Color3B(174, 231, 169)));
	}
}

void UICardHead::setSkill(const int idx,RowData* pRowPetData,const string& str)
{
	RowData* pRowSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(str));
	if (pRowSkillData)
	{
		m_pSkill[idx]->loadTexture(ResourceUtils::getSmallIconPath(pRowSkillData->getIntData("resId")));
		m_pSkill[idx]->setVisible(true);
	}
	else
	{
		m_pSkill[idx]->setVisible(false);
	}
}

void UICardHead::setSkill(const int idx,const int skillId,const int skillKey)
{
	if (skillId > -1)
	{
		RowData* pRowSkillData = DataManager::getInstance()->searchSkillById(skillId);
		if (pRowSkillData)
		{
			m_pSkill[idx]->loadTexture(ResourceUtils::getSmallIconPath(pRowSkillData->getIntData("resId")));
			m_pSkill[idx]->setVisible(true);
		}
		else
		{
			m_pSkill[idx]->setVisible(false);
		}
	}
	else
	{
		RowData* pRowSkillDataKey = DataManager::getInstance()->searchTalentSkillById(skillKey);
		vector<int> mskill;
		StringFormat::parseCsvStringVec(pRowSkillDataKey->getStringData("skillId"),mskill);

		RowData* pRowSkillData = DataManager::getInstance()->searchSkillById(mskill[0]);

		if (pRowSkillData)
		{
			m_pSkill[idx]->loadTexture(ResourceUtils::getSmallIconPath(pRowSkillData->getIntData("resId")));
			m_pSkill[idx]->setVisible(true);
		}
		else
		{
			m_pSkill[idx]->setVisible(false);
		}

		ImageView *zhezhao = dynamic_cast<ImageView*>(m_pSkillButton[idx]->getChildByName("zhe"));
		if (m_isLineup)
		{
			zhezhao->setVisible(true);
		}
		else
		{
			zhezhao->setVisible(false);
		}
	}
}

void UICardHead::onBtnClick(Ref* ref, CheckBox::EventType type,int idx,const string& str,Color3B color)
{
	SoundUtils::playSoundEffect("dianji");
	if (type == CheckBox::EventType::SELECTED)
	{
		m_pSkillPanel->setVisible(true);

		if (m_lastIndex > -1)
		{
			m_pSkillButton[m_lastIndex]->setSelectedState(false);
		}
		m_lastIndex = idx;
		m_pSkillButton[m_lastIndex]->setSelectedState(true);


		RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_id);
		RowData* pSkillData = nullptr;
		if (m_isLineup)
		{
			HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_id);
			if (idx == 0)
			{
				pSkillData = DataManager::getInstance()->searchSkillById(heroParam->mNormalSkillId);
			}
			else if (idx == 1)
			{
				pSkillData = DataManager::getInstance()->searchSkillById(heroParam->mActiveSkillId);
			}
			else if (idx == 2)
			{
				pSkillData = DataManager::getInstance()->searchSkillById(heroParam->mRushSkillId);
			}
			else if (idx == 3)
			{
				pSkillData = DataManager::getInstance()->searchSkillById(heroParam->mBuffSkillId);
			}
			else if (idx == 4)
			{
				pSkillData = DataManager::getInstance()->searchSkillById(heroParam->mTriggerSkillId);
			}
		}
		else
		{
			pSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(str));
		}

		if (!pSkillData)
		{
			pSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(str));
		}

		m_pSkillPanelImg->loadTexture("Image/Icon/global/" + str +".png"); //
		m_pSkillPanelName->setText(pSkillData->getStringData("name"));
		m_pSkillPanelName->setColor(color);

		if (m_pSkillPanelDesc->getChildByName("SkillName"))
		{
			m_pSkillPanelDesc->removeChildByName("SkillName");
		}

		SimpleRichText *richText = SimpleRichText::create();
		if (nullptr != richText)
		{
			richText->ignoreContentAdaptWithSize(false);
			richText->setAnchorPoint(Vec2(0,1));
			richText->setContentSize(m_pSkillPanelDesc->getContentSize());
			richText->setPosition(Vec2(-150,m_pSkillPanelDesc->getContentSize().height+50));
			richText->setString(pSkillData->getStringData("capTxt"),22,Color3B(0xaf, 0x8f, 0x7d));
			m_pSkillPanelDesc->addChild(richText,0,"SkillName");
		}
		
		vector<int> mAdvLvl;
		StringFormat::parseCsvStringVec(pRowPetData->getStringData("open"),mAdvLvl);
		HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_id);
		if (heroParam && !heroParam->isMainHero() && heroParam->mQuality < mAdvLvl[idx] && mAdvLvl[idx] > 0)
		{
			m_pSkillPanelActive->setVisible(true);

			switch (mAdvLvl[idx])
			{
			case 2:
				m_pSkillPanelActive->setText(DataManager::getInstance()->searchCommonTexdtById(20104));
				break;
			case 3:
				m_pSkillPanelActive->setText(DataManager::getInstance()->searchCommonTexdtById(20105));
				break;
			case 4:
				m_pSkillPanelActive->setText(DataManager::getInstance()->searchCommonTexdtById(20106));
				break;
			default:
				break;
			}

		}
		else
		{
			m_pSkillPanelActive->setVisible(false);
		}

		if (pSkillData->getIntData("skl_cd") < 0)
		{
			m_pSkillPanelCD1->setVisible(false);
		}
		else
		{
			m_pSkillPanelCD1->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCD1->getChildByName("text"))->setText(pSkillData->getStringData("skl_cd"));
		}

		if (pSkillData->getIntData("ini_cd") < 0)
		{
			m_pSkillPanelCD2->setVisible(false);
		}
		else
		{
			m_pSkillPanelCD2->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCD2->getChildByName("text"))->setText(pSkillData->getStringData("ini_cd"));
		}

		if (pSkillData->getIntData("sp") < 0)
		{
			m_pSkillPanelCost->setVisible(false);
		}
		else
		{
			m_pSkillPanelCost->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCost->getChildByName("text"))->setText(pSkillData->getStringData("sp"));
		}
	}
	else if (type == CheckBox::EventType::UNSELECTED)
	{
		m_pSkillPanel->setVisible(false);
	}
}

void UICardHead::onBtnClickPlayer(Ref* ref, CheckBox::EventType type,int idx,const string& str,const int skillId,const int skillKey,Color3B color)
{
	SoundUtils::playSoundEffect("dianji");
	if (type == CheckBox::EventType::SELECTED)
	{
		m_pSkillPanel->setVisible(true);

		if (m_lastIndex > -1)
		{
			m_pSkillButton[m_lastIndex]->setSelectedState(false);
		}
		m_lastIndex = idx;
		m_pSkillButton[m_lastIndex]->setSelectedState(true);

		RowData* pSkillData = nullptr;
		RowData* pRowSkillDataKey = nullptr;

		if (skillId > -1)
		{
			pSkillData = DataManager::getInstance()->searchSkillById(skillId);
		}
		else
		{
			pRowSkillDataKey = DataManager::getInstance()->searchTalentSkillById(skillKey);
			vector<int> mskill;
			StringFormat::parseCsvStringVec(pRowSkillDataKey->getStringData("skillId"),mskill);

			pSkillData = DataManager::getInstance()->searchSkillById(mskill[0]);
		}

		m_pSkillPanelImg->loadTexture("Image/Icon/global/" + str +".png"); //
		m_pSkillPanelName->setText(pSkillData->getStringData("name"));
		m_pSkillPanelName->setColor(color);

		if (m_pSkillPanelDesc->getChildByName("SkillName"))
		{
			m_pSkillPanelDesc->removeChildByName("SkillName");
		}

		SimpleRichText *richText = SimpleRichText::create();
		if (nullptr != richText)
		{
			richText->ignoreContentAdaptWithSize(false);
			richText->setAnchorPoint(Vec2(0,1));
			richText->setContentSize(m_pSkillPanelDesc->getContentSize());
			richText->setPosition(Vec2(-150,m_pSkillPanelDesc->getContentSize().height+30));
			richText->setString(pSkillData->getStringData("capTxt"),22,Color3B(0xaf, 0x8f, 0x7d));
			m_pSkillPanelDesc->addChild(richText,0,"SkillName");
		}

		if (pRowSkillDataKey)
		{
			vector<int> mskill;
			StringFormat::parseCsvStringVec(pRowSkillDataKey->getStringData("openLevel"),mskill);
			
			if (mskill[0] <= 0)
			{
				m_pSkillPanelActive->setVisible(false);
			}
			else
			{
				m_pSkillPanelActive->setVisible(true);
				//越南的跟中文不一样,为了统一版本,跟着越南走
				//越南的 显示的意思是   级 + 1 + 开放  什么意思嘛!!!中文	级激活
				m_pSkillPanelActive->setString("("+ DataManager::getInstance()->searchCommonTexdtById(20329) + " "+
				TO_STR(mskill[0]) + " " + DataManager::getInstance()->searchCommonTexdtById(20107) + ")");
			}
		}
		else
		{
			m_pSkillPanelActive->setVisible(false);
		}
		

		if (pSkillData->getIntData("skl_cd") < 0)
		{
			m_pSkillPanelCD1->setVisible(false);
		}
		else
		{
			m_pSkillPanelCD1->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCD1->getChildByName("text"))->setText(pSkillData->getStringData("skl_cd"));
		}

		if (pSkillData->getIntData("ini_cd") < 0)
		{
			m_pSkillPanelCD2->setVisible(false);
		}
		else
		{
			m_pSkillPanelCD2->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCD2->getChildByName("text"))->setText(pSkillData->getStringData("ini_cd"));
		}

		if (pSkillData->getIntData("sp") < 0)
		{
			m_pSkillPanelCost->setVisible(false);
		}
		else
		{
			m_pSkillPanelCost->setVisible(true);
			dynamic_cast<Text*>(m_pSkillPanelCost->getChildByName("text"))->setText(pSkillData->getStringData("sp"));
		}
	}
	else if (type == CheckBox::EventType::UNSELECTED)
	{
		m_pSkillPanel->setVisible(false);
	}
}

void UICardHead::initSkillPanel()
{
	m_pSkillPanel =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot,"skill_desc"));
	m_pSkillPanel->setVisible(false);
	m_pSkillPanel->addTouchEventListener(CC_CALLBACK_2(UICardHead::onBtnSkillPanel,this));

	m_pSkillPanelImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSkillPanel,"img_name")); 
	m_pSkillPanelName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"name")); 
	m_pSkillPanelName->setFontName(FONT_FZZHENGHEI);
	m_pSkillPanelActive = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"active")); 
	m_pSkillPanelActive->setFontName(FONT_FZZHENGHEI);

	m_pSkillPanelDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"desc")); 
	m_pSkillPanelDesc->setFontName(FONT_FZZHENGHEI);

	m_pSkillPanelCD1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"CD")); 
	m_pSkillPanelCD1->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(m_pSkillPanelCD1->getChildByName("text"))->setFontName(FONT_FZZHENGHEI);

	m_pSkillPanelCD2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"CD2")); 
	m_pSkillPanelCD2->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(m_pSkillPanelCD2->getChildByName("text"))->setFontName(FONT_FZZHENGHEI);

	m_pSkillPanelCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pSkillPanel,"cost")); 
	m_pSkillPanelCost->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(m_pSkillPanelCost->getChildByName("text"))->setFontName(FONT_FZZHENGHEI);

}

void UICardHead::onBtnSkillPanel(Ref* ref, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN && m_pSkillPanel->isVisible())
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_pSkillPanel->setVisible(false);

		if (m_lastIndex > -1)
		{
			m_pSkillButton[m_lastIndex]->setSelectedState(false);
			m_lastIndex = -1;
		}
	}
}

ImageView* UICardHead::getStarImgByStarLevel(int starLevel)
{
	return m_pStar[starLevel-1];
}

ImageView* UICardHead::getSkillImgBySkillType(int skillType)
{
	return m_pSkill[skillType-1];
}