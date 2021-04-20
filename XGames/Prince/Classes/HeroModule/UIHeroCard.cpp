#include "UIHeroCard.h"
#include "UI/UIDef.h"
#include "Utils/StringFormat.h"
#include "Utils/ResourceUtils.h"
#include "HeroModel.h"
#include "Common/Common.h"
#include "DataManager/DataManager.h"
#include "ToolModule/ToolModel.h"
#include "LayerHero.h"


UIHeroCard::UIHeroCard()
	: mTemplateId(-1)
	, mHeroType(-1)
	, mDebrisNum(0)
	, mDebrisMaxNum(0)
	, mHeroJobType(-1)
	, mIsMainHero(false)
	, mHeroParam(nullptr)
{

}

UIHeroCard::~UIHeroCard()
{

}

UIHeroCard* UIHeroCard::create(int templateId, Layout* root)
{
	UIHeroCard* heroCard = new UIHeroCard();
	if (heroCard && heroCard->init(templateId, root))
	{
		heroCard->autorelease();
		return heroCard;
	}
	else
	{
		delete heroCard;
		heroCard = nullptr;
		return heroCard;
	}
}

bool UIHeroCard::init(int templateId, Layout* root)
{
	if (!Node::init())
	{
		return false;
	}

	mTemplateId = templateId;
	initUI(templateId, root);
	return true;
}

void UIHeroCard::initUI(int templateId, Layout* root)
{
	//加载UI;
	mUI.root = root;
	this->addChild(mUI.root);
	this->setContentSize(mUI.root->getContentSize());

	mUI.heroBack = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_back"));
	//mUI.heroTagImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_tag"));
	mUI.heroJobImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_tag"));
	//mUI.heroJobImg->setScale(1.0f);
	mUI.pointImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Image_point"));
	mUI.nameTxt = dynamic_cast<Text*>(mUI.root->getChildByName("Label_name"));
	mUI.levelAtlas = dynamic_cast<TextAtlas*>(mUI.root->getChildByName("Atl_level"));
	mUI.numBackImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_num_back"));
	mUI.numTxt = dynamic_cast<Text*>(mUI.numBackImg->getChildByName("Label_num"));
	mUI.recruitFlag =  dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_recruit"));
	mUI.mask1 = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_mask1"));
	mUI.mask2 = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_mask2"));

	Sprite* spMask1 = dynamic_cast<Sprite*>(mUI.mask1->getVirtualRenderer());
	Sprite* spMask2 = dynamic_cast<Sprite*>(mUI.mask2->getVirtualRenderer());
	BlendFunc blendFunc = {GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA}; //标准正片叠底
	spMask1->setBlendFunc(blendFunc);
	spMask2->setBlendFunc(BlendFunc::ADDITIVE);

	Layout* startPanel = dynamic_cast<Layout*>(mUI.root->getChildByName("Panel_star"));
	for (int i = 0; i < HERO_MAX_STAR_COUNT; i++)
	{
		mUI.starImg[i] = dynamic_cast<ImageView*>(startPanel->getChildByName("Img_star_" + TO_STR(i)));
	}

	mUI.heroImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Image_hero"));
	mUI.borderImg = dynamic_cast<ImageView*>(mUI.root->getChildByName("Img_border"));

	mUI.nameTxt->setFontName(FONT_FZZHENGHEI);
	mUI.numTxt->setFontName(FONT_FZZHENGHEI);

	updateUI();
}

int UIHeroCard::getTemplateId()
{
	return mTemplateId;
}

int UIHeroCard::getHeroType()
{
	return mHeroType;
}

int UIHeroCard::getHeroJobType()
{
	return mHeroJobType;
}

//获取碎片数量
int UIHeroCard::getDebrisNum()
{
	return mDebrisNum;
}

//是否可招募
bool UIHeroCard::isRecruitable()
{
	return mDebrisNum > 0 && mDebrisMaxNum > 0 && mDebrisNum >= mDebrisMaxNum;
}

