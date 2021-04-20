#include "MainModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "PlayerInfoModule/PlayerInfoModel.h"
#include "SDK_TalkingData/TalkingData.h"
#include "FirstRecharge/FirstRechargeModel.h"

MainModel* MainModel::mInstance = nullptr;

MainModel::MainModel()
{
	mPowerDvalue = 0;				
	mEnergyDvalue = 0;
	powerTime = 0;
	energyTime = 0;
	mtLoginServerTime = 0;
	mBossActOpenState = false;
	mIsMonthCardOpen = true;
	mIsActCodeOpen = true;
}

MainModel::~MainModel()
{
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

MainModel* MainModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new MainModel();
	}
	return mInstance;
}

void MainModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MainModel::initData(const Json::Value& data)
{
	mMainParam.mName = data["name"].asString();
	mMainParam.mGold = data["gold"].asInt();
	mMainParam.mCuiLianMoney = data["quenching"].asInt();
	mMainParam.mEnergy = data["energy"].asInt();
	mMainParam.mMaxEnergy = data["maxEnergy"].asInt();
	mMainParam.mMaxVipExp = data["maxVipexp"].asInt();
	mMainParam.mRoleId = data["id"].asInt();
	mMainParam.mPower = data["power"].asInt();
	mMainParam.mVipExp = data["vipexp"].asInt();
	mMainParam.mVipLevel = data["vipLevel"].asInt();
	mMainParam.mCoin = data["coin"].asInt();
	mMainParam.mMaxPower = data["maxPower"].asInt();
	mMainParam.mHunTiandan = data["hunTiandan"].asInt();
	mMainParam.mJiuZhuandan = data["jiuZhuandan"].asInt();
	mMainParam.mTotalPay = data["totalPay"].asInt();

	mMainParam.mGroupId = data["bangpaiId"].asInt();
	mMainParam.mGroupName = data["bangpaiName"].asString(); 
	mMainParam.mGroupMoney = data["credit"].asInt();
	mMainParam.mPvpScore = data["score"].asInt();
	mMainParam.mTrialScore = data["trials"].asInt();
	
	mMainParam.mSex = data["sex"].asInt();
	mMainParam.mPowerRestoreTime = data["powerTime"].asInt();
	mMainParam.mEnergyRestoreTime = data["energyTime"].asInt();

	RowData* powerTimeData = DataManager::getInstance()->getDTToolSystem()->searchDataById(POWER_RES_TIME_SYSTEM_ID);
	RowData* energyTimeData = DataManager::getInstance()->getDTToolSystem()->searchDataById(ENERGY_RES_TIME_SYSTEM_ID);
	mPowerResByTime = powerTimeData->getIntData("value");
	mEnergyResByTime = energyTimeData->getIntData("value");

	// ����״̬;
	mTradingStatus.mPowerNum = data["powerNum"].asInt();
	mTradingStatus.mEnergyNum = data["energyNum"].asInt();
	mTradingStatus.mActionNum = data["actionNum"].asInt();
	mTradingStatus.mCoinNum = data["coinNum"].asInt();
	mTradingStatus.mActivityNum = data["activityNum"].asInt();
	mTradingStatus.mTowerResetNum = data["towerNum"].asInt();
	mTradingStatus.mArenaShopRefreshNum = data["arenashopNum"].asInt();
	mTradingStatus.mTrailShopRefreshNum = data["trailshopNum"].asInt();
	mTradingStatus.mFactionShopRefreshNum = data["factionshopNum"].asInt();
	mTradingStatus.mRefineShop8RefreshNum = data["hunTiandanNum"].asInt();
	mTradingStatus.mRefineShop9RefreshNum = data["jiuZhuandanNum"].asInt();
	mTradingStatus.mZahuoRefrashNum = data["goldshopNum"].asInt();
	mTradingStatus.mMysteryRefrashNum = data["mysticalshop"].asInt();
	//�׳�״̬
	FirstRechargeModel::getInstance()->mFirstRechargeState = data["payReward"].asInt();




	//boss����״̬  0-δ������1-����
	mBossActOpenState = data["bossOpenFlag"].asInt();

	//�¿�����/�����뿪��,Ŀǰios�����ر�
	mIsMonthCardOpen = data["showMonthCard"].asInt() == 0 ? false : true;
	mIsActCodeOpen = data["showActiveCode"].asInt() == 0 ? false : true;
	
	notifyObservers();

	//��¼��¼ʱ�ķ�����ʱ�����Ϊ��ֹ��������������ʵʱ�仯�Ļָ�����ʱ�䶼�����Ϊ����
	mtLoginServerTime = data["timestamp"].asUInt();


	//��¼��ǰ����/������ָ���Ҫ�ĳ�ʼʱ������, msתs
	mPowerDvalue = mMainParam.mPowerRestoreTime/1000;
	mEnergyDvalue = mMainParam.mEnergyRestoreTime/1000;

	mLoginLocalTime = time(nullptr);

	//����������������ʱ��
	openTimer();
}

