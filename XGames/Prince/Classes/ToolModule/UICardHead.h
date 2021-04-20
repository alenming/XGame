/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSign
 *  Description:	�Թ���ʱ�����̳ǳ鿨�鵽Ӣ��ͨ��;
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
	CARD_PRO_POWER = 1,   //����
	CARD_PRO_INTLIGENT,   //����
	CARD_PRO_AGILITY,    //����
};

const int STAR_CONT = 6;
const int SKILL_CONT = 5;

class UICardHead :public Node
{
public:
	UICardHead(void);
	~UICardHead(void);

	//����Ӣ��Id �Ƿ����ݽ���
	static UICardHead* create(int id,bool isLineup = false,bool isLimitBuy = false);  

	bool initUi(int id,bool isLineup,bool isLimitBuy);
	void updataUi();
	void initSkill(int id);
	void setSkill(const int idx,RowData* pRowPetData,const string& str);
	void setSkill(const int idx,const int skillId,const int skillKey); //����ר��

	//���һ�������Ǽ�����ȡ����ͼ��Ľӿڣ��������Ƕ���ʱ�õ�						�ǽ�	2015.11.5
	ImageView* getStarImgByStarLevel(int starLevel);
	//���һ�����ݼ������ͻ�ȡ����ͼƬ�Ľӿڣ������¼��ܿ������߼�������ʱ�õ�		�ǽ�	2015.11.6
	ImageView* getSkillImgBySkillType(int skillType);


	void initSkillPanel();

	void  onBtnClick(Ref* ref, CheckBox::EventType type,int idx,const string& str,Color3B color);
	void  onBtnClickPlayer(Ref* ref, CheckBox::EventType type,int idx,const string& str,const int skillId,
		const int skillKey,Color3B color);//����ר��


	void  onBtnSkillPanel(Ref* ref, Widget::TouchEventType type);

	ImageView* getScaleImg() {return m_scaleImg;};
private:
	int          m_id;
	bool         m_isLineup;
	bool         m_isLimitBuy;

	Layout*      m_pRoot;
	ImageView*   m_pBackImg;   //����ͼƬ���ף��̣������ϣ��ȣ�
	ImageView*   m_pPro;       // ����

	ImageView*   m_pStar[STAR_CONT];   //����
	ImageView*   m_pSkill[SKILL_CONT];  //����
	CheckBox*      m_pSkillButton[SKILL_CONT];  //���ܰ�ť


	Layout*      m_pSkillPanel;  //���ܼ��
	int          m_lastIndex;  //��¼�ϴ�ѡ��id
	ImageView*   m_pSkillPanelImg; //���ܼ������ϼ���ͼƬ
	Text*        m_pSkillPanelName;
	Text*        m_pSkillPanelActive;
	Text*        m_pSkillPanelDesc;
	Text*        m_pSkillPanelCD1;
	Text*        m_pSkillPanelCD2;
	Text*        m_pSkillPanelCost;


	Text*        m_pName;      //����
	ImageView    *m_scaleImg; //�鿨�������������ŵ�
};
#endif 