//是否是主角
bool UIHeroCard::isMainHero()
{
	return mIsMainHero;
}

HeroParam* UIHeroCard::getHeroParam()
{
	return mHeroParam;
}

//更新UI
void UIHeroCard::updateUI()
{
	HeroModel* heroModel = HeroModel::getInstance();
	mHeroParam = heroModel->searchHeroByTemplateId(mTemplateId);
	if (mHeroParam)
	{
		mIsMainHero = mHeroParam->isMainHero();
		mHeroType = mHeroParam->mHeroType;
		mHeroJobType = mHeroParam->mJobTyp;
		//英雄存在
		setHeroName(mHeroParam->getAdvName());
		//setHeroType(mHeroParam->mHeroType);
		setHeroJobType(mHeroParam->mJobTyp);
		setQuality(mHeroParam->mAdvLvl);
		setStars(mHeroParam->mHeroStars);
		setResId(mHeroParam->mResId);
		setLevel(mHeroParam->mLevel);
		setDebrisNum(-1, -1);
		setRecruitVisible(false);
		bool pointVisible = mHeroParam->isHeroAdvValid() || 
			mHeroParam->isHeroEquipAdvValid() || mHeroParam->isHeroEquipPutOn() || mHeroParam->isHeroStarUpValid();
		setPointVisible(pointVisible); 
		setGray(false);

		mUI.borderImg->loadTexture(ResourceUtils::getIconGlobalPath("img_kuang_" + TO_STR(mHeroParam->mQuality) + ".png"));
		mUI.heroBack->loadTexture(ResourceUtils::getIconGlobalPath("img_kapai_xiao_" + TO_STR(mHeroParam->mQuality) + ".png"));
	}
	else
	{
		RowData* heroData = DataManager::getInstance()->searchCharacterById(mTemplateId);
		RowData* itemData = DataManager::getInstance()->searchToolById(mTemplateId);

		if(heroData && itemData)
		{
			mHeroType = heroData->getIntData("tagTyp");
			mHeroJobType = heroData->getIntData("jobType");
			setHeroName(heroData->getStringData("name"));
			//setHeroType(mHeroType);
			setHeroJobType(mHeroJobType);
			setQuality(heroData->getIntData("qua"));
			setStars(heroData->getIntData("stars"));
			setResId(itemData->getIntData("resId"));
			setLevel(1);
			mDebrisNum = ToolModel::getInstance()->searchHeroDebrisNum(mTemplateId);
			mDebrisMaxNum = ToolModel::getInstance()->searchHeroDebrisMaxNum(mTemplateId);
			setDebrisNum(mDebrisNum, mDebrisMaxNum);
			setRecruitVisible(isRecruitable());
			setPointVisible(false);
			setGray(true);

			mUI.borderImg->loadTexture(ResourceUtils::getIconGlobalPath("img_kuang_" + TO_STR(heroData->getIntData("qua")) + ".png"));
			mUI.heroBack->loadTexture(ResourceUtils::getIconGlobalPath("img_kapai_xiao_" + TO_STR(heroData->getIntData("qua")) + ".png"));
		}
	}
}

void UIHeroCard::onEnter()
{
	Node::onEnter();
}

void UIHeroCard::onExit()
{
	Node::onExit();
}

void UIHeroCard::setHeroName( const string& name )
{
	mUI.nameTxt->setString(name);
}

/*
void UIHeroCard::setHeroType( int heroType )
{
	string imgPath;
	switch (heroType)
	{
	case HERO_STR: //力量
		{
			imgPath = ResourceUtils::getIconGlobalPath("img_shuxing0_li.png");
			mUI.heroTagImg->loadTexture(imgPath);
		}
		break;
	case HERO_INT: //智力
		{
			imgPath = ResourceUtils::getIconGlobalPath("img_shuxing0_zhi.png");
			mUI.heroTagImg->loadTexture(imgPath);
		}
		break;
	case HERO_AGI: //敏捷
		{
			imgPath = ResourceUtils::getIconGlobalPath("img_shuxing0_min.png");
			mUI.heroTagImg->loadTexture(imgPath);
		}
		break;
	default:
		mUI.heroTagImg->setVisible(false);
		break;
	}
	
}
*/

