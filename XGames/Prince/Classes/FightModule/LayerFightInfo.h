#ifndef LayerFightInfo_h__
#define LayerFightInfo_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "UI/UIDef.h"
#include "UIComboItem.h"
#include "FightStateMachine.h"
#include "CCArmature.h"
#include "Temp/CustomPop.h"

using namespace cocostudio;
using namespace cocos2d;
using namespace ui;

class SceneFight;

typedef struct tagUI_FIGHT_INFO
{
	Layout*	pRoot;

	//怒气底
	ImageView* pSpBackImg;
	ImageView* pSpTextImg;
	//怒气值
	Text* pSpText;
	//两个怒气条
	ProgressTimer* mBlueBar;
	ProgressTimer* mRedBar;
	Armature* mSpAni;

	//Combo
	Layout* pMyComboPanel;
	Layout* pEnemyComboPanel;

	//按钮父结点
	Layout* pBtnRightPanel;
	//加速按钮
	Button* pSpeedX1Btn;
	Button* pSpeedX2Btn;
	Button* pSpeedX3Btn;
	//自动手动按钮
	Button* pAutoFightBtn;
	//手动按钮
	Button* pManualFightBtn;
	//跳过按钮
	Button* pSkipBtn;

	//按钮父结点
	Layout* pBtnLeftPanel;
	//逃跑按钮
	Button* pEscapeBtn;

	//回合
	Layout* pRoundPnl;
	ImageView* pRoundImg;
	TextAtlas* pRoundAtlas;

	//速度
	Text* pMySpeed;
	Text* pEnemySpeed;

	//自动战斗动画
	Armature* mAutoFightAni;

	//左上角按钮位置
	Point leftBtnPoint1;

}UI_FIGHT_INFO;

enum class FIGHT_BTN_ID
{
	SPEED_X1 = 0,
	SPEED_X2,
	SPEED_X3,
	AUTO_FIGHT,
	MANUAL_FIGHT,
	SKIP,
	ESCAPE
};

class LayerFightInfo: public cocos2d::Layer
{

public:

	LayerFightInfo();
	~LayerFightInfo();

	CREATE_FUNC(LayerFightInfo);

public:

	//初始化
	virtual bool init();

	//界面显示回调
	virtual void onEnter();

	//界面退出回调
	virtual void onExit();

	//更新UI
	virtual void updateUI();

	//清空连击数
	void clearHitCount();

	//增加连击数
	void addHitCount(FightStateMachine::E_SIDE side);

	//设置跳过按钮是否显示
	void setSkipBtnVisible(bool visible);

	//设置加速按钮是否显示
	void setSpeedBtnVisible(bool visible);

	//设置跳过按钮是否显示
	void setEscapeBtnVisible(bool visible);

	//播放回合数动画
	void playRoundAni();

	//设置加速
	void setFightSpeed(int speedType, bool isLock = false);

	//设置加速按钮不显示
	void setFightSpeedBtnHide();

	//设置自动/手动
	void setAutoFight(bool autoFight, bool gray = false);

	//手动按钮灰色不可点击
	void setManualFightBtnDisable();

	//设置跳过按钮和逃跑按钮
	void setSkipEscapeBtnVisible(bool skip, bool escape);

	//显示技能tips
	void showSkillTips(int skillId, int skillType);

protected:

	//初始化UI
	virtual bool initUI();

	//按钮响应接口
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, FIGHT_BTN_ID btnId);

	//退出游戏响应接口
	void escapeCallBack( Ref* pSender, CustomRetType type );

private:

	UI_FIGHT_INFO m_ui;

	//Combo组件
	UIComboItem* mMyComboItem;
	UIComboItem* mEnemyComboItem;

	SceneFight* mParent;

	//X2加速按钮点击次数
	unsigned long mX2SpeedClickCount;

	//追打次数
	int mRushCount;
};



#endif // LayerFightInfo_h__
