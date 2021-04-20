#include "RollNum.h"

#define NUMBERHEIGHT    130
#define NUMBERWIDTH     95
#define TEXTUREHEIGHT   1300
#define MOVEPXLMAX      50

RollNum::RollNum()
{
	m_nNumber = 0;
	m_nCurTexH = 0.0f;
	m_nEndTexH = 0.0f;
	m_imgOne = nullptr;
	m_imgTwo = nullptr;
	m_oneY = 0.0f;
	m_twoY = 0.0f;
	m_moveUp = false;
	m_moveState = MOVE_STATE_ADD;
	m_movePxl = 0.0f;
	m_canNotify = false;
}

RollNum::~RollNum()
{
	
}

bool RollNum::init()
{
	if (!Layout::init())
	{
		return false;
	}
	//��������ü�
	setContentSize(Size(NUMBERWIDTH,NUMBERHEIGHT));
	setClippingEnabled(true);

	//����ͼƬ����ʹ��
	m_imgOne = ImageView::create("Image/Icon/global/img_number2.png");
	m_imgTwo = ImageView::create("Image/Icon/global/img_number2.png");
	addChild(m_imgOne);
	addChild(m_imgTwo);

	m_imgOne->setAnchorPoint(Vec2(0,0));
	m_imgTwo->setAnchorPoint(Vec2(0,0));
	m_oneY = 0.0f;
	m_twoY = m_imgOne->getContentSize().height;
	m_imgOne->setPosition(Vec2(0,m_oneY));
	m_imgTwo->setPosition(Vec2(0,m_twoY));
	return true;
}

void RollNum::updateNumber(float dt)
{
	//ÿ�����ϻ����¹����ٶȲ��ϱ仯
	if (m_moveState == MOVE_STATE_ADD)
	{
		if (m_movePxl < MOVEPXLMAX)
		{
			m_movePxl += 0.25f;
		}
	}
	else if (m_moveState == MOVE_STATE_PLUS)
	{
		if (m_movePxl > 15.0f)
		{
			m_movePxl -= 0.25f;
		}
		else
		{
			if (setArrive() && m_canNotify)//��ⷶΧ�ﵽ�����������ƶ���Ŀ������
			{
				m_moveState = MOVE_STATE_ARRIVE;
				return;
			}
		}
	}
	else if (m_moveState == MOVE_STATE_ARRIVE)
	{
		if (m_nEndTexH <=  0)
		{
			return;
		}
		else
		{
			if (m_nEndTexH < NUMBERHEIGHT && m_canNotify)
			{
				m_canNotify = false;
				auto call = []()->void
				{
					CCNotificationCenter::getInstance()->postNotification("test",NULL);
				};
				m_imgOne->runAction(CCSequence::create(CCDelayTime::create(0.2f),CCCallFunc::create(call),nullptr));
			}

			m_movePxl = (m_movePxl < m_nEndTexH/2) ? m_movePxl:m_nEndTexH/2;
			if (m_nEndTexH >= m_movePxl)
			{
				m_nEndTexH -= m_movePxl;
			}
			else
			{
				m_movePxl = m_nEndTexH;
				m_nEndTexH = 0;
			}
		}
	}

	///////////////////////�ƶ�/////////////////////////////
	if (m_moveUp)
	{
		m_oneY += m_movePxl;
		m_imgOne->setPosition(Vec2(0,m_oneY));

		m_twoY += m_movePxl;
		m_imgTwo->setPosition(Vec2(0,m_twoY));

		if (m_oneY >= m_imgTwo->getContentSize().height)
		{
			m_oneY = m_twoY-m_imgOne->getContentSize().height;
			m_imgOne->setPosition(Vec2(0,m_oneY));
		}

		if (m_twoY >= m_imgOne->getContentSize().height)
		{
			m_twoY = m_oneY-m_imgTwo->getContentSize().height;
			m_imgTwo->setPosition(Vec2(0,m_twoY));
		}
	}
	else
	{
		m_oneY -= m_movePxl;
		m_imgOne->setPosition(Vec2(0,m_oneY));

		m_twoY -= m_movePxl;
		m_imgTwo->setPosition(Vec2(0,m_twoY));

		if (m_oneY <= -m_imgOne->getContentSize().height)
		{
			m_oneY = m_twoY + m_imgTwo->getContentSize().height;
			m_imgOne->setPosition(Vec2(0,m_oneY));
		}

		if (m_twoY <= -m_imgTwo->getContentSize().height)
		{
			m_twoY = m_oneY + m_imgOne->getContentSize().height;
			m_imgTwo->setPosition(Vec2(0,m_twoY));
		}
	}
}

void RollNum::setNumber(int var,bool roll)
{   
	m_nNumber = var;
	//���㵱ǰλ���������������λ��
	m_nCurTexH = m_nNumber*NUMBERHEIGHT;

	if (roll)
	{
		m_oneY = -m_nCurTexH;
		m_imgOne->setPosition(Vec2(0,m_oneY));

		m_twoY = m_oneY + m_imgOne->getContentSize().height;
		m_imgTwo->setPosition(Vec2(0,m_twoY));
	}
}

