/******************帮派boss主界面**********************/

#ifndef GroupBossLayer_h__
#define GroupBossLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "GroupBossModel.h"
#include "GroupBossController.h"
#include "DataManager/DataManager.h"
#include "Utils/LocalTimer.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//UI结构
typedef struct tag_MainUI
{
	Layout* pRoot;			//根节点

	//活动关闭标识
	ImageView* imgActEnd;

	//帮派排名
	Layout* pnlGroupRank;
	Layout* groupRankTemp;
	ImageView* imgMyGroupRank;

	//个人排名
	Layout* pnlPlayerRank;
	Layout* playerRankTemp;
	ImageView* imgPlayerRank;

	//击退boss宝箱
	Text* txtBossName;
	Button* btnGetAll;			//一键领取
	vector<Button*> vBtnBox;
	LoadingBar* barBoxReward;

	//排行榜入口按钮
	Button* btnRankEnterGroup;
	Button* btnRankEnterPersonal;

	//右侧UI
	Text* txtDownTime;			//倒计时
	Button* btnHelp;
	Button* btnInspire;
	Button* btnAtk;
	Button* btnExchangeReward;
	Text* txtDamageAdd;
	Layout* pnlRevive;			//重生层
	Text* txtReviveTime;		//重生倒计时

	//英雄角色UI
	ImageView* imgAttriBg;
	TextAtlas* txtBossLv;
	Text* _txtBossName;
	vector<ImageView*> imgBossLife;
	LoadingBar* barBossHp;
	Layout* pnlBossAni;
	TextAtlas* txtBossOrder;
	vector<ImageView*> imgSkillBg;

}sTag_MainUI;

//按钮标记
typedef enum tag_BtnFlag
{
	BTN_GROUP_RANK,
	BTN_PLAYER_RANK,
	BTN_GETALL_REWARD,
	BTN_BOSS_HELP,
	BTN_EXCHANGE_REWARD,
	BTN_INSPIRE,
	BTN_ATTACK,
	BTN_BOX_REWARD
	
}eTag_BtnFlag;

class GroupBossLayer : public Layer, public Observer, public TimeLocalListener
{
public:
	GroupBossLayer();
	~GroupBossLayer(void);

	static GroupBossLayer* create(bool isPush);
	virtual bool  init(bool isPush);
	virtual void onEnter();
	virtual void onExit(); 
	virtual void timeTick(int dt);

private:

	void initUI();
	void onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );
	void update(float dt);
	void updateUI();

	//刷新左侧排行榜
	void updateRankUI();

	//刷新宝箱状态
	void updateBoxUI();

	//boss宝箱可领取状态动画
	void showCanGetAni(Node* parentNode);
	//移除可领取状态动画
	void removeCanGetAni(Node* parentNode);

	//刷新boss状态
	void updateBossUI();

	//刷新鼓舞信息
	void updateInspireUI();

	//开启定时器
	void openTimer();

	//倒计时UI
	void showDownTime(int dt);

	//boss冷却倒计时
	void showCoolTime(int dt);

	//更新主界面定时器
	void updateMainUI(float dt);

	//展示boss动画
	void showBossAni();

	//粒子动画
	void showBossBgAni();

	//奖励兑换的小红点
	void showRedFlag();

	//boss剩余生命数量
	void showBossLifes();

	//boss宝箱奖励弹窗
	void showBossReward(void* data);

	//boss技能相关
	void initSkillInfo();
	void setBossSkill(const int idx,RowData* pRowPetData,const string& str);
	void onBtnSkillClick(Ref* ref,Widget::TouchEventType type, int idx,const string& str,Color3B color);

	//格式化倒计时时间: xx时xx分xx秒
	string formartTime(int restTime);

	virtual void updateSelf(void* data);


private:
	sTag_MainUI m_ui;

	//是否pushScene进来
	bool m_bIsPush;

	//记录bossResId，不需每次刷新界面都创建boss骨骼动画
	int m_iBossResId;

	//前三排名列表
	vector<Layout*> m_vGroupRank;
	vector<Layout*> m_vPlayerRank;
};


#endif