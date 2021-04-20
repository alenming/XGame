#include "SkillTips.h"
#include "DataManager/DataManager.h"
#include "Common/Common.h"


SkillTips::SkillTips(void)
{
	m_nDirection = 0;
}

SkillTips::~SkillTips(void)
{

}

bool SkillTips::init()
{
	if (!Node::init())
	{
		return false;
	}

	initUI();

	return true;
}

void SkillTips::initUI()
{
	// 底图;
	string strFileName;
	strFileName.assign("Image/UICommon/SkillTips.ExportJson");
	// Root;
	m_ui.imgBg = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.imgBg);
	m_ui.imgBg->setPosition(Vec2(-m_ui.imgBg->getContentSize().width/2,-m_ui.imgBg->getContentSize().height/2));

	// 技能底图;
	m_ui.imgPanel = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgBg,"Image_23")); 

	// 技能类型;
	m_ui.txtSkillType = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgBg,"name_0")); 
	m_ui.txtSkillType->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSkillTypePos = m_ui.txtSkillType->getPosition();

	// 技能名;
	m_ui.txtSkillName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgBg,"name")); 
	m_ui.txtSkillName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSkillNamePos = m_ui.txtSkillName->getPosition();
	
	// 技能描述
	m_ui.txtSkillDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgBg,"desc")); 
	m_ui.txtSkillDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSkillDescPos = m_ui.txtSkillDesc->getPosition();
}

void SkillTips::setTipsContent(int skillType, string skillName, string skillDesc, int nDirection)
{
	string strType;
	Color3B color;

	switch (skillType)
	{
	case 1:
		strType.assign(DataManager::getInstance()->searchCommonTexdtById(20008));
		color = Color3B(231, 219, 169);
		break;
	case 2:
		strType.assign(DataManager::getInstance()->searchCommonTexdtById(20009));
		color = Color3B(240, 119, 184);
		break;
	case 3:
		strType.assign(DataManager::getInstance()->searchCommonTexdtById(20010));
		color = Color3B(244, 188, 234);
		break;
	case 4:
		strType.assign(DataManager::getInstance()->searchCommonTexdtById(20011));
		color = Color3B(182, 225, 243);
		break;
	case 5:
		strType.assign(DataManager::getInstance()->searchCommonTexdtById(20012));
		color = Color3B(174, 231, 169);
		break;
	default:
		return;
	}

	m_ui.txtSkillType->setString(strType);
	m_ui.txtSkillType->setColor(color);
	m_ui.txtSkillName->setString(skillName);
	m_ui.txtSkillName->setColor(color);
	
	// 底图方向;
	m_ui.imgPanel->setRotationSkewY((nDirection == 0) ? 0.0f : 180.0f);

	if (m_ui.txtSkillDesc->getChildByName("SkillName"))
	{
		m_ui.txtSkillDesc->removeChildByName("SkillName");
	}

	SimpleRichText *richText = SimpleRichText::create();
	if (nullptr != richText)
	{
		richText->ignoreContentAdaptWithSize(false);
		richText->setAnchorPoint(Vec2(0,1));
		richText->setContentSize(m_ui.txtSkillDesc->getContentSize());
		richText->setPosition(Vec2(-100,m_ui.txtSkillDesc->getContentSize().height+60));
		richText->setString(skillDesc,20,Color3B::WHITE);
		m_ui.txtSkillDesc->addChild(richText,0,"SkillName");
	}

	// 文字位置修正;
	Point shift(0.0f, 0.0f);
	if (nDirection != 0)
	{
		shift = Point(13.0f, 0.0f);
	}
	m_ui.txtSkillType->setPosition(shift + m_ui.txtSkillTypePos);
	m_ui.txtSkillName->setPosition(shift + m_ui.txtSkillNamePos);
	m_ui.txtSkillDesc->setPosition(shift + m_ui.txtSkillDescPos);
}

void SkillTips::setSkill(int skillId, int nDirection)
{
	RowData* dataSkill = DataManager::getInstance()->searchSkillById(skillId);

	if (dataSkill)
	{
		int skillType = dataSkill->getIntData("sklTyp");
		string skillName = dataSkill->getStringData("name");
		string skillDesc = dataSkill->getStringData("capTxt");

		setTipsContent(skillType, skillName, skillDesc, nDirection);
	}
}