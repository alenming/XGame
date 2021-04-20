#ifndef LayerAllHero_h__
#define LayerAllHero_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UITagHero.h"
#include "UIHeroCard.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/Observer.h"
#include "HeroRecruitController.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

const int ENTER_CARD_COUNT = 8;

struct UITagLayerAllHero
{
	//�����
	Layout* mRoot;

	//�����������
	Layout* mScrollBarPanel;

	//�������
	ScrollView* mHeroScrollView;

	//�رհ�ť
	Button* mCloseBtn;

	//ҳ�������
	Layout* mTabPanel;

	//Tabҳ
	CheckBox* mTabAllHero;			//ȫ��
	CheckBox* mTabAtkHero;			//����
	CheckBox* mTabDefHero;			//����
	CheckBox* mTabHelpHero;			//����
	CheckBox* mTabFirstHero;		//����

	//Tab��Ӧ
	Layout* mClickAllHero;
	Layout* mClickAtkHero;
	Layout* mClickDefHero;
	Layout* mClickHelpHero;
	Layout* mClickFirstHero;

	//������
	CustomScrollBar* mScrollBar;

};

class LayerAllHero : public Layer, public Observer
{

public:

	LayerAllHero();
	virtual ~LayerAllHero();

	CREATE_FUNC(LayerAllHero);

public:

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void update(float dt);

private:

	//���ݸ���
	virtual void updateSelf(void* data);

	//����UI
	virtual void updateUI();

	//��ʼ��UI
	void initUI();

	//������Ӧ
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

	//�л���ǩҳ
	void switchTab(int heroJob);

	//��ʾӢ���б�param1. -1��ʾ��ʾȫ��
	void showHeroList(int heroJob);

	//��ʼ��Ӣ���б�, param.�Ƿ�ȫ����ʼ��
	void initHeroList();

	//�����¼���Ӧ
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//���ؿ���֮ǰ�ȶ�ID����
	void sortTempId();

	//��ȡָ��idӢ����Ƭ����
	int getDebrisNumById(int tempId);

	//��ȡָ��idӢ���Ƿ����ļ
	bool isRecruitableById(int tempId);

	//��ȡָ��ְҵ��Ӣ������
	int getHeroCountByJob(int heroJob);

	//��ʾ�����볡����
	void showCardAnimation();

	//�����¼�
	virtual bool onTouchBegan(Touch* touch, Event* event, UIHeroCard* heroCard);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchCancelled(Touch* touch, Event* event);

private:

	//UI��
	UITagLayerAllHero mUI;

	//��ǰ��ʾ��Ӣ������
	int mHeroShowType;

	//��ǰ��ʾ��Ӣ��ְҵ
	int mHeroShowJob;

	//Ӣ�ۿ�������б�
	Vector<UIHeroCard*> mVecHeroCard;

	vector<UIHeroCard*> mVecEnterCard;

	//��ǰ����Ŀ���
	UIHeroCard* mCurrentCard;

	//ͨ��
	HeroRecruitController heroRecruitController;

	//�Ƿ��״ν���ý���
	bool mIsFirstEnter;

	//�Ƿ��Ѿ�������ƣ���ֹ�ظ����
	bool mIsCardClicked;
	
	//���ƶ��볡�����Ƿ񲥷����
	bool mIsCardAniEnd;

	//����tempId����
	vector<int> mVecCardTempId;

	//��ǰ�����Ŀ�������
	int mCurCreateNum;
};


#endif //LayerAllHero_h__