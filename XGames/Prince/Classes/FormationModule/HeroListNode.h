/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		HeroListNode
 *  Description:	����Ӣ���б�ڵ�ķ�װ;
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

	// ����ѡ��;
	void  updateSelected(bool bSelected);

	// ����ѡ��״̬;
	void  updateOnfieldState(bool bIsOnfield);

	// ����׷��;
	void  updateCombo(UI_HERO_LIST_PARAM* pParam);

	// ���»�ؿ�ƥ��״̬;
	void  updateActivityFlag(vector<int>  vcDstTemplateId);

private:
	HeroListNode(void);

	// UI;
	void  initUI(const UI_HERO_LIST_PARAM* pParam);

	// ǰ��/���ü���ͼ��;
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
