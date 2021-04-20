#ifndef LayerHeroAdvance_h__
#define LayerHeroAdvance_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "UITagHero.h"
#include "DataManager/DataManager.h"
#include "Widget/SimpleRichText.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagLayerHeroAdvance
{
	//�����
	Layout* mRoot;
	//�������
	Layout* mCardPanel;
	
	//���ܱ���
	ImageView* mSkillStrength;
	ImageView* mSkillNew;
	
	//������Ϣ
	ImageView* mImgSkillPnl;		//�������
	Layout* mSkillIconPanel;
	ImageView* mSkillTypeImg;
	Text* mSkillNameTxt;
	SimpleRichText* mSkillDesc;
	Text* mNoSkillTips;

	//ս�������
	ImageView* mImgFightPower;
	Text*	   mLabOldFight;
	Text*	   mLabNewFight;
	
	//�������
	Layout* mAdvPropertyPnl;

	//�����ʾ
	ImageView* mClickImg;

	//��ͷ
	ImageView* mArrowImg;

	//��������
	Text* mName1;
	Text* mName2;

	//������ֵ
	Text* mSpeed1;
	Text* mAtk1;
	Text* mHp1;
	Text* mPdd1;
	Text* mMdd1;
	Text* mSpeed2;
	Text* mAtk2;
	Text* mHp2;
	Text* mPdd2;
	Text* mMdd2;
	//����ݹ�����������
	Text* mAtkText;
	ImageView* mAtkIcon;

	//�ɰ�
	Layout* mPnlMask;
};

enum class SKILL_UPDATE_TYPE
{
	SKILL_NONE,		//û�����Ӽ���
	SKILL_ADD,		//��������
	SKILL_UPDATE	//����ǿ��
};

struct SkillUpdateParam
{
	SKILL_UPDATE_TYPE updateType;
	int skillId;
	int skillType;

	SkillUpdateParam()
		: updateType(SKILL_UPDATE_TYPE::SKILL_NONE)
		, skillId(-1)
		, skillType(-1)
	{}
};

class LayerHeroAdvance : public ModalLayer
{

public:

	LayerHeroAdvance();
	virtual ~LayerHeroAdvance();

	static LayerHeroAdvance* create(int templetId);

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

	//����������Ч����
	void startSkillAni();

	//�¼��ܿ���ͼ��Ķ���
	void startSkillIconAni();

	//���ܸ��²���
	SkillUpdateParam getSkillUpdateParam(RowData* advInfo1, RowData* advInfo2);

	//�����Ƿ�ɵ��
	void setLayerCanClicked(bool isCanClick);

	//���Ѻ����Ƿ���Ҫˢ��Ӣ��Ե��
	void isHeroFateChanged();

private:

	UITagLayerHeroAdvance mUI;

	//Ӣ��ģ��ID
	int mHeroId;

	//�Ƿ��м��ܿ�����ǿ��
	bool mIsSkillUpdate;

	//���һ��������¼�¿���/ǿ���ļ�������
	int m_skillType;
};

#endif //LayerHeroAdvance_h__