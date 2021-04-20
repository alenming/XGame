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
	//�����
	Layout* mRoot;
	//����
	ImageView* mBackImg;
	//�رհ�ť
	Button* mCloseBtn;
	//������ť
	Button* mPutOnBtn;
	//װ��ͼ���
	ImageView* mEquipIcon;
	//װ������
	Text* mEquipName;
	//װ������
	Text* mEquipNum;
	//װ������1
	Text* mEquipAttr1;
	//װ������2
	Text* mEquipAttr2;
	//װ������
	Text* mEquipDes;
	//��ʾ����
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

	//������Ӧ
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

private:

	UITagLayerEquipPutOn mUI;

	//װ��Id��������id
	int mEquipId;

	//Ӣ��Id��������id
	int mHeroId;

};

#endif //LayerEquipPutOn_h__

