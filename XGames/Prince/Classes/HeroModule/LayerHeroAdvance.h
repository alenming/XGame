#ifndef LayerHeroAdvance_h__
#define LayerHeroAdvance_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "UITagHero.h"
#include "DataManager/DataManager.h"
#include "Widget/SimpleRichText.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagLayerHeroAdvance
{
	//根结点
	Layout* mRoot;
	//卡牌面板
	Layout* mCardPanel;
	
	//技能标题
	ImageView* mSkillStrength;
	ImageView* mSkillNew;
	
	//技能信息
	ImageView* mImgSkillPnl;		//技能面板
	Layout* mSkillIconPanel;
	ImageView* mSkillTypeImg;
	Text* mSkillNameTxt;
	SimpleRichText* mSkillDesc;
	Text* mNoSkillTips;

	//战斗力面板
	ImageView* mImgFightPower;
	Text*	   mLabOldFight;
	Text*	   mLabNewFight;
	
	//属性面板
	Layout* mAdvPropertyPnl;

	//点击提示
	ImageView* mClickImg;

	//箭头
	ImageView* mArrowImg;

	//觉醒名字
	Text* mName1;
	Text* mName2;

	//觉醒数值
	Text* mSpeed1;
	Text* mAtk1;
	Text* mHp1;
	Text* mPdd1;
	Text* mMdd1;
	Text* mSpeed2;
	Text* mAtk2;
	Text* mHp2;
	Text* mPdd2;
	Text* mMdd2;
	//需根据攻击类型设置
	Text* mAtkText;
	ImageView* mAtkIcon;

	//蒙版
	Layout* mPnlMask;
};

enum class SKILL_UPDATE_TYPE
{
	SKILL_NONE,		//没有增加技能
	SKILL_ADD,		//新增技能
	SKILL_UPDATE	//技能强化
};

struct SkillUpdateParam
{
	SKILL_UPDATE_TYPE updateType;
	int skillId;
	int skillType;

	SkillUpdateParam()
		: updateType(SKILL_UPDATE_TYPE::SKILL_NONE)
		, skillId(-1)
		, skillType(-1)
	{}
};

class LayerHeroAdvance : public ModalLayer
{

public:

	LayerHeroAdvance();
	virtual ~LayerHeroAdvance();

	static LayerHeroAdvance* create(int templetId);

public:

	virtual bool init(int templetId);
	virtual void onEnter();
	virtual void onExit();

private:

	//初始化UI
	void initUI();

	//更新UI
	void updateUI();

	//初始化动画
	void initAnimation();

	//动画回调
	void aniAniCallBack(Armature *armature, MovementEventType movementType, const std::string& movementID, const string& nextAniName);

	//启动技能特效动画
	void startSkillAni();

	//新技能开启图标的动画
	void startSkillIconAni();

	//技能更新参数
	SkillUpdateParam getSkillUpdateParam(RowData* advInfo1, RowData* advInfo2);

	//设置是否可点击
	void setLayerCanClicked(bool isCanClick);

	//觉醒后检测是否需要刷新英雄缘分
	void isHeroFateChanged();

private:

	UITagLayerHeroAdvance mUI;

	//英雄模版ID
	int mHeroId;

	//是否有技能开启或强化
	bool mIsSkillUpdate;

	//添加一个变量记录新开启/强化的技能类型
	int m_skillType;
};

#endif //LayerHeroAdvance_h__