/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	���ɽ��������Layerʵ��;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupBuildLayer_h__
#define MyGroupBuildLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;

enum BUILD_CALLBACK_TYPE
{
	//�˳�
	BTN_BUILD_EXIT,
	//��ͨ����
	BTN_COMMON,
	//�߼�����
	BTN_HIGHTLEVEL,
	//��������
	BTN_MONEY
};
struct UI_JUAN_XIAN
{
	cocos2d::ui::Text* uiTexExp;			//����ֵ
	cocos2d::ui::Text* uiTexShengWang;		//����ֵ
	cocos2d::ui::Text* uiTexMoney;			//��Ǯ
	cocos2d::ui::Button* uiBtnDonate;		//���װ�ť
};
struct UI_GroupBuild
{
	cocos2d::ui::Layout* uiRoot;//�����
	cocos2d::ui::Button* uiExit;//�˳�
	cocos2d::ui::Layout* uiLayTouXiang;//����ͼƬ
	cocos2d::ui::Text* uiTexBuildName;//��������
	cocos2d::ui::Text* uiTexBuildLevel;//����ȼ�

	cocos2d::ui::LoadingBar* uiLoaBuildExp;//����������
	cocos2d::ui::LoadingBar* uiLoaTodayExp;//���쾭����
	cocos2d::ui::Text* uiTexBuildExp;//�����������ı�
	cocos2d::ui::Text* uiTexTodayExp;//���쾭�����ı�

	cocos2d::ui::Text* uiTexNextLevelEffeck;//��һ��Ч��
	cocos2d::ui::Text* uiTexToDayJuanXian; //���վ���
	cocos2d::ui::Text* uiTexToDayCount;//������״���
	
	cocos2d::ui::LoadingBar* uiLodbShip;//�̵꾭��
	cocos2d::ui::LoadingBar* uiLodbGroup;//���ɾ��� 

	UI_JUAN_XIAN	uiOrdinary;//��ͨ
	UI_JUAN_XIAN	uiSenior;//�߼�
	UI_JUAN_XIAN	uiTuHao; //����
};

class MyGroupBuildLayer : public ModalLayer,public Observer
{
public:
	MyGroupBuildLayer();
	~MyGroupBuildLayer();

	static Layer* create(eBuildType buildType);
	virtual void updateSelf(void* data);
public:
	bool init(eBuildType buildType);

	virtual void onEnter();

	virtual void onExit();
private:
	//��ʼ������UI
	void  initUI();
	//��ʼ��������Ϣ
	void initBuildInfo();
	//��ʼ��������Ϣ
	void iniJuanXian();

	void  onBtnClick( Ref* reff, Widget::TouchEventType type, BUILD_CALLBACK_TYPE mBtnCallBackType);
	//�˳��ص�
	void  onBtnExitCallBack();
	//��ͨ���׻ص�
	void onBtnCommonCallBack();
	//�߼����׻ص�	
	void  onBtnHoightCallBack();
	//�������׻ص�
	void  onBtnMoneyCallBack();
	//ˢ��
	void refreshBuildUIinfo();
private:
	UI_GroupBuild	m_ui;
	eBuildType _buildType;
	bool isChaoZhou;

};

#endif //MyGroupBuildLayer_h__