//��������
void MainModel::updateData(const Json::Value& data,vector<CHABGE_PRIZE_ITEM>* changPrizeItem)
{
	CHABGE_PRIZE_ITEM item;
	if (!data["gold"].isNull())
	{
		if (data["gold"].asInt() > mMainParam.mGold && changPrizeItem)
		{
			item.id = CHABGE_GOLD_ITEM_ID;
			item.amount = data["gold"].asInt() - mMainParam.mGold;
			changPrizeItem->push_back(item);
		}

		setGold(data["gold"].asInt());
	}

	if (!data["coin"].isNull())
	{
		if (data["coin"].asInt() > mMainParam.mCoin && changPrizeItem)
		{
			item.id = CHABGE_COIN_ITEM_ID;
			item.amount = data["coin"].asInt() - mMainParam.mCoin;
			changPrizeItem->push_back(item);
		}
		
		setSilver(data["coin"].asInt());
	}

	if (!data["energy"].isNull())
	{
		setEnergy(data["energy"].asInt());
	}

	if (!data["power"].isNull())
	{
		setPower(data["power"].asInt());
	}

	if (!data["credit"].isNull())
	{
		if (data["credit"].asInt() > mMainParam.mGroupMoney && changPrizeItem)
		{
			item.id = CHABGE_GROUP_ITEM_ID;
			item.amount = data["credit"].asInt() - mMainParam.mGroupMoney;
			changPrizeItem->push_back(item);
		}

		setGroupMoney(data["credit"].asInt());
	}

	if (!data["hunTiandan"].isNull())
	{
		if (data["hunTiandan"].asInt() > mMainParam.mHunTiandan && changPrizeItem)
		{
			item.id = CHABGE_HUNTIAN_ITEM_ID;
			item.amount = data["hunTiandan"].asInt() - mMainParam.mHunTiandan;
			changPrizeItem->push_back(item);
		}

		setHunTiandan(data["hunTiandan"].asInt());
	}

	if (!data["jiuZhuandan"].isNull())
	{
		if ( data["jiuZhuandan"].asInt() > mMainParam.mJiuZhuandan && changPrizeItem)
		{
			item.id = CHABGE_JIUZHUAN_ITEM_ID;
			item.amount = data["jiuZhuandan"].asInt() - mMainParam.mJiuZhuandan;
			changPrizeItem->push_back(item);
		}

		setJiuZhuandan(data["jiuZhuandan"].asInt());
	}

	if (!data["quenching"].isNull())
	{
		if (data["quenching"].asInt() > mMainParam.mCuiLianMoney && changPrizeItem)
		{
			item.id = CHABGE_CUILIAN_ITEM_ID;
			item.amount = data["quenching"].asInt() - mMainParam.mCuiLianMoney;
			changPrizeItem->push_back(item);
		}

		setCuiLianMoney(data["quenching"].asInt());
	}

	if (!data["vipexp"].isNull())
	{
		if (data["vipexp"].asInt() > mMainParam.mVipExp && changPrizeItem)
		{
			item.id = CHABGE_VIPEXP_ITEM_ID;
			item.amount = data["vipexp"].asInt() - mMainParam.mVipExp;
			changPrizeItem->push_back(item);
		}

		setVipExp(data["vipexp"].asInt());
	}
	
	if (!data["viplevel"].isNull())
	{
		setVipLv(data["viplevel"].asInt());
	}

	if (!data["score"].isNull())
	{
		if (data["score"].asInt() > mMainParam.mPvpScore && changPrizeItem)
		{
			item.id = CHABGE_PVP_ITEM_ID;
			item.amount = data["score"].asInt() - mMainParam.mPvpScore;
			changPrizeItem->push_back(item);
		}
		
		setPvpScore(data["score"].asInt());
	}

	if (!data["trials"].isNull())
	{
		if (data["trials"].asInt() > mMainParam.mTrialScore && changPrizeItem)
		{
			item.id = CHABGE_SHILIAN_ITEM_ID;
			item.amount = data["trials"].asInt() - mMainParam.mTrialScore;
			changPrizeItem->push_back(item);
		}

		setTrialScore(data["trials"].asInt());
	}
	
	notifyObservers();
}


