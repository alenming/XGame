#include "GroupBossSkillTips.h"
#include "DataManager/DataManager.h"
#include "Widget/SimpleRichText.h"
#include "UI/UIDef.h"

GroupBossSkillTips::GroupBossSkillTips()
{

}

GroupBossSkillTips::~GroupBossSkillTips()
{

}

GroupBossSkillTips* GroupBossSkillTips::create(int bossId, int idx,const string str,Color3B color )
{
	auto pRet = new GroupBossSkillTips(); 
	if (pRet && pRet->init(bossId, idx, str, color)) 
	{
		pRet->autorelease(); 
		return pRet; 
	}
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

bool GroupBossSkillTips::init(int bossId, int idx, string str, Color3B color )
{
	if(!ModalLayer::init())
	{
		return false;
	}
	m_skillIdx = idx;
	m_bossId = bossId;
	m_strDescrip = str;
	m_color = color;

	initUI();

	//点击空白处销毁
	this->setClickEmptyDispose(true);

	return true;
}

void GroupBossSkillTips::onEnter()
{
	ModalLayer::onEnter();
}

void GroupBossSkillTips::onExit()
{
	ModalLayer::onExit();
}

void GroupBossSkillTips::initUI()
{
	// 加载 UI;
	string strFileName = "Image/UI_bangpaiBOSS/UI_bangpaiBOSS_SkillTips.ExportJson";
	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);

	m_ui.pImgName = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_skill"));
	m_ui.pSkill = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_name2"));
	m_ui.pPnlDescrip = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_tips"));
	m_ui.pPnlCD = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_CD"));
	m_ui.pLabCD = dynamic_cast<Text*>(m_ui.pPnlCD->getChildByName("Lab_CD"));
	m_ui.pLabEnterCD = dynamic_cast<Text*>(m_ui.pPnlCD->getChildByName("Lab_EnterCD"));
	m_ui.pLabNuQi = dynamic_cast<Text*>(m_ui.pPnlCD->getChildByName("Lab_xiaoHao"));

	m_ui.pSkill->setString("");
	m_ui.pLabCD->setFontName(FONT_FZZHENGHEI);
	m_ui.pLabEnterCD->setFontName(FONT_FZZHENGHEI);
	m_ui.pLabNuQi->setFontName(FONT_FZZHENGHEI);
	m_ui.pSkill->setFontName(FONT_FZZHENGHEI);
	
	//查表
	RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_bossId);
	if(pRowPetData != nullptr)
	{
		RowData* pSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(m_strDescrip));

		if(pSkillData != nullptr)
		{
			m_ui.pSkill->setString(pSkillData->getStringData("name"));
			m_ui.pSkill->setColor(m_color);

			m_ui.pPnlDescrip->removeAllChildren();
			SimpleRichText *richText = SimpleRichText::create();
			if (nullptr != richText)
			{
				richText->ignoreContentAdaptWithSize(false);
				richText->setAnchorPoint(Vec2(0.5,0.5));
				richText->setPosition(Point(180, 32));
				richText->setContentSize(m_ui.pPnlDescrip->getContentSize());
				richText->setString(pSkillData->getStringData("capTxt"),22,Color3B(0xaf, 0x8f, 0x7d));
				m_ui.pPnlDescrip->addChild(richText,0,"SkillName");
			}
		}
	}

	//技能类型
	if(m_skillIdx == 0)
	{
		m_ui.pImgName->loadTexture("Image/Icon/global/atkID.png");
		m_ui.pPnlCD->setVisible(false);
	}
	else if(m_skillIdx == 1)
	{
		m_ui.pImgName->loadTexture("Image/Icon/global/a_sklID.png");
		m_ui.pPnlCD->setVisible(true);

		//查技能表，CD相关
		RowData* pSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(m_strDescrip));
		if(pSkillData)
		{
			int skillCD = pSkillData->getIntData("skl_cd");
			int skillInCD = pSkillData->getIntData("ini_cd");
			int skillSP = pSkillData->getIntData("sp");

			m_ui.pLabCD->setString(TO_STR(skillCD));
			m_ui.pLabEnterCD->setString(TO_STR(skillInCD));
			m_ui.pLabNuQi->setString(TO_STR(skillSP));
		}
	}
	else if(m_skillIdx == 2)
	{
		m_ui.pImgName->loadTexture("Image/Icon/global/p1_sklID.png");
		m_ui.pPnlCD->setVisible(false);
	}
	else if(m_skillIdx == 3)
	{
		m_ui.pImgName->loadTexture("Image/Icon/global/p2_sklID.png");
		m_ui.pPnlCD->setVisible(false);
	}
	else if(m_skillIdx == 4)
	{
		m_ui.pImgName->loadTexture("Image/Icon/global/p3_sklID.png");
		m_ui.pPnlCD->setVisible(false);
	}

	this->setOpacity(0);
}

