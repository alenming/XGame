#include "LayerHeroFate.h"
#include "CocoStudio.h"
#include "UI/UIDef.h"
#include "Utils/StringFormat.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolModel.h"
#include "DataManager/DataManager.h"
#include "Utils/ResourceUtils.h"
#include "ResourceManager/ResourceLoader.h"
#include "Communication/Command.h"
#include "MainModule/MainModel.h"
#include "Temp/CustomTips.h"
#include "FateModel.h"
#include "Widget/LayerCommHeadInfo.h"
#include "GuideCenter/GuideManager.h"

const float TOOL_SMALL_HERO_SCALE = 0.84;
const float TOOL_SMALL_HERO_WIDTH = 120.0*TOOL_SMALL_HERO_SCALE; //scale:0.84

const int TOOL_TAG_FATE_CAN_ADD = -99;//可以增加缘分的位置
const int TOOL_TAG_FATE_DEFAULT = -100;//不能增加缘分的位置

//动画tag
const int LINE_ANI_TAG = 10000;


LayerHeroFate::LayerHeroFate()	
{	
	m_nCurIndex = 0;
	m_nCurHeroId = 0;

	m_nDisHeroBeginIndex = 0;
	m_nDisHeroBigBeginIndex = 2;

	m_nMoveCount = 0;

	HeroModel::getInstance()->addObserver(this);
}

LayerHeroFate::~LayerHeroFate()
{
	HeroModel::getInstance()->removeObserver(this);

	if(m_ui.pBtnTiedFates)
	{
		m_ui.pBtnTiedFates->release();
	}
	if(m_ui.pBtnReplaceFates)
	{
		m_ui.pBtnReplaceFates->release();
	}
	if(m_ui.pBtnRefreshFates)
	{
		m_ui.pBtnRefreshFates->release();
	}
}

bool LayerHeroFate::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUI();
	//initPopup();

	if (HeroModel::getInstance()->getMainHero()->mSex == HERO_SEX_MALE)
	{
		m_ui.pImgMainRole->loadTexture("Image/UINewfate/Image/img_nan_fate.png");
	}
	else
	{
		m_ui.pImgMainRole->loadTexture("Image/UINewfate/Image/img_nv_fate.png");
	}

	if (FateModel::getInstance()->isInited())
	{
		initHeroList();
		showFates();
	}
	return true;
}

void LayerHeroFate::onExit()
{
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UINewfate");
}

void LayerHeroFate::onEnter()
{
	Layer::onEnter();

	//如果结缘丹发生改变要更新
	if (this->getChildByName("_layerCommHeadInfo"))
	{
		LayerCommHeadInfo *_layerCommHeadInfo = dynamic_cast<LayerCommHeadInfo*>(this->getChildByName("_layerCommHeadInfo"));
		_layerCommHeadInfo->updateInfo();
	}


	if (!FateModel::getInstance()->isInited())
	{
		//隔一帧再拉数据，避免破坏迭代器导致崩溃
		auto func = [this](float dt)
		{
			HeroFateController::sendGetFateMsg();
			this->getScheduler()->unschedule("LayerHeroFate", this);
		};
		this->getScheduler()->schedule(func, this, 0, 0, 0.0f, false, "LayerHeroFate");
	}
	
}


