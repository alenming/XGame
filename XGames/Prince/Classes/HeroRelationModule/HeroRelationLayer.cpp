#include "HeroRelationLayer.h"
#include "MainModule/MainCityScene.h"
#include "Widget/LayerCommHeadInfo.h"
#include "HeroRelationDataDef.h"
#include "SmithyModule/SmithyDef.h"
#include "SmithyModule/SmithyModel.h"
#include "SmithyModule/SceneSmithy.h"
#include "HeroModule/SceneHero.h"
#include "HeroModule/LayerHeroInfo.h"
#include "HeroRelationModel.h"


HeroRelationLayer::HeroRelationLayer()
{
	m_bIsPush = false;
	m_iTempId = 0;
}

HeroRelationLayer::~HeroRelationLayer( void )
{

}

void HeroRelationLayer::onEnter()
{
	Layer::onEnter();

	//获取所需缘分数据
	HeroRelationModel::getInstance()->initFatedData(m_iTempId);
	getRelationData();
	updateUI();
}

void HeroRelationLayer::onExit()
{
	Layer::onExit();
}

HeroRelationLayer* HeroRelationLayer::create(int heroId, bool isPush, LayerHero* layer)
{
	HeroRelationLayer* _layer = new HeroRelationLayer();
	if (_layer->init(heroId, isPush, layer))
	{
		_layer->autorelease();
		return _layer;
	}
	else
	{
		delete _layer;
		return nullptr;
	}
}

bool HeroRelationLayer::init(int heroId, bool isPush, LayerHero* layer )
{
	if (!Layer::init())
	{
		return false;
	}

	m_bIsPush = isPush;
	m_iTempId = heroId;
	m_heroLayer = layer;

	initUI();

	return true;
}

void HeroRelationLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/Ui_Fate/Ui_Fate.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	//返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_HERO_FATE);
	this->addChild(_layerCommHeadInfo);

	m_ui.pImgFiveFate = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_Attri_fiveFate"));
	m_ui.pImgSixFate = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_Attri_sixFate"));

	//五缘分属性
	Layout* fiveFateAttri = dynamic_cast<Layout*>(m_ui.pImgFiveFate->getChildByName("Pnl_property"));
	vector<int> testIntVec;
	testIntVec.clear();
	vector<sUiAttri> testVec;
	testVec.clear();
	m_ui.vFiveFateAttri.clear();
	for(int i=0; i<5; i++)
	{
		sUiAttri pAttri;
		//pAttri.pImgAttriIcon = dynamic_cast<ImageView*>(fiveFateAttri->getChildByName("Img_Attri_" + TO_STR(i+1)));
		pAttri.pLabFateName = dynamic_cast<Text*>(fiveFateAttri->getChildByName("Lab_FateName_" + TO_STR(i+1)));
		pAttri.pLabFateObj = dynamic_cast<Text*>(fiveFateAttri->getChildByName("Lab_fateDst" + TO_STR(i+1)));
		pAttri.pLabAttriName = dynamic_cast<Text*>(fiveFateAttri->getChildByName("Lab_AttriName_" + TO_STR(i+1)));

		pAttri.pLabFateName->setFontName(FONT_FZZHENGHEI);
		pAttri.pLabFateObj->setFontName(FONT_FZZHENGHEI);
		pAttri.pLabAttriName->setFontName(FONT_FZZHENGHEI);

		m_ui.vFiveFateAttri.push_back(pAttri);

		//一些需要调整字体的文本
		Text* tips1 = dynamic_cast<Text*>(fiveFateAttri->getChildByName("Lab_tips" + TO_STR(i+1)));
		tips1->setFontName(FONT_FZZHENGHEI);
		if(i < 3)
		{
			Text* tips2 = dynamic_cast<Text*>(fiveFateAttri->getChildByName("Lab_tips" + TO_STR(i+1) + "_1"));
			tips2->setFontName(FONT_FZZHENGHEI);
		}
	}

	//六缘分属性
	Layout* sixFateAttri = dynamic_cast<Layout*>(m_ui.pImgSixFate->getChildByName("Pnl_property"));
	m_ui.vSixFateAttri.clear();
	for(int i=0; i<6; i++)
	{
		sUiAttri pAttri;
		//pAttri.pImgAttriIcon = dynamic_cast<ImageView*>(sixFateAttri->getChildByName("Img_Attri_" + TO_STR(i+1)));
		pAttri.pLabFateName = dynamic_cast<Text*>(sixFateAttri->getChildByName("Lab_FateName_" + TO_STR(i+1)));
		pAttri.pLabFateObj = dynamic_cast<Text*>(sixFateAttri->getChildByName("Lab_fateDst" + TO_STR(i+1)));
		pAttri.pLabAttriName = dynamic_cast<Text*>(sixFateAttri->getChildByName("Lab_AttriName_" + TO_STR(i+1)));
		
		pAttri.pLabFateName->setFontName(FONT_FZZHENGHEI);
		pAttri.pLabFateObj->setFontName(FONT_FZZHENGHEI);
		pAttri.pLabAttriName->setFontName(FONT_FZZHENGHEI);
		
		m_ui.vSixFateAttri.push_back(pAttri);

		//一些需要调整字体的文本
		Text* tips1 = dynamic_cast<Text*>(sixFateAttri->getChildByName("Lab_tips" + TO_STR(i+1)));
		tips1->setFontName(FONT_FZZHENGHEI);
		if(i < 3)
		{		
			Text* tips2 = dynamic_cast<Text*>(sixFateAttri->getChildByName("Lab_tips" + TO_STR(i+1) + "_1"));		
			tips2->setFontName(FONT_FZZHENGHEI);
		}
	}

	m_ui.pPnlHeroBg = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_heroBg"));

	//六个缘分介绍
	Layout* fateDetails = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_fateBase"));
	m_ui.vFateInfo.clear();
	for(int i=0; i<6; i++)
	{
		sFateInfo fateDetail;
		fateDetail.pImgFateBg = dynamic_cast<ImageView*>(fateDetails->getChildByName("IMG_FateObject0" + TO_STR(i+1)));
		fateDetail.pPnlFateObj = dynamic_cast<Layout*>(fateDetail.pImgFateBg->getChildByName("Panel_FateObject"));
		fateDetail.pBtnLvUp = dynamic_cast<Button*>(fateDetail.pImgFateBg->getChildByName("BTN_FateLevelUp"));
		fateDetail.pImgMaxLv = dynamic_cast<ImageView*>(fateDetail.pImgFateBg->getChildByName("IMG_FateLevelMax"));
		fateDetail.pLabFateName = dynamic_cast<Text*>(fateDetail.pImgFateBg->getChildByName("Lab_fateName"));
		fateDetail.pLabFateLv = dynamic_cast<Text*>(fateDetail.pImgFateBg->getChildByName("Lab_level"));
		fateDetail.pLabMaxLv = dynamic_cast<Text*>(fateDetail.pImgFateBg->getChildByName("Lab_maxLv"));
		fateDetail.pLabNextAttri = dynamic_cast<Text*>(fateDetail.pImgFateBg->getChildByName("Lab_nextAttri"));

		fateDetail.pLabFateName->setFontName(FONT_FZZHENGHEI);
		fateDetail.pLabFateLv->setFontName(FONT_FZZHENGHEI);
		fateDetail.pLabMaxLv->setFontName(FONT_FZZHENGHEI);
		fateDetail.pLabNextAttri->setFontName(FONT_FZZHENGHEI);

		m_ui.vFateInfo.push_back(fateDetail);
	}
}

void HeroRelationLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName, int heroId )
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");

		switch (nWidgetName)
		{
		case BTN_JUMP_CL_HORSE:
			{
				HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
				if(hero == nullptr)
				{
					return;
				}

				//等级判定
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL))
				{
					auto callBack = [=](float dt)->void
					{
						this->getScheduler()->unschedule("HeroRelationLayer::switchHorse", this);
						Director::getInstance()->pushScene(SceneSmithy::create(hero->mHeroId, hero->mHorseId, EQUIP_ITEM_TYPE_HORSE, SMITHY_OP_TYPE_CL));
					};

					getScheduler()->schedule(callBack, this, 0, 0, 0, false, "HeroRelationLayer::switchHorse");					
				}
			}
			break;
		case BTN_JUMP_CL_BOOK:
			{
				HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
				if(hero == nullptr)
				{
					return;
				}

				//等级判定
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL))
				{
					auto callBack = [=](float dt)->void
					{
						this->getScheduler()->unschedule("HeroRelationLayer::switchBook", this);
						Director::getInstance()->pushScene(SceneSmithy::create(hero->mHeroId, hero->mBookId, EQUIP_ITEM_TYPE_BOOK, SMITHY_OP_TYPE_CL));
					};

					getScheduler()->schedule(callBack, this, 0, 0, 0, false, "HeroRelationLayer::switchBook");		
				}	
			}
			break;
		case BTN_JUMP_CL_WEAPON:
			{
				HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
				if(hero == nullptr)
				{
					return;
				}

				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL))
				{
					auto callBack = [=](float dt)->void
					{
						this->getScheduler()->unschedule("HeroRelationLayer::switchWeapon", this);
						Director::getInstance()->pushScene(SceneSmithy::create(hero->mHeroId, hero->mWeaponId, EQUIP_ITEM_TYPE_WEAPON, SMITHY_OP_TYPE_CL));				
					};

					getScheduler()->schedule(callBack, this, 0, 0, 0, false, "HeroRelationLayer::switchWeapon");					
				}	
			}
			break;
		case BTN_JUMP_HERO:
			{
				if(heroId == 0)
				{
					return;
				}
				if(HeroModel::getInstance()->searchHeroByTemplateId(heroId) != nullptr)
				{
					auto callBack = [=](float dt)->void
					{
						if(!m_bIsPush)
						{
							Director::getInstance()->replaceScene(SceneHero::create(HeroModel::getInstance()->searchHeroByTemplateId(heroId)->mHeroId));
						}
						else
						{
							m_heroLayer->showHeroList(HeroModel::getInstance()->searchHeroByTemplateId(heroId)->mHeroId);
							Director::getInstance()->popScene();
						}

						this->getScheduler()->unschedule("HeroRelationLayer::switchScene", this);
					};

					getScheduler()->schedule(callBack, this, 0, 0, 0, false, "HeroRelationLayer::switchScene");
				}
				else
				{
					//没有的英雄进入详情页, tempId都转换成heroId
					LayerHeroInfo* layer = LayerHeroInfo::create(heroId);
					layer->show(this);
				}
			}
			break;
		default:
			break;
		}
	}
}

void HeroRelationLayer::update( float dt )
{
	this->unscheduleUpdate();
	if(m_bIsPush)
	{
		Director::getInstance()->popScene();
	}
	else
	{
		Director::getInstance()->replaceScene(SceneHero::create(HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId)->mHeroId));
	}
}

