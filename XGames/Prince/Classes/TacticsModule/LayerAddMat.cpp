#include "LayerAddMat.h"
#include "Utils/SoundUtils.h"
#include "UI/UIDef.h"
#include "ModelTactics.h"
#include "ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Temp/CustomTips.h"
#include "LayerTactics.h"
#include "GuideCenter/GuideManager.h"

//道具列表大小
#define SIZE_PROP_BTN			Size(710, 142)

//每个tableView列表上的道具Tag
#define TAG_PROP_CELL			100

LayerAddMat::LayerAddMat()
{
	m_scrollBar = nullptr;
	m_pSelectedMat = nullptr;
	m_iPropTotalNum = 0;
	m_bIsSelect = false;
	m_vSeledProp.clear();
}

LayerAddMat::~LayerAddMat()
{

}

LayerAddMat* LayerAddMat::create(vector<int>* vecSeledMat, eRowType curRowType)
{
	LayerAddMat* _layer = new LayerAddMat();
	if (nullptr != _layer && _layer->init(vecSeledMat, curRowType))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerAddMat::init(vector<int>* vecSeledMat, eRowType curRowType)
{
	if(!ModalLayer::init())
	{
		return false;
	}

	m_pSelectedMat = vecSeledMat;

	//先计算各道具数量
	initPropNum();

	initUI(curRowType);

	return true;
}

void LayerAddMat::onEnter()
{
	ModalLayer::onEnter();

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_TACTICS_SELECT);
}

void LayerAddMat::onExit()
{
	ModalLayer::onExit();
}

void LayerAddMat::initUI(eRowType curRowType)
{
	//加载经验道具选择界面
	string strFileName;
	strFileName.assign("Image/UiZhenFa/UiSelectProp.ExportJson");

	//选择经验道具界面视图
	m_ui.pnlSelExpProp = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pnlSelExpProp);

	//经验值
	m_ui.txtCurExp = (Text*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Lab_getExp"));
	m_ui.txtCurExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtNextLvExp = (Text*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Lab_nextExp"));
	m_ui.txtNextLvExp->setFontName(FONT_FZZHENGHEI);

	sCurRowData curRow = ModelTactics::getInstance()->getCurRowData(curRowType);
	//当前经验值
	int curTotalExp = curRow.mRowLvUpExp == -1 ? 0 : curRow.mRowCurExp;
	for(auto iter = m_pSelectedMat->begin(); iter != m_pSelectedMat->end(); ++iter)
	{
		RowData* propData = DataManager::getInstance()->searchToolById(*iter);
		if(propData != nullptr)
		{
			curTotalExp += propData->getIntData("value");
		}
	}	
	if(curRow.mRowLvUpExp != -1)
	{
		m_ui.txtNextLvExp->setString(TO_STR(curRow.mRowLvUpExp));
	}
	else
	{
		m_ui.txtNextLvExp->setString(DataManager::getInstance()->searchCommonTexdtById(20126));
	}
	m_ui.txtCurExp->setString(TO_STR(curTotalExp));

	//关闭按钮
	m_ui.btnClose = (Button*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Btn_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerAddMat::onBtnClick, this));

	//确认按钮
	m_ui.btnSure = (Button*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Btn_Sure"));
	m_ui.btnSure->addTouchEventListener(CC_CALLBACK_2(LayerAddMat::onBtnClick, this));

	//滚动面板
	m_ui.tbViewPnl = (Layout*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Pnl_tbView"));

	//按钮模板
	m_ui.btnMaterial = (Button*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Btn_temp_prop"));
	m_ui.btnMaterial->setVisible(false);

	//创建个tableView
	m_tbViewExpProp = extension::TableView::create(this, m_ui.tbViewPnl->getContentSize(), nullptr);
	m_tbViewExpProp->setDirection(extension::ScrollView::Direction::VERTICAL);
	m_tbViewExpProp->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	m_tbViewExpProp->reloadData();
	m_tbViewExpProp->setDelegate(this);
	m_tbViewExpProp->setBounceable(true);
	m_tbViewExpProp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_tbViewExpProp->setPosition(Point(10, 0));
	m_ui.tbViewPnl->addChild(m_tbViewExpProp);

	//设置滚动进度条
	m_scrollBar = CustomScrollBar::create();
	m_scrollBar->initScrollBar(m_tbViewExpProp->getViewSize().height);
	m_scrollBar->setPosition(m_tbViewExpProp->getPosition() + Point(26, 30));
	m_scrollBar->setScrollBarLengthPercent(1.0f);
	m_scrollBar->setScrollBarPercentY(0.0f);

	ImageView* scrollBg = (ImageView*)(Helper::seekWidgetByName(m_ui.pnlSelExpProp, "Img_scrollBg"));
	scrollBg->addChild(m_scrollBar);

	m_scrollBar->setScrollBarLengthPercent(m_tbViewExpProp->getViewSize().height / m_tbViewExpProp->getContentSize().height);
	m_scrollBar->setScrollBarPercentY(0.0f);
}

void LayerAddMat::onBtnClick( Ref* ref, Widget::TouchEventType type)
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");

		LayerTactics* layer = (LayerTactics*)this->getParent();
		layer->updateSelExpPropView();

		this->dispose();
	}
}

void LayerAddMat::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{

}

Size LayerAddMat::cellSizeForTable(extension::TableView *table)
{
	return SIZE_PROP_BTN;
}


extension::TableViewCell* LayerAddMat::tableCellAtIndex(extension::TableView *table, ssize_t idx)
{
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		cell = extension::TableViewCell::create();

		//左
		Button* btnL = (Button*)m_ui.btnMaterial->clone();
		btnL->setName("LEFT_BTN");
		btnL->setPosition(Vec2(2, cell->getContentSize().height/2));
		btnL->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		Text* matIdL = Text::create();
		matIdL->setString("");
		matIdL->setName("MAT_ID");
		btnL->addChild(matIdL);
		matIdL->setVisible(false);
		Text* txtPropNameL = (Text*)(btnL->getChildByName("Lab_name"));
		Text* txtPropExpL = (Text*)(btnL->getChildByName("Lab_exp"));
		txtPropNameL->setFontName(FONT_FZZHENGHEI);
		txtPropExpL->setFontName(FONT_FZZHENGHEI);
		onBtnTouchEvent(btnL);
		cell->addChild(btnL);

		//右
		Button* btnR = (Button*)m_ui.btnMaterial->clone();
		btnR->setName("RIGHT_BTN");
		btnR->setPosition(Vec2(SIZE_PROP_BTN.width/2, cell->getContentSize().height/2));
		btnR->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		Text* matIdR = Text::create();
		matIdR->setString("");
		matIdR->setName("MAT_ID");
		btnR->addChild(matIdR);
		matIdR->setVisible(false);
		Text* txtPropNameR = (Text*)(btnR->getChildByName("Lab_name"));
		Text* txtPropExpR = (Text*)(btnR->getChildByName("Lab_exp"));
		txtPropNameR->setFontName(FONT_FZZHENGHEI);
		txtPropExpR->setFontName(FONT_FZZHENGHEI);
		onBtnTouchEvent(btnR);
		cell->addChild(btnR);
	}

	//设置cell
	setCellContent(cell, idx);

	return cell;
}