//初始化UI
void LayerHeroFate::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewfate/UINewfate_main.ExportJson"));
	m_ui.pRoot->setPosition(cocos2d::Vec2((size.width-m_ui.pRoot->getContentSize().width)/2, (size.height- m_ui.pRoot->getContentSize().height)/2));

	// 将UILayer层加入到当前的场景
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		CC_CALLBACK_2(LayerHeroFate::onBtnBack, this), HEAD_INFO_TYPE_FATE);
	this->addChild(_layerCommHeadInfo,0,"_layerCommHeadInfo");

	//
	m_ui.pLayHeros =  dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_yingxiong"));
	m_ui.pLayTiedFates =  dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_jieyuan"));

	//英雄
	m_ui.pSvHeros = dynamic_cast<ScrollView*>(m_ui.pLayHeros->getChildByName("ScrollView_liebiao"));
	m_ui.pSvHeros->addEventListenerScrollView(this, (SEL_ScrollViewEvent)&LayerHeroFate::onScrollViewEvent);
	//
	m_ui.pBtnLeft =  dynamic_cast<Button*>(m_ui.pLayHeros->getChildByName("Button_xiangzuo"));
	m_ui.pBtnLeft->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnLeft, this));
	m_ui.pBtnRight =  dynamic_cast<Button*>(m_ui.pLayHeros->getChildByName("Button_xiangyou"));
	m_ui.pBtnRight->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnRight, this));

	//结缘
	for (int i = 0; i < MAX_HERO_FATE_COUNT; i++)
	{
		//缘分
		m_ui.pLayFates[i] =  dynamic_cast<Layout*>(m_ui.pLayTiedFates->getChildByName("Panel_yuefen"+_TO_STR(i+1)));
		m_ui.pHeroFates[i]	= UIToolHeader::create();
		m_ui.pHeroFates[i]->setTouchEnabled(true);
		m_ui.pHeroFates[i]->setScale(0.8);
		m_ui.pHeroFates[i]->setPosition(Vec2(m_ui.pLayFates[i]->getContentSize().width/2, m_ui.pLayFates[i]->getContentSize().height/2));
		m_ui.pLayFates[i]->addChild(m_ui.pHeroFates[i]);
		//m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i));
		//缘分说明
		m_ui.pImgDes[i] =  dynamic_cast<ImageView*>(m_ui.pLayTiedFates->getChildByName("Image_shuoming"+_TO_STR(i+1)));
		//初始属性箭头隐藏
		m_ui.pImgDes[i]->getChildByName("Image_shangsheng")->setVisible(false);
		Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxing"));
		pText->setFontName(FONT_FZZHENGHEI);
		Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxingming"));
		pTextAttri->setFontName(FONT_FZZHENGHEI);
		m_ui.posFatesIsSpace[i] = true;

		//解除缘分按钮
		Button* removeFateBtn =  dynamic_cast<Button*>(m_ui.pLayFates[i]->getChildByName("Button_jiechuyuanfen"));
		removeFateBtn->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnRemoveFates, this, i));
		removeFateBtn->setVisible(false);
	}

	//主角
	m_ui.pImgMainRole =  dynamic_cast<ImageView*>(m_ui.pLayTiedFates->getChildByName("Image_zhujue"));

	//刷新
	ImageView* pImgRefreshBg =  dynamic_cast<ImageView*>(m_ui.pLayTiedFates->getChildByName("Image_di"));
	m_ui.pTextRefreshCost =  dynamic_cast<Text*>(pImgRefreshBg->getChildByName("Label_jieyuandan"));
	m_ui.pTextRefreshCost->setFontName(FONT_FZZHENGHEI);
	int toolTempId = 0;
	int costCount = 0;
	if (FateModel::getInstance()->getRefreshCost(toolTempId, costCount))
	{
		m_ui.pTextRefreshCost->setText(_TO_STR(costCount));
	}

	//三个按钮容器
	m_ui.pBtnPnl1 = (Layout*)(m_ui.pLayTiedFates->getChildByName("Panel_button1"));
	m_ui.pBtnPnl2 = (Layout*)(m_ui.pLayTiedFates->getChildByName("Panel_button2"));
	m_ui.pBtnPnl3 = (Layout*)(m_ui.pLayTiedFates->getChildByName("Panel_button3"));

	//结识缘分按钮
	m_ui.pBtnTiedFates =  Button::create(
		"Image/UINewfate/Image/btn_jieshiyuanfen_1.png",
		"Image/UINewfate/Image/btn_jieshiyuanfen_2.png");
	m_ui.pBtnTiedFates->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnTiedFates, this));
	m_ui.pBtnTiedFates->setPosition(m_ui.pBtnPnl1->getContentSize()/2);
	m_ui.pBtnTiedFates->retain();
	//刷新缘分按钮
	m_ui.pBtnRefreshFates = Button::create(
		"Image/UINewfate/Image/btn_shuaxinyuanfen_1.png",
		"Image/UINewfate/Image/btn_shuaxinyuanfen_2.png",
		"Image/UINewfate/Image/btn_shuaxinyuanfen_3.png");
	m_ui.pBtnRefreshFates->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnRefreshFates, this));
	m_ui.pBtnRefreshFates->setPosition(m_ui.pBtnPnl1->getContentSize()/2);
	m_ui.pBtnRefreshFates->retain();
	//替换按钮
	m_ui.pBtnReplaceFates = Button::create(
		"Image/UINewfate/Image/btn_tihuan_1.png",
		"Image/UINewfate/Image/btn_tihuan_2.png");
	m_ui.pBtnReplaceFates->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnRefreshReplace, this));
	m_ui.pBtnReplaceFates->setPosition(m_ui.pBtnPnl1->getContentSize()/2);
	m_ui.pBtnReplaceFates->retain();
}

void LayerHeroFate::onBtnBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if(checkNeedReplaceFate())
		{
			return;
		}
		Director::getInstance()->popScene();
	}
}

void LayerHeroFate::onBtnLeft(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_ui.pSvHeros->getChildren().size() <= 7)
		{
			return;
		}
		//		
		if (m_nDisHeroBeginIndex > 0)
		{
			Vec2 vec = m_ui.pSvHeros->getInnerContainer()->getPosition();
			m_nDisHeroBeginIndex --;
			m_nDisHeroBigBeginIndex = m_nDisHeroBeginIndex+1;
			vec.x += TOOL_SMALL_HERO_WIDTH;
			MoveTo* pMove = MoveTo::create(0.1, vec);
			EaseBackIn *pEaseBounce = EaseBackIn::create(pMove);
			m_ui.pSvHeros->getInnerContainer()->stopAllActions();
			m_ui.pSvHeros->getInnerContainer()->runAction(pEaseBounce);
		}
	}
}
void LayerHeroFate::onBtnRight(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_ui.pSvHeros->getChildren().size() <= 7)
		{
			return;
		}
		if (m_nDisHeroBeginIndex < m_ui.pSvHeros->getChildren().size()-7)
		{
			Vec2 vec = m_ui.pSvHeros->getInnerContainer()->getPosition();	
			m_nDisHeroBeginIndex ++;
			m_nDisHeroBigBeginIndex = m_nDisHeroBeginIndex+1;
			vec.x -= TOOL_SMALL_HERO_WIDTH;
			MoveTo* pMove = MoveTo::create(0.1, vec);
			EaseBackIn *pEaseBounce = EaseBackIn::create(pMove);
			m_ui.pSvHeros->getInnerContainer()->stopAllActions();
			m_ui.pSvHeros->getInnerContainer()->runAction(pEaseBounce);//setPosition(vec);	
		}	
	}
}

void LayerHeroFate::onBtnTiedFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nCurHeroId <= 0)
			return;
		int toolTempId = 0;
		int costCount = 0;
		if (!FateModel::getInstance()->getRefreshCost(toolTempId, costCount))
		{
			return;
		}
		const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(toolTempId);
		//若拥有足够道具，则提示：结缘成功，播放动画，新属性出现，界面变为下方状态
		if (pToolInfo && pToolInfo->nstack>=costCount)
		{
			if (m_nCurHeroId > 0)
			{
				//int pos = FateModel::getInstance()->getFateInfo(m_nCurHeroId)->pos;
				HeroFateController::sendBindFateMsg(m_nCurHeroId, m_nCurIndex + 1);
			}
		}
		else//没有足够道具，则带底提示：道具不足
		{
			//CustomTips::show("道具不足");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10236));
		}
	}
}
void LayerHeroFate::onBtnRemoveFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//先锁定选中项
		if(index != m_nCurIndex)
		{
			clearBtnPanel();
			m_nCurIndex = index;
			m_nCurHeroId = m_ui.pHeroFates[m_nCurIndex]->getTag();	
			//设置选中项
			for(int i=0; i<MAX_HERO_FATE_COUNT; i++)
			{
				m_ui.pHeroFates[i]->setSelected(false);
			}
			m_ui.pHeroFates[m_nCurIndex]->setSelected(true);
			m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
		}
		if (m_nCurHeroId > 0)//弹出提示框：提示是否确认解除缘分		
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10237), 
		CC_CALLBACK_2(LayerHeroFate::onCustomRetRemoveFates, this), CUSTOM_YES_NO);
	}
}