void HeroRelationLayer::updateUI()
{
	//背景英雄
	ImageView* imgHeroBg = ImageView::create("Image/Icon/integrityCard/" + TO_STR(m_iTempId) + ".png");
	imgHeroBg->setPosition(m_ui.pPnlHeroBg->getContentSize()/2);
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(m_iTempId);
	if(pointRowData != nullptr)
	{
		vector<float> pointVect;
		StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
		imgHeroBg->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
	}
	m_ui.pPnlHeroBg->addChild(imgHeroBg);

	if(m_vFateId.size() == 5)
	{
		m_ui.pImgFiveFate->setVisible(true);
		m_ui.pImgSixFate->setVisible(false);

		showFiveFatePnl();
	}
	else
	{
		m_ui.pImgFiveFate->setVisible(false);
		m_ui.pImgSixFate->setVisible(true);

		showSixFataPnl();
	}

	//下排缘分信息
	for(int i=0; i<m_ui.vFateInfo.size(); i++)
	{
		m_ui.vFateInfo.at(i).pImgFateBg->setVisible(false);
		if(i < m_vFateAttri.size())
		{
			m_ui.vFateInfo.at(i).pImgFateBg->setVisible(true);

			//道具ID
			HeroParam* curHero = HeroModel::getInstance()->searchHeroByTemplateId(m_iTempId);
			if(curHero == nullptr)
			{
				return;
			}
			CL_Param m_paramCL;
			m_paramCL.clear();

			m_ui.vFateInfo.at(i).pBtnLvUp->setVisible(!m_vFateAttri.at(i).isFateMaxLv);
			if(m_vFateAttri.at(i).isFateActed)
			{
				//激活后按钮变成升级
				m_ui.vFateInfo.at(i).pBtnLvUp->loadTextures("Image/Ui_Fate/Image/btn_shengji_1.png", "Image/Ui_Fate/Image/btn_shengji_2.png");
			}
			else
			{
				//未激活时按钮变成激活
				m_ui.vFateInfo.at(i).pBtnLvUp->loadTextures("Image/Ui_Fate/Image/btn_jihuo_1.png", "Image/Ui_Fate/Image/btn_jihuo_2.png");
			}
			UIToolHeader* headIcon = nullptr;
			if(m_vFateAttri.at(i).fateType == 4)
			{
				m_ui.vFateInfo.at(i).pBtnLvUp->addTouchEventListener(CC_CALLBACK_2(HeroRelationLayer::onBtnClick, this, BTN_JUMP_CL_HORSE, 0));	
				SmithyModel::getInstance()->getCLParam(curHero->mHorseId, EQUIP_ITEM_TYPE_HORSE, m_paramCL);
				headIcon = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquDstIcon));
			}
			else if(m_vFateAttri.at(i).fateType == 5)
			{
				m_ui.vFateInfo.at(i).pBtnLvUp->addTouchEventListener(CC_CALLBACK_2(HeroRelationLayer::onBtnClick, this, BTN_JUMP_CL_BOOK, 0));	
				SmithyModel::getInstance()->getCLParam(curHero->mBookId, EQUIP_ITEM_TYPE_BOOK, m_paramCL);
				headIcon = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquDstIcon));
			}
			else if(m_vFateAttri.at(i).fateType == 6)
			{
				m_ui.vFateInfo.at(i).pBtnLvUp->addTouchEventListener(CC_CALLBACK_2(HeroRelationLayer::onBtnClick, this, BTN_JUMP_CL_WEAPON, 0));			
				SmithyModel::getInstance()->getCLParam(curHero->mWeaponId, EQUIP_ITEM_TYPE_WEAPON, m_paramCL);
				headIcon = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquDstIcon));
			}
			else
			{
				m_ui.vFateInfo.at(i).pBtnLvUp->addTouchEventListener(CC_CALLBACK_2(HeroRelationLayer::onBtnClick, this, BTN_JUMP_HERO, m_vFateAttri.at(i).fateHeroId));	
				headIcon = UIToolHeader::create(m_vFateAttri.at(i).fateHeroId);	
			}
			if(headIcon != nullptr)
			{
				headIcon->setTipsEnabled(false);
				headIcon->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				m_ui.vFateInfo.at(i).pPnlFateObj->removeAllChildren();
				m_ui.vFateInfo.at(i).pPnlFateObj->addChild(headIcon);

				if(!m_vFateAttri.at(i).isFateActed)
				{
					headIcon->setMaskIcon("Image/Icon/global/btn_kuang_zhuangbei_zhezhao.png");
				}
			}

			m_ui.vFateInfo.at(i).pImgMaxLv->setVisible(m_vFateAttri.at(i).isFateMaxLv);

			if(!m_vFateAttri.at(i).isFateActed)
			{
				m_ui.vFateInfo.at(i).pLabMaxLv->setString("");
				m_ui.vFateInfo.at(i).pLabNextAttri->setString(m_vFateAttri.at(i).attriDes);
				m_ui.vFateInfo.at(i).pLabFateName->setString(m_vFateAttri.at(i).fateName);
				m_ui.vFateInfo.at(i).pLabFateLv->setString("lv" + TO_STR(0));

				m_ui.vFateInfo.at(i).pLabNextAttri->setColor(Color3B(0xAF, 0x8F, 0x7D));
			}
			else
			{
				if(m_vFateAttri.at(i).isFateMaxLv)
				{
					//达到最高级
					m_ui.vFateInfo.at(i).pLabNextAttri->setString("");
					//m_ui.vFateInfo.at(i).pLabMaxLv->setString("已达到最高级");
					m_ui.vFateInfo.at(i).pLabMaxLv->setString(DataManager::getInstance()->searchCommonTexdtById(10243));
				}
				else
				{
					m_ui.vFateInfo.at(i).pLabMaxLv->setString("");
					m_ui.vFateInfo.at(i).pLabNextAttri->setString(m_vFateAttri.at(i).attriDes2);
				}
				
				m_ui.vFateInfo.at(i).pLabFateName->setString(m_vFateAttri.at(i).fateName);
				m_ui.vFateInfo.at(i).pLabFateLv->setString("lv" + TO_STR(m_vFateAttri.at(i).fateLevel));
			}
		}
	}
}

