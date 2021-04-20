#include "ShowFightPowerAction.h"
#include "math.h"
#include "Utils/StringFormat.h"
#include "Common/Common.h"
#include "UI/UIDef.h"

#define NUM_FONTSIZE	30.0f
#define TIME_SCROLL		0.5f

ShowFightPowerAction::ShowFightPowerAction()
{
	m_actionBg = nullptr;
	m_vLabNum.clear();
	m_vCurNum.clear();
	m_vGoalNum.clear();
	m_txtDvalue = nullptr;
	m_fightPowerNode = nullptr;
	m_length = 0;
	m_scrollTime = 0.0f;
}

ShowFightPowerAction::~ShowFightPowerAction()
{

}

ShowFightPowerAction* ShowFightPowerAction::create(int fightPower1, int fightPower2)
{
	if(fightPower1 == fightPower2)
	{
		return nullptr;
	}

	//背景设置
	ShowFightPowerAction* _fightPowerAction = new ShowFightPowerAction;
	if (_fightPowerAction && _fightPowerAction->init())
	{
		_fightPowerAction->autorelease();
		_fightPowerAction->initUI(fightPower1, fightPower2);
		return _fightPowerAction;
	}

	CC_SAFE_DELETE(_fightPowerAction);
	return nullptr;
}

void ShowFightPowerAction::initUI(int fightPower1, int fightPower2)
{
	int dValue = fightPower2 - fightPower1;
	initActionBg(dValue);
	
	m_length = getNumLength(fightPower1 > fightPower2 ? fightPower1 : fightPower2);
	setNumber(fightPower1, false);
	setNumber(fightPower2, true);

	initNumUI(fightPower1, fightPower2);

	m_dValueNode->setPosition(m_fightPowerNode->getPositionX() + m_fightPowerNode->getContentSize().width +
		m_dValueNode->getContentSize().width/2	+ 10, m_fightPowerNode->getPositionY());

	this->schedule(schedule_selector(ShowFightPowerAction::updateNumber), 0.05);
}

void ShowFightPowerAction::initActionBg(int dValue)
{
	m_actionBg = Node::create();
	Sprite* spriteBg = Sprite::create("Image/Icon/global/img_zhandouli2.png");
	m_actionBg->setContentSize(spriteBg->getContentSize());
	spriteBg->setPosition(m_actionBg->getContentSize()/2);
	spriteBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_actionBg->addChild(spriteBg);

	m_dValueNode = Node::create();
	m_dValueNode->setContentSize(Size(200, m_actionBg->getContentSize().height));

	Sprite* spriteUp = nullptr;
	m_txtDvalue = Text::create();
	m_txtDvalue->setString(_TO_STR(abs(dValue)));
	m_txtDvalue->setFontName(FONT_FZZHENGHEI);
	m_txtDvalue->setFontSize(NUM_FONTSIZE);
	if(dValue > 0)
	{
		spriteUp = Sprite::create("Image/Icon/global/img_up.png");
		m_txtDvalue->setColor(Color3B(0x56, 0xff, 0x3f));
		setTextAddStroke(m_txtDvalue, Color3B(0x2b, 0x4b, 0x13), 3);
	}
	else
	{
		spriteUp = Sprite::create("Image/Icon/global/img_down.png");
		m_txtDvalue->setColor(Color3B(0xff, 0x37, 0x37));
		setTextAddStroke(m_txtDvalue, Color3B(0x5a, 0x23, 0x19), 3);
	}
	spriteUp->setPosition(Point(10, m_dValueNode->getContentSize().height/2));
	spriteUp->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

	m_txtDvalue->setPosition(spriteUp->getPosition() + Point(spriteUp->getContentSize().width/2 + 10, 0));
	m_txtDvalue->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

	m_dValueNode->addChild(spriteUp);
	m_dValueNode->addChild(m_txtDvalue);

	m_dValueNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_actionBg->addChild(m_dValueNode);

	this->addChild(m_actionBg);
}

void ShowFightPowerAction::initNumUI(int fightPower1, int fightPower2)
{
	//排好length列数字
	m_fightPowerNode = Node::create();
	char str[10] = {0};
	for(int i = 0; i < m_length; i++)
	{	
		sprintf(str, "%d", m_vCurNum.at(i));
		Text* temp = Text::create();
		temp->setFontName(FONT_FZZHENGHEI);
		temp->setFontSize(NUM_FONTSIZE);
		temp->setAnchorPoint(Point(0,0.5));
		temp->setPosition(Point(5+i * (NUM_FONTSIZE-7), 0));
		temp->setColor(Color3B(0xe8, 0xde, 0xb5));
		temp->setString(_TO_STR(m_vCurNum.at(i)));
		m_vLabNum.push_back(temp);
		m_fightPowerNode->addChild(temp);
	}

	m_fightPowerNode->setPosition(m_actionBg->getContentSize()/2);
	m_fightPowerNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_actionBg->addChild(m_fightPowerNode, 1);
}

int ShowFightPowerAction::getNumLength(int fightPower)
{
	int length = 0;
	while(fightPower)
	{
		length++;
		fightPower /= 10;
	}
	return length;
}

void ShowFightPowerAction::setNumber(int number, bool isGoalNumber)
{
	int length = getNumLength(number);
	//先判断战力跨位数的情况
	int gridZero = m_length - length;
	while (gridZero >= 0)
	{
		if(!isGoalNumber)
		{
			m_vCurNum.push_back(0);
		}
		else
		{
			m_vGoalNum.push_back(0);
		}
		gridZero--;
	}
	for(int i=0; i<length; i++)
	{
		int num = number / pow(10, length - i - 1);	
		if(!isGoalNumber)
		{
			m_vCurNum.push_back(num);				
		}
		else
		{
			m_vGoalNum.push_back(num);
		}
		number -= num*pow(10, length - i - 1);
	}
}

void ShowFightPowerAction::updateNumber(float dt)
{
	for(int i=0; i<m_vLabNum.size(); i++)
	{
		int num = atoi(m_vLabNum.at(i)->getString().c_str());
		if(++num > 9)
		{
			num = 0;
		}
		m_vLabNum.at(i)->setString(_TO_STR(num));
	}
	m_scrollTime += dt;
	if(m_scrollTime > TIME_SCROLL)
	{
		for(int i=0; i<m_vLabNum.size(); i++)
		{
			m_vLabNum.at(i)->setString(_TO_STR(m_vGoalNum.at(i)));
			setTextAddStroke(m_vLabNum.at(i), Color3B(0x7d, 0x3f, 0x1c), 3);
		}
		numberDoAction();

		this->unschedule(schedule_selector(ShowFightPowerAction::updateNumber));
	}
}

void ShowFightPowerAction::numberDoAction()
{
	Sequence* act = Sequence::create(
		EaseSineOut::create(ScaleTo::create(0.2f, 2.0f)),
		EaseSineIn::create(ScaleTo::create(0.2f, 1.0f)),
		CallFuncN::create([=](Node* pSender){
			moveOut();
	}), nullptr);

	m_fightPowerNode->runAction(act);
}

void ShowFightPowerAction::moveOut()
{
	Sequence* act = Sequence::create(
		Spawn::create(
		FadeOut::create(1.0f),
		MoveBy::create(1.0f, Point(0, 100)), nullptr),	
		CallFuncN::create([=](Node* pSender){
			this->removeFromParentAndCleanup(true);
	}), nullptr);

	this->runAction(act);
}