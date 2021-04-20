/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampLayer
 *  Description:	星盘层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef StarsCampNewLayer_h_
#define  StarsCampNewLayer_h_

#include "StarsCampDef.h"
#include "StarsCampController.h"
#include "StarsCampModel.h"
#include "DataManager/DataTable.h"
#include "StarsCamp.h"
#include "Widget/LayerCommHeadInfo.h"



enum SC_CallBack
{
	SC_Default,
	SC_Left,
	SC_Right,
	SC_GotoFace,
	SC_GotoStarsSon,
	SC_Tip
};

class StarsCampLayer:public Layer,public Observer
{
public:
	~StarsCampLayer();

	CREATE_FUNC(StarsCampLayer);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	 void  myupdate(float dt);
protected:
	StarsCampLayer();

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	
	void initUi();
	void updateNextFram(float dt);
	//请求数据之后更新界面
	void freshLayerForBigLayer();
	//更新星盘五个人物界面;
	void freshLayerForFiveHero(Widget* wid, int yeshu);
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, SC_CallBack mBtnCallBackType,int campId);
	//设置左右按钮显示隐藏
	void setButtonShowOrHide();
	//点击星盘后的所有操作
	void freshCleckStarCamp(int sonid);
	//设置点击变大变小
	void setBigOrSmall(int sonId);
	//设置改变选中之后下方的属性值
	void setAttbrute( );
	//设置新盘开启动画
	void setNewCampAnimation(int newCampId, bool openOrclose);

private:

	UI_StarsCamp_AllCamp m_ui;
	//放星星
	std::vector<Widget*> vecStars;
	//放星盘
	std::vector<Widget*> vecSonStCamp;

};




#endif // !StarsCampNewLayer_h_
