#ifndef LayerHeroLevel_h__
#define LayerHeroLevel_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "HeroModel.h"
#include "ToolModule/UIToolHeader.h"
#include "UITagHero.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagHeroLevel
{
	//�������
	Layout* mRoot;
	ImageView* mLevelUpPnl;
	Text* mCurLvlAndMaxLvl;
	LoadingBar* mExpBar;
	TextAtlas* mCurExpText;
	TextAtlas* mMaxExpText;
	UIToolHeader* mExpHeader[EXP_ITEM_COUNT];
};

class LayerHeroLevel : public ModalLayer, public Observer
{

public:

	LayerHeroLevel();
	virtual ~LayerHeroLevel();

	static LayerHeroLevel* create(HeroParam* heroParam);

public:

	virtual void updateSelf(void* data);
	virtual bool init(HeroParam* heroParam);

	virtual void onEnter();
	virtual void onExit();

private:

	void updateUI();

	//�����Ӧ
	void onExpItemClicked(Ref* ref, Widget::TouchEventType type, int itemId);

	//�ж�����
	void judgeLongPressed(float delta);

	//ִ�г���
	void onLongPressed(float delta);

	//�ݹ������������
	void calExp(int curExp, int maxExp);

private:

	//Ӣ�۲���
	HeroParam* mHeroParam;
	
	//UI
	UITagHeroLevel mUI;

	//��ǰѡ�еĵ���ͼ��
	UIToolHeader* mCurIcon;

	//��ǰѡ�е��߼Ӷ��پ���
	int mExpAdd;
	//��ǰѡ�еĵ����������
	int mMaxNum;
	//����ʹ������
	int mUseNum;
	//��ǰ�ȼ�
	int mHeroLevel;
	//��ǰ����
	int mCurExp;

	//�Ƿ���볤��״̬
	bool isLongPressed;
	
};

#endif //LayerHeroLevel_h__