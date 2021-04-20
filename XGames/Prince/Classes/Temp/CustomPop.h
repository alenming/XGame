/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		CustomPop
 *  Description:	公共弹窗;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/30/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef CustomPop_h__
#define CustomPop_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// 回调返回值类型;
enum CustomRetType
{
	RET_TYPE_OK,			// 确定;
	RET_TYPE_CANCEL,		// 取消;
	RET_TYPE_CLOSE,			// 关闭;
	RET_TYPE_RECHARGE		// 充值;
};

// 窗体类型;
enum CustomPopType
{
	CUSTOM_ID_YES,
	CUSTOM_YES_NO,
	CUSTOM_YES_NO_CANCEL,
	CUSTOM_RECHARGE_NO		// 充值;
};

// 回调函数类型;
typedef std::function<void(Ref*, CustomRetType)> CustomPopCallback;

class CustomPop : public LayerColor
{
public:
	~CustomPop();

	virtual void  onEnter();
	virtual void  onExit();

	/********************************************************************
	 *  接口说明: 1. show;
	 *			  2. showUtf8(传入UTF-8格式的字符串，防止UTF-8被重复转码);
	 *  参数列表: [in](string)strContent : 需要显示的文本;
	 *			  [in](CustomPopCallback)callbackFunc : 回调函数指针(可选);
	 *  回调参数: (CustomRetType) 参见Enum类型定义;
	 ********************************************************************/
	static void show(const string strContent, CustomPopCallback callbackFunc = nullptr, CustomPopType type = CUSTOM_ID_YES);
    static void showUtf8(const string strContent, CustomPopCallback callbackFunc = nullptr, CustomPopType type = CUSTOM_ID_YES);

	static CustomPop*  create(const string strContent, CustomPopCallback callbackFunc = nullptr, CustomPopType type = CUSTOM_ID_YES);

	//是否显示弹出动画
	void showPopAnimation(bool isShow);

private:
	CustomPop();

    virtual bool  init(const string strContent, CustomPopCallback callbackFunc, CustomPopType type = CUSTOM_ID_YES);

	bool  initUI(const string strContent);

	void  update(float dt);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, CustomRetType nRetType);

	void  onAnimationDone(Node* pSender, CustomRetType type);

private:
	CustomPopCallback _callbackFunc;

	CustomPopType m_type;

	Layout*		m_pRoot;
	Layout*		m_pnlPop;
	Text*		m_txtContent;
	Button*		m_btnOK;
	Button*		m_btnCancel;
	Button*		m_btnClose;
	Button*		m_btnRecharge;

	//是否显示弹出动画
	bool mShowPopAnimation;


	enum UICustomPopWidget
	{
		Pnl_Custom_Pop,
		Lab_Text,
		Btn_Close,
		Btn_OK,
		Btn_Cancel
	};
};


#endif // CustomPop_h__