//���ð�����Ϣ
void MainModel::setGroupInfo(int groupId, const string groupName)
{
	mMainParam.mGroupId = groupId;
	mMainParam.mGroupName = groupName;
}

//���ý����������Դ���Ϊ�����ݲ��ϱ�
void MainModel::setGold(int num, bool isBuy)
{
	int addGold = num - mMainParam.mGold;
	mMainParam.mGold = num;

	//��������ӣ��Ҳ�Ϊ����;��ʱ���ϱ�����
	if (addGold > 0 && !isBuy)
	{
		TalkingData::onReward(addGold);
	}
}

//������������
void MainModel::setSilver(int num)
{
	mMainParam.mCoin = num;
}

//��������
void MainModel::setPower(int num)
{
	mMainParam.mPower = num;
	setTimerState();
}

//���þ���
void MainModel::setEnergy(int num)
{
	mMainParam.mEnergy = num;
	setTimerState();
}

void MainModel::setGroupMoney(int num)
{
	mMainParam.mGroupMoney = num;
}

void MainModel::setHunTiandan(int num)
{
	mMainParam.mHunTiandan = num;
}

void MainModel::setJiuZhuandan(int num)
{
	mMainParam.mJiuZhuandan = num;
}

void MainModel::setCuiLianMoney(int num)
{
	mMainParam.mCuiLianMoney = num;
}

void MainModel::setVipExp(int num)
{
	mMainParam.mVipExp = num;
}

void MainModel::setVipLv(int num)
{
	mMainParam.mVipLevel = num;
}

MainParam* MainModel::getMainParam()
{
	return &mMainParam;
}

TradingStatus* MainModel::getTradingStatus()
{
	return &mTradingStatus;
}

bool MainModel::getMonthCardIsOpen()
{
	return mIsMonthCardOpen;
}

bool MainModel::getActCodeIsOpen()
{
	return mIsActCodeOpen;
}

void MainModel::setTimerState()
{
	//�������������
	if(mMainParam.mPower < mMainParam.mMaxPower)
	{
		if(mPowerDvalue == 0)
		{
			mPowerDvalue = mPowerResByTime;
		}
		powerTime = mPowerDvalue + (mMainParam.mMaxPower-mMainParam.mPower-1)*mPowerResByTime;	
	}
	else if(mMainParam.mPower >= mMainParam.mMaxPower)
	{
		powerTime = 0;
		mPowerDvalue = 0;
	}

	//�������
	if(mMainParam.mEnergy < mMainParam.mMaxEnergy)
	{
		if(mEnergyDvalue == 0)
		{
			mEnergyDvalue = mEnergyResByTime;
		}
		energyTime = mEnergyDvalue + (mMainParam.mMaxEnergy-mMainParam.mEnergy-1)*mEnergyResByTime;
	}
	else if(mMainParam.mEnergy >= mMainParam.mMaxEnergy)
	{
		energyTime = 0;
		mEnergyDvalue = 0;
	}

	//�����;�����һû�ָ�����Ҫ�������ü�����
	if(powerTime > 0 || energyTime > 0)
	{
		time_t currentTime = time(nullptr);

		//���㵱ǰ������ʱ��
		time_t mCurServerTime = mtLoginServerTime + currentTime - mLoginLocalTime;

		setEndTimeStamp((powerTime > energyTime ? powerTime	: energyTime) + mCurServerTime);
		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
	else
	{
		//ֹͣ������
		setEndTimeStamp(0);
	}
}

void MainModel::openTimer()
{
	//����������ͬһ����ʱ��(this)
	if(mMainParam.mPower < mMainParam.mMaxPower || mMainParam.mEnergy < mMainParam.mMaxEnergy)
	{
		//�����ָ�����Ҫ��������
		powerTime = mPowerDvalue + (mMainParam.mMaxPower-mMainParam.mPower-1)*mPowerResByTime;

		//�����ָ�����Ҫ��������
		energyTime = mEnergyDvalue + (mMainParam.mMaxEnergy-mMainParam.mEnergy-1)*mEnergyResByTime;

		//�����;�������һ��������������ʱ��������д��Ϊ׼
		//����ʱ���ü������رյ�ʱ�������������;���ͻ��(��ҩ/�������ĵ�)�����ʱ�������setPower/setEnergy�и���
		setEndTimeStamp((powerTime > energyTime ? powerTime : energyTime) + mtLoginServerTime);

		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
}

void MainModel::doPowerTimerEvent(int diffTime)
{	
	//����Ǻ�̨��ͣ�л���
	if(diffTime > 1)
	{
		if(diffTime >= mPowerDvalue)
		{
			int powerAddNum = (diffTime - mPowerDvalue)/mPowerResByTime + 1;
			mMainParam.mPower += powerAddNum;

			if(mMainParam.mPower >= mMainParam.mMaxPower)
			{
				mMainParam.mPower = mMainParam.mMaxPower;
				mPowerDvalue = 0;
				powerTime = 0;
			}
			else
			{
				mPowerDvalue = mPowerResByTime - (diffTime - mPowerDvalue) % mPowerResByTime;
				powerTime -= diffTime;
			}		

			//��������, ˢ�������Ϣ����
			ObserverParam obParam;
			obParam.id = POWER_COUNT_ADD;	
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);

			//ˢ����������������
			notifyObservers();
		}
		else
		{
			mPowerDvalue -= diffTime;
			powerTime -= diffTime;		

			//ˢ�������Ϣ���浹��ʱ
			ObserverParam obParam;
			obParam.id = UPDATE_POWER_TIME;
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);
		}
	}
	else
	{		
		mPowerDvalue -= diffTime;
		powerTime -= diffTime;
		if(mPowerDvalue == 0)
		{
			++mMainParam.mPower;
			
			//��������, ˢ�������Ϣ����
			ObserverParam obParam;
			obParam.id = POWER_COUNT_ADD;	
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);

			//ˢ����������������
			MainModel::getInstance()->notifyObservers();

			if(mMainParam.mPower >= mMainParam.mMaxPower)
			{
				mPowerDvalue = 0;
				powerTime = 0;
			}
			else
			{
				mPowerDvalue = mPowerResByTime;
			}
		}

		//ˢ�������Ϣ���浹��ʱ
		ObserverParam obParam;
		obParam.id = UPDATE_POWER_TIME;
		PlayerInfoModel::getInstance()->notifyObservers(&obParam);
	}
}

