#include "VipModel.h"
#include "VipLayer.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "Communication/Command.h"
#include "Utils/SystemUtils.h"
#include "OperateActModule/OperateActModel.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ReYunSDK/ReYunController.h"



VipModel* VipModel::ms_pInstance = nullptr;

VipModel::VipModel()
{
	m_bIsOpenPay = false;
	m_bIsInitted = false;
	m_curBuyVipBoxId = 0;
	mMonthCardResDays = -1;
	mMonthCardOutTime = 0;
	m_curPayRmb = 0;
    m_vcOrderUnPurchased.clear();
	m_vecVipInfo.clear();
	m_vecRechargeBtnInfo.clear();
	initFromLocal();
	m_curVipLv = MainModel::getInstance()->getMainParam()->mVipLevel;
	m_curVipExp = MainModel::getInstance()->getMainParam()->mVipExp;
	m_curVipMaxExp = MainModel::getInstance()->getMainParam()->mMaxVipExp;
}

VipModel::~VipModel()
{
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

VipModel* VipModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new VipModel();
	}
	return ms_pInstance;
}

void VipModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void VipModel::initFromLocal()
{
	//各等级vip信息表
	multimap<int, RowData>& vipMap = DataManager::getInstance()->getDTVip()->getTableData();

	//遍历VIP数据表
	for(auto iter = vipMap.begin(); iter != vipMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);

		sVipData vipInfo;

		vipInfo.mVipId = rowData->getIntData("id");
		vipInfo.mVipMaxExp = rowData->getIntData("exp");
		vipInfo.mVipBoxOldPrice = rowData->getIntData("oldprice");
		vipInfo.mVipBoxPrice = rowData->getIntData("price");
		vipInfo.mVipSpecDetails = rowData->getStringData("des");
		vipInfo.mVipLv = rowData->getStringData("name");

		vipInfo.mIsTaked = false;

		string boxInfo = rowData->getStringData("box");

		if(!boxInfo.empty())
		{
			vector<string> vecStr;
			StringFormat::parseCsvStringVec(boxInfo, vecStr);

			for(string strTmp : vecStr)
			{
				int nIndex1 = strTmp.find_first_of(',', 0);
				string strTmp1 = strTmp.substr(0, nIndex1);
				string strTmp2 = strTmp.erase(0, nIndex1 + 1);
				int boxId = atoi(strTmp1.c_str());
				int count = atoi(strTmp2.c_str());

				sVipSpecBox vipSpecBox;
				vipSpecBox.mboxId = boxId;
				vipSpecBox.mCount = count;

				vipInfo.mVecVipSpecBox.push_back(vipSpecBox);
			}
		}

		m_vecVipInfo.push_back(vipInfo);
	}

	std::sort(m_vecVipInfo.begin(), m_vecVipInfo.end(), 
		[](const sVipData& p1, const sVipData& p2)->bool
	{
		//按vip等级从小到大排序
		return p1.mVipId < p2.mVipId;
	});

	//根据渠道号获取表中数据
	multimap<int, RowData>& rechargeMap = DataManager::getInstance()->getDTPay()->getTableData();
	int channel = SystemUtils::getChannelId();
	for(auto iter = rechargeMap.begin(); iter != rechargeMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		if(rowData && rowData->getIntData("channel") != channel)
		{
			CCLOG("渠道号和配表不一致! 当前渠道号 = %d, 表中渠道号 = %d", channel, rowData->getIntData("channel"));
			continue;
		}

		sRechargeData rechargeItem;
		rechargeItem.id = rowData->getIntData("id");
		rechargeItem.channel = channel;
		rechargeItem.money = rowData->getIntData("money");
		rechargeItem.payGold = rowData->getIntData("payGold");
		rechargeItem.resId = rowData->getIntData("resId");
		rechargeItem.mark = rowData->getIntData("mark");
		rechargeItem.markDes = rowData->getStringData("markDes");
		rechargeItem.des = rowData->getStringData("des");

		m_vecRechargeBtnInfo.push_back(rechargeItem);
	}
	std::sort(m_vecRechargeBtnInfo.begin(), m_vecRechargeBtnInfo.end(), 
		[](const sRechargeData& p1, const sRechargeData& p2)->bool
	{
		//按id从小到大排序
		return p1.id < p2.id;
	});

	//如果月卡功能关闭
	if(!MainModel::getInstance()->getMonthCardIsOpen())
	{
		//月卡，至尊卡强制改为25，68元档普通充值，不带双倍，不带立返
		if(m_vecRechargeBtnInfo.size() > 2)
		{
			m_vecRechargeBtnInfo.at(0).money = 25;
			m_vecRechargeBtnInfo.at(0).payGold = 250;
			m_vecRechargeBtnInfo.at(0).resId = 12007;
			m_vecRechargeBtnInfo.at(0).mark = 0;
			m_vecRechargeBtnInfo.at(0).markDes = "";
			m_vecRechargeBtnInfo.at(0).des = "";

			m_vecRechargeBtnInfo.at(1).money = 68;
			m_vecRechargeBtnInfo.at(1).payGold = 680;
			m_vecRechargeBtnInfo.at(1).resId = 12006;
			m_vecRechargeBtnInfo.at(1).mark = 0;
			m_vecRechargeBtnInfo.at(1).markDes = "";
			m_vecRechargeBtnInfo.at(1).des = "";
		}
	}
}