void  LayerHeroFate::onCustomRetRemoveFates(Ref* pSender, CustomRetType type)
{
	//确认解除后，英雄回到可结识列表中
	if (RET_TYPE_OK == type)
	{
		if (m_nCurHeroId > 0)
			HeroFateController::sendCancelFateMsg(m_nCurHeroId);
	}
}
void LayerHeroFate::onBtnRefreshFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nCurHeroId <= 0)
			return;
		//若有足够道具时，打开刷新成功界面1
		int toolTempId = 0;
		int costCount = 0;
		if (!FateModel::getInstance()->getRefreshCost(toolTempId, costCount))
		{
			return;
		}
		const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(toolTempId);
		if (pToolInfo && pToolInfo->nstack>=costCount)
		{
			/*a.在界面1中点击替换，则保存新属性显示界面2
			b.点击刷新，保持原属性不变，更新新属性
			c.点击关闭按钮，保存原属性返回主界面*/
			HeroFateController::sendrRefreshFateMsg(m_nCurHeroId);		
		}
		else//没有足够道具，则带底提示：道具不足
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10236));
		}
	}
}

void LayerHeroFate::onBtnHeroClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int heroId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (heroId == -1)
		{
			return;//空体
		}
		if (FateModel::getInstance()->getFateInfo(heroId) != nullptr)
		{
			return;//已经添加
		}
		int nomalCount = getCanTiedFatesCount( HeroModel::getInstance()->getMainHeroLevel(), 10047);//主角缘分开放等级
		int vipCount = getCanTiedFatesCount( MainModel::getInstance()->getMainParam()->mVipLevel, 10048);//主角缘分开放VIP等级

		vector<FateItemInfo>& vecFates = FateModel::getInstance()->getFateInfos();
		//  排查占坑没结缘的，判断是否有空位
		int nPos = vecFates.size();
		for(auto vecItem : vecFates)
		{
			if(!vecItem.isTied)
			{
				nPos--;
			}
		}
		//当存在可用空位时点击英雄头像，头像移动至空位中，且在上方列表中消失，其他英雄补齐。
		if (nomalCount + vipCount > nPos)
		{
			//插入第一个空位
			int index = getFatesSpacepos(nomalCount, vipCount);
			if(index == -1)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10238));
				return;
			}
		
			//先检测有没有刷新没替换的
			if(checkNeedReplaceFate())
			{
				return;
			}

			//先隐藏当前红叉叉
			if(m_nCurIndex < MAX_HERO_FATE_COUNT && m_nCurIndex >= 0)
			{
				m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);
				m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
			}	

			FateItemInfo item;
			item.heroId			= heroId;
			item.pos			= index + 1;
			item.isTied			= false;
			FateModel::getInstance()->addFateItem(item);
		
			//如果该位置有未结缘的英雄，先清理
			if(m_ui.pHeroFates[index]->getTag() > 0)
			{
				m_nCurHeroId = m_ui.pHeroFates[index]->getTag();
				m_nCurIndex = index;
				cancelSelectedHeroIcon();
				m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);
				m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
			}
			

			HeroParam* pHeroParam = HeroModel::getInstance()->searchHeroById(heroId);
			if (pHeroParam != nullptr)
			{
				m_ui.pHeroFates[index]->setMainIcon(ResourceUtils::getSmallIconPath(pHeroParam->mResId));
			}	
			m_ui.pHeroFates[index]->setTag(heroId);
			m_ui.pHeroFates[index]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
			m_ui.pHeroFates[index]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, index, heroId));
			//显示描述
			m_ui.pImgDes[index]->setVisible(false);	

			//
			for (int i = 0; i < MAX_HERO_FATE_COUNT; i++)
			{
				m_ui.pHeroFates[i]->setSelected(false);
			}
			m_ui.pHeroFates[index]->setSelected(true);
			//
			clearBtnPanel();
			m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);
			m_ui.pBtnTiedFates->setScale(0.1f);
			m_ui.pBtnTiedFates->runAction(EaseSineOut::create(ScaleTo::create(0.5, 1.0f)));
		
			m_nCurHeroId = heroId;
			m_nCurIndex = index;

			UIToolHeader* pHero = dynamic_cast<UIToolHeader*>(this->m_ui.pSvHeros->getChildByTag(heroId));
			if (pHero != nullptr)
			{
				pHero->setTiedFateIcon("Image/Icon/global/img_kejieyuan.png");
			}
		}
		else//当不存在空位时，点击英雄头像判断下列两种情况
		{
			//	若已完全开启所有空位，提示：解除英雄缘分获得更多空位
			if (nomalCount + vipCount >= MAX_HERO_FATE_COUNT)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10238));
			}
			else//	若没有完全开启所有空位，提示：提升等级或VIP获得更多空位
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10239));
			}
		}
	}
}

