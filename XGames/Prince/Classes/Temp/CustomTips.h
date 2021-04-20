/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		CustomTips
 *  Description:	公共弹窗;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/30/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef CustomTips_h__
#define CustomTips_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define NORMAL_TIPS_BACKGROUND_IMG "Image/Icon/global/img_tishi_di.png"

class CustomTips : public Layer
{
public:
	~CustomTips();

	virtual void  onEnter();
	virtual void  onExit();

	/********************************************************************
	 *  接口说明: 1. show;
	 *			  2. showUtf8(传入UTF-8格式的字符串，防止UTF-8被重复转码);
	 *  参数列表: [in](string)strContent : 需要显示的文本;
	 *			  [in](CustomTipsCallback)callbackFunc : 回调函数指针(可选);
	 *  回调参数: (CustomRetType) 参见Enum类型定义;
	 ********************************************************************/
	static void  show(const string& strContent, const string& strBgName="Image/Icon/global/img_tishi_di.png", const int& fontSize=35, const cocos2d::Color3B& fontColor=cocos2d::Color3B(0xf9, 0xee, 0x82));
    static void  showUtf8(const string& strContent, const string& strBgName="Image/Icon/global/img_tishi_di.png", const int& fontSize=35, const cocos2d::Color3B& fontColor=cocos2d::Color3B(0xf9, 0xee, 0x82));

	static CustomTips*  create(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor);

private:
	CustomTips();

    virtual bool  init(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor);

	bool  initUI(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor);

	//销毁
	void dispose();
	//销毁回调
	void callBackDispose(float dt);	
	
private:

	Layout*		m_pRoot;
	Layout*		m_pnlPop;
	ImageView*  m_pImgBg;
	Text*		m_txtContent;
};


#endif // CustomTips_h__