#include "DoubleBonusListener.h"


DoubleBonusListener::DoubleBonusListener(DoubleBonusListenerCallback callbackFunc)
	: _callback(callbackFunc)
{
}


DoubleBonusListener::~DoubleBonusListener(void)
{
	_callback = nullptr;
	LocalTimer::getInstance()->removeTimeStampListener(this);
}

void DoubleBonusListener::start( int nStamp )
{
	// ������ʱ;
	setTimeStamp(nStamp);
	LocalTimer::getInstance()->addTimeStampListener(this);
}

void DoubleBonusListener::timeOut()
{
	if (_callback)
		_callback();
}
