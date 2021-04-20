#ifndef LayerCharacter_h__
#define LayerCharacter_h__

#include "UITagHero.h"
#include "UI/ModalLayer.h"
#include "HeroModel.h"
#include "spine/spine-cocos2dx.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "HeroController.h"
#include "HeroModel.h"
#include "Widget/SkillTips.h"
#include "FormationModule/CustomScrollBar.h"
#include "StarsSuModule/StarsSuController.h"

using namespace spine;

//觉醒类型(品质/升星)
enum growType
{
	DEFAULT_TYPE,
	QUATITY_TYPE,
	STAR_UP_TYPE
};

class LayerHero : public ModalLayer, public Observer, public LayerSmithyObserver
{

public:

	LayerHero();
	virtual ~LayerHero();

	//创建构造函数
	CREATE_FUNC(LayerHero);

public:

	//初始化
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	//设置当前显示英雄的id
	void setCurrentHeroId(int heroId);

	//LayerSmithyObserver
	virtual void onLayerSmithyClose();

	//显示英雄
	void showHeroList(int curId = 0);

	//动画结束前，设置滚动区不可点击
	void setScrollViewClickEnabled(bool isCanClick);
	//动画结束前，设置觉醒/突破按钮不可点击
	void setAdvBreakBtnClickEnabled(bool isCanClick);

public :
	//设置英雄名字颜色，很多界面会用到
	static void setNameColor(Text* text, int color);

private:

	//初始化UI
	void initUI();

	//初始化英雄列表
	void initHeroList(int curId = 0);

	//初始化装备图标
	void initEquipIcon(UIToolHeader** equipIcon, Node* parent, HERO_BTN_ID btnId);

	//更新英雄UI
	void updateAllUI();
	//更新所有装备
	void updateAllEquip();
	//更新单个装备
	void updateEquip(Node* equipNode, int equipId, EQUIP_ITEM_TYPE equipType);
	//更新英雄属性
	void updateUIAttri();
	//更新进阶信息
	void updateAdvInfo();
	//更新卡牌
	void updateHeroCard();
	//更新英雄升星信息
	void updateHeroStarUpInfo();
	//更新万能碎片面板信息
	void updateAlmightyPanel();
	//兑换碎片后更新碎片数据和进度条显示
	void updateChipsDetails();

	//升星属性面板滑动事件
	void onStarsUpScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//更新缘分信息
	void updateFateUI();

	//添加spine骨骼动画
	void addHeroSpine();
	//播放粒子特效
	void updateHeroParticle();
	//添加卡牌
	void addHeroCard();

	//开始觉醒, 如果是突破需要做额外的触摸屏蔽操作
	void startAdvance(bool isBreak);
	//开始升星
	void startStarUp();

	//增加英雄属性值变化动画
	bool addAnimationAttri(HeroParam* heroParam);
	void onUpdateAnimationAttri(float ft);

	void onUpdateAnimationAdvAttri(float ft);

	//增加技能动画：觉醒时若激活了技能显示
	void addAnimationSkill(Button* skillBtn);
	//移出技能动画
	void removeAnimationSkill(Button* skillBtn);

	//移出装备特效
	void removeAllEquipEffe();

	//更新英雄红色标记
	void updateHeroRedFlag();

	//更新装备可进阶标记
	void updateEquipRedFlag();

	//数据更新通知接口
	virtual void updateSelf(void* data);

	//英雄列表响应
	void onHeroListClicked(Ref* ref, Widget::TouchEventType type, int nHeroId);
	void onScrollViewEvent(Ref* sender, cocos2d::ui::ScrollviewEventType type);

	//按钮响应
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);
	void onBtnFateClicked(Ref* ref, Widget::TouchEventType type, int nIndex);

	//进阶材料不足，点击到数据来源界面
	void onBtnMatClicked(Ref* ref, Widget::TouchEventType type, int ntmplateId);

	void popupToolInfo(EQUIP_ITEM_TYPE equipType);
	const int getToolId(EQUIP_ITEM_TYPE equipType, HeroParam* pParam);

	const int getNeedToolId(const EQUIP_ITEM_TYPE& equipType);
	UIToolHeader* getEquipBtn(const EQUIP_ITEM_TYPE equipType);

	//显示升星/觉醒确认窗口
	void showAdvComfirmLayer(growType type);

	//英雄列表滚动到当前英雄的位置
	void scrollHeroList();

	//显示进入动画
	void showEnterAnimation();

	//显示进入动画
	void showBtnEnterAnimation();

	//添加觉醒按钮动画
	void addAnimationStartAdv(Button* advBtn);

	//播放觉醒按钮动画
	void startAnimationStartAdv(Button* advBtn, bool play);

	//播放觉醒动画
	void startAnimationAdv();

	//觉醒预览属性的动画
	void startPreviewNextValue();

	//因为添加新英雄名字太长，动态调整 人物称号+点+名字 的坐标
	void setNickNamePos();

	//缘分属性和iconId转换
	int  getHeroFateIconId(string heroAttriName);

	//觉醒后检测是否需要刷新英雄缘分
	void checkHeroFate();

private:

	//角色界面UI成员
	HeroUI mUI;

	//当前显示的英雄序号
	int mCurrentHeroIndex;

	//当前显示的英雄id
	int mCurrentHeroId;

	//当前显示的英雄参数
	HeroParam* mHeroParam;

	//兵书马书开放等级
	int m_iHorseEquipOpenLv;
	int m_iBookEquipOpenLv;

	//通信器
	HeroController mHeroController;

	//是否为弹出窗口
	bool mIsPopUp;

	//是否首次进入该界面
	bool mIsFirstEnter;

	//记录本次觉醒/突破时消耗的材料类型数量
	int mMatKindsCount;

	//记录觉醒/突破时动画播放前的英雄ID, 防止快速切换英雄时导致动画播放出问题
	int mPlayAniHeroId;

	//升星属性面板滚动条
	CustomScrollBar* m_starsAttriScrollBar;

	//星宿
	StarsSuController *m_StarsSuController;

	//当前所在属性界面
	curPanel mCurPanel;
};


#endif //LayerCharacter_h__