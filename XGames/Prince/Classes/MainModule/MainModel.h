#ifndef MainModel_h__
#define MainModel_h__

#include "Utils/Observer.h"
#include "Communication/Json/json.h"
#include "Utils/LocalTimer.h"

//ϵͳ��������������ID
#define POWER_RES_TIME_SYSTEM_ID		10077
#define ENERGY_RES_TIME_SYSTEM_ID		10078

struct CHABGE_PRIZE_ITEM //�����ڵ�
{
	int id;
	int amount;
};
const int CHABGE_GOLD_ITEM_ID = 11001; //���
const int CHABGE_COIN_ITEM_ID = 11002; //����
const int CHABGE_PVP_ITEM_ID = 11003; //����������
const int CHABGE_SHILIAN_ITEM_ID = 11004; //��������
const int CHABGE_GROUP_ITEM_ID = 11005; //���ɻ���
const int CHABGE_HUNTIAN_ITEM_ID = 11006; //���쵤
const int CHABGE_JIUZHUAN_ITEM_ID = 11007; //��ת��
const int CHABGE_VIPEXP_ITEM_ID = 11008; //VIP����
const int CHABGE_CUILIAN_ITEM_ID = 11009; //������Ƭ
const int CHABGE_EXP_ITEM_ID = 11010; //���Ǿ���

class MainParam
{

public:

	MainParam()
		: mRoleId(0)
		, mGold(0)
		, mMaxVipExp(0)
		, mMaxEnergy(0)
		, mMaxPower(0)
		, mPower(0)
		, mEnergy(0)
		, mVipExp(0)
		, mVipLevel(0)
		, mCoin(0)
		, mFinishedExpId(0)
		, mStarNum(0)
		, mMazeId(0)
		, mMazePower(0)
		, mHunTiandan(0)
		, mJiuZhuandan(0)
		, mPowerRestoreTime(0)
		, mEnergyRestoreTime(0)
		, mTotalPay(0)
		, mGroupId(0)
		, mGroupMoney(0)
		, mCuiLianMoney(0)
		, mPvpScore(0)
		, mTrialScore(0)
	{}
	~MainParam(){}

public:

	//��������
	string mName;

	//��ɫid
	int mRoleId;

	//��ǰԪ������
	int mGold;

	//Vip��ǰ�ȼ������
	int mMaxVipExp;

	//���������
	int mMaxEnergy;

	//�����������
	int mMaxPower;

	//��ǰ����ֵ
	int mPower;

	//��ǰ����ֵ
	int mEnergy;

	//��ǰvip����
	int mVipExp;

	//��ǰvip�ȼ�
	int mVipLevel;

	//��ǰ��Ϸ������
	int mCoin;

	//��ǰ��ɵ�����Id
	int mFinishedExpId;

	//�ؿ��Ǽ�������
	int mStarNum;

	//�Թ��ж���
	int mMazeId;
	int mMazePower;

	//���쵤
	int mHunTiandan;

	//��ת��
	int mJiuZhuandan;

	//�����ָ�ʱ��
	int mPowerRestoreTime;

	//�����ָ�ʱ��
	int mEnergyRestoreTime;

	//��ɫ�Ա�, 1Ϊ����, 0ΪŮ��
	int mSex;

	//��ҵ�֧���ܶ��λRMB
	int mTotalPay;

	//����id
	int mGroupId;

	//��������
	string mGroupName; 

	//���ɻ���
	int mGroupMoney;
	
	//������Ƭ
	int mCuiLianMoney;

	// ����������;
	int mPvpScore;

	// ��������;
	int mTrialScore;
};

// ��Ϸ����״̬;
class TradingStatus
{
public:
	TradingStatus()
		: mPowerNum(0)
		, mEnergyNum(0)
		, mActionNum(0)
		, mCoinNum(0)
		, mActivityNum(0)
		, mTowerResetNum(0)
		, mArenaShopRefreshNum(0)
		, mTrailShopRefreshNum(0)
		, mFactionShopRefreshNum(0)
		, mRefineShop8RefreshNum(0)
		, mRefineShop9RefreshNum(0)
		, bEnableSweep(false)
		, mZahuoRefrashNum(0)
		, mMysteryRefrashNum(0)
	{};
	~TradingStatus() {};

public:

