/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerIntro
 *  Description:	开场动画的封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			01/16/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerIntro_h__
#define LayerIntro_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIVideoPlayer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

class LayerIntro : public Layer
{
public:
	~LayerIntro(void);
	CREATE_FUNC(LayerIntro);

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	void  onVideoPlayEvent(Ref* pSender, experimental::ui::VideoPlayer::EventType type);

#endif


private:
	LayerIntro(void);

	void  initUI();

	void  enterGame();

private:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	experimental::ui::VideoPlayer*   m_pVideoPlayer;
#endif

	Layout*		m_pRoot;
	Layout*		m_pnlIntro;
	Button*		m_btnSkip;

	enum UI_INTRO_WIDGET
	{
		Btn_Skip
	};

};

#endif // LayerIntro_h__
