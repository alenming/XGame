#ifndef RobController_h__
#define RobController_h__

#include "RobDef.h"
#include "Communication/ICommListener.h"


class RobController : public Ref, public ICommListener
{
public:
	RobController();
	~RobController();

	static RobController* getInstance();
	static void destroyInstance();

	// 请求界面信息;
	void  handleRobMainInfo();

	// 刷新对手;
	void  handleRefreshEnemy(int nChipId);

	// 掠夺记录;
	void  handleRobRecord();

	// 免战;
	void  handleRest(int flag);

	// 合成;
	void  handleCompose(const int& id, const int& flag);

	//掠夺10次
	void  handleRobTen();

	// 免战倒计时;
	void  startRestTimer(int nRestTime);

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);


private:

	// 主界面信息;
	void  procRobMainInfo(const char* szData);

	// 对手信息;
	void  procRefreshEnemy(const char* szData);

	// 掠夺记录;
	void procRobRecord(const char* szData);

	// 合成结果;
	void procCompose(const char* szData);

	// 掠夺10次结果;
	void  procRobTenResult(const char* szData);

	// 掠夺推送消息;
	void  procRobPushUpdate(const char* szData);

	// 免战;
	void  procRest(const char* szData);
	void  refreshCountDown(float delta);

private:
	static RobController*  m_pInstance;

	// 免战时间;
	int  m_nRestTime;

	// 免战类型;
	int  m_restType;
};

#endif 
