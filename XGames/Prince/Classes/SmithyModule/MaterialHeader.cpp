#include "MaterialHeader.h"


MaterialHeader::MaterialHeader(void)
	: _callback(nullptr)
	, m_imgFrame(nullptr)
	, m_btnAdd(nullptr)
	, m_Material(nullptr)
	, m_btnDelete(nullptr)
{
	m_param.clear();
}


MaterialHeader::~MaterialHeader(void)
{
}

MaterialHeader* MaterialHeader::create( MaterialClickCallback callback, SelectedMaterialParam param )
{
	MaterialHeader* _header = new MaterialHeader;
	if (nullptr != _header && _header->init(callback, param))
	{
		_header->autorelease();
		return _header;
	}

	CC_SAFE_DELETE(_header);
	return nullptr;
}

bool MaterialHeader::init( MaterialClickCallback callback, SelectedMaterialParam param )
{
	if (!Node::init())
	{
		return false;
	}
	_callback = callback;
	m_param = param;

	initUI();
	return true;
}

void MaterialHeader::initUI()
{
	// 分两种，初始化有物品和无物品;
	// 总是先初始化为空的情况;
	m_btnAdd = Button::create("Image/Icon/global/btn_kuang_zhuangbei_jia.png");
	m_btnAdd->setScale(0.9f);
	this->addChild(m_btnAdd, 1);
	m_imgFrame = ImageView::create("Image/Icon/global/btn_kuang_zhuangbei_1.png");
	m_imgFrame->setScale(0.85f);
	m_imgFrame->setPosition(Vec2(m_btnAdd->getContentSize().width/2.0f, m_btnAdd->getContentSize().height/2.0f));
	m_btnAdd->addChild(m_imgFrame);

	m_btnAdd->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			m_imgFrame->setScale(0.95f);
		}
		else if (Widget::TouchEventType::CANCELED == type)
		{
			m_imgFrame->setScale(0.85f);
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_imgFrame->setScale(0.85f);
			if (_callback != nullptr)
				_callback(MCE_ADD);
		}
	});

	// 然后有的话再添加物品;
	if (!m_param.empty())
		updateMaterial(m_param);
}

void MaterialHeader::updateMaterial( SelectedMaterialParam param )
{
	// 无->有;
	if (m_param.empty() && !param.empty())
	{
		m_param = param;

		m_Material = UIToolHeader::create(m_param.toolInfo->ntemplateId);
		m_Material->setScale(0.8f);
		this->addChild(m_Material, 2);

		if (m_btnAdd != nullptr)
			m_btnAdd->setTouchEnabled(false);

		if (m_btnDelete == nullptr)
		{
			m_btnDelete = Button::create("Image/Icon/global/btn_close2_1.png", "Image/Icon/global/btn_close2_2.png");
			m_btnDelete->setPosition(Vec2(m_btnAdd->getContentSize().width/2.0f - 10.0f, m_btnAdd->getContentSize().height/2.0f - 10.0f));
			this->addChild(m_btnDelete, 4);
		}
		m_btnDelete->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
			if (Widget::TouchEventType::ENDED == type)
				removeMaterial();
		});
	}
	// 有->替换;
	else if (!m_param.empty() && !param.empty())
	{
		m_param = param;

		if (m_Material != nullptr)
		{
			m_Material->removeFromParent();
			m_Material = nullptr;
		}
		m_Material = UIToolHeader::create(m_param.toolInfo->ntemplateId);
		this->addChild(m_Material, 2);
	}
	// 有->无;
	else if (!m_param.empty() && param.empty())
	{
		removeMaterial();
	}
}

void MaterialHeader::removeMaterial()
{
	m_param.clear();
	if (_callback != nullptr)
		_callback(MCE_DEL);

	if (m_Material != nullptr)
	{
		m_Material->removeFromParent();
		m_Material = nullptr;
	}

	if (m_btnAdd != nullptr)
		m_btnAdd->setTouchEnabled(true);


	getScheduler()->schedule([this](float dt)->void {
		if (nullptr != m_btnDelete)
		{
			m_btnDelete->removeFromParent();
			m_btnDelete = nullptr;
		}
		this->getScheduler()->unschedule("MaterialHeader::removeMaterial", this);
	},
		this, 0, 0, 0, false, "MaterialHeader::removeMaterial");
}

void MaterialHeader::lock( bool block /*= true*/ )
{
	// 有材料时;
	if (m_Material != nullptr && m_btnDelete != nullptr)
	{
		m_btnDelete->setTouchEnabled(!block);
	}
	// 无材料时;
	else if (m_Material == nullptr && m_btnAdd != nullptr)
	{
		m_btnAdd->setTouchEnabled(!block);
	}
}

void MaterialHeader::customFadeIn()
{
	if (m_Material != nullptr)
	{
		m_Material->setOpacity(255);
		m_Material->runAction(EaseSineOut::create(FadeOut::create(0.2f)));
	}
}

void MaterialHeader::customFadeOut()
{
	if (m_Material != nullptr)
	{
		m_Material->setOpacity(0);
		m_Material->runAction(EaseSineIn::create(FadeIn::create(0.2f)));
	}
}
