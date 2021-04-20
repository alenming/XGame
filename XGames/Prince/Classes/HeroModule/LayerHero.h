#ifndef LayerCharacter_h__
#define LayerCharacter_h__

#include "UITagHero.h"
#include "UI/ModalLayer.h"
#include "HeroModel.h"
#include "spine/spine-cocos2dx.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "HeroController.h"
#include "HeroModel.h"
#include "Widget/SkillTips.h"
#include "FormationModule/CustomScrollBar.h"
#include "StarsSuModule/StarsSuController.h"

using namespace spine;

//��������(Ʒ��/����)
enum growType
{
	DEFAULT_TYPE,
	QUATITY_TYPE,
	STAR_UP_TYPE
};

class LayerHero : public ModalLayer, public Observer, public LayerSmithyObserver
{

public:

	LayerHero();
	virtual ~LayerHero();

	//�������캯��
	CREATE_FUNC(LayerHero);

public:

	//��ʼ��
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	//���õ�ǰ��ʾӢ�۵�id
	void setCurrentHeroId(int heroId);

	//LayerSmithyObserver
	virtual void onLayerSmithyClose();

	//��ʾӢ��
	void showHeroList(int curId = 0);

	//��������ǰ�����ù��������ɵ��
	void setScrollViewClickEnabled(bool isCanClick);
	//��������ǰ�����þ���/ͻ�ư�ť���ɵ��
	void setAdvBreakBtnClickEnabled(bool isCanClick);

public :
	//����Ӣ��������ɫ���ܶ������õ�
	static void setNameColor(Text* text, int color);

private:

	//��ʼ��UI
	void initUI();

	//��ʼ��Ӣ���б�
	void initHeroList(int curId = 0);

	//��ʼ��װ��ͼ��
	void initEquipIcon(UIToolHeader** equipIcon, Node* parent, HERO_BTN_ID btnId);

	//����Ӣ��UI
	void updateAllUI();
	//��������װ��
	void updateAllEquip();
	//���µ���װ��
	void updateEquip(Node* equipNode, int equipId, EQUIP_ITEM_TYPE equipType);
	//����Ӣ������
	void updateUIAttri();
	//���½�����Ϣ
	void updateAdvInfo();
	//���¿���
	void updateHeroCard();
	//����Ӣ��������Ϣ
	void updateHeroStarUpInfo();
	//����������Ƭ�����Ϣ
	void updateAlmightyPanel();
	//�һ���Ƭ�������Ƭ���ݺͽ�������ʾ
	void updateChipsDetails();

	//����������廬���¼�
	void onStarsUpScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//����Ե����Ϣ
	void updateFateUI();

	//���spine��������
	void addHeroSpine();
	//����������Ч
	void updateHeroParticle();
	//��ӿ���
	void addHeroCard();

	//��ʼ����, �����ͻ����Ҫ������Ĵ������β���
	void startAdvance(bool isBreak);
	//��ʼ����
	void startStarUp();

	//����Ӣ������ֵ�仯����
	bool addAnimationAttri(HeroParam* heroParam);
	void onUpdateAnimationAttri(float ft);

	void onUpdateAnimationAdvAttri(float ft);

	//���Ӽ��ܶ���������ʱ�������˼�����ʾ
	void addAnimationSkill(Button* skillBtn);
	//�Ƴ����ܶ���
	void removeAnimationSkill(Button* skillBtn);

	//�Ƴ�װ����Ч
	void removeAllEquipEffe();

	//����Ӣ�ۺ�ɫ���
	void updateHeroRedFlag();

	//����װ���ɽ��ױ��
	void updateEquipRedFlag();

	//���ݸ���֪ͨ�ӿ�
	virtual void updateSelf(void* data);

	//Ӣ���б���Ӧ
	void onHeroListClicked(Ref* ref, Widget::TouchEventType type, int nHeroId);
	void onScrollViewEvent(Ref* sender, cocos2d::ui::ScrollviewEventType type);

	//��ť��Ӧ
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);
	void onBtnFateClicked(Ref* ref, Widget::TouchEventType type, int nIndex);

	//���ײ��ϲ��㣬�����������Դ����
	void onBtnMatClicked(Ref* ref, Widget::TouchEventType type, int ntmplateId);

	void popupToolInfo(EQUIP_ITEM_TYPE equipType);
	const int getToolId(EQUIP_ITEM_TYPE equipType, HeroParam* pParam);

	const int getNeedToolId(const EQUIP_ITEM_TYPE& equipType);
	UIToolHeader* getEquipBtn(const EQUIP_ITEM_TYPE equipType);

	//��ʾ����/����ȷ�ϴ���
	void showAdvComfirmLayer(growType type);

	//Ӣ���б��������ǰӢ�۵�λ��
	void scrollHeroList();

	//��ʾ���붯��
	void showEnterAnimation();

	//��ʾ���붯��
	void showBtnEnterAnimation();

	//��Ӿ��Ѱ�ť����
	void addAnimationStartAdv(Button* advBtn);

	//���ž��Ѱ�ť����
	void startAnimationStartAdv(Button* advBtn, bool play);

	//���ž��Ѷ���
	void startAnimationAdv();

	//����Ԥ�����ԵĶ���
	void startPreviewNextValue();

	//��Ϊ�����Ӣ������̫������̬���� ����ƺ�+��+���� ������
	void setNickNamePos();

	//Ե�����Ժ�iconIdת��
	int  getHeroFateIconId(string heroAttriName);

	//���Ѻ����Ƿ���Ҫˢ��Ӣ��Ե��
	void checkHeroFate();

private:

	//��ɫ����UI��Ա
	HeroUI mUI;

	//��ǰ��ʾ��Ӣ�����
	int mCurrentHeroIndex;

	//��ǰ��ʾ��Ӣ��id
	int mCurrentHeroId;

	//��ǰ��ʾ��Ӣ�۲���
	HeroParam* mHeroParam;

	//�������鿪�ŵȼ�
	int m_iHorseEquipOpenLv;
	int m_iBookEquipOpenLv;

	//ͨ����
	HeroController mHeroController;

	//�Ƿ�Ϊ��������
	bool mIsPopUp;

	//�Ƿ��״ν���ý���
	bool mIsFirstEnter;

	//��¼���ξ���/ͻ��ʱ���ĵĲ�����������
	int mMatKindsCount;

	//��¼����/ͻ��ʱ��������ǰ��Ӣ��ID, ��ֹ�����л�Ӣ��ʱ���¶������ų�����
	int mPlayAniHeroId;

	//����������������
	CustomScrollBar* m_starsAttriScrollBar;

	//����
	StarsSuController *m_StarsSuController;

	//��ǰ�������Խ���
	curPanel mCurPanel;
};


#endif //LayerCharacter_h__