/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSign
 *  Description:	迷宫限时购买，商城抽卡抽到英雄通用;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			06/29/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef UICardHead_h_
#define UICardHead_h_

#include "CocosGUI.h"
#include "cocos2d.h"
#include "CocoStudio.h"
#include "ToolDef.h"
#include "DataManager/DataTable.h"
#include "HeroModule/HeroModel.h"
#include "Utils/StringFormat.h"
#include "Widget/SimpleRichText.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

enum UI_Card_Head_Pro
{
	CARD_PRO_POWER = 1,   //力量
	CARD_PRO_INTLIGENT,   //智力
	CARD_PRO_AGILITY,    //敏捷
};

const int STAR_CONT = 6;
const int SKILL_CONT = 5;

class UICardHead :public Node
{
public:
	UICardHead(void);
	~UICardHead(void);

	//传人英雄Id 是否阵容界面
	static UICardHead* create(int id,bool isLineup = false,bool isLimitBuy = false);  

	bool initUi(int id,bool isLineup,bool isLimitBuy);
	void updataUi();
	void initSkill(int id);
	void setSkill(const int idx,RowData* pRowPetData,const string& str);
	void setSkill(const int idx,const int skillId,const int skillKey); //主角专用

	//添加一个根据星级数获取星星图标的接口，播放升星动画时用到						智杰	2015.11.5
	ImageView* getStarImgByStarLevel(int starLevel);
	//添加一个根据技能类型获取技能图片的接口，播放新技能开启或者技能升级时用到		智杰	2015.11.6
	ImageView* getSkillImgBySkillType(int skillType);


	void initSkillPanel();

	void  onBtnClick(Ref* ref, CheckBox::EventType type,int idx,const string& str,Color3B color);
	void  onBtnClickPlayer(Ref* ref, CheckBox::EventType type,int idx,const string& str,const int skillId,
		const int skillKey,Color3B color);//主角专用


	void  onBtnSkillPanel(Ref* ref, Widget::TouchEventType type);

	ImageView* getScaleImg() {return m_scaleImg;};
private:
	int          m_id;
	bool         m_isLineup;
	bool         m_isLimitBuy;

	Layout*      m_pRoot;
	ImageView*   m_pBackImg;   //背景图片（白，绿，蓝，紫，橙）
	ImageView*   m_pPro;       // 属性

	ImageView*   m_pStar[STAR_CONT];   //星星
	ImageView*   m_pSkill[SKILL_CONT];  //技能
	CheckBox*      m_pSkillButton[SKILL_CONT];  //技能按钮


	Layout*      m_pSkillPanel;  //技能简介
	int          m_lastIndex;  //记录上次选中id
	ImageView*   m_pSkillPanelImg; //技能简介面板上技能图片
	Text*        m_pSkillPanelName;
	Text*        m_pSkillPanelActive;
	Text*        m_pSkillPanelDesc;
	Text*        m_pSkillPanelCD1;
	Text*        m_pSkillPanelCD2;
	Text*        m_pSkillPanelCost;


	Text*        m_pName;      //名字
	ImageView    *m_scaleImg; //抽卡动画用于做缩放的
};
#endif 