void MainModel::doEnergyTimerEvent(int diffTime)
{
	//����Ǻ�̨��ͣ�л���
	if(diffTime > 1)
	{
		if(diffTime >= mEnergyDvalue)
		{
			int energyAddNum = (diffTime - mEnergyDvalue)/mEnergyDvalue + 1;
			mMainParam.mEnergy += energyAddNum;

			if(mMainParam.mEnergy >= mMainParam.mMaxEnergy)
			{
				mMainParam.mEnergy = mMainParam.mMaxEnergy;
				mEnergyDvalue = 0;
				energyTime = 0;
			}
			else
			{
				mEnergyDvalue = mEnergyResByTime - (diffTime - mEnergyDvalue) % mEnergyResByTime;
				energyTime -= diffTime;
			}		

			//��������, ˢ�������Ϣ����
			ObserverParam obParam;
			obParam.id = ENERGY_COUNT_ADD;	
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);

			//ˢ�������澫������
			notifyObservers();
		}
		else
		{
			mEnergyDvalue -= diffTime;
			energyTime -= diffTime;		

			//ˢ�������Ϣ���浹��ʱ
			ObserverParam obParam;
			obParam.id = UPDATE_ENERGY_TIME;
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);
		}
	}
	else
	{		
		mEnergyDvalue -= diffTime;
		energyTime -= diffTime;
		if(mEnergyDvalue == 0)
		{
			++mMainParam.mEnergy;

			//��������, ˢ�������Ϣ����
			ObserverParam obParam;
			obParam.id = ENERGY_COUNT_ADD;	
			PlayerInfoModel::getInstance()->notifyObservers(&obParam);

			//ˢ�������澫������
			notifyObservers();

			if(mMainParam.mEnergy >= mMainParam.mMaxEnergy)
			{
				mEnergyDvalue = 0;
				energyTime = 0;
			}
			else
			{
				mEnergyDvalue = mEnergyResByTime;
			}
		}

		//ˢ�������Ϣ���浹��ʱ
		ObserverParam obParam;
		obParam.id = UPDATE_ENERGY_TIME;
		PlayerInfoModel::getInstance()->notifyObservers(&obParam);
	}
}

void MainModel::timeTick(int dt)
{
	if(powerTime > 0)
	{
		doPowerTimerEvent(dt);
	}
	if(energyTime > 0)
	{
		doEnergyTimerEvent(dt);
	}
}
