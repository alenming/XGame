/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ChapterController
 *  Description:	关卡模块控制器的实现;
 *					1. 服务器数据接收/发送接口;
 *					2. 本地csv表数据解析;
 *					3. 流程控制;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef ChapterController_h__
#define ChapterController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "ChapterDef.h"
#include "Sweep/SweepDef.h"
#include "LayerChapter.h"
#include "LayerBarrier.h"
#include "LayerFrame.h"

class ChapterController : public Ref, public ICommListener 
{
public:
	~ChapterController(void);

	static ChapterController* getInstance();
	static void destroyInstance();

	//////////////////////////////////// 初始化; //////////////////////////////////////
	// 请求pve配置和进度数据;
	void  handlePveData();

	// 初始化本地数据;
	void  initLocalData();

	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 请求章节数据;
	bool  handleChapterDetail();

	// 请求活动关卡状态数据;
	void  handleActivityState();

	// 设置章节视图;
	void  setChapterFrameView(LayerFrame* pView) { m_pChapterFrameView = pView; };
	void  setChapterView(LayerChapter* pView) { m_pChapterView = pView; };
	void  setBarrierView(LayerBarrier* pView);

	//////////////////////////////////// 流程控制; //////////////////////////////////////
	// 预设目的章节和关卡;
	void  setDstChapter(int nChapterId);
	void  setDstBarrier(int nBarrierId);

	// 章节选择;
	void  onChapterSelected(int nChapterId) { m_nCurSelChapterId = nChapterId; };

	// 关卡(难度)选择;
	void  onBarrierSelected(int nBarrierId);
	bool  onBarrierLevelSelected(BARRIER_LEVEL nLv);

	// 请求开始战斗;
	int   handleFight(string& strReason);

	// 进入扫荡前的预判断;
	int   handlePreStartSweep(int nCount, string& strReason, int& nSweepFinalCount);

	// 扫荡次数判定(传入期望扫荡次数);
	int   getMaxSweepCount(int OriCount, string& strReason);

	// 请求开始/终止扫荡;
	bool  handleStartSweep(int nCount, string& strReason, int nFlag = 0);

	// 领取星级奖励;
	void  handleStarReward(int nStarIndex);

	// 查询当前选中的关卡;
	int   getCurrSelectedBarrier() { return m_nCurSelBarrierId; };

	// 重置关卡;
	int   handlePreResetBarrier();
	void  handleResetBarrier();
	
private:

	ChapterController(void);

	// 解析本地csv表格数据(章节和关卡);
	void  procChapterData();
	void  procBarrierData();

	// 请求关卡进度数据;
	void  handleFightRecordData();

	// 处理服务器返回的关卡进度数据;
	void  procFightRecordData(const char* szData);
	void  procChapterListData(const Json::Value& data);
	void  procBarrierListData(const Json::Value& data, FR_Barrier& record);

	// 扫荡开始;
	void  procStartSweep(const char* szData);

	// 扫荡倒计时;
	void  sweepCountDown(float delta);

	// 扫荡结算;
	void  handleSweepSettlement(int nIndex);
	void  procSweepSettlement(const char* szData);

	// 刷新关卡状态并处理预设章节;
	void  procUpdateChapterState();

	// 处理服务器返回的领取星级奖励操作结果;
	void  procStarReward(const char* szData);

	// 跳转到指定章节/关卡;
	void  jump2ChapterBarrier();

	// 重置关卡;
	void  procResetBarrier(const char* szData);

private:

	static ChapterController*  m_pInstance;

	// 章节和关卡视图;
	LayerFrame*			m_pChapterFrameView;
	LayerChapter*		m_pChapterView;
	LayerBarrier*		m_pBarrierView;

	// 预设的目的章节和关卡;
	int  m_nDstChapterId;
	int  m_nDstBarrierId;

	// 当前选中的章节Id;
	int   m_nCurSelChapterId;

	// 当前选中的关卡Id(主Id和具体难度关卡Id);
	int   m_nCurSelBarrierId;

	// 扫荡状态;
	SweepState  m_sweepState;

	// 当前正在领取的星级奖励索引;
	int  m_nCurStarIndex;

	// 扫荡类型(0-扫荡券  1-金币扫荡);
	int  m_nSweepType;

	// 扫荡展示结果次数;
	int  m_nSweepResultTimes;

};

#endif // ChapterController_h__