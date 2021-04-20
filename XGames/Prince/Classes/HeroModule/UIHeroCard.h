#ifndef UIHeroCard_h__
#define UIHeroCard_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "HeroModel.h"
#include "CCArmature.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

const int HERO_MAX_STAR_COUNT = 6;

struct UITagHeroCard
{
	//�����
	Layout* root;
	//Ӣ������
	ImageView* heroTagImg;
	//Ӣ��ְҵ
	ImageView* heroJobImg;
	//Ӣ������
	ImageView* heroImg;
	//С���
	ImageView* pointImg;
	//����
	ImageView* starImg[HERO_MAX_STAR_COUNT];
	//Ӣ������
	Text* nameTxt;
	//Ӣ�۵ȼ�
	TextAtlas* levelAtlas;
	//��Ƭ������
	ImageView* numBackImg;
	//��Ƭ����
	Text* numTxt;
	//�߿���Ʒ�ʷ����仯
	ImageView* borderImg;
	//Ӣ�۵�
	ImageView* heroBack;
	//����ļ���
	ImageView* recruitFlag;
	//����ļ����
	Armature* recruitAni;

	//��������
	ImageView* mask1;
	ImageView* mask2;

	UITagHeroCard()
	{
		memset(this, 0, sizeof(UITagHeroCard));
	}
};


class UIHeroCard : public Node
{

public:

	UIHeroCard();
	virtual ~UIHeroCard();

	static UIHeroCard* create(int templateId, Layout* root);

public:

	virtual bool init(int templateId, Layout* root);
	virtual void onEnter();
	virtual void onExit();

	//����UI
	void updateUI();

	//��ȡģ��id
	int getTemplateId();

	//��ȡӢ������
	int getHeroType();

	//��ȡӢ��ְҵ����
	int getHeroJobType();

	//��ȡ��Ƭ����
	int getDebrisNum();

	//�Ƿ����ļ
	bool isRecruitable();
	
	//�Ƿ�������
	bool isMainHero();

	//��ȡӢ������
	HeroParam* getHeroParam();

	//��ȡroot�ӿڣ����㿨����һЩ����͸������صĶ���
	Layout* getCardRoot();

private:

	//��ʼ��UI
	void initUI(int templateId, Layout* root);

	//����Ӣ������
	void setHeroName(const string& name);

	//����Ӣ�����ͣ������ǡ���
	//void setHeroType(int heroType);

	//����Ӣ��ְҵ����
	void setHeroJobType(int heroJob);

	//����Ӣ��Ʒ��
	void setQuality(int qua);

	//����Ӣ���Ǽ�
	void setStars(int starCount);

	//����Ӣ��ͼƬ
	void setResId(int resId);

	//����Ӣ�۵ȼ�
	void setLevel(int level);

	//����Ӣ����Ƭ����
	void setDebrisNum(int curNum, int maxNum);

	//���ÿ���ļ���
	void setRecruitVisible(bool visible);

	//���ûҶ�
	void setGray(bool gray);

	//����С���
	void setPointVisible(bool visible);

private:

	UITagHeroCard mUI;

	//Ӣ��ģ��id
	int mTemplateId;

	//Ӣ�����ͣ�������
	int mHeroType;

	//Ӣ��ְҵ����
	int mHeroJobType;

	//Ӣ����Ƭ����
	int mDebrisNum;

	//�ϳ�Ӣ��������Ƭ���������
	int mDebrisMaxNum;

	//�Ƿ�������
	bool mIsMainHero;

	//����Ӣ������
	HeroParam* mHeroParam;

};


#endif //UIHeroCard_h__