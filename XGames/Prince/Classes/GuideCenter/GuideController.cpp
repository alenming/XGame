#include "GuideController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"


GuideController::GuideController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

GuideController::~GuideController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void GuideController::onRecvMessage( int nCommandID, const char* szData )
{
	switch (nCommandID)
	{
	case nMAIN_CMD_GUIDE_PROCESS:
		parseGuideProecss(szData);
		break;
	}
	return;
}

void GuideController::sendGuideProcess( int chainId, int groupId )
{
	Json::FastWriter writer;
	Json::Value data;
	data["chainId"] = chainId;
	data["groupId"] = groupId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GUIDE_PROCESS, json_file.c_str());
}

void GuideController::parseGuideProecss(const char* szData)
{

}