ssize_t LayerAddMat::numberOfCellsInTableView(extension::TableView *table)
{
	return m_iPropTotalNum%2 > 0 ? (m_iPropTotalNum/2+1) : m_iPropTotalNum/2;
}

void LayerAddMat::scrollViewDidScroll( extension::ScrollView* view )
{
	Size size = view->getContentSize();
	Vec2 vec = view->getContentOffset();

	//滚动条重设进度
	float percentY = -vec.y / (size.height - view->getViewSize().height);
	m_scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
}

void LayerAddMat::setCellContent( extension::TableViewCell* cell, ssize_t idx )
{
	//先查出该经验道具id，思路：该道具前面共有idx*2个
	int nId = 0;
	int curNum = 0;
	for(int i=0; i<m_vRowUpMaterial.size(); i++)
	{
		curNum += m_vRowUpMaterial.at(i).amount;
		if(curNum >= idx*2+1)
		{
			nId = m_vRowUpMaterial.at(i).id;
			break;
		}
	}

	//左侧道具
	Button* itemBtnL = (Button*)cell->getChildByName("LEFT_BTN");
	Button* itemBtnR = (Button*)cell->getChildByName("RIGHT_BTN");

	if(nId == 0)
	{
		//没有更多道具了
		if(itemBtnL != nullptr)
		{
			itemBtnL->setVisible(false);
		}
		if(itemBtnR != nullptr)
		{
			itemBtnR->setVisible(false);
		}

		return;
	}
	if(itemBtnL != nullptr)
	{
		itemBtnL->setVisible(true);
		itemBtnL->setTag(2*idx);
		RowData* propData = DataManager::getInstance()->searchToolById(nId);
		string propName = "";
		int expValue = 0;
		if(propData != nullptr)
		{
			propName = propData->getStringData("itemName");
			expValue = propData->getIntData("value");
		}

		Text* txtPropName = (Text*)(itemBtnL->getChildByName("Lab_name"));
		Text* txtPropExp = (Text*)(itemBtnL->getChildByName("Lab_exp"));
		txtPropName->setString(propName);
		txtPropExp->setString(DataManager::getInstance()->searchCommonTexdtById(20119) +": " + TO_STR(expValue));
		Text* txtId = (Text*)(itemBtnL->getChildByName("MAT_ID"));
		txtId->setString(TO_STR(nId));

		Layout* pnl = (Layout*)itemBtnL->getChildByName("Pnl_icon");
		pnl->removeAllChildren();
		UIToolHeader* prop = UIToolHeader::create(nId);
		prop->setPosition(Vec2(6, 6));
		prop->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		pnl->addChild(prop);

		CheckBox* checkBox = (CheckBox*)itemBtnL->getChildByName("CheckBox");
		checkBox->setSelectedState(false);
		checkBox->setTouchEnabled(false);

		for(int i=0; i<m_vSeledProp.size(); i++)
		{
			if(idx == m_vSeledProp.at(i).idx && m_vSeledProp.at(i).side == 0)
			{
				checkBox->setSelectedState(true);
				break;
			}
		}
	}

	//右侧道具
	//先查出该经验道具id，思路：该道具前面共有idx*2+1个
	nId = 0;
	curNum = 0;
	for(int i=0; i<m_vRowUpMaterial.size(); i++)
	{
		curNum += m_vRowUpMaterial.at(i).amount;
		if(curNum >= idx*2+2)
		{
			nId = m_vRowUpMaterial.at(i).id;
			break;
		}
	}
	if(nId == 0)
	{
		//没有更多道具了
		if(itemBtnR != nullptr)
		{
			itemBtnR->setVisible(false);
		}

		return;
	}
	if(itemBtnR != nullptr)
	{
		itemBtnR->setVisible(true);
		itemBtnR->setTag(2*idx+1);
		RowData* propData = DataManager::getInstance()->searchToolById(nId);
		string propName = "";
		int expValue = 0;
		if(propData != nullptr)
		{
			propName = propData->getStringData("itemName");
			expValue = propData->getIntData("value");
		}

		Text* txtPropName = (Text*)(itemBtnR->getChildByName("Lab_name"));
		Text* txtPropExp = (Text*)(itemBtnR->getChildByName("Lab_exp"));
		txtPropName->setString(propName);
		txtPropExp->setString(DataManager::getInstance()->searchCommonTexdtById(20119) +": " + TO_STR(expValue));
		Text* txtId = (Text*)(itemBtnR->getChildByName("MAT_ID"));
		txtId->setString(TO_STR(nId));

		Layout* pnl = (Layout*)itemBtnR->getChildByName("Pnl_icon");
		pnl->removeAllChildren();
		UIToolHeader* prop = UIToolHeader::create(nId);
		prop->setPosition(Vec2(6, 6));
		prop->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		pnl->addChild(prop);

		CheckBox* checkBox = (CheckBox*)itemBtnR->getChildByName("CheckBox");
		checkBox->setSelectedState(false);
		checkBox->setTouchEnabled(false);

		for(int i=0; i<m_vSeledProp.size(); i++)
		{
			if(idx == m_vSeledProp.at(i).idx && m_vSeledProp.at(i).side == 1)
			{
				checkBox->setSelectedState(true);
				break;
			}
		}
	}
}

