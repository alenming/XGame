/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		FormationHero
 *  Description:	阵型中英雄的封装;
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

	// 设置颜色;
	virtual void  setColor(Color3B color);

	// 设置透明度;
	void  setTransparent(int nValue);

	// 设置血条(0-100);
	void  setHpBarPercent(float fPercent);

	// 更新追打;
	void  updateCombo(UI_HERO_LIST_PARAM param);

private:
	FormationHero(void);

	void  initUI();

	//string  getSkillStateIcon(AbstractSkill::RESULT_STATE nState);

private:

	// Hero数据;
	UI_HERO_LIST_PARAM  m_HeroData;

	// 骨骼动画;
	SpineAnimation*		m_pSkelHero;

	// 前置/后置状态;
	ImageView* m_imgPreState;
	ImageView* m_imgNextState;

	// 血条;
	ImageView*   m_imgHpBarBg;
	LoadingBar*  m_hpBar;
};

#endif // FormationHero_h__
