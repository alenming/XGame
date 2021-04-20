#ifndef LayerHeroInfo_h__
#define LayerHeroInfo_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "UITagHero.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "SmithyModule/SmithyModel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

const int GET_WAY_CONT = 3;

struct UITagGetWay
{
	UIToolHeader* getWayIcon;
	Text* chapterNameTxt;
	Text* stageNameTxt;
};

struct UITagLayerHeroInfo
{
	//根结点
	Layout* root;
	//碎片图标
	Layout* iconPnl;
	//碎片名称
	Text* nameTxt;
	//碎片数量
	Text* debrisNumTxt;
	//描述
	Text* desTxt;
	//关闭按钮
	Button* closeBtn;
	//获取途径
	Button* getWayBtn;
	//卡牌面板
	Layout* cardPnl;
	//英雄属性标记
	ImageView* heroTag;
	//英雄属性描述
	Text* heroTxt;
	//英雄职业
	ImageView* heroJob;
	//英雄资质
	//TextAtlas*  heroTalent;
	ImageView* heroTalent;
};

class LayerHeroInfo : public ModalLayer
{

public:

	LayerHeroInfo();
	virtual ~LayerHeroInfo();

	static LayerHeroInfo* create(int templateId);

public:

	virtual bool init(int templateId);
	virtual void onEnter();
	virtual void onExit();

	//按键响应
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

private:

	void initUI();

private:

	UITagLayerHeroInfo mUI;

	//英雄模版id
	int mTemplateId;

	//英雄碎片id
	int mDebrisTemplateId;

};

#endif //LayerHeroInfo_h__