void HeroRelationLayer::showFiveFatePnl()
{
	for(int i=0; i<m_vFateAttri.size(); i++)
	{
		if(i<m_ui.vFiveFateAttri.size())
		{
			//m_ui.vFiveFateAttri.at(i).pImgAttriIcon->setVisible(false);
			m_ui.vFiveFateAttri.at(i).pLabFateName->setString(m_vFateAttri.at(i).fateName + ":");
			m_ui.vFiveFateAttri.at(i).pLabFateObj->setString(m_vFateAttri.at(i).fateObjName);
			m_ui.vFiveFateAttri.at(i).pLabAttriName->setString(m_vFateAttri.at(i).attriChName + " +" 
				+ TO_STR(m_vFateAttri.at(i).attriValue*100) + "%");

			if(m_vFateAttri.at(i).isFateActed)
			{
				m_ui.vFiveFateAttri.at(i).pLabFateName->setColor(Color3B(0xff, 0xf9, 0x59));
				setTextAddStroke(m_ui.vFiveFateAttri.at(i).pLabFateName, Color3B(0xba, 0x0b, 0x0b), 2);
				m_ui.vFiveFateAttri.at(i).pLabFateObj->setColor(Color3B(0xff, 0xf9, 0x59));
				m_ui.vFiveFateAttri.at(i).pLabAttriName->setColor(Color3B(0xff, 0xf9, 0x59));
			}
			else
			{
				m_ui.vFiveFateAttri.at(i).pLabFateName->setColor(Color3B(0x76, 0xbb, 0xc3));
				setTextAddStroke(m_ui.vFiveFateAttri.at(i).pLabFateName, Color3B(0x1c, 0x49, 0x8d), 2);
				m_ui.vFiveFateAttri.at(i).pLabFateObj->setColor(Color3B(0x8c, 0x8c, 0x8c));
				m_ui.vFiveFateAttri.at(i).pLabAttriName->setColor(Color3B(0x8c, 0x8c, 0x8c));
			}
		}
	}
}

void HeroRelationLayer::showSixFataPnl()
{
	for(int i=0; i<m_vFateAttri.size(); i++)
	{
		if(i < m_ui.vSixFateAttri.size())
		{
			m_ui.vSixFateAttri.at(i).pLabFateName->setString(m_vFateAttri.at(i).fateName + ":");
			m_ui.vSixFateAttri.at(i).pLabFateObj->setString(m_vFateAttri.at(i).fateObjName);
			m_ui.vSixFateAttri.at(i).pLabAttriName->setString(m_vFateAttri.at(i).attriChName + " +"
				+ TO_STR(m_vFateAttri.at(i).attriValue*100) + "%");

			if(m_vFateAttri.at(i).isFateActed)
			{
				m_ui.vSixFateAttri.at(i).pLabFateName->setColor(Color3B(0xff, 0xf9, 0x59));
				setTextAddStroke(m_ui.vSixFateAttri.at(i).pLabFateName, Color3B(0xba, 0x0b, 0x0b), 2);
				m_ui.vSixFateAttri.at(i).pLabFateObj->setColor(Color3B(0xff, 0xf9, 0x59));
				m_ui.vSixFateAttri.at(i).pLabAttriName->setColor(Color3B(0xff, 0xf9, 0x59));
			}
			else
			{
				m_ui.vSixFateAttri.at(i).pLabFateName->setColor(Color3B(0x76, 0xbb, 0xc3));
				setTextAddStroke(m_ui.vSixFateAttri.at(i).pLabFateName, Color3B(0x1c, 0x49, 0x8d), 2);
				m_ui.vSixFateAttri.at(i).pLabFateObj->setColor(Color3B(0x8c, 0x8c, 0x8c));
				m_ui.vSixFateAttri.at(i).pLabAttriName->setColor(Color3B(0x8c, 0x8c, 0x8c));
			}
		}
		//最后一个是显示技能名称
		if(i == m_ui.vSixFateAttri.size()-1)
		{
			m_ui.vSixFateAttri.at(i).pLabAttriName->setString(m_vFateAttri.at(i).rightSkillName);
		}
	}
}

void HeroRelationLayer::getRelationData()
{
	HeroRelationModel::getInstance()->getRelationHeroId(m_vFateHeroId);
	//HeroRelationModel::getInstance()->getRelationActLimit(m_vFateLimit);
	HeroRelationModel::getInstance()->getRelationIDList(m_vFateId);
	HeroRelationModel::getInstance()->getRelationAttriList(m_vFateAttri);
}

string HeroRelationLayer::getAttriIconPath( string attriEnName )
{
	if(attriEnName == "hpCoe")
	{
		return "Image/Icon/global/img_xueliang.png";
	}
	else if(attriEnName == "atkCoe")
	{
		return "Image/Icon/global/img_gongji.png";
	}
	else if(attriEnName == "pddCoe")
	{
		return "Image/Icon/global/img_wufang.png";
	}
	else if(attriEnName == "mddCoe")
	{
		return "Image/Icon/global/img_mofang.png";
	}
	else
	{
		return "";
	}
}