void LayerHeroFate::onBtnFateClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index, int heroId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_ui.posFatesIsSpace[index])
		{
			if (TOOL_TAG_FATE_DEFAULT == heroId)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10239));
			}
			else if(heroId > 0)
			{
				if(checkNeedReplaceFate())
				{
					return;
				}
				clearBtnPanel();
				//如果当前没选中这个未结缘的，则不清理，只设为选中状态
				if(index != m_nCurIndex)
				{
					//先隐藏当前红叉叉
					if(m_nCurIndex < MAX_HERO_FATE_COUNT && m_nCurIndex >= 0)
					{
						m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);
						m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
					}	
					m_nCurHeroId = heroId;
					m_nCurIndex = index;
					//设置选中项
					if (index != m_nCurIndex)
					{
						m_ui.pHeroFates[m_nCurIndex]->setSelected(false);
					}
					m_ui.pHeroFates[m_nCurIndex]->setSelected(true);
					m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);
					m_ui.pBtnTiedFates->setScale(0.1f);
					m_ui.pBtnTiedFates->runAction(EaseSineOut::create(ScaleTo::create(0.5, 1.0f)));
					m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
				}
				else
				{
					cancelSelectedHeroIcon();
				}
				
				//CustomTips::show("提升等级或VIP获得更多空位");
			}
			return;
		}

		if(checkNeedReplaceFate())
		{
			return;
		}
		clearBtnPanel();
		//点击已结缘英雄头像时 下方显示刷新按钮
		if (FateModel::getInstance()->heroIsTiedFate(heroId))
		{
			m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
			m_ui.pBtnRefreshFates->setScale(0.1f);
			m_ui.pBtnRefreshFates->runAction(EaseSineOut::create(ScaleTo::create(0.5, 1.0f)));
			m_nCurHeroId = heroId;
			//设置选中项
			if (index != m_nCurIndex)
			{
				m_ui.pHeroFates[m_nCurIndex]->setSelected(false);
			}
			//重设当前的索引时，先把之前的红叉叉隐藏
			m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);
			m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");

			m_nCurIndex = index;
			m_ui.pHeroFates[m_nCurIndex]->setSelected(true);
			//显示红叉叉
			m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(true);
			m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
		}
		else//点击未结缘英雄头像时 从容器移除该头像
		{
			//如果当前没选中这个未结缘的，则不清理，只设为选中状态
			if(index != m_nCurIndex)
			{
				m_nCurHeroId = heroId;
				m_nCurIndex = index;
				//设置选中项
				if (index != m_nCurIndex)
				{
					m_ui.pHeroFates[m_nCurIndex]->setSelected(false);
				}
				m_ui.pHeroFates[m_nCurIndex]->setSelected(true);
				m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);
				m_ui.pBtnTiedFates->setScale(0.1f);
				m_ui.pBtnTiedFates->runAction(EaseSineOut::create(ScaleTo::create(0.5, 1.0f)));

				m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
			}
			else
			{
				cancelSelectedHeroIcon();
			}		
		}	
	}
}

void LayerHeroFate::onBtnRefreshRefresh(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		HeroFateController::sendrRefreshFateMsg(m_nCurHeroId);
	}
}
void LayerHeroFate::onBtnRefreshReplace(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		HeroFateController::sendReplaceFateMsg(m_nCurHeroId);
		//
		/*a.在界面1中点击替换，则保存新属性显示界面2
		b.点击刷新，保持原属性不变，更新新属性
		c.点击关闭按钮，保存原属性返回主界面*/
	}
}

void LayerHeroFate::cancelSelectedHeroIcon()
{
	//点击取消，英雄头像消失，返回上方列表
	if (m_nCurHeroId > 0)
	{
		m_ui.posFatesIsSpace[m_nCurIndex] = true;
		//
		m_ui.pHeroFates[m_nCurIndex]->setMainIcon("Image/Icon/global/img_kaifang0_jia.png");
		m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
		m_ui.pHeroFates[m_nCurIndex]->setSelected(false);
		m_ui.pHeroFates[m_nCurIndex]->setTag(TOOL_TAG_FATE_CAN_ADD);
		m_ui.pHeroFates[m_nCurIndex]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, m_nCurIndex, TOOL_TAG_FATE_CAN_ADD));
		clearBtnPanel();
		FateModel::getInstance()->removeFateItem(m_nCurHeroId);
		UIToolHeader* pHero = dynamic_cast<UIToolHeader*>(this->m_ui.pSvHeros->getChildByTag(m_nCurHeroId));
		if (pHero != nullptr)
		{
			pHero->setTiedFateIconFlag(false);
		}
		//
		m_nCurHeroId = -1;
		//设置选中项
		int selIndex = -1;
		int nomalCount = getCanTiedFatesCount( HeroModel::getInstance()->getMainHeroLevel(), 10047);//主角缘分开放等级
		if(nomalCount > 0)
		{
			selIndex = 0;		//默认选中第一个框
		}
		if (-1 != selIndex)
		{
			m_nCurIndex = selIndex;
			m_ui.pHeroFates[selIndex]->setSelected(true);

			vector<FateItemInfo>& vecFates = FateModel::getInstance()->getFateInfos();
			if (vecFates.size() > 0)
			{
				m_nCurHeroId = m_ui.pHeroFates[0]->getTag();
				if(m_nCurHeroId > 0)
				{
					m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
					if(FateModel::getInstance()->heroIsTiedFate(m_nCurHeroId))
					{
						m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
						//显示红叉叉
						m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(true);
					}
					else
					{
						m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);
					}
				}
			}
		}
	}
}

void LayerHeroFate::tiedFatesResult(ObserverParam* obParam)
{
	m_ui.posFatesIsSpace[m_nCurIndex] = false;
	int heroId = *((int*)obParam->updateData);

	clearBtnPanel();
	m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
	//
	//显示描述
	FateItemInfo* pItemInfo = FateModel::getInstance()->getFateInfo(heroId);
	if (pItemInfo != nullptr)
	{
		m_ui.pImgDes[m_nCurIndex]->setVisible(true);
		//
		ImageView* pIcon = dynamic_cast<ImageView*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Image_shuxing"));
		pIcon->loadTexture(FateModel::getInstance()->getAttriIconName(pItemInfo->proType));
		Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxing"));
		pText->setText("+" + _TO_STR(pItemInfo->fProVal*100) + "%");
		pText->setColor(FateModel::getInstance()->getColorByQua(pItemInfo->fProQua));
		Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxingming"));
		pTextAttri->setText(getFateAttriName(pItemInfo->proType));
		m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(true);

		//播放心形动画
		playTiedFatesHeartAni();
		//
		UIToolHeader* pHero = dynamic_cast<UIToolHeader*>(this->m_ui.pSvHeros->getChildByTag(heroId));
		if (pHero != nullptr)
		{
			pHero->setTiedFateIcon("Image/Icon/global/img_yijieyuan.png");
		}
	}
}