void LayerAddMat::initPropNum()
{
	ModelTactics* pInstance = ModelTactics::getInstance();
	m_iPropTotalNum = 0;
	m_vRowUpMaterial.clear();
	for(int i=0; i<pInstance->m_vRowMaterialCostCoin.size(); i++)
	{
		int nId = pInstance->m_vRowMaterialCostCoin.at(i).id;

		const TOOL_INFO* info = ToolModel::getInstance()->GetToolInfoBytemplateId(nId);
		if(info)
		{
			m_iPropTotalNum += info->nstack;
			sRowUpMaterial matData;
			matData.id = nId;
			matData.amount = info->nstack;
			m_vRowUpMaterial.push_back(matData);
		}
	}

	initSeledProp();
}

void LayerAddMat::onBtnTouchEvent( Button *btn )
{
	btn->setTouchEnabled(false);
	auto _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(false);
	_touchListener->onTouchBegan = [=](Touch* touch, Event* event)->bool {
		Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
		Size size = event->getCurrentTarget()->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);

		Point locationInParent = m_ui.tbViewPnl->convertToNodeSpace(touch->getLocation());
		Rect rectParent = Rect(0, 0, m_ui.tbViewPnl->getContentSize().width,
			m_ui.tbViewPnl->getContentSize().height);

		if (rect.containsPoint(locationInNode) && rectParent.containsPoint(locationInParent) && m_ui.pnlSelExpProp->isVisible()
			&& event->getCurrentTarget() != m_ui.btnClose && event->getCurrentTarget() != m_ui.btnSure)
		{
			SoundUtils::playSoundEffect("dianji");
			btn->loadTextureNormal("Image/UiZhenFa/icon/btn_kuang2.png");
			m_bIsSelect = false;
			return true;
		}
		return false;
	};
	_touchListener->onTouchEnded = [=](Touch* touch, Event* event)->void {
		btn->loadTextureNormal("Image/UiZhenFa/icon/btn_kuang1.png");
		if(m_bIsSelect)
		{
			return;
		}
		else
		{
			Text* txt = (Text*)btn->getChildByName("MAT_ID");
			int nId = atoi(txt->getString().c_str());
			onBtnSelExpProp(btn, nId);
		}
	};
	_touchListener->onTouchMoved = [=](Touch* touch, Event* event)->void {
		if (abs(touch->getDelta().x) > 10.0f || abs(touch->getDelta().y) > 10.0f)
		{
			m_bIsSelect = true;
		}
	};
	_touchListener->onTouchCancelled = [=](Touch* touch, Event* event)->void {
		btn->loadTextureNormal("Image/UiZhenFa/icon/btn_kuang1.png");
	};
	btn->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, btn);
}

