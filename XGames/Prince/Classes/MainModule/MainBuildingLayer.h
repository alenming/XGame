#ifndef PerspectLayer_h__
#define PerspectLayer_h__

#include "UI/UIDef.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocos2d::ui;

const int FLOOR_COUNT = 7;
const int BUILDING_COUNT = 25;

typedef function<void(void)> MainCityScrollCallBack;

typedef struct tagUIBuilding
{
	//������ʵ��
	ImageView* buildingEntity;

	//��������
	ImageView* buildingTitle;

	//������id
	int buildingId;

	//����������
	string buildingName;

}UIBuilding;

typedef struct tagUIPerspect
{
	//�����
	Layout* root;

	//��һ��
	cocos2d::ui::ScrollView* firstFloor;

	//���б�
	vector<cocos2d::ui::ScrollView*> vecFloor;

	//�������б�
	vector<UIBuilding> vecBuilding;

}UIPerspect;

// enum MAIN_BUILDING_TAG
// {
// 	BUILDING_PVP = 2,				//������
// 	BUILDING_SHOP = 4,				//�̳�
// 	BUILDING_LOVE = 6,				//��Ե
// 	BUILDING_FACTION = 7,			//����
// 	BUILDING_TOWER = 9,				//����
// 	BUILDING_REFINE = 13,		    //����¯��
// 	BUILDING_STAR_CAMP = 14,		//�Թ�
// 	BUILDING_PVE = 15,				//�ؿ�
// };

enum MAIN_TAG
{
	TAG_MAZE = 5,			//�þ�
	TAG_SEAL = 6,			//��ӡ
	TAG_PVE = 7,			//ð��
	TAG_TOWER = 11,			//������
	TAG_DAILY_PVE = 12,		//ÿ����ս
	TAG_REFINE = 13,		//����¯
	TAG_FACTION = 17,		//����
	TAG_LOVE = 18,			//��Ե
	TAG_SHOP = 19,			//����
	TAG_BOSS = 20,			//����BOSS
	TAG_PVP = 23,			//������
};

class MainBuildingLayer : public Layer,public Observer
{

public:

	MainBuildingLayer();
	virtual ~MainBuildingLayer();

	CREATE_FUNC(MainBuildingLayer);

	//���ݱ仯��֪ͨUI��������
	virtual void updateSelf(void* data);
public:

	virtual bool init() override;

	virtual void onEnter() override;

	//����������Ӧ
	void onBuildingClick(int buildingTagId);

	//��ת��ĳ����
	void jumpToScene(int buildingTagId);

	//��ת����ǰ��¼λ��
	void jumpToCurrent();

	//��ת���ٷֱ�
	void jumpToPercent(float percent);

	//�������ٷֱ�
	void scrollToPercent(float percent, const MainCityScrollCallBack& callback);

	//������Ļ�ֱ��ʼ����ʼ��������
	static float getInitialPercent();

	//���ó�ʼ��������
	static void setInitialPercent();

	//��ȡ����ͼ���ϵ�С���
	Node* getBuildingRetFlag(int buildingTagId);

	//ͳһ���ý�����ǩ����
	void setTileVisible();

	//�����涯���뽨������
	//string getBuildingAniName(MAIN_BUILDING_TAG tag);

private:

	//��ʼ�������¼�
	void initTouchEvent();

	//�����¼�
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//������������
	void onTouchScrollEvent(Ref* pSender, Widget::TouchEventType type);

	//�����¼�
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchCancelled(Touch* touch, Event* event);

	//alpha���
	bool alphaTouchCheck(ImageView* image, Touch *touch);

	//��ʾ������ѡ��Ч��
	void showSelectedEffect(UIBuilding* building);

	//���ؽ�����ѡ��Ч��
	void removeSelectedEffect(UIBuilding* building);

private:

	UIPerspect mUI;

	//��ǰ����Ľ�����
	UIBuilding* mTouchBuilding;

	//��¼�µ�ǰ�����ٷֱ�
	static float mScrollPercent;
	static bool mIsInitScrollPercent;

	//����ͼƬ
	Sprite* mSunShineSprite;

};




#endif //PerspectLayer_h__