	// �����ѹ������;
	int  mPowerNum;

	// �����ѹ������;
	int  mEnergyNum;

	// �Թ��ж����ѹ������;
	int  mActionNum;

	// �����ѹ������;
	int  mCoinNum;

	// ���ս�ѹ������;
	int  mActivityNum;

	// ���������ѹ������;
	int  mTowerResetNum;

	// �������̵���ˢ�´���;
	int  mArenaShopRefreshNum;

	// �����̵���ˢ�´���;
	int  mTrailShopRefreshNum;

	// �����̵���ˢ�´���;
	int  mFactionShopRefreshNum;

	// ����¯ - �����̵���ˢ�´���;
	int  mRefineShop8RefreshNum;

	// ����¯ - �Ź��̵���ˢ�´���;
	int  mRefineShop9RefreshNum;

	//�ӻ��̵�ˢ�´���
	int mZahuoRefrashNum;

	//�����̵�ˢ�´���
	int mMysteryRefrashNum;

	// �Ƿ���ɨ��;
	bool  bEnableSweep;
};

class MainModel : public Observable, public TimeLocalListener
{

public:

	MainModel();
	virtual ~MainModel();

	static MainModel* getInstance();
	static void destroyInstance();

public:

	//��ȡ��ɫ����
	MainParam* getMainParam();

	// ��ȡ����״̬;
	TradingStatus* getTradingStatus();

	//��ʼ������
	void initData(const Json::Value& data);
	//��������(��Ҫ��ʾ���ҽ���)
	void updateData(const Json::Value& data,vector<CHABGE_PRIZE_ITEM>* changPrizeItem = nullptr); 

	//������������ʱ��
	void openTimer();

	//���ý��������isBuy������Դ�Ƿ��ǹ����ȡ
	void setGold(int num, bool isBuy = false);

	//������������
	void setSilver(int num);

	//��������
	void setPower(int num);

	//���þ���
	void setEnergy(int num);

	//���ð�����Ϣ
	void setGroupInfo(int groupId, const string groupName);

	//���ðﹱ����
	void setGroupMoney(int num);

	//���û��쵤
	void setHunTiandan(int num);

	//���þ�ת��
	void setJiuZhuandan(int num);

	//���ô�����Ƭ
	void setCuiLianMoney(int num);

	//vip����
	void setVipExp(int num);

	//vip�ȼ�
	void setVipLv(int num);

	// ���¾���������;
	void setPvpScore(int num) { mMainParam.mPvpScore = num; };

	// ������������;
	void setTrialScore(int num) { mMainParam.mTrialScore = num; };

	//setPower/setEnergy֮�����¼��������״̬
	void setTimerState();

	//��ȡ�¿�����״̬, ����trueΪ����
	bool getMonthCardIsOpen();

	//��ȡ�����뿪��״̬, ����trueΪ����
	bool getActCodeIsOpen();

	//����������ʱ
	void doPowerTimerEvent(int diffTime);

	//��������ʱ
	void doEnergyTimerEvent(int diffTime);

	void timeTick(int dt);

private:

	static MainModel* mInstance;

	//������Ϣ
	MainParam mMainParam;
	// ����״̬;
	TradingStatus mTradingStatus;

//�����������ָ����

public:

	//�����������ָ�����Ҫ����ʱ��
	int powerTime;
	int energyTime;

	//boss����״̬
	int mBossActOpenState;

private:

	
	int mPowerDvalue;				//��ǰ��������ָ���Ҫ��ʱ��
	int mEnergyDvalue;				//��ǰ��ľ����ָ���Ҫ��ʱ��

	//��¼��¼ʱ�ķ�����ʱ���
	time_t mtLoginServerTime;

	int mPowerResByTime;			//ÿxxx��ָ�һ������
	int mEnergyResByTime;			//ÿxxx��ָ�һ�㾫��

	//��¼ʱ��¼�ı���ʱ��
	time_t mLoginLocalTime;

	//�¿�����
	bool mIsMonthCardOpen;

	//�����뿪��
	bool mIsActCodeOpen;
};





#endif //MainModel_h__