void LayerHeroFate::cancelFatesResult()
{
	m_ui.posFatesIsSpace[m_nCurIndex] = true;

	//移除线形动画
	removeTiedFatesLineAni(LINE_ANI_TAG+m_nCurIndex);

	clearBtnPanel();
	m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);

	m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);

	m_ui.pImgDes[m_nCurIndex]->setVisible(false);	

	UIToolHeader* pHero = dynamic_cast<UIToolHeader*>(this->m_ui.pSvHeros->getChildByTag(m_nCurHeroId));
	if (pHero != nullptr)
	{
		pHero->setTiedFateIcon("Image/Icon/global/img_kejieyuan.png");
	}
}

void LayerHeroFate::refreshFatesResult(ObserverParam* obParam)
{
	clearBtnPanel();
	m_ui.pBtnPnl1->addChild(m_ui.pBtnRefreshFates);
	m_ui.pBtnPnl2->addChild(m_ui.pBtnReplaceFates);

	//更新描述
	int heroId = *((int*)obParam->updateData);
	FateItemInfo* pItemInfo = FateModel::getInstance()->getFateInfo(heroId);
	if (pItemInfo != nullptr)
	{
		Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxing"));
		pText->setText("+" + _TO_STR(pItemInfo->fNewProVal*100) + "%");
		pText->setColor(FateModel::getInstance()->getColorByQua(pItemInfo->fNewQua));
		Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxingming"));
		pTextAttri->setText(getFateAttriName(pItemInfo->proType));

		//箭头
		ImageView* pImgFlag = (ImageView*)(m_ui.pImgDes[m_nCurIndex]->getChildByName("Image_shangsheng"));
		pImgFlag->setVisible(true);
		if(pItemInfo->fProVal < pItemInfo->fNewProVal)
		{
			pImgFlag->loadTexture("Image/Icon/global/img_up.png");
			//刷新按钮置为不可点击
			m_ui.pBtnRefreshFates->setEnabled(false);
			m_ui.pBtnRefreshFates->setBright(false);
		}
		else if(pItemInfo->fProVal > pItemInfo->fNewProVal)
		{
			pImgFlag->loadTexture("Image/Icon/global/img_down.png");
		}
		else
		{
			pImgFlag->setVisible(false);
		}
	}
}

void LayerHeroFate::replaceFatesResult(ObserverParam* obParam)
{
	clearBtnPanel();
	m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);

	int heroId = *((int*)obParam->updateData);

	//更新描述
	FateItemInfo* pItemInfo = FateModel::getInstance()->getFateInfo(heroId);
	if (pItemInfo != nullptr)
	{		
		m_ui.pImgDes[m_nCurIndex]->getChildByName("Image_shangsheng")->setVisible(false);

		Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxing"));
		pText->setText("+" + _TO_STR(pItemInfo->fNewProVal*100) + "%");
		pText->setColor(FateModel::getInstance()->getColorByQua(pItemInfo->fProQua));
		Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxingming"));
		pTextAttri->setText(getFateAttriName(pItemInfo->proType));
	}
}

string LayerHeroFate::getFateAttriName(const string& attriType)
{
	if (attriType.compare("hp") == 0)
	{
		//return STR_UTF8("血量");				//血量
		return DataManager::getInstance()->searchCommonTexdtById(10008);				//血量
	}
	else if (attriType.compare("atk") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10009);				//攻击
	}
	else if (attriType.compare("pdd") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10010);				//物防
	}
	else if (attriType.compare("mdd") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10011);				//法防
	}
	else if (attriType.compare("hit") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10012);				//命中
	}
	else if(attriType.compare("miss") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10013);				//闪避
	}
	else if(attriType.compare("criPrb") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10014);				//暴击
	}
	else if(attriType.compare("criRes") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10015);				//暴抗
	}
	else if (attriType.compare("criCoe") == 0)
	{
		return DataManager::getInstance()->searchCommonTexdtById(10016);				//暴伤
	}
	
	return "";
}

void LayerHeroFate::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;
	switch (obParam->id)
	{
	case nMAIN_CMD_HERO_FATE_GET_INFO://获取主角缘分主界面信息
		{
			initHeroList();
			showFates();

			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_FATE);
		}
		break;
	case nMAIN_CMD_HERO_FATE_TIED_FATES://结识主角缘分
		{
			tiedFatesResult(obParam);
		}
		break;
	case nMAIN_CMD_HERO_FATE_CANCEL_FATES://取消主角缘分
		{
			cancelFatesResult();
		}
		break;
	case nMAIN_CMD_HERO_FATE_REFRESH_FATES://刷新缘分
		{
			refreshFatesResult(obParam);
		}
		break;
	case nMAIN_CMD_HERO_FATE_REPLACE_FATES://替换缘分
		{
			replaceFatesResult(obParam);
		}
		break;
	default:
		break;
	}
}


