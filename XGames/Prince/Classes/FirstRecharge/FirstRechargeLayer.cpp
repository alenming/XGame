
#include "Common/Common.h"
#include "Temp/CustomTips.h"
#include "Communication/Command.h"
#include "Widget/ActionCreator.h"
#include "VIPModule/VipScene.h"
#include "DataManager/DataManager.h"
#include "ToolModule/UIToolHeader.h"
#include "FirstRechargeLayer.h"
#include "FirstRechargeModel.h"
#include "Temp/PopReward.h"
#include "VipModule/VipScene.h"
#include "Widget/LayerGetCard.h"

FirstRechargeLayer::FirstRechargeLayer()
{
	m_iHeroTemplateId = 0;
	m_iChipCount = 0;
}

FirstRechargeLayer::~FirstRechargeLayer()
{

}

void FirstRechargeLayer::onEnter()
{
	ModalLayer::onEnter();
	ActionCreator::runCommDlgAction(m_ui.pRoot->getChildByName("Pnl_Recharge"));
	FirstRechargeModel::getInstance()->addObserver(this);
}

void FirstRechargeLayer::onExit()
{
	ModalLayer::onExit();
	FirstRechargeModel::getInstance()->removeObserver(this);
}

bool FirstRechargeLayer::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void FirstRechargeLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIRecharge/UIRecharge.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);
	
	Layout* parent = (Layout*)m_ui.pRoot->getChildByName("Pnl_Recharge");
	parent->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.rechargeBtn = (Button*)parent->getChildByName("Btn_Pay");
	m_ui.rechargeBtn->addTouchEventListener(CC_CALLBACK_2(FirstRechargeLayer::onBtnClick, this, FIRST_RECHARGE_BTN_TAG::RECHARGE));
	m_ui.getRewardBtn = (Button*)parent->getChildByName("Btn_Get");
	m_ui.getRewardBtn->addTouchEventListener(CC_CALLBACK_2(FirstRechargeLayer::onBtnClick, this, FIRST_RECHARGE_BTN_TAG::GETREWARD));
	m_ui.closeBtn = (Button*)parent->getChildByName("Btn_Close");
	m_ui.closeBtn->addTouchEventListener(CC_CALLBACK_2(FirstRechargeLayer::onBtnClick, this, FIRST_RECHARGE_BTN_TAG::CLOSE));

	//首充奖励
	vector<sReward> rewardInfo = FirstRechargeModel::getInstance()->getFirstRechargeReward();
	for(int i=0; i<rewardInfo.size() && i<4; i++)
	{
		UIToolHeader* icon = UIToolHeader::create(rewardInfo.at(i).id);
		if(icon != nullptr)
		{
			Layout* pnl = (Layout*)parent->getChildByName("Pnl_Item" + _TO_STR(i+1));
			pnl->removeAllChildren();
			icon->setPosition(pnl->getContentSize()/2);
			if(i == 0)
			{
				icon->setScale(1.2f);
			}
			RowData* rowData = DataManager::getInstance()->searchToolById(rewardInfo.at(i).id);
			if(rowData)
			{
				//如果列表有英雄
				if(rowData->getIntData("itemTyp") == 4)
				{
					m_iHeroTemplateId = rewardInfo.at(i).id;
				}

				//如果送的是英雄碎片
				icon->setNumEx(rewardInfo.at(i).count);
			}
			icon->setToolId(rewardInfo.at(i).id);
			icon->showLightEff();
			icon->setTipsEnabled(true);		
			pnl->addChild(icon);
		}
	}

	switch (FirstRechargeModel::getInstance()->mFirstRechargeState)
	{
	case 0:
		{
			m_ui.rechargeBtn->setVisible(true);
			m_ui.getRewardBtn->setVisible(false);
		}
		break;
	case 1:
		{
			m_ui.rechargeBtn->setVisible(false);
			m_ui.getRewardBtn->setVisible(true);
		}
	default:
		break;
	}
}

void FirstRechargeLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, FIRST_RECHARGE_BTN_TAG nWidgetName )
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case FIRST_RECHARGE_BTN_TAG::RECHARGE:
			Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
			break;
		case FIRST_RECHARGE_BTN_TAG::GETREWARD:
			mController.sendGetRewardMsg();
			break;
		case FIRST_RECHARGE_BTN_TAG::CLOSE:
			this->dispose();
			break;
		default:
			break;
		}
	}
}

void FirstRechargeLayer::showReward()
{
	//判断首充奖励列表是否有英雄整卡	
	if(m_iHeroTemplateId != 0)
	{
		this->dispose();
		LayerGetCard *_layerGetCard = LayerGetCard::create(m_iHeroTemplateId, false,1);
		Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
	}
	else
	{
		vector<sReward> rewardInfo = FirstRechargeModel::getInstance()->getFirstRechargeReward();
		vector<PopRewardItem*> vecRewardItem;

		for(auto info : rewardInfo)
		{
			if(info.id == TOOL_ID_GOLD)
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(info.count);
				vecRewardItem.push_back(rewardItem);
			}
			else if(info.id == TOOL_ID_COIN)
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(info.count);
				vecRewardItem.push_back(rewardItem);
			}
			else
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(info.id, info.count);
				vecRewardItem.push_back(rewardItem);
			}
		}

		PopReward::show(vecRewardItem);

		//奖励领取按钮置为不可点击
		m_ui.getRewardBtn->setEnabled(false);
		m_ui.getRewardBtn->setBright(false);
	}
}

void FirstRechargeLayer::updateSelf(void* data)
{
	if(data == nullptr)
	{
		return;
	}
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_FIRST_RECHARGE:
		showReward();
		break;
	case nMAIN_CMD_PAY_RESULT:
		{
			//如果是首充信息
			m_ui.rechargeBtn->setVisible(false);
			m_ui.getRewardBtn->setVisible(true);
		}
		break;
	default:
		break;
	}
}
