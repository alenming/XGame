/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		FormationHero
 *  Description:	������Ӣ�۵ķ�װ;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/19/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef FormationHero_h__
#define FormationHero_h__

#include "FormationDef.h"
#include "FightModule/UIFighter.h"

class FormationHero : public Node
{
public:
	~FormationHero(void);

	static FormationHero* create(const UI_HERO_LIST_PARAM param);

	virtual bool  init(const UI_HERO_LIST_PARAM param);

	// ������ɫ;
	virtual void  setColor(Color3B color);

	// ����͸����;
	void  setTransparent(int nValue);

	// ����Ѫ��(0-100);
	void  setHpBarPercent(float fPercent);

	// ����׷��;
	void  updateCombo(UI_HERO_LIST_PARAM param);

private:
	FormationHero(void);

	void  initUI();

	//string  getSkillStateIcon(AbstractSkill::RESULT_STATE nState);

private:

	// Hero����;
	UI_HERO_LIST_PARAM  m_HeroData;

	// ��������;
	SpineAnimation*		m_pSkelHero;

	// ǰ��/����״̬;
	ImageView* m_imgPreState;
	ImageView* m_imgNextState;

	// Ѫ��;
	ImageView*   m_imgHpBarBg;
	LoadingBar*  m_hpBar;
};

#endif // FormationHero_h__