//初始化英雄列表
void LayerHeroFate::initHeroList()
{
	m_nDisHeroTotleCount = 0;
	m_ui.pSvHeros->removeAllChildren();
	Vector<HeroParam*>*  pVecHeros = HeroModel::getInstance()->getHeroList();

	//显示数量=英雄数-已结缘数-主角
	vector<FateItemInfo>& vecFates = FateModel::getInstance()->getFateInfos();
	int disCount = pVecHeros->size()-1;//-vecFates.size()
	//列表人数小于7时使用空框体代替
	vector<int>  disVecHeros;
	for (auto& iter:(*pVecHeros))
	{
		if (iter->mRoleType != HERO_TYPE::MAIN_HERO)
		{
			disVecHeros.push_back(iter->mHeroId);
		}		
	}
	//至少显示7个
	if (disVecHeros.size() < 7)
	{
		if (disCount == 1)
		{
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
		}
		else if (disCount == 2)
		{
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
		}
		else if (disCount == 3)
		{
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
		}
		else if (disCount == 4)
		{
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.push_back(-1);
			disVecHeros.push_back(-1);
		}
		else if (disCount == 5)
		{
			disVecHeros.insert(disVecHeros.begin(), -1);
			disVecHeros.push_back(-1);
		}
		else if (disCount == 6)
		{
			disVecHeros.push_back(-1);
		}
	}
	disCount = disVecHeros.size();

	int scrollRealWidth = 0;
	scrollRealWidth = disCount*TOOL_SMALL_HERO_WIDTH + 20;
	int nScrollViewOriWidth = m_ui.pSvHeros->getSize().width;

	int scrollViewWidth = (scrollRealWidth > nScrollViewOriWidth) ? scrollRealWidth : nScrollViewOriWidth;
	m_ui.pSvHeros->setInnerContainerSize(Size(scrollViewWidth, m_ui.pSvHeros->getContentSize().height ));
	cocos2d::Size innerSize = m_ui.pSvHeros->getInnerContainerSize();
	//显示逻辑，显示7个，两边最小，中间最大，梯度显示
	float fDisX = 0;
	int index = 0;
	for (auto& iter:disVecHeros)	
	{
		m_nDisHeroTotleCount ++;
		if (iter == -1)
		{
			UIToolHeader* pHero = UIToolHeader::create();
			pHero->setStateImg("Image/Icon/global/btn_kuang_juese_1.png", "Image/Icon/global/btn_kuang_juese_2.png");
			pHero->setTouchEnabled(false);
			pHero->setScale(TOOL_SMALL_HERO_SCALE);
			fDisX = TOOL_SMALL_HERO_WIDTH/2 + 14 + TOOL_SMALL_HERO_WIDTH*index;
			pHero->setPosition(Vec2(fDisX, m_ui.pSvHeros->getSize().height/2 ));
			m_ui.pSvHeros->addChild(pHero, 0, iter);	
		}
		else
		{
			HeroParam*  pHeroParam = HeroModel::getInstance()->searchHeroById(iter);
			if (pHeroParam == nullptr)
			{
				continue;
			}
			UIToolHeader* pHero = UIToolHeader::create(ResourceUtils::getSmallIconPath(pHeroParam->mResId), "Image/Icon/global/btn_kuang_juese_1.png", "Image/Icon/global/btn_kuang_juese_2.png");
			pHero->setLvl(pHeroParam->mLevel);
			pHero->setStarsNum(pHeroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
			pHero->setTouchEnabled(true);
			pHero->setScale(TOOL_SMALL_HERO_SCALE);
			fDisX = TOOL_SMALL_HERO_WIDTH/2 + 14 + TOOL_SMALL_HERO_WIDTH*index;
			pHero->setPosition(Vec2(fDisX, m_ui.pSvHeros->getSize().height/2 ));
			pHero->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnHeroClicked, this, pHeroParam->mHeroId));
			//显示已结缘，可结缘图标
			FateItemInfo* pFateItemInfo = FateModel::getInstance()->getFateInfo(pHeroParam->mHeroId);
			if (pFateItemInfo != nullptr )
			{
				if (pFateItemInfo->isTied)
				{
					pHero->setTiedFateIcon("Image/Icon/global/img_yijieyuan.png");
				}
				else
				{
					pHero->setTiedFateIcon("Image/Icon/global/img_kejieyuan.png");
				}
				pHero->setTiedFateIconFlag(true);
			}
			else
			{
				pHero->setTiedFateIconFlag(false);
			}	
			m_ui.pSvHeros->addChild(pHero, 0, iter);				
		}
		//		
		index ++;
	}	
}