void VipModel::initFromSvr(const Json::Value& vipData)
{
	//vip等级和经验
	m_curVipExp = vipData["vipExp"].asInt();
	m_curVipLv = vipData["vipLevel"].asInt();
	m_bIsOpenPay = vipData["vipbuy"].asInt() == 1 ? true : false;
	//已经购买过的特权礼包
	Json::Value isTakedGift = vipData["boxList"];

	for(int i=0; i<isTakedGift.size(); i++)
	{
		int vipId = isTakedGift[i].asInt();
		m_vecVipInfo.at(vipId).mIsTaked = true;
	}
	
	//购买过的一些标志
	
	Json::Value rechargeList = vipData["buyItemList"];

	for(int i=0; i<rechargeList.size(); i++)
	{
		for(auto& item : m_vecRechargeBtnInfo)
		{
			if(rechargeList[i].asInt() == item.id)
			{
				item.rechargedFlag = true;
				break;
			}
		}
	}

	//月卡截止时间戳
	mMonthCardOutTime = vipData["monthcard"].asUInt();
	//月卡剩余天数
	time_t curServerTime = LocalTimer::getInstance()->getCurServerTime();

	if(mMonthCardOutTime > curServerTime)
	{
		mMonthCardResDays = (mMonthCardOutTime - curServerTime)/(24*3600);
		setEndTimeStamp(mMonthCardOutTime);
		LocalTimer::getInstance()->addLocalTimerListener(this);
	}

	//通知UI, 初始化已购买礼包状态
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_VIP_BUY_GIFT;
	notifyObservers(&obParam);

	m_bIsInitted = true;
}

void VipModel::payResultPush(const Json::Value& payResultData)
{
	int payId = payResultData["buyItemId"].asInt();

	setMonthCardOutTime(payResultData["monthcard"].asUInt());

	time_t monthCardTime = getMonthCardOutTime();
	time_t curServerTime = LocalTimer::getInstance()->getCurServerTime();

	m_curVipLv = MainModel::getInstance()->getMainParam()->mVipLevel;
	m_curVipExp = MainModel::getInstance()->getMainParam()->mVipExp;
	for(auto& item : m_vecVipInfo)
	{
		if(item.mVipId == m_curVipLv)
		{
			m_curVipMaxExp = item.mVipMaxExp;
			break;
		}
	}
	m_curVipMaxExp = MainModel::getInstance()->getMainParam()->mMaxVipExp;

	setEndTimeStamp(monthCardTime);
	LocalTimer::getInstance()->addLocalTimerListener(this);
	if(monthCardTime > curServerTime)
	{
		setMonthCardResDays((monthCardTime - curServerTime)/(24*3600));
	}
	else
	{
		setMonthCardResDays(-1);
	}
	for(auto& item : m_vecRechargeBtnInfo)
	{
		if(item.id == payId)
		{
			item.rechargedFlag = true;
			if(item.mark == 1)
			{
				//月卡
				OperateActModel::getInstance()->setBuyMonthCard();
			}
			else if(item.mark == 2)
			{
				//至尊卡
				OperateActModel::getInstance()->setBuyExtremeCard();
			}
			break;
		}
	}

	ObserverParam obParam;
	obParam.id = nMAIN_CMD_PAY_RESULT;
	VipModel::getInstance()->notifyObservers(&obParam);
}

