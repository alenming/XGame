#include "LayerAddMaterial.h"
#include "SmithyModel.h"
#include "ToolModule/ToolModel.h"
#include "Utils/SoundUtils.h"

LayerAddMaterial::LayerAddMaterial(void)
	: _callback(nullptr)
	, m_nSelectedCount(0)
	, m_MaterialCellSize(Size::ZERO)
	, m_nodeParent(nullptr)
{
	m_vcSelectedMaterials.clear();
	m_vcAllMaterials.clear();
}


LayerAddMaterial::~LayerAddMaterial(void)
{
}

LayerAddMaterial* LayerAddMaterial::create( int nToolId, AddMaterialCallback callback, vector<SelectedMaterialParam> vcSelectedMaterials )
{
	LayerAddMaterial* _layer = new LayerAddMaterial;
	if (nullptr != _layer && _layer->init(nToolId, callback, vcSelectedMaterials))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerAddMaterial::init( int nToolId, AddMaterialCallback callback, vector<SelectedMaterialParam> vcSelectedMaterials )
{
	if (!Layer::init())
		return false;

	m_nEquToolId = nToolId;
	_callback = callback;
	m_vcSelectedMaterials = vcSelectedMaterials;
	m_nSelectedCount = m_vcSelectedMaterials.size();
	initUI();
	return true;
}

void LayerAddMaterial::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIAddMaterial.ExportJson"));
	m_ui.pRoot->setContentSize(size);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);

	// Bg;
	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Res_Bg"));

	m_ui.txtGetExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Exp"));
	m_ui.txtGetExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtGetExp->setString("");
	m_ui.txtNeedExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Label_68"));
	m_ui.txtNeedExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtNeedExp->setString("");

	// data;
	vector<const TOOL_INFO*> vcAll = SmithyModel::getInstance()->getQHMaterial_HorseBook(m_nEquToolId, 99);
	for (int i = 0; i < vcAll.size(); ++i)
	{
		SelectedMaterialParam smp;
		smp.toolInfo = vcAll.at(i);
		smp.nExp = DataManager::getInstance()->searchToolById(smp.toolInfo->ntemplateId)->getIntData("value");;
		smp.nLogicOrder = i;
		m_vcAllMaterials.push_back(smp);
	}
	QH_Param qhParam;
	SmithyModel::getInstance()->getQHParam(m_nEquToolId, qhParam);
	m_ui.txtNeedExp->setString(_TO_STR(qhParam.nMaxExp - qhParam.nExp));
	updateGetExp();

	// node size;
	Layout* pnl = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIMaterial.ExportJson"));
	if (pnl != nullptr)
	{
		m_MaterialCellSize = Size(pnl->getContentSize().width * 2.0f, pnl->getContentSize().height);
	}
	m_nodeParent = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Res_Bg_1"));

	// Content
	m_ui.tableView = extension::TableView::create(this, Size(710.0f, 435.0f), nullptr);
	m_ui.tableView->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
	m_ui.tableView->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	m_ui.tableView->reloadData();
	m_ui.tableView->setDelegate(this);
	m_ui.tableView->setPosition(Vec2(60.0f, 125.0f));
	m_ui.imgBg->addChild(m_ui.tableView);

	// Button;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Close"));
	m_ui.btnOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Confirm"));
	auto responder = [=](Ref* pSender, Widget::TouchEventType type)->void {
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			if (_callback)
				_callback(m_vcSelectedMaterials, true);

			getScheduler()->schedule([this](float dt)->void {
				this->getScheduler()->unschedule("LayerAddMaterial::initUI", this);
				this->removeFromParent();
			},
				this, 0, 0, 0, false, "LayerAddMaterial::initUI");
		}
	};
	m_ui.btnOK->addTouchEventListener(responder);
	m_ui.btnClose->addTouchEventListener(responder);
	m_ui.btnOK->setLocalZOrder(m_ui.btnOK->getLocalZOrder()+1);
}

bool LayerAddMaterial::onMaterialSelected( SelectedMaterialParam smp, MaterialNodeSelectEvent event )
{
	if (event == MNSE_SELECTED)
	{
		if (m_nSelectedCount >= 4)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20170));
			return false;
		}

		m_vcSelectedMaterials.push_back(smp);
		m_nSelectedCount++;
		updateGetExp();
		return true;
	}
	else if (event == MNSE_UNSELECTED)
	{
		CCASSERT(m_vcSelectedMaterials.size() == m_nSelectedCount, "");

		// 容错，这里不可能为空;
		if (m_vcSelectedMaterials.size() > 0)
		{
			auto it = find_if(m_vcSelectedMaterials.begin(), m_vcSelectedMaterials.end(),
				[=](SelectedMaterialParam _param)->bool {
					return _param == smp;
			});
			assert(it != m_vcSelectedMaterials.end());
			m_vcSelectedMaterials.erase(it);
			m_nSelectedCount--;
			updateGetExp();
		}
		return true;
	}
}

void LayerAddMaterial::tableCellTouched( extension::TableView* table, extension::TableViewCell* cell )
{
	//
}

