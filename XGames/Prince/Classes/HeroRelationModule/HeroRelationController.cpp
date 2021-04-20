#include "HeroRelationController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"

HeroRelationController::HeroRelationController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

HeroRelationController::~HeroRelationController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void HeroRelationController::onRecvMessage( int nCmdID, const char* szData )
{

}

