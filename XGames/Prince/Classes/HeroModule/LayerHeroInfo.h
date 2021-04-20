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
	//�����
	Layout* root;
	//��Ƭͼ��
	Layout* iconPnl;
	//��Ƭ����
	Text* nameTxt;
	//��Ƭ����
	Text* debrisNumTxt;
	//����
	Text* desTxt;
	//�رհ�ť
	Button* closeBtn;
	//��ȡ;��
	Button* getWayBtn;
	//�������
	Layout* cardPnl;
	//Ӣ�����Ա��
	ImageView* heroTag;
	//Ӣ����������
	Text* heroTxt;
	//Ӣ��ְҵ
	ImageView* heroJob;
	//Ӣ������
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

	//������Ӧ
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

private:

	void initUI();

private:

	UITagLayerHeroInfo mUI;

	//Ӣ��ģ��id
	int mTemplateId;

	//Ӣ����Ƭid
	int mDebrisTemplateId;

};

#endif //LayerHeroInfo_h__