bool RollNum::setArrive()
{
	m_nEndTexH = 0.0f;

	if (m_oneY <= 0) //��һ������
	{
		if (abs(m_oneY) < m_nCurTexH) //�õ�һ��
		{
			m_nEndTexH = m_nCurTexH - abs(m_oneY);
		}
		else if (abs(m_oneY) > m_nCurTexH) //�õڶ���
		{
			m_nEndTexH = m_nCurTexH + m_imgOne->getContentSize().height - abs(m_oneY);
		}
		else
		{
			return true;
		}
	}

	if (m_twoY <= 0) //�ڶ�������
	{
		if (abs(m_twoY) < m_nCurTexH) //�õ�һ��
		{
			m_nEndTexH = m_nCurTexH - abs(m_twoY);
		}
		else if (abs(m_twoY) > m_nCurTexH) //�õڶ���
		{
			m_nEndTexH = m_nCurTexH + m_imgTwo->getContentSize().height - abs(m_twoY);
		}
		else
		{
			return true;
		}
	}

	if (m_nEndTexH <= 2*NUMBERHEIGHT)
	{
		return true;
	}
	return false;
}

int RollNum::getNumber()
{
	return m_nNumber;
}

void RollNum::setRun(float delay)
{
	m_movePxl = 0.0f;
	m_moveState = MOVE_STATE_ADD;
	this->unschedule(schedule_selector(RollNum::updateNumber));
	//���ڸ������������λ�ã��γɹ���Ч��
	this->schedule(schedule_selector(RollNum::updateNumber), 0.0f,kRepeatForever,delay);
}

void RollNum::setStop()
{
	if (m_movePxl < 7.5f)
	{
		m_movePxl = 7.5f;
	}
	m_moveState = MOVE_STATE_PLUS;
}

////////////////////////////////////////////////////////////////////////////////////////////////

RollNumGroup::RollNumGroup()
{
	canNotifyIdx = 0;
	m_runNum = false;
	m_stopNum = false;
	CCNotificationCenter::getInstance()->addObserver(this,callfuncO_selector(RollNumGroup::setNotity),"test",NULL);
}

RollNumGroup::~RollNumGroup()
{
	CCNotificationCenter::getInstance()->removeAllObservers(this);
}

RollNumGroup *RollNumGroup::createWithGameLayer(Node* pGameLayer, int nDigit)
{
	RollNumGroup *pRollNumGroup = new RollNumGroup();
	if(pRollNumGroup && pRollNumGroup->initWithGameLayer(pGameLayer, nDigit))
	{
		//���Լ�����Ϊ�ڴ��Զ�����Ķ���
		pRollNumGroup->autorelease();
		return pRollNumGroup;
	}
	else
	{
		CC_SAFE_DELETE(pRollNumGroup);
		return NULL;
	}
}

bool RollNumGroup::initWithGameLayer(Node* pGameLayer, int nDigit)
{
	m_pGameLayer = pGameLayer;
	canNotifyIdx = 0;
	//�b�d���ּy��
	m_vecRollNum.clear();
	for(int i = 0; i < nDigit; i++)
	{
		//�������ְ��е�ÿһ������λ
		RollNum *pRollNum = RollNum::create();
		//������λ��ʾ������Ϸ������
		m_pGameLayer->addChild(pRollNum,999);
		m_vecRollNum.push_back(pRollNum);
	}
	m_pGameLayer->addChild(this); //�������ጷ�֮�᲻�����}�{��
	m_vecRollNum.at(canNotifyIdx)->setCanNotify(true); //��һ������֪ͨ

	return true;
}

void RollNumGroup::setPosition(cocos2d::CCPoint pt)
{
	for(int i = 0; i < m_vecRollNum.size(); ++i)
	{
		RollNum *pRollNum = (RollNum *)m_vecRollNum.at(i);
		pRollNum->setPosition(pt);
		//������������ƶ������20.7px
		pt.x += 112;
	}
}

bool RollNumGroup::setRun()
{
	if (m_runNum)
	{
		return false;
	}
	m_runNum = true;
	m_stopNum = false;

	canNotifyIdx = 0;
	for(int i = 0; i < m_vecRollNum.size(); ++i)
	{
		RollNum *pRollNum = (RollNum *)m_vecRollNum.at(i);
		pRollNum->setRun();
		pRollNum->setCanNotify(false);
	}
	m_vecRollNum.at(canNotifyIdx)->setCanNotify(true); //��һ������֪ͨ

	return true;
}

void RollNumGroup::setStop(int nValue)
{
	if (m_stopNum)
	{
		return;
	}
	m_stopNum = true;
	for(int i = m_vecRollNum.size()-1; i >= 0; i--)
	{
		RollNum *pRollNum = (RollNum *)m_vecRollNum.at(i);
		int num = nValue % 10;//ȡ������ʾ�Ļ��ְ��е�ĩλ�ϵ���
		//������Ӧλ�����ֹ���
		pRollNum->setNumber(num);
		pRollNum->setStop();
		//ȥ��Ԥ��ʾ�Ļ�������ĩλ��������һλ������ʾ����
		nValue = nValue / 10;
	}
}

void RollNumGroup::setCurNum(int nValue)
{
	for(int i = m_vecRollNum.size()-1; i >= 0; i--)
	{
		RollNum *pRollNum = (RollNum *)m_vecRollNum.at(i);
		int num = nValue % 10;//ȡ������ʾ�Ļ��ְ��е�ĩλ�ϵ���
		//������Ӧλ�����ֹ���
		pRollNum->setNumber(num,true);
		//ȥ��Ԥ��ʾ�Ļ�������ĩλ��������һλ������ʾ����
		nValue = nValue / 10;
	}
}

void RollNumGroup::setNotity(Ref* sender)
{
	canNotifyIdx++;
	if (canNotifyIdx < m_vecRollNum.size())
	{
		m_vecRollNum.at(canNotifyIdx)->setCanNotify(true); //��һ������֪ͨ
	}
	else
	{
		m_runNum = false;
		CCNotificationCenter::getInstance()->postNotification("getMoney",NULL);//֪ͨ�ⲿ�Ѿ�������Ŀ�ĵ�
	}
}