void UIHeroCard::setHeroJobType(int heroJob)
{
	string imgPath;
	mUI.heroJobImg->setVisible(true);
	switch (heroJob)
	{
	case HERO_ATK: //攻击
		{
			imgPath = ResourceUtils::getIconJobPath("img_shuxing0_gong.png");
			mUI.heroJobImg->loadTexture(imgPath);
		}
		break;
	case HERO_DEFENSE: //防御
		{
			imgPath = ResourceUtils::getIconJobPath("img_shuxing0_fang.png");
			mUI.heroJobImg->loadTexture(imgPath);
		}
		break;
	case HERO_HELP: //辅助
		{
			imgPath = ResourceUtils::getIconJobPath("img_shuxing0_fu.png");
			mUI.heroJobImg->loadTexture(imgPath);
		}
		break;
	case HERO_FIRST: //先手
		{
			imgPath = ResourceUtils::getIconJobPath("img_shuxing0_xian.png");
			mUI.heroJobImg->loadTexture(imgPath);
		}
		break;

	default:
		mUI.heroJobImg->setVisible(false);
		break;
	}
}

void UIHeroCard::setQuality( int qua )
{
	if(mHeroParam)
	{
		RowData* advInfo1 = DataManager::getInstance()->searchHeroAdvanceById(mHeroParam->mTemplateId, qua);
		LayerHero::setNameColor(mUI.nameTxt, advInfo1->getIntData("qua"));
	}
	else
	{
		RowData* heroData = DataManager::getInstance()->searchCharacterById(mTemplateId);
		LayerHero::setNameColor(mUI.nameTxt, heroData->getIntData("qua"));
	}
}

void UIHeroCard::setStars(int starCount)
{
	for (int i = 0; i < HERO_MAX_STAR_COUNT; i++)
	{
		bool visible = i < starCount;
		mUI.starImg[i]->setVisible(visible);
	}
}

void UIHeroCard::setResId( int resId )
{
	mUI.heroImg->loadTexture(ResourceUtils::getHalfCardPath(resId));
}

void UIHeroCard::setLevel( int level )
{
	mUI.levelAtlas->setString(TO_STR(level));
}

void UIHeroCard::setDebrisNum( int curNum, int maxNum )
{
	if (curNum >= 0 && maxNum >= 0)
	{
		mUI.numBackImg->setVisible(true);
		mUI.numTxt->setString(TO_STR(curNum) + "/" + TO_STR(maxNum));
	}
	else
	{
		mUI.numBackImg->setVisible(false);
	}
}

void UIHeroCard::setRecruitVisible( bool visible )
{
	mUI.recruitFlag->setVisible(visible);

	if (visible)
	{
		if (!mUI.recruitAni)
		{
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhaomu/AniZhaomu.ExportJson");
			mUI.recruitAni = Armature::create("AniZhaomu");
			mUI.recruitAni->setPosition(this->getContentSize() / 2);
			mUI.recruitAni->getAnimation()->playByIndex(0);
			this->addChild(mUI.recruitAni, 999);
		}
		mUI.recruitAni->setVisible(true);
	}
	else
	{
		if (mUI.recruitAni)
		{
			mUI.recruitAni->getAnimation()->stop();
			mUI.recruitAni->setVisible(false);
		}
	}
}

void UIHeroCard::setGray( bool gray )
{
	mUI.mask1->setVisible(gray);
	mUI.mask2->setVisible(gray);
}

void UIHeroCard::setPointVisible(bool visible)
{
	mUI.pointImg->setVisible(visible);
}

Layout* UIHeroCard::getCardRoot()
{
	return mUI.root;
}

