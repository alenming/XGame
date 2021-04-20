#include "FormationController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Json/json.h"
#include "FormationModel.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "TowerModule/TowerModel.h"

FormationController* FormationController::m_pInstance = nullptr;

FormationController* FormationController::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new FormationController;
	}

	return m_pInstance;
}

FormationController::FormationController(void)
{
	m_bReplacing = false;
	m_bIsWaittingResponse = false;
	SocketCommunicator::getInstance()->addListener(this);
}


FormationController::~FormationController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}


void FormationController::releaseInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


void FormationController::handleFormation()
{
	// 拉取试炼阵型;
	if (FormationModel::getInstance()->getFormationMode() == F_MODE_TRAIL)
	{
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_GET_FORMATION, "");
	}
	else
	{
		//SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_DOWNLOAD_FORMATION, "");

		// 普通模式的阵型数据从英雄数据里拿;
		// 简化处理，先把原来消息回包里的内容拿到这里;
		// 取英雄列表;
		vector<int>  vcFormation(9, -1);
		Vector<HeroParam*>* vcHeroList = HeroModel::getInstance()->getHeroList();
		for (unsigned int i = 0; i < vcHeroList->size(); i++)
		{
			if (vcHeroList->at(i)->isOnBattle())
			{
				HERO_POS pos = (HERO_POS)(vcHeroList->at(i)->mBattleFlag-1);
				vcFormation.at(pos) = vcHeroList->at(i)->mHeroId;
			}
		}
		FormationModel::getInstance()->updateFormation(vcFormation);
		FormationModel::getInstance()->setMaxHeroCount(FORMATION_MAX_HERO_COUNT);

		// 初次进入阵型模块的处理，此时UI的初始化已经完毕;
		procFirstEnter(vcFormation);
	}
	
}


void FormationController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_DOWNLOAD_FORMATION:
		{
			procFormation(szData);
		}
		break;

	case nMAIN_CMD_TRAIL_GET_FORMATION:
		{
			procTrailFormation(szData);
		}
		break;

	case nMAIN_CMD_UPLOAD_FORMATION:
	case nMAIN_CMD_TRAIL_SET_FORMATION:
		{
			procUploadFormation(szData);
		}
		break;

	default:
		break;
	}
}


void FormationController::procFormation(const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value pets = data["pets"];

		FormationModel::getInstance()->setMaxHeroCount(FORMATION_MAX_HERO_COUNT);

		// 阵型;
		vector<int>  vcFormation;
		int nHeroId = 0;
		for (unsigned int i = 0; i < pets.size(); i++)
		{
			nHeroId = pets[i].asInt();
			vcFormation.push_back(nHeroId);
		}
		FormationModel::getInstance()->updateFormation(vcFormation);

		// 初次进入阵型模块的处理，此时UI的初始化已经完毕;
		procFirstEnter(vcFormation);
	}
}


void FormationController::procUploadFormation(const char* szData)
{
	m_bIsWaittingResponse = false;

	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		// 此处不适用通用的throwException，结果成功或失败都需要后续处理;
		bool bResult = root["result"].asBool();
		FormationModel::getInstance()->procUploadFormation(bResult);
		if (bResult)
		{
			if (FormationModel::getInstance()->getFormationMode() != F_MODE_TRAIL)
			{
				// 更新关联模块;
				Json::Value data = root["data"];
				MainModel::getInstance()->updateData(data);
				Json::Value petLists = data["petList"];
				HeroModel::getInstance()->updateData(petLists);
				Json::Value items = data["itemList"];
				ToolModel::getInstance()->updateToolJsonData(items);
				ObserverParam observerParam;
				observerParam.id = nMAIN_CMD_DOWNLOAD_FORMATION;
				HeroModel::getInstance()->notifyObservers(&observerParam);
			}
		}
	}
}


void FormationController::procFirstEnter(vector<int>& vcFormation)
{
	// 初次进入阵型模块，选中主角;
	HeroParam* param = nullptr;
	for (unsigned int i = 0; i < vcFormation.size(); i++)
	{
		if (FORMATION_EMPTY_POS == vcFormation.at(i)) 
			continue;

		param = HeroModel::getInstance()->searchHeroById(vcFormation.at(i));
		if (nullptr == param)
		{
			CCLOG("searchHeroById  FALSE!!!!!");
			return;
		}

		if (HERO_TYPE::MAIN_HERO == param->mRoleType)
		{
			//selectHeroByPos((HERO_POS)i);
			selectHero(vcFormation.at(i));
		}
	}
}