//
void LayerHeroFate::showFates()
{
	//nomalCount是已开放的非VIP格子
	int nomalCount = getCanTiedFatesCount( HeroModel::getInstance()->getMainHeroLevel(), 10047);//主角缘分开放等级

	//vipCount是已开放的VIP格子数量
	int vipCount = getCanTiedFatesCount( MainModel::getInstance()->getMainParam()->mVipLevel, 10048);//主角缘分开放VIP等级
	vipCount +=4;//VIP从第5个开始显示
	//
	vector<FateItemInfo>& vecFates = FateModel::getInstance()->getFateInfos();
	int index = 0;
	for (int i = 0; i < MAX_HERO_FATE_COUNT; i++)	
	{
		if (i < 4)//非VIP格子
		{
			if (i < nomalCount)//已经开放的
			{
				FateItemInfo* fateInfo = FateModel::getInstance()->getFateInfoByPos(i + 1);

				if (fateInfo)//已经结识显示英雄
				{			
					//FateItemInfo& pItemInfo = vecFates.at(i);
					HeroParam* pHeroParam = HeroModel::getInstance()->searchHeroById(fateInfo->heroId);
					if (pHeroParam != nullptr)
					{
						m_ui.pHeroFates[i]->setMainIcon(ResourceUtils::getSmallIconPath(pHeroParam->mResId));
					}	
					m_ui.pHeroFates[i]->setTag(fateInfo->heroId);
					m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
					m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, fateInfo->heroId));
					
					if (fateInfo->isTied)
					{
						m_ui.posFatesIsSpace[i] = false;
						//显示描述
						m_ui.pImgDes[i]->setVisible(true);
						//
						ImageView* pIcon = dynamic_cast<ImageView*>(m_ui.pImgDes[i]->getChildByName("Image_shuxing"));
						pIcon->loadTexture(FateModel::getInstance()->getAttriIconName(fateInfo->proType));
						Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxing"));
						pText->setText("+" + _TO_STR(fateInfo->fProVal*100) + "%");
						pText->setColor(FateModel::getInstance()->getColorByQua(fateInfo->fProQua));
						Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxingming"));
						pTextAttri->setText(getFateAttriName(fateInfo->proType));

						//播放线形动画
						playTiedFatesLineAni(i);
					}
					else
					{
						m_ui.pImgDes[i]->setVisible(false);
					}
				}
				else //未结识显示“+”
				{
					m_ui.pHeroFates[i]->setMainIcon("Image/Icon/global/img_kaifang0_jia.png");
					m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
					m_ui.pImgDes[i]->setVisible(false);
					m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, TOOL_TAG_FATE_CAN_ADD));
					m_ui.pHeroFates[i]->setTag(TOOL_TAG_FATE_CAN_ADD);
				}
			}
			else//未开放的显示默认图标
			{
				m_ui.pHeroFates[i]->setMainIcon("Image/Icon/global/img_kaifang1" + _TO_STR(i+1) +"ji.png");
				m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
				m_ui.pImgDes[i]->setVisible(false);

				m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, TOOL_TAG_FATE_DEFAULT));
				m_ui.pHeroFates[i]->setTag(TOOL_TAG_FATE_DEFAULT);
			}
		}
		else//VIP从第5个开始显示
		{
			if (i>3 && i<vipCount)//vip已经开放的
			{
				FateItemInfo* fateInfo = FateModel::getInstance()->getFateInfoByPos(i + 1);
				if (fateInfo)//已经结识显示英雄
				{
					m_ui.posFatesIsSpace[i] = false;
					//FateItemInfo& pItemInfo = vecFates.at(nomalCount+i-4);
					HeroParam* pHeroParam = HeroModel::getInstance()->searchHeroById(fateInfo->heroId);
					if (pHeroParam != nullptr)
					{
						m_ui.pHeroFates[i]->setMainIcon(ResourceUtils::getSmallIconPath(pHeroParam->mResId));
					}
					m_ui.pHeroFates[i]->setTag(fateInfo->heroId);
					m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
					m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, fateInfo->heroId));
					
					if (fateInfo->isTied)
					{
						//显示描述
						m_ui.pImgDes[i]->setVisible(true);
						//
						ImageView* pIcon = dynamic_cast<ImageView*>(m_ui.pImgDes[i]->getChildByName("Image_shuxing"));
						pIcon->loadTexture(FateModel::getInstance()->getAttriIconName(fateInfo->proType));
						Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxing"));
						pText->setText("+" + _TO_STR(fateInfo->fProVal*100) + "%");
						pText->setColor(FateModel::getInstance()->getColorByQua(fateInfo->fProQua));
						Text* pTextAttri = dynamic_cast<Text*>(m_ui.pImgDes[i]->getChildByName("Label_shuxingming"));
						pTextAttri->setText(getFateAttriName(fateInfo->proType));

						//播放线形动画
						playTiedFatesLineAni(i);
					}
					else
					{
						m_ui.pImgDes[i]->setVisible(false);
					}
				}
				else //未结识显示“+”
				{
					m_ui.pHeroFates[i]->setMainIcon("Image/Icon/global/img_kaifang0_jia.png");
					m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
					m_ui.pImgDes[i]->setVisible(false);

					m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, TOOL_TAG_FATE_CAN_ADD));
					m_ui.pHeroFates[i]->setTag(TOOL_TAG_FATE_CAN_ADD);
				}
			}
			else//未开放的显示默认图标
			{
				m_ui.pHeroFates[i]->setMainIcon("Image/Icon/global/img_kaifang2" + _TO_STR(i-4+1) +"_V.png");
				m_ui.pHeroFates[i]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_weikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_weikaifang_2.png");
				m_ui.pImgDes[i]->setVisible(false);

				m_ui.pHeroFates[i]->addTouchEventListener(CC_CALLBACK_2(LayerHeroFate::onBtnFateClicked, this, i, TOOL_TAG_FATE_DEFAULT));
				m_ui.pHeroFates[i]->setTag(TOOL_TAG_FATE_DEFAULT);
			}
		}			
	}
	if (vecFates.size()>0)
	{
		m_nCurIndex = 0;
		m_nCurHeroId = m_ui.pHeroFates[m_nCurIndex]->getTag();
		m_ui.pHeroFates[m_nCurIndex]->setSelected(true);
		m_ui.pHeroFates[m_nCurIndex]->setStateImg("Image/Icon/global/btn_yuanfen_kuang_yikaifang_1.png", "Image/Icon/global/btn_yuanfen_kuang_yikaifang_2.png");
		clearBtnPanel();
		if(m_nCurHeroId > 0)
		{
			if (FateModel::getInstance()->heroIsTiedFate(m_nCurHeroId))
			{
				//红叉显示，只显示刷新按钮
				m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(true);		
				m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
			}
			else//点击英雄头像结识缘分时 红叉隐藏，只显示结缘按钮
			{
				m_ui.pLayFates[m_nCurIndex]->getChildByName("Button_jiechuyuanfen")->setVisible(false);
				m_ui.pBtnPnl3->addChild(m_ui.pBtnTiedFates);
			}
		}
	}
}


int LayerHeroFate::getCanTiedFatesCount(int lvl, int sysId)
{
	//可结识缘分数
	int canCount = 0;
	//
	RowData* sysData = DataManager::getInstance()->searchToolSystemById(sysId);
	if (sysData != nullptr)
	{
		string strLvls = sysData->getStringData("value");
		vector<int>vecLvls;
		StringFormat::parseCsvStringVec(strLvls, vecLvls);

		for (auto& iter:vecLvls)
		{
			if (lvl >= iter)
			{
				canCount ++;
			}
		}
	}
	return canCount;
}

void LayerHeroFate::startHeroToFates(int heroId)
{
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(LayerHeroFate::callBackHeroToFates), this, 0.1, false);
	m_nHeroToFatesHeroId = heroId;
}
void LayerHeroFate::callBackHeroToFates(float dt)
{
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerHeroFate::callBackHeroToFates), this);
	initHeroList();
}