extension::TableViewCell* LayerAddMaterial::tableCellAtIndex( extension::TableView *table, ssize_t idx )
{
	extension::TableViewCell* cell = table->dequeueCell();
	ostringstream oss;
	if(!cell)
	{
		// 每个cell放置一个Layout，其中承载2个材料节点;
		cell = extension::TableViewCell::create();
		Layout* pnl = Layout::create();
		pnl->setContentSize(m_MaterialCellSize);
		pnl->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		log("++++++++++++++++ create cell at idx = %d", idx);

		for (int i = 0; i < 2; ++i)
		{
			// 实际上这里拿到的idx是第几排，而不是第几个;
			// 要从第几排推算出本排放置的是第几个，即原始索引号;
			// 与numberOfCellsInTableView方法中设置的排数相对应;
			int oriIndex = idx*2+i;
			MaterialNodeParam param;
			if (oriIndex >= m_vcAllMaterials.size())
			{
				// 创建参数模板;
				param.widgetLogicParent = m_nodeParent;
				SelectedMaterialParam smp;
				param.smParam = smp;
				param.nExp = 0;
				param.smParam.nExp = param.nExp;
				param.strName = "";
				param.bSelected = false;
			}
			else
			{
				// 创建参数;
				param.widgetLogicParent = m_nodeParent;
				param.smParam = m_vcAllMaterials.at(oriIndex);
				param.nExp = DataManager::getInstance()->searchToolById(param.smParam.toolInfo->ntemplateId)->getIntData("value");
				param.smParam.nExp = param.nExp;
				param.strName = m_vcAllMaterials.at(oriIndex).toolInfo->strName;
				auto iter = find_if(m_vcSelectedMaterials.begin(), m_vcSelectedMaterials.end(), [=](SelectedMaterialParam _smp)->bool {
					return _smp == param.smParam;
				});
				param.bSelected = (iter != m_vcSelectedMaterials.end());
			}

			// 创建节点模板;
			MaterialNode* _material = MaterialNode::create(param, CC_CALLBACK_2(LayerAddMaterial::onMaterialSelected, this));
			_material->setPosition(Vec2(i*_material->getContentSize().width, 0.0f));
			oss.str("");
			oss << "material_fixed_idex_" << i;
			_material->setName(oss.str());
			_material->setVisible(oriIndex < m_vcAllMaterials.size());
			pnl->addChild(_material);
		}

		pnl->setName("pnl_fixed_name");
		cell->addChild(pnl);
	}
	else
	{
		log("++++++++++++++++ reload cell at idx = %d", idx);
		Layout* pnl = dynamic_cast<Layout*>(cell->getChildByName("pnl_fixed_name"));
		if (nullptr != pnl)
		{
			for (int i = 0; i < 2; ++i)
			{
				int oriIndex = idx*2+i;
				CC_BREAK_IF(oriIndex >= m_vcAllMaterials.size());
				log("++++++++++++++++++++ oriIndex = %d", oriIndex);

				MaterialNodeParam param;
				param.widgetLogicParent = m_nodeParent;
				param.smParam = m_vcAllMaterials.at(oriIndex);
				param.nExp = DataManager::getInstance()->searchToolById(param.smParam.toolInfo->ntemplateId)->getIntData("value");
				param.smParam.nExp = param.nExp;
				param.strName = m_vcAllMaterials.at(oriIndex).toolInfo->strName;
				auto iter = find_if(m_vcSelectedMaterials.begin(), m_vcSelectedMaterials.end(), [=](SelectedMaterialParam _smp)->bool {
					return _smp == param.smParam;
				});
				param.bSelected = (iter != m_vcSelectedMaterials.end());

				oss.str("");
				oss << "material_fixed_idex_" << i;
				MaterialNode* _material = dynamic_cast<MaterialNode*>(pnl->getChildByName(oss.str()));
				if (nullptr != _material)
				{
					_material->updateParam(param);
					_material->setVisible(oriIndex < m_vcAllMaterials.size());
				}
			}
		}
	}
	return cell;
}

ssize_t LayerAddMaterial::numberOfCellsInTableView( extension::TableView *table )
{
	return (m_vcAllMaterials.size()%2 == 0) ? m_vcAllMaterials.size()/2 : m_vcAllMaterials.size()/2+1;
}

cocos2d::Size LayerAddMaterial::cellSizeForTable( extension::TableView *table )
{
	return m_MaterialCellSize;

	// 重复加载json文件会导致明显卡顿;
	/*Layout* pnl = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIMaterial.ExportJson"));
	if (pnl != nullptr)
	{
		return Size(pnl->getContentSize().width * 2.0f, pnl->getContentSize().height);
	}
	return Size::ZERO;*/
}

void LayerAddMaterial::updateGetExp()
{
	int total = 0;
	for_each(m_vcSelectedMaterials.begin(), m_vcSelectedMaterials.end(), [=, &total](SelectedMaterialParam smp){
		total += smp.nExp;
	});
	m_ui.txtGetExp->setString(_TO_STR(total));
}