void FormationController::procTrailFirstEnter(vector<int>& vcTrailFormation)
{
	// 空阵型，返回;
	if (vcTrailFormation.empty())
	{
		return;
	}

	// 选中第一个非空阵位;
	for (unsigned int i = 0; i < vcTrailFormation.size(); ++i)
	{
		if (vcTrailFormation.at(i) == FORMATION_EMPTY_POS)
		{
			continue;
		}
		
		selectHero(vcTrailFormation.at(i));
		break;
	}
}


int FormationController::handleFormationLocal()
{
	if (m_bIsWaittingResponse)
	{
		return -4;
	}

	// 普通模式下，若阵型无变化，不上传;
	if (FormationModel::getInstance()->getFormationMode() != F_MODE_TRAIL)
	{
		if (!FormationModel::getInstance()->isFormationChanged())
		{
			FormationModel::getInstance()->procUploadFormation(true);
			return 0;
		}
	}

	// 获取最终阵型;
	vector<int>  vcFormation;
	FormationModel::getInstance()->getFormation(vcFormation);

	if (vcFormation.size() < FORMATION_MAX_POS_COUNT)
	{
		CCLOG("ERROR ---> handleFormationLocal, formation pos less than FORMATION_MAX_POS_COUNT");
		return -5;
	}

	// 上传至服务器;
	Json::FastWriter writer;
	Json::Value root;

	for (unsigned int i = 0; i < FORMATION_MAX_POS_COUNT; i++)
	{
		root["petList"].append(vcFormation.at(i));
	}

	// Combo;
	root["maxCombo"] = FormationModel::getInstance()->getMaxCombo();
	
	string json_file = writer.write(root);
	if (FormationModel::getInstance()->getFormationMode() == F_MODE_TRAIL)
	{
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_SET_FORMATION, json_file.c_str());
	}
	else
	{
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_UPLOAD_FORMATION, json_file.c_str());
	}
	return 0;
}


int FormationController::handleHeroField()
{
	int nHeroId = FormationModel::getInstance()->getCurSelectedHero();
	if (nHeroId != FORMATION_EMPTY_POS)
	{
		// Hero已上阵，处理下阵;
		if (FormationModel::getInstance()->isHeroOnField(nHeroId))
		{
			return procHeroOffField(nHeroId);
		}
		// Hero未上阵，处理上阵;
		else
		{
			return procHeroOnField(nHeroId);
		}
	}
	else
	{
		return -3;
	}
}


int FormationController::procHeroOnField(int nHeroId, HERO_POS nPos)
{
	// 上阵限制条件;
	//   1.小伙伴;

	//   2.人数;
	if (FormationModel::getInstance()->getHeroOnFieldCount() >= FormationModel::getInstance()->getMaxHeroCount())
	{
		CCLOG("ERROR! ---> too many hero!");
		return -2;
	}

	// Hero上阵;
	FormationModel::getInstance()->setHeroOnField(nHeroId, nPos);
	return 0;
}


int FormationController::procHeroOffField(int nHeroId)
{
	// 下阵限制条件;
	//   1.主角不能下阵;
	HeroParam* param = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (nullptr == param)
	{
		CCLOG("searchHeroById  FALSE!!!!!");
		return -3;
	}

	if (FormationModel::getInstance()->getFormationMode() != F_MODE_TRAIL)
	{
		if (HERO_TYPE::MAIN_HERO == param->mRoleType)
		{
			CCLOG("ERROR! ---> main hero can not off field");
			return -1;
		}
	}

	// Hero下阵;
	FormationModel::getInstance()->setHeroOffField(nHeroId);
	return 0;
}


bool FormationController::handleSwapFormation(HERO_POS nPos1, HERO_POS nPos2)
{
	// 交换站位;
	FormationModel::getInstance()->swapFormation(nPos1, nPos2);
	return true;
}


