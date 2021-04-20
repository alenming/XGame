/************************************************

阵法模块UI

************************************************/

#ifndef LayerTactics_h__
#define LayerTactics_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "TacticsController.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//排属性
typedef struct row_Attributes_Data
{
	ImageView* imgRowAttributesImg;						//排属性图片
	Text* txtRowAttributesKind;							//排属性种类
	Text* txtRowAttributesValue;						//排属性值
	Text* txtRowAttributesValueUp;						//排升级属性值
	ImageView* imgRowUpgradeFlag;						//排属性值升降标志

	row_Attributes_Data()
	{
		memset(this, 0, sizeof(row_Attributes_Data));
	}
}sRow_Attributes_Data;

//阵法选项
typedef struct tactics_List
{
	Layout* pnl;									//阵法容器
	Button* btnTactics;								//阵法按钮
	Text*	txtName;								//阵法名字(文字)
	ImageView* imgFlag;								//阵法是否正在使用标志
	Text*	txtLevel;								//阵法等级

	tactics_List()
	{
		memset(this, 0, sizeof(tactics_List));
	}
}sTactics_List;

typedef struct tagUI_Tactics 
{
	//根节点
	Layout*		pRoot;							

	//阵法主界面
	Layout* pnlMainView;							//阵法主界面
	Text* txtFightPower;							//战斗力
	Text* txtTacticsAttributes;						//阵法属性
	Button* btnRobMaterial;							//掠夺入口
	
	Text* txtFrontLv;								//前排等级
	Text* txtMidLv;									//中排等级
	Text* txtBackLv;								//后排等级

	Button* btnFrontRow;							//前排按钮
	Button* btnMidRow;								//中排按钮
	Button* btnBackRow;								//后排按钮
	ui::ScrollView* scrollTacticsList;				//阵法列表
	Text* txtOpenTips;								//开启阵法提示
	vector<sTactics_List> sTacticsList;				//阵法列表

	//排位强化界面
	ImageView* imgRowView;							//排视图
	ImageView* imgRow;								//选中排图片
	Text* txtRowLv;									//排等级
	Text* txtRowLvUpExp;							//升级所需经验
	LoadingBar* barExpProgress;						//经验进度条
	LoadingBar* barExpProgressPreview;				//经验进度预览
	vector<sRow_Attributes_Data> sRowData;			//排属性相关
	Text* txtRowLvUpCoin;							//排升级银币
	vector<Button*> btnPropAdd;						//添加升级材料
	vector<Button*> btnPropCancel;					//取消添加的材料
	Button* btnAutoAdd;								//排升级自动添加道具
	Button* btnUpgrade;								//强化

	//精炼界面
	ImageView* imgRefineView;						//精炼视图
	Layout* pnlTactics;								//阵法图标
	ImageView* imgTacticsIcon;						//阵法图标
	Layout* pnlMaterial1;							//材料1
	Layout* pnlMaterial2;							//材料2
	Text*	txtTacticsName;							//阵法名
	Text*	txtUnactivated;							//未激活
	Text*   txtQuality;								//精炼品质
	Text*	txtAttribute1;							//属性1
	Text*   txtAttribute2;							//属性2
	Text*	txtNextLvCoin;							//升级所需银币
	Text*	txtNeedMaterailTips;					//所需数量/等级/未激活
	Text*	txtMaterialCount;						//所需材料
	Button* btnUse;									//使用
	Button* btnRefine;								//精炼

	//选择道具界面
	Layout* pnlSelExpProp;							//选择道具视图
	Text*   txtCurExp;								//当前经验
	Text*   txtNextLvExp;							//下一级需要经验
	Button* btnSure;								//确认按钮
	Button* btnClose;								//关闭

	Layout* tbViewPnl;								//滚动列表

}UI_Tactics;

enum UI_Tactics_BtnFlag
{
	Btn_Front_Row,
	Btn_Mid_Row,
	Btn_Back_Row,
	Btn_PropAdd1,
	Btn_PropAdd2,
	Btn_PropAdd3,
	Btn_PropAdd4,
	Btn_PropCancel1,
	Btn_PropCancel2,
	Btn_PropCancel3,
	Btn_PropCancel4,
	Btn_AutoAdd,
	Btn_Upgrade,
	Btn_Use,
	Btn_Refine,
	Btn_Close,
	Btn_Sure,
	Btn_Rob_Mat
};