int LayerHeroFate::getFatesSpacepos(int nomalCount, int vipCount)
{
	for (int i = 0; i < MAX_HERO_FATE_COUNT; i++)
	{
		if(m_ui.posFatesIsSpace[i])
		{
			if(i < nomalCount)
			{
				return i;
			}
			else
			{
				if(vipCount != 0 && i >= 4 && i <= vipCount+3)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

void LayerHeroFate::onScrollViewEvent(Ref* sender, ScrollviewEventType type)
{
	ScrollView* pScroll = (ScrollView*)sender;
	Size contentSize = pScroll->getContentSize();
	Size size = pScroll->getInnerContainerSize();
	if (contentSize.width>=size.width)
	{
		return;
	}
	const Vector<Node*> vecHeros = m_ui.pSvHeros->getChildren();
	switch (type)
	{
	case SCROLLVIEW_EVENT_SCROLLING:
		{
			Vec2 vec = pScroll->getInnerContainer()->getPosition();	
			float fx = vec.x;
			int beginIndex = -vec.x/TOOL_SMALL_HERO_WIDTH;	
			if (vecHeros.size()<=7)
			{
				beginIndex = 0;
			}
			else if (beginIndex > vecHeros.size()-7)
			{
				beginIndex = vecHeros.size()-7;
			}
			if (beginIndex < 0)
			{
				beginIndex = 0;
			}
			if (beginIndex != m_nDisHeroBeginIndex)
			{
				m_nDisHeroBeginIndex = beginIndex;
			}			
		}
		break;
	default:
		break;
	}
}
void LayerHeroFate::clearBtnPanel()
{
	m_ui.pBtnPnl1->removeAllChildrenWithCleanup(true);
	m_ui.pBtnPnl2->removeAllChildrenWithCleanup(true);
	m_ui.pBtnPnl3->removeAllChildrenWithCleanup(true);

	//重置刷新按钮为可点击状态
	m_ui.pBtnRefreshFates->setEnabled(true);
	m_ui.pBtnRefreshFates->setBright(true);
}

void LayerHeroFate::playTiedFatesHeartAni()
{
	auto aniCallBack = [&, this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
	};

	auto aniCallBack1 = [&, this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
		playTiedFatesLineAni(m_nCurIndex);
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anijieyuanxian/Anijieyuanxian.ExportJson");
	Armature* aniMainHeroHeart = Armature::create("Anijieyuanxian");
	aniMainHeroHeart->getAnimation()->play("Ani_xin");
	aniMainHeroHeart->getAnimation()->setMovementEventCallFunc(aniCallBack);
	aniMainHeroHeart->setPosition(m_ui.pLayFates[m_nCurIndex]->getContentSize() / 2);
	m_ui.pLayFates[m_nCurIndex]->addChild(aniMainHeroHeart,99);

	Armature* aniFateHeroHeart = Armature::create("Anijieyuanxian");
	aniFateHeroHeart->getAnimation()->play("Ani_xin");
	aniFateHeroHeart->getAnimation()->setMovementEventCallFunc(aniCallBack1);
	aniFateHeroHeart->setPosition(m_ui.pImgMainRole->getContentSize() / 2);
	m_ui.pImgMainRole->addChild(aniFateHeroHeart,99);
}

void LayerHeroFate::playTiedFatesLineAni(int index)
{
	if(index < 0 || index > 7)
	{
		return;
	}
	
	string aniName = "";
	switch (index)
	{
	case 0:
	case 4:
		aniName = "Ani_1xian";
		break;
	case 1:
	case 5:
		aniName = "Ani_2xian";
		break;
	case 2:
	case 6:
		aniName = "Ani_3xian";
		break;
	case 3:
	case 7:
		aniName = "Ani_4xian";
		break;
	default:
		break;
	}

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anijieyuanxian/Anijieyuanxian.ExportJson");
	Armature* ani = Armature::create("Anijieyuanxian");
	ani->setPosition(Point(m_ui.pLayTiedFates->getContentSize() / 2) - Point(6, 22));
	if(index == 1 || index == 3 || index == 4 || index == 6)
	{
		ani->setScaleX(-1);
	}
	ani->getAnimation()->play(aniName);
	ani->setTag(LINE_ANI_TAG + index);

	m_ui.pLayTiedFates->addChild(ani);
}

void LayerHeroFate::removeTiedFatesLineAni(int aniTag)
{
	if(m_ui.pLayTiedFates->getChildByTag(aniTag))
	{
		m_ui.pLayTiedFates->removeChildByTag(aniTag);
	}
}

bool LayerHeroFate::checkNeedReplaceFate()
{
	if(m_nCurIndex < MAX_HERO_FATE_COUNT && m_nCurIndex >= 0)
	{
		m_nCurHeroId = m_ui.pHeroFates[m_nCurIndex]->getTag();	
	}
		
	if(m_nCurHeroId > 0)
	{
		FateItemInfo* fateInfo = FateModel::getInstance()->getFateInfo(m_nCurHeroId);
		if(fateInfo->fProVal < fateInfo->fNewProVal)
		{
			//刷新到更高的值没替换
			auto call = [this](Ref* ref, CustomRetType type)->void
			{
				if (type == RET_TYPE_OK)
				{
					//替换
					HeroFateController::sendReplaceFateMsg(m_nCurHeroId);
				}
				else
				{
					FateItemInfo* curFateInfo = FateModel::getInstance()->getFateInfo(m_nCurHeroId);
					Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxing"));
					pText->setText("+" + _TO_STR(curFateInfo->fProVal*100) + "%");
					pText->setColor(FateModel::getInstance()->getColorByQua(curFateInfo->fProQua));
					m_ui.pImgDes[m_nCurIndex]->getChildByName("Image_shangsheng")->setVisible(false);		//箭头隐藏
					curFateInfo->fNewProVal = 0.0f;
					clearBtnPanel();
					m_ui.pBtnPnl3->addChild(m_ui.pBtnRefreshFates);
					m_ui.pBtnRefreshFates->setScale(0.1f);
					m_ui.pBtnRefreshFates->runAction(EaseSineOut::create(ScaleTo::create(0.5, 1.0f)));
				}
			};
			//提示要不要替换
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10240), call, CUSTOM_YES_NO_CANCEL);

			return true;
		}
		else
		{
			FateItemInfo* curFateInfo = FateModel::getInstance()->getFateInfo(m_nCurHeroId);
			Text* pText = dynamic_cast<Text*>(m_ui.pImgDes[m_nCurIndex]->getChildByName("Label_shuxing"));
			pText->setText("+" + _TO_STR(curFateInfo->fProVal*100) + "%");
			pText->setColor(FateModel::getInstance()->getColorByQua(curFateInfo->fProQua));
			m_ui.pImgDes[m_nCurIndex]->getChildByName("Image_shangsheng")->setVisible(false);		//箭头隐藏
			curFateInfo->fNewProVal = 0.0f;
		}
	}
	return false;
}