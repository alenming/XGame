/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampScene
 *  Description:	星盘模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/

#ifndef StarsCampStoneLayer_h_
#define StarsCampStoneLayer_h_

#include "StarsCampDef.h"
#include "StarsCampLayerOne.h"

enum STO_LAYBA
{
	STO_LAYBA_FANYE,
	STO_LAYBA_SHOP,
	STO_LAYBA_XIANGQIAN
};
enum STO_TYPE
{
	TYEP_NORMAL,
	TYEP_SHOP,
	TYEP_XIANGQIAN
};

class StarsCampStoneLayer:public Layer,public Observer
{
public:
	~StarsCampStoneLayer();

	CREATE_FUNC(StarsCampStoneLayer);
	 
protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();
	void initShopFace();
	void updateAllInfo();
	bool initOneStone(Layout* lay, int stoId, int __i);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type,STO_LAYBA TYEP,int yeshu,int geshu);

	//设置哪一页显示
	void setWhichShow();
	//设置状态
	void setStoneShow(int stroID,STO_TYPE type);
	
	void sendShopInfo(int id);
	void sendXiangqian(int id);

	void reseiveShop();
	void resiiveXiangQian();
	//购买动画
	void playShopStoneAnimation();
	//镶嵌动画
	void playXiangQianAnimation(int ye);
	//镶嵌完动画
	void playingXiangQianAnimation();
	StarsCampStoneLayer();
private:
	UI_StarsCamp_Shop m_ui;
	UI_StarsCamp_OneSto m_StoUI;

	std::vector<Layout*> fourLayou;

	std::vector<Layout*> oneFace;

	std::vector<Button*> fiveButton;

	int curYe;
	int curSto;
};
#endif // !StarsCampScene_h_
