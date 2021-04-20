/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		�ϻ������ֹ���
 *  Description:	�ϻ������ֹ���ʵ��;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			5/9/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef RollNum_h__
#define RollNum_h__

#include "cocos2d.h"
#include "CocosGUI.h"
using namespace cocos2d::ui;
using namespace std;
USING_NS_CC;

enum MOVE_STATE
{
	MOVE_STATE_ADD,
	MOVE_STATE_PLUS,
	MOVE_STATE_ARRIVE,
};

/**
 ���ֵ�ÿ��λ����Ҫ�������λ�ϵ����ֵĹ���
 */
class RollNum : public Layout
{
public:
    RollNum();
    virtual ~RollNum();
    CREATE_FUNC(RollNum);
    
    //����Ҫ�����������֣��Լ������ķ���
    void setNumber(int var,bool roll = false);
	int getNumber();
	void setRun(float delay = 0.0f); //��ʼ
	void setStop(); //����
	bool setArrive(); //�����ƶ���Ŀ������

private:
    bool init();
	CC_SYNTHESIZE(bool,m_moveUp,MoveUp);
	CC_SYNTHESIZE(bool,m_canNotify,CanNotify);
    int m_nNumber;
    float m_nEndTexH;//��������ĸ߶�
	float m_nCurTexH;//��ǰ��ʾ���������λ��
	ImageView *m_imgOne;
	ImageView *m_imgTwo;
	float m_oneY;
	float m_twoY;
	MOVE_STATE m_moveState;
	float m_movePxl;
    //���ϸ�������������ʾ��λ�ã��ﵽ�������ֵ�Ч��
    void updateNumber(float dt);
};

/**
 �������ֵ�ÿ��λ����������ÿ��λ��λ�ã����������ֵ�ÿ��λ������RollNum������ʾ
 */
class RollNumGroup : public cocos2d::Node
{
public:
    RollNumGroup();
    virtual ~RollNumGroup();
    static RollNumGroup *createWithGameLayer(Node *pGameLayer, int nDigit);
    //������ʾ�����֣��ڲ��Ὣ���ַֽ�ɵ�������Ȼ��������ö�Ӧ��setNumber����
	bool setRun();
	void setStop(int nValue);
	void setCurNum(int nValue);

    //��ʼ���������ֵ�λ�ã�����ָ�����ұߵ��Ǹ����ֵ�λ��
    void setPosition(cocos2d::CCPoint pt);
	void setNotity(Ref* sender); //֪ͨ�ⲿ�Ѿ�������Ŀ�ĵ�
private:
    CC_SYNTHESIZE(Node*, m_pGameLayer, GameLayer);
	CC_SYNTHESIZE(bool, m_runNum, RunNum);

    //�洢���ֵ�ÿ��λ
	vector<RollNum*> m_vecRollNum;
    bool initWithGameLayer(Node*pGameLayer, int nDigit);

	int  canNotifyIdx;
	bool m_stopNum;
};

#endif /* defined(RollNum_h__) */
