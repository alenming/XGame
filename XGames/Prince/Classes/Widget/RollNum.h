/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		老虎机数字滚动
 *  Description:	老虎机数字滚动实现;
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
 数字的每个位，主要控制这个位上得数字的滚动
 */
class RollNum : public Layout
{
public:
    RollNum();
    virtual ~RollNum();
    CREATE_FUNC(RollNum);
    
    //设置要滚动到得数字，以及滚动的方向
    void setNumber(int var,bool roll = false);
	int getNumber();
	void setRun(float delay = 0.0f); //开始
	void setStop(); //结束
	bool setArrive(); //设置移动到目标数字

private:
    bool init();
	CC_SYNTHESIZE(bool,m_moveUp,MoveUp);
	CC_SYNTHESIZE(bool,m_canNotify,CanNotify);
    int m_nNumber;
    float m_nEndTexH;//数字纹理的高度
	float m_nCurTexH;//当前显示数字纹理的位置
	ImageView *m_imgOne;
	ImageView *m_imgTwo;
	float m_oneY;
	float m_twoY;
	MOVE_STATE m_moveState;
	float m_movePxl;
    //不断更新数字纹理显示的位置，达到滚动数字的效果
    void updateNumber(float dt);
};

/**
 管理数字的每个位，包括控制每个位的位置，及解析数字的每个位，调度RollNum进行显示
 */
class RollNumGroup : public cocos2d::Node
{
public:
    RollNumGroup();
    virtual ~RollNumGroup();
    static RollNumGroup *createWithGameLayer(Node *pGameLayer, int nDigit);
    //设置显示的数字，内部会将数字分解成单个数，然后逐个调用对应的setNumber方法
	bool setRun();
	void setStop(int nValue);
	void setCurNum(int nValue);

    //初始化各个数字的位置，参数指定最右边的那个数字的位子
    void setPosition(cocos2d::CCPoint pt);
	void setNotity(Ref* sender); //通知外部已经滚动到目的地
private:
    CC_SYNTHESIZE(Node*, m_pGameLayer, GameLayer);
	CC_SYNTHESIZE(bool, m_runNum, RunNum);

    //存储数字的每个位
	vector<RollNum*> m_vecRollNum;
    bool initWithGameLayer(Node*pGameLayer, int nDigit);

	int  canNotifyIdx;
	bool m_stopNum;
};

#endif /* defined(RollNum_h__) */
