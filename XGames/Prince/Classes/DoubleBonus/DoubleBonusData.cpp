#include "DoubleBonusData.h"
#include "DoubleBonusModel.h"

DoubleBonusData::DoubleBonusData(void)
	: nModuleId(-1)
	, nType(DOUBLE_BONUS_TYPE_NONE)
	, nStartStamp(0)
	, nEndStamp(0)
	, listener(nullptr)
	, preState(DOUBLE_BONUS_STATE_INVALID)
{
}


DoubleBonusData::~DoubleBonusData(void)
{
	resetListener();
}

DOUBLE_BONUS_STATE DoubleBonusData::getState()
{
	DOUBLE_BONUS_STATE state;
	time_t now = LocalTimer::getInstance()->getCurServerTime();//time(0);
	if ((nStartStamp - 3600*24*2) < now && now < nStartStamp)
		state = DOUBLE_BONUS_STATE_EXPECTED;
	else if ( nStartStamp <= now && now < nEndStamp)
		state = DOUBLE_BONUS_STATE_VALID;
	else
		state = DOUBLE_BONUS_STATE_INVALID;
	return state;
}

bool DoubleBonusData::isValid()
{
	return (getState() == DOUBLE_BONUS_STATE_VALID);
}

bool DoubleBonusData::startListener()
{
	if (nullptr == listener)
	{
		int nStamp = 0;
		DOUBLE_BONUS_STATE state = getState();
		if (state == DOUBLE_BONUS_STATE_EXPECTED)
			nStamp = nStartStamp;
		else if (state == DOUBLE_BONUS_STATE_VALID)
			nStamp = nEndStamp;

		if (nStamp != 0)
		{
			preState = state;
			listener = new DoubleBonusListener(CC_CALLBACK_0(DoubleBonusData::onTimeOut, this));
			listener->start(nStamp);
			return true;
		}
		else
			return false;
	}
	return true;
}

void DoubleBonusData::onTimeOut()
{
	// 定时器到期，后续工作下一帧执行;
	auto callBack = [=](float dt)->void
	{
		Director::getInstance()->getScheduler()->unschedule("DoubleBonusData::onTimeOut", this);

		// 重置listener;
		resetListener();

		// 判定到期类型，是开始还是结束;
		DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE  timeoutType;
		if (preState == DOUBLE_BONUS_STATE_EXPECTED)
		{
			timeoutType = DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START;
			startListener();
		}
		else if (preState == DOUBLE_BONUS_STATE_VALID)
		{
			timeoutType = DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::END;
		}

		// 通知到Model，以更新UI;
		DoubleBonusModel::getInstance()->onTimeOut(timeoutType, &nType);
	};
	Director::getInstance()->getScheduler()->schedule(callBack, this, 0, 0, 0, false, "DoubleBonusData::onTimeOut");
}

void DoubleBonusData::resetListener()
{
	if (listener != nullptr)
	{
		delete listener;
		listener = nullptr;
	}
}
