#include "CCShake.h"

CCShake * CCShake::create(float _duration, float strength)
{
	 return createWithStrength( _duration, strength, strength );
}

CCShake * CCShake::createWithStrength(float _duration, float strength_x, float strength_y)
{
	CCShake *pRet = new CCShake();  

	if (pRet && pRet->initWithDuration(_duration, strength_x, strength_y))  
	{  
		pRet->autorelease();  
	}  
	else  
	{  
		CC_SAFE_DELETE(pRet);  
	}  


	return pRet;  
}

bool CCShake::initWithDuration(float _duration, float strength_x, float strength_y)
{
	if (CCActionInterval::initWithDuration(_duration))  
	{  
		m_strength_x = strength_x;  
		m_strength_y = strength_y;  
		return true;  
	}  

	return false;  
}

CCShake* CCShake::reverse() const 
{
	CCASSERT(false, "reverse() not supported in CCShake");
	return nullptr;
}

CCShake * CCShake::clone() const 
{
	// no copy constructor
	auto a = new CCShake();
	a->initWithDuration(_duration, m_strength_x,m_strength_y);
	a->autorelease();
	return a;
}


void CCShake::startWithTarget(Node *target)
{
	CCActionInterval::startWithTarget( target );  

	// save the initial position  
	m_initial_x = target->getPosition().x;  
	m_initial_y = target->getPosition().y; 
}

static float fgRangeRand( float min, float max )  
{  
	float rnd = CCRANDOM_0_1();  
	return rnd*(max-min)+min;  
} 

void CCShake::update(float time)
{
	float randx = fgRangeRand( -m_strength_x, m_strength_x )*time;  
	float randy = fgRangeRand( -m_strength_y, m_strength_y )*time;  

	// move the target to a shaked position  
	this->getTarget()->setPosition( ccpAdd(ccp(m_initial_x, m_initial_y),ccp( randx, randy)));  
}

void CCShake::stop()
{
	// Action is done, reset clip position  
	this->getTarget()->setPosition( ccp( m_initial_x, m_initial_y ) );  

	CCActionInterval::stop(); 
}

