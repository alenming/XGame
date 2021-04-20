#include "LayerFrame.h"
#include "LayerChapter.h"
#include "ChapterController.h"
#include "ChapterModel.h"
#include "MainModule/MainCityScene.h"
#include "GuideCenter/GuideManager.h"
#include "Widget/LayerCommHeadInfo.h"
#include "MainModule/MainCityLoadingScene.h"
#include "SceneChapter.h"

LayerFrame::LayerFrame(void)
{
	m_mapLayerChapter.insert(pair<CHAPTER_TYPE, Layer*>(CHAPTER_TYPE_STORY, nullptr));
	m_mapLayerChapter.insert(pair<CHAPTER_TYPE, Layer*>(CHAPTER_TYPE_EVENT, nullptr));
	ChapterController::getInstance()->setChapterFrameView(this);
	m_CurSelPage = CHAPTER_TYPE_INVALID;
}

void LayerFrame::onExit()
{
	// 释放资源;
	//cocostudio::GUIReader::getInstance()->destroyInstance();
	//ResourceLoader::getInstance()->removeUIResource("UIChapter");

	Layer::onExit();
}


LayerFrame::~LayerFrame(void)
{
	ChapterController::getInstance()->setChapterFrameView(nullptr);
	// 销毁控制器;
	if (false == ChapterModel::getInstance()->getSweepState()->bSweeping)
	{
		ChapterController::destroyInstance();
	}
}

bool LayerFrame::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();
	
	return true;
}

void LayerFrame::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIChapter/UIFrame.ExportJson"));

	////// 前景层;
	m_ui.pnlFrame = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Frame)));
	m_ui.pnlFrame->setSize(visibleSize);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Frame_Back);
	},
		HEAD_INFO_TYPE_ADVENTURE);
	_layerCommHeadInfo->setPosition(Point(-pos.x, 0.0f) + _layerCommHeadInfo->getPosition());
	m_ui.pnlFrame->addChild(_layerCommHeadInfo, 9);

	// 目录索引;
	ostringstream oss;
	for (unsigned int i = 0; i < 5; ++i)
	{
		oss.str("");
		oss << "Btn_" << i+1;
		m_ui.btnChapterIndex[i] = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.btnChapterIndex[i]->addTouchEventListener(CC_CALLBACK_2(LayerFrame::onChapterIndexBtnClicked, this, i));
	}

	// 将UI_Frame层加入到当前场景;
	this->addChild(m_ui.pRoot);
}


void LayerFrame::onBtnClicked( Ref* ref, cocos2d::ui::Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Frame_Back:
			{
				if (SceneChapter::getChapterBackFlag())
				{
					Director::getInstance()->popScene();
				}
				else
				{
					Director::getInstance()->replaceScene(MainCityLoadingScene::create());
				}
			}
			break;

		default:
			break;
		}
	}
}

void LayerFrame::switchLayerChapter(CHAPTER_TYPE pageType, int nLocalZOrder)
{
	if (pageType == m_CurSelPage)
		return;

	if (nullptr == m_mapLayerChapter.find(pageType)->second)
	{
		Layer* layerChapter = nullptr;
		switch (pageType)
		{
		case CHAPTER_TYPE_STORY:
			{
				layerChapter = LayerChapter::create();
				if (nullptr != layerChapter)
				{
					m_mapLayerChapter.find(pageType)->second = layerChapter;
					this->m_ui.pRoot->addChild(layerChapter, nLocalZOrder);
				}
			}
			break;

// 		case CHAPTER_TYPE_EVENT:
// 			{
// 			}
// 			break;

		default:
			break;
		}
	}

	m_CurSelPage = pageType;
	showLayerChapter(m_CurSelPage);
}


void LayerFrame::showLayerChapter(CHAPTER_TYPE pageType)
{
	for (auto iter = m_mapLayerChapter.begin(); iter != m_mapLayerChapter.end(); iter++)
	{
		if (nullptr != iter->second)
		{
			iter->second->setVisible(pageType == iter->first || CHAPTER_TYPE_STORY == iter->first);
			//this->setFrameEnabled(pageType == CHAPTER_TYPE_STORY);
		}
	}
}

void LayerFrame::setFrameEnabled(bool  bEnabled)
{
	// 处理前景层的显示或隐藏;
	m_ui.pnlFrame->setVisible(bEnabled);
	m_ui.pnlFrame->setEnabled(bEnabled);

	auto it = m_mapLayerChapter.find(m_CurSelPage);
	if (it != m_mapLayerChapter.end())
	{
		it->second->setVisible(bEnabled);
	}
}

void LayerFrame::onChapterIndexBtnClicked( Ref* ref, Widget::TouchEventType type, int nIndex )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		/*
			Index	  0   1   2   3   4
			Chapter	  1   10  20  30  40
		*/
		int nOrder = (nIndex == 0) ? 1 : (nIndex * 10);

		// test;
		//nOrder = (nIndex == 0) ? 1 : (nIndex * 2);

		auto iter = m_mapLayerChapter.find(CHAPTER_TYPE_STORY);
		assert(iter != m_mapLayerChapter.end());
		if (iter->second != nullptr)
		{
			LayerChapter* _layer = dynamic_cast<LayerChapter*>(iter->second);
			if (_layer != nullptr)
			{
				_layer->switch2Chapter(nOrder, CHAPTER_NODE_POS_LEFT);
			}
		}
	}
}
