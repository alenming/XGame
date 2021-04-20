#include "GroupBossShowBoxLayer.h"
#include "GroupBossModel.h"
#include "DataManager/DataManager.h"
#include "ToolModule/UIToolHeader.h"
#include "Common/Common.h"


GroupBossShowBoxLayer::GroupBossShowBoxLayer():
	_boxId(0)
{
}

GroupBossShowBoxLayer::~GroupBossShowBoxLayer()
{
}

GroupBossShowBoxLayer* GroupBossShowBoxLayer::create(int boxId)
{ 
	auto pRet = new GroupBossShowBoxLayer(); 
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

bool GroupBossShowBoxLayer::init(int boxId)
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

void GroupBossShowBoxLayer::initUI()
{
	// 加载 UI;
	string strFileName = "Image/UI_bangpaiBOSS/UI_bangpaiBOSS_ShowBox.ExportJson";
	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.uiRoot);

	for (int i = 1; i <= 3; i++)
	{
		BOSS_SHOW_ONEITEM oneItem;
		oneItem.Panel_reward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_box_"+TO_STR(i)));
		oneItem.Panel_reward->setVisible(false);
		m_ui._allItem.push_back(oneItem);
	}
}

void GroupBossShowBoxLayer::onEnter()
{
	ModalLayer::onEnter();

	//显示的道具
	auto allItem = GroupBossModel::getInstance()->getBoxRewardByID(_boxId);

	for (unsigned int i = 0; allItem != nullptr && i < allItem->vBoxRewardInfo.size(); i++)
	{
		auto oneItem = allItem->vBoxRewardInfo.at(i);
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

	}
}

void GroupBossShowBoxLayer::onExit()
{
	ModalLayer::onExit();
}



