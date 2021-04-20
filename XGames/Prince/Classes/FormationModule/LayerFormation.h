/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerFormation
 *  Description:	阵型UI;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/09/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerFormation_h__
#define LayerFormation_h__

#include "FormationDef.h"
#include "Utils/Observer.h"
#include "FormationController.h"
#include "FormationModel.h"
#include "SceneFormation.h"
#include "Temp/CustomTips.h"
#include "Widget/SkillTips.h"
#include "HeroListNode.h"
#include "FormationHero.h"

/*
enum Formation_Parent_Type
{
	TYPE_UNAVAILABLE,
	TYPE_THIS_SCENE,		// SceneFormation创建;
	TYPE_HERO,
	TYPE_ARENA,
	TYPE_TRAIL,
	TYPE_BARRIER,
	TYPE_MAIN_CITY
};
*/

class SceneFormation;
class LayerFormation : public Layer, public Observer
{
public:
	LayerFormation(void);
	~LayerFormation(void);

	//CREATE_FUNC(LayerFormation);
	static LayerFormation* create(/*Formation_Parent_Type type, */Formation_Mode mode = F_MODE_FORMATION);

	virtual bool  init(/*Formation_Parent_Type type, */Formation_Mode mode = F_MODE_FORMATION);

	virtual void  onEnter();
	virtual void  onExit();

	virtual void  updateSelf(void* pData);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// Hero列表选中;
	void  onHeroListClicked(Ref* ref, Widget::TouchEventType type, int nHeroId);

	// 阵型中选中;
	void  onHeroOnFieldClicked(Ref* ref, Widget::TouchEventType type, int nPos);

	// 阵位选中;
	void  onPosClicked(Ref* ref, Widget::TouchEventType type, HERO_POS nPos);

	// 弹窗回调;
	void  popCallbackFunc(Ref* ref, CustomRetType nResult);

	// 滚动事件;
	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	// test callback
	void  myTestCallbackFunc(Ref* ref, CustomRetType nResult);

private:

	void  initUI();

	// 更新Hero列表;
	void  updateHeroList(vector<UI_HERO_LIST_PARAM> pParam);

	// 更新阵型;
	void  updateFormation(UI_FORMATION_PARAM* pParam);

	// 英雄上阵;
	void  procHeroOnField(UI_HERO_LIST_PARAM param);

	// 英雄下阵;
	void  procHeroOffField(UI_HERO_LIST_PARAM param);

	// 英雄换位(1->2);
	void  procHeroSwap(UI_SWAP_FORMATION_PARAM param);

	// 更新底座状态;
	void  updatePosBaseState(HERO_POS nPos, int nHeroId, bool bSelected);

	// 更新技能展示;
	void  updateSkillInfo(const vector<FORMATION_SKILL_PARAM> vcSkillInfo);

	// 更新Combo;
	void  updateCombo(Combo_Skill_Param* pParam);

	// 更新当前选中;
	void  refreshCurSelected(const vector<int> vcSelected);

	// 拖动节点;
	void  enableDrag(Node* node, bool bEnable = true);

	// 根据Id查找Hero对应的本地阵型UI节点;
	FormationHero*  getFormationHero(int nHeroId);

	// 获取阵型位置坐标;
	Vec2  getFormationPosVec(HERO_POS nPos);

	// 位移判定;
	//virtual int   judgeEndPosition(Node* node, Vec2 posEnd);
	HERO_POS   judgeEndPosition(Node* node, Touch* touch);

	// 换位动画;
	virtual void  startAnimationPosChanging(Node* src, HERO_POS nDstPos);

	// 动画结束回调;
	virtual void  onAnimationDone(Node* pSender, HERO_POS nDstPos);

	// 长按回调;
	virtual void  updateLongPressed(float delta);

	// 从阵位计算显示层级;
	int  calcLocalZOrder(HERO_POS nPos);

	// 错误码;
	string  getFormationErrorMsg(int nErrCode);

	// 透明区域检测;
	bool  alphaCheck(ImageView* image, Point pt_Global);
	bool  alphaTouchCheck(ImageView* image, Touch* touch);

	// 移动Hero时，其余Hero半透明;
	void  setTransparentWithoutMe(Node* pSelf, bool bTrans = true);

	// 进场特效;
	void  playEnterAnimation();

	// 更新阵法数据;
	void  updateTacticsData();

	// 更新英雄追打展示;
	void  updateHeroCombo(int nHeroId);

	// ;
	void  addClickEvent(HeroListNode* hero, int nHeroId);
	void  removeClickEvent(HeroListNode* hero);

	// 刷新关卡英雄匹配;
	void  refreshActivityHeroList();

	// 活动关卡英雄列表名称;
	void  setHeroNameBright(int nIndex, string strName, bool bBright);

private:

	// 
	FormationController*  m_pController;

	UI_Formation  m_ui;

	// 当前选中Hero头像是否可拖动(长按);
	bool m_bEnableMove;

	// 当前触摸到的Pos;
	HERO_POS  m_nCurTouchPos;

	// 当前选中的Pos;
	HERO_POS  m_nCurSelPos;

	// Hero列表UI资源<Id, Hero>;
	map<int, HeroListNode*>		m_mapUIHeroList;

	// 阵型UI资源<Id, Hero>;
	map<int, FormationHero*>	m_mapUIFormationList;

	// 技能列表原始位置;
	vector<Vec2>				m_vcOriSkillPos;

	// 技能信息;
	vector<FORMATION_SKILL_PARAM>		m_vcSkillParam;

	// 是否拖动;
	bool	m_bIsDrag;

	// 是否长按;
	bool	m_bIsLongPress;

	// 是否已设置半透明;
	bool    m_bIsTransparent;

	// 阵型位置;
	Vec2  m_posSet[FORMATION_MAX_POS_COUNT];

	// 当前阵型显示模式;
	Formation_Mode  m_nMode;

	// 保存阵型后是否开始战斗;
	bool  m_bFighting;

	// 是否首次进入;
	bool  m_bIsFirstEnter;

	// 英雄点击判定标记;
	bool  m_bIsHeroClicked;

	// 是否允许点击;
	bool  m_bGlobalEnableClick;

	// 当前视图(英雄列表/追打展示)是否允许点击;
	bool  m_bIsShowComboList;

	enum UI_FORMATION_WIDGET
	{
		Btn_Back,

		// 阵型;
		Pnl_Formation,
		Img_Hero_List_Bg,
		ScrollView_HeroList,
		Img_Field,
		Img_Selected,

		// 技能;
		Btn_Edit_Skill,
		Btn_Skill1,
		Btn_Skill2,
		Btn_Skill3,
		Btn_Skill4,
		Btn_Skill5,

		// 连击;
		Btn_Combo,
		Btn_Close_Combo,
		Pnl_Combo,
		Img_Combo_Bg,
		Img_Combo_Num_1,
		Img_Combo_Num_2,

		// 阵法;
		Btn_Zhenfa,

		// 开始战斗;
		Btn_Fight
	};

};

#endif // LayerFormation_h__

