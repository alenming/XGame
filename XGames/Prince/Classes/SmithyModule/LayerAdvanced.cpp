#include "LayerAdvanced.h"
#include "LayerSmithy.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "Widget/ActionCreator.h"

LayerAdvanced::LayerAdvanced(void)
{
}


LayerAdvanced::~LayerAdvanced(void)
{
}

LayerAdvanced* LayerAdvanced::create( Node* parent )
{
	LayerAdvanced* _layer = new LayerAdvanced;
	if (nullptr != _layer && _layer->init(parent))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerAdvanced::init( Node* parent )
{
	if (!Layer::init())
		return false;

	m_parent = parent;
	initUI();
	return true;
}

void LayerAdvanced::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	LayerSmithy* parent = dynamic_cast<LayerSmithy*>(m_parent);
	assert(nullptr != parent);

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIJJGold.ExportJson"));
	m_ui.pRoot->setContentSize(size);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);

	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_JJ_Gold_Bg_1"));
	ActionCreator::runCommDlgAction(m_ui.imgBg);
	SoundUtils::playSoundEffect("tanchuang");
	m_ui.imgContent = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_JJ_Gold_Bg_2"));

	m_ui.txtEquName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Equip_1"));
	m_ui.txtEquName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtEquDstName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Equip_2"));
	m_ui.txtEquDstName->setFontName(FONT_FZZHENGHEI);

	ostringstream oss;
	for (int i = 0; i < 2; ++i)
	{
		// 属性名称;
		oss.str("");
		oss << "Lab_Attr_" << i+1;
		m_ui.txtAttr[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.txtAttr[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.txtAttr[i]->setString("");

		// 当前属性值;
		oss.str("");
		oss << "Lab_Attr_Value_" << i+1;
		m_ui.txtAttrValue[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.txtAttrValue[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.txtAttrValue[i]->setString("");

		// 属性增量预览;
		oss.str("");
		oss << "Lab_Attr_Add_" << i+1;
		m_ui.txtAttrAdd[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.txtAttrAdd[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.txtAttrAdd[i]->setString("");
	}

	// 消耗金币;
	m_ui.txtGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Number"));
	m_ui.txtGold->setFontName(FONT_FZZHENGHEI);
	m_ui.txtGold->setString("");

	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Close"));
	m_ui.btnClose->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			getScheduler()->schedule([=](float dt)->void {
				this->getScheduler()->unschedule("LayerAdvanced::initUI", this);
				this->removeFromParent();
			},
				this, 0, 0, 0, false, "LayerAdvanced::initUI");
		}
	});

	m_ui.btnJJ = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_JJ"));
	m_ui.btnJJ->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::ENDED == type)
		{
			if (MainModel::getInstance()->getMainParam()->mGold < parent->m_paramJJ.nCostGold)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10007));
			}
			else
			{
				parent->m_pController->handleJJ(parent->m_nCurSelToolId, parent->m_paramJJ.nCostGold);
				getScheduler()->schedule([this](float dt)->void {
					this->getScheduler()->unschedule("LayerAdvanced::initUI", this);
					this->removeFromParent();
				},
					this, 0, 0, 0, false, "LayerAdvanced::initUI");
			}
		}
	});


	///////////////////////////////// 直接加载数据; /////////////////////////////////////////

	// 刷新UI;
	Node* _pre = m_ui.imgContent->getChildByName("tag_JJ_Equ_Icon_1");
	if (_pre != nullptr)
	{
		_pre->removeFromParent();
		_pre = nullptr;
	}
	int nTemplateId = ToolModel::getInstance()->GetToolInfo(parent->m_paramJJ.nEquId)->ntemplateId;
	_pre = UIToolHeader::create(ResourceUtils::getSmallIconPath(parent->m_paramJJ.nEquIcon));
	((UIToolHeader*)_pre)->setToolId(nTemplateId);
	((UIToolHeader*)_pre)->setIconQua(parent->m_paramJJ.nEquQua);
	RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(nTemplateId);
	CCASSERT(pEquipment != nullptr, "");
	((UIToolHeader*)_pre)->setJJLvl(parent->m_paramJJ.nEquQua, pEquipment->getIntData("stars"));
	((UIToolHeader*)_pre)->setLvlWithoutBg(parent->m_paramJJ.nEquAdvLv);
	_pre->setPosition(Vec2(170.0f, 400.0f));
	_pre->setName("tag_JJ_Equ_Icon_1");
	m_ui.imgContent->addChild(_pre, 2);
	m_ui.txtEquName->setString(parent->m_paramJJ.strEquName);
	m_ui.txtEquName->setColor(parent->getEquipQuaColor(parent->m_paramJJ.nEquQua));

	Node* _dst = m_ui.imgContent->getChildByName("tag_JJ_Equ_Icon_2");
	if (_dst != nullptr)
	{
		_dst->removeFromParent();
		_dst = nullptr;
	}
	_dst = UIToolHeader::create(ResourceUtils::getSmallIconPath(parent->m_paramJJ.nEquDstIcon));
	((UIToolHeader*)_dst)->setToolId(parent->m_paramJJ.nEquDstTemplateId);
	((UIToolHeader*)_dst)->setIconQua(parent->m_paramJJ.nEquDstQua);
	pEquipment = DataManager::getInstance()->searchEquipmentById(parent->m_paramJJ.nEquDstTemplateId);
	CCASSERT(pEquipment != nullptr, "");
	((UIToolHeader*)_dst)->setJJLvl(parent->m_paramJJ.nEquDstQua, pEquipment->getIntData("stars"));
	((UIToolHeader*)_dst)->setLvlWithoutBg(parent->m_paramJJ.nEquDstAdvLv);
	_dst->setPosition(Vec2(410.0f, 400.0f));
	_dst->setName("tag_JJ_Equ_Icon_2");
	m_ui.imgContent->addChild(_dst, 2);
	m_ui.txtEquDstName->setString(parent->m_paramJJ.strEquDstName);
	m_ui.txtEquDstName->setColor(parent->getEquipQuaColor(parent->m_paramJJ.nEquDstQua));

	//assert(parent->m_paramJJ.vcBase.size() == 1);
	oss.str("");
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (parent->m_paramJJ.vcBase.size() > i)
		{
			m_ui.txtAttr[i]->setString(parent->m_paramJJ.vcBase.at(i).strAttrName);

			char szNum1[32], szNum2[32];
			memset(&szNum1, 0, 32);
			memset(&szNum2, 0, 32);
			if (parent->m_paramJJ.vcBase.at(i).attrType >= ATTR_TYPE_HP_COE)
			{
				sprintf(szNum1, "%0.2lf%%", parent->m_paramJJ.vcBase.at(i).fAttrValue * 100.0f);
				sprintf(szNum2, "%0.2lf%%", parent->m_paramJJ.vcBase.at(i).fAttrAdd * 100.0f);
			}
			else
			{
				sprintf(szNum1, "%d", (int)(parent->m_paramJJ.vcBase.at(i).fAttrValue));
				sprintf(szNum2, "%d", (int)(parent->m_paramJJ.vcBase.at(i).fAttrAdd));
			}

			m_ui.txtAttrValue[i]->setString(szNum1);//_TO_STR(param.vcBase.at(i).fAttrValue));

			oss.str("");
			if (parent->m_paramJJ.vcBase.at(i).fAttrAdd > 0.000001f)
			{
				oss << "+";
			}

			oss << szNum2;
			m_ui.txtAttrAdd[i]->setString(oss.str());
			m_ui.txtAttrAdd[i]->setVisible(!parent->m_paramJJ.bFinal);
			m_ui.txtAttrAdd[i]->setOpacity(255);

			// 长度适配;
			m_ui.txtAttrValue[i]->setPosition(Point(m_ui.txtAttr[i]->getContentSize().width + 10.0f, 0.0f)
				+ m_ui.txtAttr[i]->getPosition());
			m_ui.txtAttrAdd[i]->setPosition(Point(m_ui.txtAttrValue[i]->getContentSize().width + 12.0f, 0.0f)
				+ m_ui.txtAttrValue[i]->getPosition());
		}
	}

	m_ui.txtGold->setString(_TO_STR(parent->m_paramJJ.nCostGold));
}