void FormationController::handlePreReplace()
{
	// 过滤无效情况;
	if (FORMATION_EMPTY_POS == FormationModel::getInstance()->getCurSelectedHero())
	{
		return;
	}

	// 打开开关;
	m_bReplacing = !m_bReplacing;
}

int FormationController::handleReplaceHero(int nHeroId1, int nHeroId2)
{
	// 同Id不替换;
	if (nHeroId1 == nHeroId2)
	{
		return 0;
	}

	// 是否上阵;
	bool bFlag1 = FormationModel::getInstance()->isHeroOnField(nHeroId1);
	bool bFlag2 = FormationModel::getInstance()->isHeroOnField(nHeroId2);

	// 1. 两个Hero都已上阵;
	if (bFlag1 && bFlag2)
	{
		HERO_POS nPos1 = FormationModel::getInstance()->getPosOfHero(nHeroId1);
		HERO_POS nPos2 = FormationModel::getInstance()->getPosOfHero(nHeroId2);

		if (nPos1 == Hero_Pos_Invalid || nPos2 == Hero_Pos_Invalid)
		{
			CCLOG("ERROR! ---> handleReplaceHero, position error");
			return -3;
		}

		handleSwapFormation(nPos1, nPos2);
	}
	// 2. A-Hero上阵，B-Hero未上阵;
	else if ((bFlag1 && !bFlag2) || (!bFlag1 && bFlag2))
	{
		// 主角不允许下阵;
		HeroParam* param1 = HeroModel::getInstance()->searchHeroById(nHeroId1);
		HeroParam* param2 = HeroModel::getInstance()->searchHeroById(nHeroId1);
		if (nullptr == param1 || nullptr == param2)
		{
			CCLOG("searchHeroById  FALSE!!!!!");
			return -1;
		}
		if (HERO_TYPE::MAIN_HERO == param1->mRoleType
			|| HERO_TYPE::MAIN_HERO == param2->mRoleType)
		{
			CCLOG("ERROR! ---> main hero can not be replaced");
			return -1;
		}

		// 上阵的Hero;
		int nHeroOnField = bFlag1 ? nHeroId1 : nHeroId2;

		// 未上阵的Hero;
		int nHeroNotOnField = bFlag1 ? nHeroId2 : nHeroId1;

		// A下阵;
		HERO_POS nPos = FormationModel::getInstance()->getPosOfHero(nHeroOnField);
		FormationModel::getInstance()->setHeroOffField(nHeroOnField);

		// 然后B上阵;
		FormationModel::getInstance()->setHeroOnField(nHeroNotOnField, nPos);

	}
	// 3. 两个Hero都未上阵;
	else if (!bFlag1 && !bFlag2)
	{
		return -3;
	}

	return 0;
}


int FormationController::selectHero(int nHeroId)
{
	if (nHeroId == FORMATION_EMPTY_POS)
	{
		return -3;
	}

	// 更新选中之前先预留上一次的选中，因为可能是替换操作;
	//int nPreSelectedHeroId = FormationModel::getInstance()->getCurSelectedHero();

	// 更新当前选中;
	FormationModel::getInstance()->setCurSelected(nHeroId, Hero_Pos_Invalid);

	// 更新技能信息;
	FormationModel::getInstance()->updateSkillInfo(nHeroId);

	// 如果当前操作是替换Hero，处理之;
	/*
	if (m_bReplacing)
	{
		// 重置开关;
		m_bReplacing = !m_bReplacing;

		// 替换Hero;
		return handleReplaceHero(nPreSelectedHeroId, nHeroId);
	}
	*/

	// 选中未上阵的Hero直接操作上阵(by Phil 12/17/2014 @zcjoy);
	if(!FormationModel::getInstance()->isHeroOnField(nHeroId))
	{
		// 试炼模式下判断Hero血量;
		if (FormationModel::getInstance()->getFormationMode() == F_MODE_TRAIL)
		{
			HeroParam* param = HeroModel::getInstance()->searchHeroById(nHeroId);
			assert(param != nullptr);
			if (param->mHpCur <= 0)
			{
				return -6;
			}
		}
		return handleHeroField();
	}

	return 0;
}


