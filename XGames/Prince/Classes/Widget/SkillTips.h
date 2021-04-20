/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SkillTips
 *  Description:	技能tips封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/16/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef SkillTips_h__
#define SkillTips_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "SimpleRichText.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;


// 技能tips封装;
typedef struct tagUI_FormationSkillTips
{
	Layout*		imgBg;
	ImageView   *imgPanel;
	Text*		txtSkillType;
	Text*		txtSkillName;
	Text*		txtSkillDesc;

	Vec2		txtSkillTypePos;
	Vec2		txtSkillNamePos;
	Vec2		txtSkillDescPos;

	tagUI_FormationSkillTips()
	{
		memset(this, 0, sizeof(tagUI_FormationSkillTips));
	}
}UI_FormationSkillTips;


class SkillTips : public Node
{

public:

	SkillTips(void);
	~SkillTips(void);

	CREATE_FUNC(SkillTips);

public:

	virtual bool  init();

	//设定显示内容
	void setTipsContent(int skillType, string skillName, string skillDesc, int nDirection = 0);

	//设定显示内容
	void setSkill(int skillId, int nDirection = 0);

private:

	void  initUI();

private:

	UI_FormationSkillTips  m_ui;

	// 底图方向(0: left  1:right);
	int		m_nDirection;
};

#endif // SkillTips_h__
