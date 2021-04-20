/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		HeroListNode
 *  Description:	阵型英雄列表节点的封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/18/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef FormationListNode_h__
#define FormationListNode_h__

#include "FormationDef.h"

class HeroListNode : public Button
{
public:
	~HeroListNode(void);

	static HeroListNode* create(const UI_HERO_LIST_PARAM* pParam);

	virtual bool init(const UI_HERO_LIST_PARAM* pParam);

	// 更新选中;
	void  updateSelected(bool bSelected);

	// 更新选中状态;
	void  updateOnfieldState(bool bIsOnfield);

	// 更新追打;
	void  updateCombo(UI_HERO_LIST_PARAM* pParam);

	// 更新活动关卡匹配状态;
	void  updateActivityFlag(vector<int>  vcDstTemplateId);

private:
	HeroListNode(void);

	// UI;
	void  initUI(const UI_HERO_LIST_PARAM* pParam);

	// 前置/后置技能图标;
	//string  getSkillStateIcon(AbstractSkill::RESULT_STATE nState);

private:

	// Hero;
	UIToolHeader* m_hero;

	ImageView* m_imgPreState;
	ImageView* m_imgArrow;
	ImageView* m_imgNextState;
	ImageView* m_imgNoneState;
	ImageView* m_imgSame;

	UI_HERO_LIST_PARAM m_param;

};

#endif // FormationListNode_h__
