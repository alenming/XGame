#ifndef LayerHeroStarsUp_h__
#define LayerHeroStarsUp_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "UITagHero.h"
#include "DataManager/DataManager.h"
#include "LayerHero.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagLayerHeroStarsUp
{
	//�����
	Layout* mRoot;
	//�������
	Layout* mCardPanel;
	//����
	ImageView* mStarImg1[STAR_COUNT];
	Point mStarPos1[STAR_COUNT];
	ImageView* mStarImg2[STAR_COUNT];
	Point mStarPos2[STAR_COUNT];
	
	//�������
	Layout* mStarsUpPropertyPnl;

	//�����ʾ
	ImageView* mClickImg;

	//��ͷ
	ImageView* mArrowImg;

	//������ֵ
	Text* mAtkGrow1;
	Text* mHpGrow1;
	Text* mPddGrow1;
	Text* mMddGrow1;
	Text* mSpeed1;
	Text* mAtkGrow2;
	Text* mHpGrow2;
	Text* mPddGrow2;
	Text* mMddGrow2;
	Text* mSpeed2;

	Text* mAtkAdd;
	Text* mHpAdd;
	Text* mPddAdd;
	Text* mMddAdd;

	//ս������
	Text* mFightPower1;
	Text* mFightPower2;
};

class LayerHeroStarsUp : public ModalLayer
{

public:

	LayerHeroStarsUp();
	virtual ~LayerHeroStarsUp();

	static LayerHeroStarsUp* create(int templetId);

public:

	virtual bool init(int templetId);
	virtual void onEnter();
	virtual void onExit();

private:

	//��ʼ��UI
	void initUI();

	//����UI
	void updateUI();

	//��ʼ������
	void initAnimation();

	//�����ص�
	void aniAniCallBack(Armature *armature, MovementEventType movementType, const std::string& movementID, const string& nextAniName);

	//���Ƕ���
	void startUpStarAni();
	//�������Ƕ���
	void startStarAni();

private:

	UITagLayerHeroStarsUp mUI;

	//Ӣ��ģ��ID
	int mHeroId;

	//��������
	int mStarCount1;
	int mStarCount2;
};

#endif //LayerHeroStarsUp_h__