/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	�������ò�����Layerʵ��;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupSetLayer_h__
#define MyGroupSetLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"

#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;


#define USER_MAX_LEVEL 10108   //������ȼ�����
#define USER_OPEN_FUNCTION 10016  //���ɿ����ȼ�,Ҳ����ͼ�����ɵȼ�


enum SETLAYER_CALLBACK_TYPE
{
	//�˳�
	BTN_SETLAYER_EXIT,
	//��ɢ
	BTN_DISSOLVE_FAC,
	//����
	BTN_SAVE,
	//����
	BTN_SET,
	//���
	BTN_AUDIT,
	//+++
	BTN_ADD,
	//----
	BTN_SUBTRACT
};

struct UI_GroupSet
{
	//�����
	cocos2d::ui::Layout* uiRoot;

	cocos2d::ui::Layout* uiSetRoot;
	cocos2d::ui::Layout* uiAuditRoot;

	//�˳�
	cocos2d::ui::Button* uiExit;
	//��Ա��
	cocos2d::ui::Text* uiTexMember;

	//��
	cocos2d::ui::CheckBox* uiCheckBoxOn;
	//��
	cocos2d::ui::CheckBox* uiCheckBoxOff;
	//+++
	cocos2d::ui::Button* uiAdd;
	//-----
	cocos2d::ui::Button* uiSubtract;
	//����ȼ�Text
	cocos2d::ui::Text* uiTexJoinInLevel;
	//����
	cocos2d::ui::Button* uiSave;

	//��ɢ
	cocos2d::ui::Button* uiDissolved;
	//����
	cocos2d::ui::Button* uiSet;
	//���
	cocos2d::ui::Button* uiAudit;

	cocos2d::ui::ListView* memberList;


};

class MyGroupSetLayer : public ModalLayer,public Observer
{
public:
	MyGroupSetLayer();
	~MyGroupSetLayer();
	static Layer* create();
	virtual void updateSelf(void* data);

public:
	bool init();

	virtual void onEnter();
	void myupdateADD(float dt);
	void myupdateSUB(float dt);
	virtual void onExit();
private:
	//��ʼ������UI
	void  initUI();
	//�����Ա����
	void initAutyUi();
	//�ص�����
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, SETLAYER_CALLBACK_TYPE mBtnCallBackType);
	//�ص�����
	void  onBtnClickADDorSub( Ref* reff, Widget::TouchEventType type, SETLAYER_CALLBACK_TYPE mBtnCallBackType);
	//��ѡ��ص�
	void  onCheckBoxCallBackON(Ref* reff,CheckBoxEventType type);
	void  onCheckBoxCallBackOFF(Ref* reff,CheckBoxEventType type);
	//��ɢ���ɻص�
	void  onBtnDissolveCallBack();
	//�������ûص�
	void  onBtnSaveCallBack();
	//ͬ�����ص�
	void  onBtnAcceptCallBack(Ref* reff, Widget::TouchEventType type,int uid);
	//�ܾ�����ص�
	void  onBtnRefuseCallBack(Ref* reff, Widget::TouchEventType type,int uid);
	//���ñ��
	void setJoinInFlag( );
	//���¼���ȼ�Text
	void setJoinInLevelText(int mJoinInLevel);
	//������˷�ʽ��ʾ
	void setJoinLimitCheckBox(eJoinType _first);
	//����+��-֮�����ʾ
	void setAddOrSubtract(bool _bb);
	//���ó�Ա��ֵ��ʾ
	void setMemberText();
	//����ˢ��listView
	void ReflushListView();

	//�ж�����
	void judgeLongPressed(float delta);

	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type); //�����б��¼� 
private:
	UI_GroupSet	m_ui;
	int mJoinInLevelMin;
	int mJoinInLevelMax;
	sJoinLimit mJoinlimit;

	CustomScrollBar* m_RankPalyerBar;
	bool firstRunToAuty;
	bool isCanLa;

	//�Ƿ���볤��״̬
	bool isLongPressed;

};

#endif //MyGroupSetLayer_h__