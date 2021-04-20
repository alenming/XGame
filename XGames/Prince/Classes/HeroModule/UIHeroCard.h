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
	//根结点
	Layout* root;
	//英雄类型
	ImageView* heroTagImg;
	//英雄职业
	ImageView* heroJobImg;
	//英雄形象
	ImageView* heroImg;
	//小红点
	ImageView* pointImg;
	//星星
	ImageView* starImg[HERO_MAX_STAR_COUNT];
	//英雄名字
	Text* nameTxt;
	//英雄等级
	TextAtlas* levelAtlas;
	//碎片数量底
	ImageView* numBackImg;
	//碎片数量
	Text* numTxt;
	//边框，随品质发生变化
	ImageView* borderImg;
	//英雄底
	ImageView* heroBack;
	//可招募标记
	ImageView* recruitFlag;
	//可招募动画
	Armature* recruitAni;

	//卡牌遮罩
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

	//更新UI
	void updateUI();

	//获取模版id
	int getTemplateId();

	//获取英雄类型
	int getHeroType();

	//获取英雄职业类型
	int getHeroJobType();

	//获取碎片数量
	int getDebrisNum();

	//是否可招募
	bool isRecruitable();
	
	//是否是主角
	bool isMainHero();

	//获取英雄数据
	HeroParam* getHeroParam();

	//获取root接口，方便卡牌做一些类似透明度相关的动画
	Layout* getCardRoot();

private:

	//初始化UI
	void initUI(int templateId, Layout* root);

	//设置英雄名字
	void setHeroName(const string& name);

	//设置英雄类型，力、智、敏
	//void setHeroType(int heroType);

	//设置英雄职业类型
	void setHeroJobType(int heroJob);

	//设置英雄品质
	void setQuality(int qua);

	//设置英雄星级
	void setStars(int starCount);

	//设置英雄图片
	void setResId(int resId);

	//设置英雄等级
	void setLevel(int level);

	//设置英雄碎片数量
	void setDebrisNum(int curNum, int maxNum);

	//设置可招募标记
	void setRecruitVisible(bool visible);

	//设置灰度
	void setGray(bool gray);

	//设置小红点
	void setPointVisible(bool visible);

private:

	UITagHeroCard mUI;

	//英雄模版id
	int mTemplateId;

	//英雄类型，力智敏
	int mHeroType;

	//英雄职业类型
	int mHeroJobType;

	//英雄碎片数量
	int mDebrisNum;

	//合成英雄所需碎片的最大数量
	int mDebrisMaxNum;

	//是否是主角
	bool mIsMainHero;

	//已有英雄数据
	HeroParam* mHeroParam;

};


#endif //UIHeroCard_h__