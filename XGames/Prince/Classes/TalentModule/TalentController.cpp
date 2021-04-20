#include "TalentController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "HeroModule/HeroModel.h"


TalentController::TalentController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

TalentController::~TalentController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void TalentController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TALENT_REPLACE:
		parseReplaceSkillMsg(szData);
		break;
	default:
		break;
	}
}

void TalentController::sendReplaceSkillMsg(int skillType, int skillId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["sklTyp"] = skillType;
	val["skillId"] = skillId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TALENT_REPLACE, json_file.c_str());
}

void TalentController::parseReplaceSkillMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (!throwException(root))
		{
			Json::Value data = root["data"];
			int nomalSklId = data["atkID"].asInt();
			int activeSklId = data["a_sklID"].asInt();
			int rushSklId = data["p1_sklID"].asInt();
			int buffSklId = data["p2_sklID"].asInt();
			int triggerSklId = data["p3_sklID"].asInt();

			HeroParam* heroParam = HeroModel::getInstance()->getMainHero();
			heroParam->mNormalSkillId = nomalSklId;
			heroParam->mActiveSkillId = activeSklId;
			heroParam->mRushSkillId = rushSklId;
			heroParam->mBuffSkillId = buffSklId;
			heroParam->mTriggerSkillId = triggerSklId;
		}

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_TALENT_REPLACE;
		HeroModel::getInstance()->notifyObservers(&observerParam);

	}
}

