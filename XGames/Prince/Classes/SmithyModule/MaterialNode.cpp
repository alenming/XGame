#include "MaterialNode.h"
#include "Utils/SoundUtils.h"

MaterialNode::MaterialNode(void)
	: m_bCancelSelected(false)
{
	m_param.clear();
}


MaterialNode::~MaterialNode(void)
{
}

MaterialNode* MaterialNode::create( MaterialNodeParam param, MaterialNodeSelectCallback callback )
{
	MaterialNode* _node = new MaterialNode;
	if (nullptr != _node && _node->init(param, callback))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool MaterialNode::init( MaterialNodeParam param, MaterialNodeSelectCallback callback )
{
	if (!Node::init())
		return false;

	m_param = param;
	_callback = callback;
	initUI();
	return true;
}

void MaterialNode::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIMaterial.ExportJson"));
	this->addChild(m_ui.pRoot);
	this->setContentSize(m_ui.pRoot->getContentSize());
	this->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	m_ui.btnNode = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Res"));
	m_ui.btnNode->setTouchEnabled(false);
	auto _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(false);
	_touchListener->onTouchBegan = [=](Touch* touch, Event* event)->bool {
		Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
		Size size = event->getCurrentTarget()->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);

		Point locationInParent = m_param.widgetLogicParent->convertToNodeSpace(touch->getLocation());
		Rect rectParent = Rect(40.0f, 40.0f, m_param.widgetLogicParent->getContentSize().width - 80.0f,
			m_param.widgetLogicParent->getContentSize().height - 80.0f);
		
		if (rect.containsPoint(locationInNode) && rectParent.containsPoint(locationInParent))
		{
			SoundUtils::playSoundEffect("dianji");
			m_ui.btnNode->loadTextureNormal("Image/UIEquip/icon/btn_kuang2.png");
			m_bCancelSelected = false;
			return true;
		}
		return false;
	};
	_touchListener->onTouchEnded = [=](Touch* touch, Event* event)->void {
		m_ui.btnNode->loadTextureNormal("Image/UIEquip/icon/btn_kuang1.png");
		if (m_bCancelSelected)
			return;
		
		if (_callback)
		{
			bool bRet = _callback(m_param.smParam,
				!m_ui.cbState->getSelectedState() ? MaterialNodeSelectEvent::MNSE_SELECTED : MaterialNodeSelectEvent::MNSE_UNSELECTED);

			if (bRet)
			{
				m_ui.cbState->setSelectedState(!m_ui.cbState->getSelectedState());
			}
		}
	};
	_touchListener->onTouchMoved = [=](Touch* touch, Event* event)->void {
		if (abs(touch->getDelta().x) > 10.0f || abs(touch->getDelta().y) > 10.0f)
		{
			m_bCancelSelected = true;
		}
	};
	_touchListener->onTouchCancelled = [=](Touch* touch, Event* event)->void {
		m_ui.btnNode->loadTextureNormal("Image/UIEquip/icon/btn_kuang1.png");
	};
	m_ui.btnNode->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, m_ui.btnNode);

	m_ui.item = UIToolHeader::create(m_param.smParam.empty() ? -1 : m_param.smParam.toolInfo->ntemplateId);
	if (nullptr != m_ui.item)
	{
		m_ui.item->setTouchEnabled(false);
		m_ui.item->setScale(0.8f);
		m_ui.item->setPosition(Vec2(70.0f, m_ui.btnNode->getContentSize().height/2.0f));
		m_ui.btnNode->addChild(m_ui.item);
	}

	m_ui.txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Name"));
	m_ui.txtName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtName->setString(m_param.strName);
	m_ui.txtExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Exp"));
	m_ui.txtExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtExp->setString(_TO_STR(m_param.nExp));
	m_ui.cbState = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRoot, "CheckBox_Select"));
	m_ui.cbState->setSelectedState(m_param.bSelected);
	m_ui.cbState->setTouchEnabled(false);
	/*m_ui.cbState->addEventListener([=](Ref* pSender, CheckBox::EventType type){
		if (_callback)
		{
			_callback(m_param.smParam,
				(type == CheckBox::EventType::SELECTED) ? MaterialNodeSelectEvent::MNSE_SELECTED : MaterialNodeSelectEvent::MNSE_UNSELECTED);
		}
	});*/

}

void MaterialNode::updateParam( MaterialNodeParam param )
{
	m_param = param;

	if (m_ui.item != nullptr)
	{
		m_ui.item->removeFromParent();
	}

	m_ui.item = UIToolHeader::create(m_param.smParam.toolInfo->ntemplateId);
	m_ui.item->setTouchEnabled(false);
	m_ui.item->setScale(0.8f);
	m_ui.item->setPosition(Vec2(70.0f, m_ui.btnNode->getContentSize().height/2.0f));
	m_ui.btnNode->addChild(m_ui.item);

	if (m_ui.txtName != nullptr)
	{
		m_ui.txtName->setString(m_param.strName);
	}
	if (m_ui.txtExp != nullptr)
	{
		m_ui.txtExp->setString(_TO_STR(m_param.nExp));
	}

	m_ui.cbState->setSelectedState(m_param.bSelected);
}