void LayerAddMat::onBtnSelExpProp(Button* btn, int nId)
{
	CheckBox* checkBox = (CheckBox*)btn->getChildByName("CheckBox");

	if(m_pSelectedMat->size() > 3 && !checkBox->getSelectedState())
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20127));
		return;
	}

	RowData* propData = DataManager::getInstance()->searchToolById(nId);
	int expValue = 0;
	if(propData != nullptr)
	{
		expValue = propData->getIntData("value");
	}

	//按钮的一些标记
	int tag = btn->getTag();
	int btnIdx = (tag - tag%2)/2;
	int btnSide = tag%2;
	int isDeled = false;
	if(checkBox->getSelectedState())
	{
		if(!m_pSelectedMat->empty())
		{
			m_pSelectedMat->erase(
				std::remove_if(m_pSelectedMat->begin(), m_pSelectedMat->end(), 
				[&](int item)
			{
				if (item == nId && !isDeled)
				{
					checkBox->setSelectedState(false);
					m_ui.txtCurExp->setString(_TO_STR(atoi(m_ui.txtCurExp->getString().c_str()) - expValue));
					isDeled = true;
					return true;
				}
				return false;
			}	
			), m_pSelectedMat->end());
		}	

		//删除选中的标记
		isDeled = false;
		m_vSeledProp.erase(
			std::remove_if(m_vSeledProp.begin(), m_vSeledProp.end(), 
			[&](sSeledProp item)
		{
			if (item.idx == btnIdx && item.side == btnSide && !isDeled)
			{
				isDeled = true;
				return true;
			}
			return false;
		}	
		), m_vSeledProp.end());
	}
	else
	{
		checkBox->setSelectedState(true);
		m_ui.txtCurExp->setString(_TO_STR(atoi(m_ui.txtCurExp->getString().c_str()) + expValue));
		int matId = nId;
		m_pSelectedMat->push_back(matId);

		//增加选中的标记
		sSeledProp prop;
		prop.idx = btnIdx;
		prop.side = btnSide;
		m_vSeledProp.push_back(prop);
	}	
}

void LayerAddMat::initSeledProp()
{
	m_vSeledProp.clear();

	int matId = 0;
	int seledNum = 0;
	for(int i=0; i<m_pSelectedMat->size(); i++)
	{
		int side = -1;
		int curNum = 0;
		if(matId != m_pSelectedMat->at(i))
		{
			seledNum = 0;
			matId = m_pSelectedMat->at(i);
		}

		for(int j=0; j<m_vRowUpMaterial.size(); j++)
		{
			if(m_pSelectedMat->at(i) > m_vRowUpMaterial.at(j).id)
			{
				curNum += m_vRowUpMaterial.at(j).amount;
			}
			else
			{
				continue;
			}
		}

		sSeledProp prop;
		prop.idx = (curNum+seledNum)/2;
		prop.side = (curNum+seledNum)%2;
		m_vSeledProp.push_back(prop);

		seledNum++;
	}
}
