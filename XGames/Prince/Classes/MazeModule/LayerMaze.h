#ifndef LayerMaze_h__
#define LayerMaze_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "IMazeView.h"
#include "MazeController.h"
#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"
#include "LevelUp/LevelUpController.h"

#include "LayerMazeLimitBuy.h"
#include "LayerMazeLimitBuyItem.h"
#include "MainModule/MainModel.h"
#include "Widget/LayerCommHeadInfo.h"

class LayerMaze : public ModalLayer,public Observer,public TimeStampListener, public IMazeView, public IMazeLimitBuyItemObserver, IMazeLimitBuyObserver 
{

public:
	LayerMaze();
	~LayerMaze();

	static LayerMaze* create(bool pushScene);
	
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	bool init(bool pushScene);
	virtual void onExit();
	virtual void onEnter();

	virtual void onGetAllMazesResult(const int& nResult); // 进入迷宫首先获取信息更新迷宫界面	
	virtual void onRandomRoadResult(const int& nResult); // 接收迷宫随机事件Id（对话 遭遇 boss 限时购买）
	virtual void onRewardResult(Maze_ReWard_Show reward); //事件奖励

	virtual void onOpenBoxResult(const int& boxId);  //打开宝箱
	virtual void onOpenUnCompleteBoxResult(const int& boxId);  //打开宝箱

	virtual void onOpenBoxEndResult(); //关闭宝箱
	virtual void onRewardOpenCardResult(CHOOSEED_ITEM_REWARD reward);

	virtual void onMazeLimitClicked();
	virtual void onMazeLimitBuyEnd();
	virtual void onMazeLimitBuyWait();
public:	
	void setFilishedFlag(bool finish) {m_isFilishedFlag = finish;}; 

protected:
	void initUI();
	void updateMazeInfo();
	void updateRandomInfo(const int& eventId);
	//按钮事件
	void onBtnTalkNext(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onBtnBoxOpen(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId);
	void onBtnMeetChallenge(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId); //偶遇强敌挑战
	void onBtnMeetRound(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId); //偶遇强敌逃跑
	void onMainClicked(Ref* sender, Widget::TouchEventType type); //点击场景获取事件
	void onMapListClicked(Ref* sender, Widget::TouchEventType type, int nId,int i);//点击藏宝图信息
	void onChooseCard(Ref* sender,Widget::TouchEventType type,int index); //点击宝藏翻牌
	void onGiveUpChooseCard(Ref* sender,Widget::TouchEventType type); //确定放弃抽卡
	void onCancleGiveUpChooseCard(Ref* sender,Widget::TouchEventType type); //取消放弃抽卡
	void baozangClose(Ref* sender, Widget::TouchEventType type);

	void showPowerLimit();
	void talkEvent(const int& nId); //对话事件
	void meetEvent(const int& nId); //偶遇事件
	void addChooseCardPanel(); //添加翻牌界面
	void setChooseCardFreeCount(int openCount); 
	bool baozangEvent(); //宝藏事件
	void cardObCamera();  //翻牌动画


	void boxEvent(const int& nId);
	void showLimitBuy();
	void showLimitBuyItem();

	//下端显示文字
	void setNotice(const string& strContent);
	void setTalkInfo(const string& strName, const string& strContent);

	//横幅闪烁
	void hengFuAnimition(int idx);
	void hengFuAnimitionCallFunN(cocos2d::Node* pNode,int idx);

	//场景抖动前进
	void goAnimition();
	void nextBtnAnimition(bool isStart=true);

	void fight(const int& fightId); //发送战斗信息

	void gotoFirst();  //恢复初始状态
	void returnFirst(const float& fDelay=0.0);
	void callBackReturnFirst(float dt);


	//对话动态显示
	void animationTalkOut(string strText, bool isL2R=true);
	void callBackTalkOut(float dt);

	//文字图片跳动
	void setImgJump(vector<ImageView*> vec);
	void callFuc(ImageView *img);

	//倒计时
	void startCountDown(int sec);
	//超时通知
	virtual void timeOut();
private:
	// UI控件集合
	UI_Maze		m_ui;

	int         m_nCurMazeId;
	// 
	MazeController* m_pMazeController;

	//
	MAZE_STEP_STATE m_stepState;   //迷宫事件的各个阶段
	int				m_nCurEventId;
	int             m_TreasureMapId;   //本次打开的藏宝图ID
	int             m_BoxId;
	int             m_openCount; //已经翻牌次数
	bool            m_isOrbitCamera; //是否在翻牌
	int             m_spendGoldCount; //翻牌发费

	int				m_nMapAnimitionIndex;
	int				m_nMapAnimitionEndIndex;
	bool			m_isFilishedFlag;     //事件是否完成
	bool			m_isLimitFlag;
	bool			m_isAnimitionRunning;
	//正在获取网络数据
	bool			m_isGetDataFlag;
	//对白
	std::string		m_strTalkContent;
	int				m_nTalkContentLength;
	bool   m_isPushScene;
};

#endif // 
