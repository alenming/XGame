#ifndef TalentLayer_h__
#define TalentLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/UIDef.h"
#include "Utils/Observer.h"
#include "TalentController.h"
#include "Widget/SkillTips.h"
#include "TalentData.h"
#include "TalentModel.h"

USING_NS_CC;
using namespace std;
using namespace cocostudio;
using namespace ui;

//左侧技能描述UI
struct TalentDesUI
{
	//左侧UI
	ImageView*		  selectedSkillIcon;			//选中的技能icon
	ImageView*		  selectedSkillType;			//选中的技能类型
	Text*			  selectedSkillName;			//选中的技能名称
	SimpleRichText*	  selectedSkillDes;				//选中的技能描述
	Layout*			  selectedSkillCD;				//选中奥义技能CD相关
};

struct TalentSkillUI
{
	TalentSkillUI()
		: skillBox(nullptr)
		, skillIcon(nullptr)
		, skillNewOpen(nullptr)
		, selectedState(nullptr)
		, mask(nullptr)
		, skillUnlockLvl(nullptr)
		, skillName(nullptr)
	{}

	//右侧UI
	CheckBox*  skillBox;
	ImageView* skillIcon;		
	ImageView* skillNewOpen;	//新开启标志
	ImageView* selectedState;	//选中态
	ImageView* mask;			//遮罩
	Text*	   skillName;
	Text* skillUnlockLvl;	
	Layout*    skillPnlClick;	//代理触摸层
};

class TalentLayer : public Layer, public Observer
{

public:

	TalentLayer();
	virtual ~TalentLayer();

	CREATE_FUNC(TalentLayer);

public:

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual void update(float dt);

	virtual void updateSelf(void* data) override;

private:

	//初始化UI
	void initUI();

	//更新左侧UI
	void initSkillDesUI();

	//面板切入动画
	void showEnterAni();

	//更新技能列表
	void initSkillList(Node* parent, vector<TalentSkillUI>* vecSkillList, string name, int skillTypeId);

	//更新技能列表
	void updateSkillList(vector<TalentSkillUI>* vecSkillList, int currSklId);

	//更新所有技能
	void updateSkill();

	//获取当前主角等级
	int getMainHeroLvl();

	//技能选取回调
	void onSkillSelected(Ref* target, Widget::TouchEventType eventType, vector<TalentSkillUI>* vecSkillList);

	//关闭按钮回调
	void onBtnClose(Ref* target, Widget::TouchEventType eventType);

	//显示技能tips
	void showTips(TalentSkillData* selectData);

	//清理其它状态
	void resetCheckBoxState();

private:

	//根节点
	Layout* m_pRoot;
	TalentDesUI m_DesUI;

	TalentModel* m_pModelInstance;
	vector<TalentSkillData> mVecSkillData;

	//当前英雄等级
	int mCurMainHeroLv;

	//图片底
	ImageView* mBackImg; 

	//通信监听器
	TalentController mTalentController;

	//主动技能列表
	vector<TalentSkillUI> mVecManuSklList;

	//普通攻击列表
	vector<TalentSkillUI> mVecNomalSklList;

	//被动1列表
	vector<TalentSkillUI> mVecRushSklList;

	//被动2列表
	vector<TalentSkillUI> mVecBuffSklList;

	//被动3列表
	vector<TalentSkillUI> mVecTriggerSklList;
};




#endif //TalentLayer_h__