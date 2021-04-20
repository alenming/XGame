#ifndef LayerCreateRole_h__
#define LayerCreateRole_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Widget/SpineAnimation.h"
#include "Widget/EditBoxEx.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UICreateRole
{
	Layout* root;
	Layout* parent;

	Button* heroMaleBtn;
	Button* heroFemaleBtn;
	ImageView* heroImg;
	ImageView* tvImg;
	Layout* rolePanel;

	vector<ImageView*> vecHeroStar;
	vector<ImageView*> vecHeroInfo;

	TextField* nameTextField;
	EditBoxEx* nameEditBoxEx;

	Button* changeBtn;
	Button* startBtn;
};

class LayerCreateRole : public Layer, public Observer
{
	
public:

	LayerCreateRole();
	virtual ~LayerCreateRole();

	CREATE_FUNC(LayerCreateRole);

	static Scene* createScene();

public:

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void updateSelf(void* data);

private:

	//��ʼ��UI
	void initUI();

	//��ʼ����������
	void initSkeleton();

	//��ʼ������
	void initAnimation();

	//��ʼ�������б�
	void initActionList();

	//ѡ��Ӣ����Ӧ
	void onMaleHeroSelect(Ref* target, Widget::TouchEventType eventType);
	void onFemaleHeroSelect(Ref* target, Widget::TouchEventType eventType);

	//��ʼ��ť��Ӧ
	void onBtnStart(Ref* target, Widget::TouchEventType eventType);

	//�����ְ�ť��Ӧ
	void onBtnChangeName(Ref* target, Widget::TouchEventType eventType);

	//������ɫ������Ӧ
	void onChangeAction(Ref* target, Widget::TouchEventType eventType);

	//ѡ��Ӣ��
	void selectMaleHero(bool playAni = true);
	void selectFemaleHero(bool playAni = true);

	//����ѡ�񶯻�
	void playSelectAni();

	//��������
	void setRoleName();

	//���ù�������
	void setSkeletonAction();

private:

	UICreateRole mUI;

	//�Ƿ�ѡ�����Խ�ɫ
	bool mIsSelectMale;

	SpineAnimation* mMaleSkeleton;
	SpineAnimation* mFemaleSkeleton;

	Armature* mSelectAni;
	Armature* mTextAni;

	//��ɫ�������Խ�ɫ�����б�
	vector<string> mMaleNameList;

	//��ɫ����Ů�Խ�ɫ�����б�
	vector<string> mFemaleNameList;

	//��ǰ������������
	int mMaleNameIndex;

	//��ǰŮ����������
	int mFemaleNameIndex;

	//���Խ�ɫ�����б�
	vector<string> mVecMaleAction;

	//Ů�Խ�ɫ�����б�
	vector<string> mVecFemaleAction;

	//��ǰ���Զ����б�����
	int mMaleActionIndex;

	//��ǰŮ�Զ����б�����
	int mFemaleActionIndex;

};




#endif //LayerCreateRole_h__