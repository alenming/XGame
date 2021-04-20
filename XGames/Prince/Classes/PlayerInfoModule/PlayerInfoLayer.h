#ifndef PlayerInfoPlayer_h__
#define PlayerInfoPlayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "Utils/Observer.h"
#include "MainModule/MainModel.h"
#include "Widget/EditBoxEx.h"
#include "Widget/ButtonSwitch.h"
#include "PlayerInfoController.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace std;


typedef struct tagUI_PlayerInfo 
{
	Layout*		pRoot;							//根节点

	//玩家信息界面
	Layout*		pnlHead;			//玩家头像
	ImageView*  imgPlayerInfoPnl;	//玩家信息层
	ImageView*  imgModifyNamePnl;	//修改名称层
	ImageView*  imgSysSettingPnl;	//系统设置层
	Button*		btnVip;				//玩家vip等级
	ImageView*  imgCard;			//月卡/年卡
	Text*		txtName;			//玩家昵称
	Text*		txtGang;			//帮派
	Text*		txtUID;				//UID码
	Text*		txtExp;				//经验值
	Text*		txtPower;			//体力值
	Text*		txtEnergy;			//精力值
	Text*		txtPowerTime;		//体力恢复时间
	Text*		txtEnergyTime;		//精力恢复时间
	Text*		txtPowerWillFull;	//后体力恢复满
	Text*		txtEnergyWillFull;	//后精力恢复满
	Text*		txtPowerFull;		//体力已满
	Text*		txtEnergyFull;		//精力已满
	LoadingBar*	barExp;				//经验条

	//修改昵称界面
	TextField*		txtInputName;	//输入名字/随机
	Text*		txtPrice;			//修改昵称需要的金币
	EditBoxEx*  editBoxName;		//输入名字框

	//系统设置界面
	TextField*		txtInputCode;	//输入兑换码
	EditBoxEx*  editBoxCode;		//输入框
	Text*		txtVersion;			//版本号

	tagUI_PlayerInfo()
	{
		memset(this, 0, sizeof(tagUI_PlayerInfo));
	}
}UI_PlayerInfo;

//按钮对应的标志
enum UI_PlayerInfo_Widget
{
	btn_infoCloseBtn,				//玩家信息界面关闭按钮
	btn_modifyCloseBtn,				//修改昵称界面关闭按钮
	btn_settingCloseBtn,			//系统设置界面关闭按钮
	btn_vipLvBtn,					//vip等级按钮
	btn_monthCard,					//月卡
	btn_modifyNickBtn,				//修改昵称
	btn_backToLoginBtn,				//返回登录
	btn_sysSettingBtn,				//系统设置
	btn_sysGmBtn,                      //客服系统
	btn_diceBtn,					//骰子
	btn_cancelBtn,					//取消
	btn_sureBtn,					//确定
	btn_exchangeBtn					//兑换
};

enum UI_SysSet_Switch_Tag
{
	switch_musicTag = 1001,			//音效开关tag
	switch_soundTag,
	swtich_powerTag,				//体力已满tag
	swtich_energyTag,				//精力已满tag
	swtich_freeTipsTag,				//免费体力tag
	swtich_bossTag					//世界boss
};

class PlayerInfoLayer : public ModalLayer, public Observer
{
public:
	PlayerInfoLayer();
	~PlayerInfoLayer();
	CREATE_FUNC(PlayerInfoLayer);

	//提供一个直接进入系统设置
	static PlayerInfoLayer* createSysSetLayer();

public:
	bool init();

	virtual void onEnter();
	virtual void onExit();

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	//按钮回调
	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//初始化各种UI
	void  initUI();
	//初始化玩家信息界面
	void  initPlayerInfoView();		
	//初始化修改昵称界面
	void  initModifyNameView();
	//初始化系统设置界面
	void  initSystemSetView();

	//展示兑换获得的奖励
	void  showExChangeReward();

	//修改昵称
	void setRoleName();

	//更新体力/精力倒计时
	void  updateResTime(int st, bool isPower);

	//设置按钮的开关处理
	void  valueChanged(Ref* sender, Control::EventType controlEvent);

private:

	UI_PlayerInfo m_ui;
	MainParam*	  playerInfo;

	//角色名字列表
	vector<string> mNameList;

	//当前名字索引
	int mNameIndex;

	//是否直接切入系统设置
	bool m_bIsEnterSysSet;

	//开关按钮
	vector<ButtonSwitch*> m_vBtnSwitch;

	PlayerInfoController mController;
};

#endif