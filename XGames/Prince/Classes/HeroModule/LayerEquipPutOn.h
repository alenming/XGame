#ifndef LayerEquipPutOn_h__
#define LayerEquipPutOn_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UITagHero.h"
#include "Utils/Observer.h"
#include "HeroRecruitController.h"
#include "UI/ModalLayer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagLayerEquipPutOn
{
	//根结点
	Layout* mRoot;
	//背景
	ImageView* mBackImg;
	//关闭按钮
	Button* mCloseBtn;
	//穿戴按钮
	Button* mPutOnBtn;
	//装备图标框
	ImageView* mEquipIcon;
	//装备名称
	Text* mEquipName;
	//装备数量
	Text* mEquipNum;
	//装备属性1
	Text* mEquipAttr1;
	//装备属性2
	Text* mEquipAttr2;
	//装备描述
	Text* mEquipDes;
	//提示文字
	Text* mTips;
};


class LayerEquipPutOn : public ModalLayer, public Observer
{

public:

	LayerEquipPutOn();
	virtual ~LayerEquipPutOn();

	static LayerEquipPutOn* create(int heroId, int equipId);

public:

	virtual bool init(int heroId, int equipId);
	virtual void onEnter();
	virtual void onExit();

private:

	virtual void updateSelf(void* data);

	void initUI();

	//按键响应
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

private:

	UITagLayerEquipPutOn mUI;

	//装备Id，服务器id
	int mEquipId;

	//英雄Id，服务器id
	int mHeroId;

};

#endif //LayerEquipPutOn_h__

