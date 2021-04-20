#include "LimitShowBoxLayer.h"
#include "LimitTimeHeroModel.h"
#include "ToolModule/UIToolHeader.h"
#include "Common/Common.h"
#define  TREE_ITEM	3

LimitShowBoxLayer::LimitShowBoxLayer():
	_boxId(0)
{
}

LimitShowBoxLayer::~LimitShowBoxLayer()
{
}

LimitShowBoxLayer* LimitShowBoxLayer::create(int boxId)
{ 
	auto pRet = new LimitShowBoxLayer(); 
	if (pRet && pRet->init(boxId)) 
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

bool LimitShowBoxLayer::init(int boxId)
{
	if(!ModalLayer::init())
	{
		return false;
	}
	_boxId = boxId;

	initUI();
	//点击空白处销毁
	this->setClickEmptyDispose(true);
	return true;
}

void LimitShowBoxLayer::initUI()
{
	// 加载 UI;
	string strFileName = "Image/UIxssj/UIxssj_reward.ExportJson";
	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.uiRoot);


	m_ui.Label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Label_number"));
	for (int i = 1; i <= TREE_ITEM; i++)
	{
		SHOWBOX_ONEITEM oneItem;
		oneItem.Label_itemnumber = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Label_itemnumber_"+TO_STR(i)));
		oneItem.Panel_reward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Panel_reward_"+TO_STR(i)));
		oneItem.Label_itemnumber->setVisible(false);
		oneItem.Panel_reward->setVisible(false);
		m_ui._allItem.push_back(oneItem);
	}
	auto Image_bgreward = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Image_bgreward"));
	this->setOpacity(0);
	//this->setContentSize(Image_bgreward->getContentSize());
}

void LimitShowBoxLayer::onEnter()
{
	ModalLayer::onEnter();

	//显示的道具
	auto allItem = LimitTimeHeroModel::getInstance()->getRewardBoxById(_boxId)->vecRewardData;
	for (unsigned int i = 0; i < allItem.size(); i++)
	{
		auto oneItem = allItem.at(i);
		auto _layout = dynamic_cast<Layout*>(m_ui._allItem.at(i).Panel_reward);
		_layout->removeAllChildren();
		UIToolHeader* _item = UIToolHeader::create(oneItem.rewardId);
		_item->setAnchorPoint(Vec2(0,0));
		_item->setPosition(Vec2(0,0));
		_item->setScale(0.87f);
		_item->setNumEx(oneItem.rewardCnt);
		_item->setTipsEnabled(true);
		_item->setToolId(oneItem.rewardId);;
		_layout->addChild(_item);
		_layout->setVisible(true);

		auto _text = dynamic_cast<Text*>(m_ui._allItem.at(i).Label_itemnumber);
		RowData* _row = DataManager::getInstance()->searchToolById(oneItem.rewardId);
		std::string strName = (_row == nullptr) ? "" : _row->getStringData("itemName");
		_text->setString(strName);
		_text->setFontName(FONT_FZZHENGHEI);
		_text->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(_text, Color3B(0x79, 0x03, 0x03),2);
	}

	m_ui.Label_number->setString(TO_STR(LimitTimeHeroModel::getInstance()->getRewardBoxById(_boxId)->scorePoint));
	m_ui.Label_number->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_number->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.Label_number, Color3B(0x79, 0x03, 0x03),2);
}

void LimitShowBoxLayer::onExit()
{
	ModalLayer::onExit();
}