enum Sel_Tactics_BtnFlag
{
	Btn_Tactics_First,
	Btn_Tactics_Second,
	Btn_Tactics_Third,
	Btn_Tactics_Fourth,
	Btn_Tactics_Fifth,
	Btn_Tactics_Sixth,
	Btn_Tactics_Seventh
};

//记录排升级前的一些属性
typedef struct curRowAtr
{
	int curRowLevel;			//当前排等级
	int curRowAtk;				//当前排攻击值
	int curRowHp;				//当前排血量值
	int curRowPdd;				//当前排物防值
	int curRowMdd;				//当前排法防值

	curRowAtr()
	{
		curRowLevel = 0;
		curRowAtk = 0;
		curRowHp = 0;
		curRowPdd = 0;
		curRowMdd = 0;
	}
}sCurRowAtr;

class LayerTactics : public Layer, public Observer
{
public:
	LayerTactics();
	~LayerTactics(void);

	//CREATE_FUNC(LayerTactics);
	static LayerTactics* create(bool isPush);

public:
	virtual bool  init(bool isPush);

	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

public:
	void initUI();

	//初始化阵法主界面
	void initMainTacticsUI();

	//初始化排强化界面
	void initRowUpgradeUI();

	//当勾选的排升级材料足够时，显示下一级增加的属性预览
	void showRowNextLevelAttributes();

	//初始化精炼界面
	void initRefineUI();

	void onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	//选择阵法
	void onBtnSelTacticsClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	//刷新道具选择界面
	void updateSelExpPropView();

private:
	//读取排升级的道具数据
	void initRowUpMaterialData();

	//更新排界面
	void updateRowView();

	//更新是否有新阵法被激活
	void updateNewActivedTactics();

	//排和阵法精炼界面切换
	void switchRowAndRefineView(int type);

	//更新从服务器读过来的阵法列表数据
	void updateTacticsDataList();

	//使用(更换)阵法后更新数据
	void updateCurTacticsData();

	//阵法强化后更新阵法精炼视图
	void updateDataRefineView();

	//阵法强化后更新阵法列表品质
	void updateTacticsQuality();

	//更新战斗力
	void updateDataFightPower();

	//设置经验预览进度条值和动画
	void setPreviewProgressBar(float percentValue);
	//初始化经验预览进度条值并卸载动作
	void unLoadPreviewProgressBar();

	//排升级属性值和增加属性的标识动画
	void setPreviewNextValue();
	//卸载属性值和增加属性的标识动画
	void unLoadPreviewNextValue();

	//排切入动画
	void rowEnterAni();
	//阵法切入动画
	void tacticsEnterAni();

	//指定阵法激活文字动画
	void startNewTacActiviedAni();
	//指定阵法激活特效
	void newTacActiviedIconAni();
	
	//设置指定阵法为激活状态
	void setTacticsActiviedState();

	//播放阵法精炼的特效
	void playUpgradeTacAni();

	//播放排强化进度条特效
	void playRowProgressBarAni();
	//播放排强化文字的特效
	void playUpgradeRowAni();

	//掠夺后需要刷新阵法精炼材料数量
	void updateTacMatCount();

	virtual void updateSelf(void* data);

private:
	//场景进入方式
	bool m_bIsPushScene;

	UI_Tactics m_ui;

	//当前选中排
	eRowType m_eCurRowType;

	//当前选中的阵法(不是当前使用的)
	int m_iCurTacticsKind;

	//阵法是否满足精炼条件
	bool m_bIsTacticsMatEnough;

	//排升级道具Id
	vector<int> m_vRowUpgradeMatId;

	//当前选中的经验材料需消耗的银币
	int m_iCostCoin;

	//新阵法激活的序号
	int m_iIdx;

	//记录排升级前的已激活的阵法
	bool m_activiedTacs[NUM_TACTICS_LIST];

	//记录强化前排相关属性, 播放动画用到
	sCurRowAtr m_sCurRowAtr;

	//记录精炼的阵法，防止动画播放时快速切换导致UI显示不正常的问题
	int m_iCurRefineTacticsKind;

	//记录排强化动画播放是否结束了，结束前不向后台请求，防止动画播放错乱
	bool m_bIsRowAniEnd;

	//记录播放动画时需要的排
	eRowType mPlayAniRow;

	//选中的排升级材料ID
	vector<int> m_vSeledMat;

	//当前是否是阵法精炼界面
	bool m_bIsCurRefineView;
};

#endif LayerTactics_h__