int FormationController::selectHeroByPos(HERO_POS nPos)
{
	if (nPos == Hero_Pos_Invalid)
	{
		return -3;
	}

	// 更新选中之前先预留上一次的选中，因为可能是替换操作;
	int nPreSelectedHeroId = FormationModel::getInstance()->getCurSelectedHero();

	// 更新当前选中;
	FormationModel::getInstance()->setCurSelected(FORMATION_EMPTY_POS, nPos);

	// 更新技能信息;
	int nHeroId = FormationModel::getInstance()->getHeroAtPos(nPos);
	if (nHeroId != FORMATION_EMPTY_POS)
	{
		FormationModel::getInstance()->updateSkillInfo(nHeroId);

		// 选中已上阵的Hero直接操作下阵(by Phil 12/17/2014 @zcjoy);
		return handleHeroField();
	}

	// 如果当前操作是替换Hero，处理之;
	/*
	if (m_bReplacing)
	{
		// 重置开关;
		m_bReplacing = !m_bReplacing;

		// 如果第2次选择了阵型空位，则再次分解为上阵或阵型换位操作;
		if (nHeroId == FORMATION_EMPTY_POS)
		{
			// Step1:已上阵 + Step2:空位 = 换位操作;
			if (FormationModel::getInstance()->isHeroOnField(nPreSelectedHeroId))
			{
				handleSwapFormation(FormationModel::getInstance()->getPosOfHero(nPreSelectedHeroId), nPos);
				FormationModel::getInstance()->updateSkillInfo(nPreSelectedHeroId);
			}
			// Step1:未上阵 + Step2:空位 = 上阵操作;
			else
			{
				return procHeroOnField(nPreSelectedHeroId, nPos);
			}
		}
		else
		{
			// 替换Hero;
			int nRet = handleReplaceHero(nPreSelectedHeroId, nHeroId);
			FormationModel::getInstance()->updateSkillInfo(nPreSelectedHeroId);
			return nRet;
		}
	}
	*/

	return 0;
}

void FormationController::handleSelectEmptyPos( HERO_POS nPos )
{
	FormationModel::getInstance()->setCurSelected(FORMATION_EMPTY_POS, nPos);
}

void FormationController::procTrailFormation( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		bool bResult = root["result"].asBool();
		if (bResult)
		{
			Json::Value data = root["data"];
			Json::Value petList = data["petList"];
			map<int, Trail_Formation_Attr>  mapAttr;
			HeroParam* _heroParam = nullptr;
			for (unsigned int i = 0; i < petList.size(); i++)
			{
				Trail_Formation_Attr attr;
				Json::Value pet = petList[i];
				attr.nHeroId = pet["petId"].asInt();
				attr.fFc = pet["battlePower"].asDouble();
				mapAttr.insert(pair<int, Trail_Formation_Attr>(attr.nHeroId, attr));

				// 设置血量;
				_heroParam = HeroModel::getInstance()->searchHeroById(attr.nHeroId);
				assert(_heroParam != nullptr);
				_heroParam->mHp = pet["fullHp"].asInt();
				_heroParam->mHpCur = pet["hp"].asInt();
			}

			// 阵型;
			vector<int>  vcTrailFormation;
			Json::Value matrix = data["matrix"];
			for (unsigned int i = 0; i < matrix.size(); i++)
			{
				vcTrailFormation.push_back(matrix[i].asInt());
			}

			// 怒气值;
			int nRage = data["nuqi"].asInt();
			HeroModel::getInstance()->setRage(nRage);

			// 最大上阵英雄数量;
			//int nMaxCount = data["maxPets"].asInt();
			FormationModel::getInstance()->setMaxHeroCount(FORMATION_MAX_HERO_COUNT);

			// 更新数据;
			FormationModel::getInstance()->setTrailAttr(mapAttr);
			FormationModel::getInstance()->updateFormation(vcTrailFormation);

			// 初次进入阵型模块的处理;
			procTrailFirstEnter(vcTrailFormation);
		}
		else
		{
			CustomPop::show(STR_ANSI(root["msg"].asString()));
		}
	}
}

int FormationController::checkTrailFormation()
{
	int nCount = FormationModel::getInstance()->getHeroOnFieldCount();
	return (nCount > 0) ? 0 : -7;
}
