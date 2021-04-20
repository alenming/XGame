#ifndef LayerGoldActivityModel_h__
#define LayerGoldActivityModel_h__

#include "cocos2d.h"
#include "Utils/Observer.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "Communication/Json/features.h"
#include "Communication/Json//value.h"
#include "GoldActivityDef.h"
#include "Communication/SocketCommunicator.h"
#include "Utils/LocalTimer.h"

USING_NS_CC;
using namespace std;

class GoldActivityModel : public Observable ,public TimeLocalListener,public Ref
{
public:
	virtual ~GoldActivityModel(void);

	//活动单例
	static GoldActivityModel* getInstance();
	static void destroyInstance();
	//通知监听者
	void notify(GoldActivityNotify_Type eventType, void* data = nullptr);

	//计时通知 param: 时间间隔
	virtual void timeTick(int dt);
	void setLocalTime();
	int getActOverSec() {return m_actOverSec;};
	int getOpenTimeSec() {return m_openTimeSec;};

	void parseTableInfo(const Json::Value &data);//解析表格信息
	void parsechangeGold(const Json::Value &data); //解析兌換金幣
	void parseotherChangeGoldInfo(const Json::Value &data); //解析他人兌換金幣記錄

	int getGoldNum() {return m_goldNum;};
	GoldActivityTable getGoldTbale() {return m_GoldTbale;};//数据
	int getChangeCount() {return m_changeCount;}; //已兑换次数
	deque<OtherPlayerChange> getOtherChange() {return m_OtherChange;};

	void getMoney();
	
	bool isLoaded() {return m_isLoad;};
	bool isHaveChange();
private:
	GoldActivityModel(void);

	static GoldActivityModel* mInstance;

	GoldActivityTable m_GoldTbale;//数据
	int m_changeCount; //已兑换次数 

	int	m_actOverSec;		//活动结束(展示之前)剩余秒数
	int	m_openTimeSec;		//活动离开时(展示之前)剩余秒数
	bool b_isNotifyMainInfo;

	int m_goldNum; //兌換金幣數量

	deque<OtherPlayerChange> m_OtherChange;

	OtherPlayerChange m_myChangeInfo;

	bool m_isLoad;
};

#endif