void VipModel::onPaySuc(std::string orderId)
{
	//支付后，服务器推送前，先记录一下
	SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL, GetIntegerFromXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL) + m_curPayRmb);
    
	//客户端不上报,改为服务端上报
    //ReyunController::ReYunPaySuccessful(orderId,  APPSTORE, MONEY_TYPE_REYUN::REYUN_MONEYTYPE_CNY, (float)m_curPayRmb);
    
}

void VipModel::onAppStorePaySuc(std::string orderId)
{
    // 过滤订单号;
    auto iter = find(m_vcOrderUnPurchased.begin(), m_vcOrderUnPurchased.end(), orderId);
    if (iter != m_vcOrderUnPurchased.end())
    {
        //支付后，服务器推送前，先记录一下
        SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL, GetIntegerFromXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL) + m_curPayRmb);
        
        // 热云统计以这个节点为准;
        //CCLOG("++++++++++++++++ VipModel::onAppStorePaySuc ==> reyun, %d", m_curPayRmb);
		//客户端不上报,改为服务端上报
        //ReyunController::ReYunPaySuccessful(orderId,  APPSTORE, MONEY_TYPE_REYUN::REYUN_MONEYTYPE_CNY, (float)m_curPayRmb);
        
        m_vcOrderUnPurchased.erase(iter);
    }
    
}

void VipModel::setCurPayRmb(int rmb)
{
	m_curPayRmb = rmb;
}

int VipModel::getCurVipLv()
{
	return m_curVipLv;
}

int VipModel::getCurVipExp()
{
	return m_curVipExp;
}

int VipModel::getCurVipMaxExp()
{
	return m_curVipMaxExp;
}

sVipData VipModel::getVipInfo(int vipId)
{
	return m_vecVipInfo.at(vipId);
}

vector<sVipData>& VipModel::getVecVipInfo()
{
	return m_vecVipInfo;
}

bool VipModel::getIsOpenPay()
{
	return m_bIsOpenPay;
}

void VipModel::setCurBuyVipBoxId(int vipId)
{
	m_curBuyVipBoxId = vipId;
}

int VipModel::getCurBuyVipBoxId()
{
	return m_curBuyVipBoxId;
}

vector<sRechargeData>& VipModel::getVecRechargeInfo()
{
	return m_vecRechargeBtnInfo;
}

bool& VipModel::getIsInitted()
{
	return m_bIsInitted;
}

int& VipModel::getMonthCardResDays()
{
	return mMonthCardResDays;
};

time_t& VipModel::getMonthCardOutTime()
{
	return mMonthCardOutTime;
}

bool VipModel::isCanGetVipBox()
{
	int curGold = MainModel::getInstance()->getMainParam()->mGold;
	for(auto vipBox : m_vecVipInfo)
	{
		//能领取的条件：1.没领取过，2.vip等级到达，3.金币足够购买礼包
		if(!vipBox.mIsTaked && (m_curVipLv >= vipBox.mVipId) && (curGold >= vipBox.mVipBoxPrice))
		{
			return true;
		}
	}

	return false;
}

void VipModel::setMonthCardResDays(int days)
{
	mMonthCardResDays = days;
}

void VipModel::setMonthCardOutTime(time_t outTime)
{
	mMonthCardOutTime = outTime;
}

void VipModel::timeTick(int dt)
{
	int mothCardDays = (mMonthCardOutTime - LocalTimer::getInstance()->getCurServerTime())/(24*3600);
	if(mMonthCardResDays != mothCardDays)
	{
		mMonthCardResDays = mothCardDays;
		//通知UI, 刷新月卡剩余天数
		ObserverParam obParam;
		obParam.id = MONTH_CARD_RES_DAYS;
		notifyObservers(&obParam);
	}
}

void VipModel::setOrderId(std::string orderId)
{
    if (orderId.empty())
        return;
    
    auto iter = find(m_vcOrderUnPurchased.begin(), m_vcOrderUnPurchased.end(), orderId);
    if (iter == m_vcOrderUnPurchased.end())
    {
        m_vcOrderUnPurchased.push_back(orderId);
    }
}