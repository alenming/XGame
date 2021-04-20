#ifndef MainTaskLayer_h__
#define MainTaskLayer_h__

#include "cocos2d.h"
#include "UI/ModalLayer.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/Observer.h"
#include "MainTaskModel.h"

using namespace cocostudio;
using namespace ui;

USING_NS_CC;


class MainTaskNode;

class MainTaskLayer : public ModalLayer, public Observer
{

public:

	enum TAB_TAG
	{
		TAB_NONE,
		TAB_DALY,		//�ճ�����
		TAB_ACHIEVEMENT	//�ɾ�����
	};

	enum BTN_TAG
	{
		CLOSE,			//�رհ�ť
		DALIY,			//�ճ�����ť
		ACHIEVEMENT		//�ɾ�����ť
	};

	MainTaskLayer();
	virtual ~MainTaskLayer();

	CREATE_FUNC(MainTaskLayer);

public:

	//��ʼ��
	virtual bool init();

	virtual void onEnter();
	virtual void onExit(); 

	//����
	virtual void updateSelf(void* data);

private:

	//��ʼ��UI
	void initUI();

	//��ʼ���б�
	void initList();

	//����С�����
	void updateTaskFlag();

	//��ť���
	void onBtnClicked(Ref* target, Widget::TouchEventType eventType, BTN_TAG btnTag);

	//ǰ����ť���
	void onGotoClicked(Ref* target, Widget::TouchEventType eventType, int jumpScene);

	//��ȡ��ť���
	void onReceiveClicked(Ref* target, Widget::TouchEventType eventType, int taskId);

	//�����¼���Ӧ
	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	//�л�tab
	void switchTab(TAB_TAG tabTag, bool isInitList = true);

	//��ӽ�����Ϣ
	void addRewardInfo(MainTaskNode* taskNode, MainTaskParam* taskParam);

	//����������ȡ����
	void showPopReward(MainTaskParam* taskParam);

private:

	//�ճ�����ť
	ImageView* mDailyBrightImg;
	ImageView* mDailyGrayImg;
	ImageView* mDailyFlag;

	//�ɾ�����ť
	ImageView* mAchievementBrightImg;
	ImageView* mAchievementGrayImg;
	ImageView* mAchievementFlag;
	
	//�رհ�ť
	Button* mCloseBtn;

	//�������
	ScrollView* mScrollPanel;

	//������
	CustomScrollBar* mScrollBar;

	//��ǰ��ǩҳ
	TAB_TAG mTabTag;

	//�б��Ƿ���Ҫ���Ŷ���
	bool m_bIsNeedListAct;
};

class MainTaskNode : public Button
{

public:

	MainTaskNode();
	virtual ~MainTaskNode();

	static MainTaskNode* create(const std::string& normalImage, const std::string& selectedImage = "");

public:

	virtual bool init(const std::string& normalImage, const std::string& selectedImage = "");

private:

	void initUI();


public:

	//����id
	int mTaskId;

	//��������
	int mTaskType;

	//��������
	Text* mTaskName;

	//������ɽ���
	Text* mTaskNum;

	//��������
	Text* mTaskDes;

	//������
	ImageView* mReceivePower;

	//�콱��
	ImageView* mReceiveReward;

	//ʱ��δ��
	ImageView* mTimeNot;

	//ǰ����ť
	Button* mGotoBtn;

	//ͼ��λ��
	Layout* mIconPanel;

	//����λ��
	Layout* mRewardPanel;

};

#endif //MainTaskLayer_h__