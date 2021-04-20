#include "FormationModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "FightModule/Skill/AbstractSkill.h"
#include "FormationController.h"

FormationModel* FormationModel::mInstance = nullptr;

FormationModel::FormationModel(void)
	: m_nCurSelectedHero(FORMATION_EMPTY_POS)
	, m_nCurSelectedPos(Hero_Pos_Invalid)
	, m_nMaxCombo(0)
	, m_nFcTeam(0)
	, m_yueliHp(0.0f)
	, m_yueliAtk(0.0f)
	, m_yueliMdd(0.0f)
	, m_yueliPdd(0.0f)
	, m_fAtkCoe(0.0f)
	, m_fHpCoe(0.0f)
	, m_fMddCoe(0.0f)
	, m_fPddCoe(0.0f)
	, m_fComboCoe(0.0f)
	, m_nMode(Formation_Mode::F_MODE_FORMATION)
	, m_nMaxHeroCount(1)
{
	m_mapHeroOnField.clear();
	m_mapHeroOffField.clear();
	m_mapHeroRushRecord.clear();
	m_mapHeroTriggerRecord.clear();
	m_vcFreeEquipment.clear();
	m_mapHeroEquipment.clear();
	m_mapFcSingleA.clear();
	m_mapFcSingleB.clear();
	m_vcFormation.clear();
	m_vcFriends.clear();
	m_mapTrailAttr.clear();
	m_vcFormationBackup.clear();
}


FormationModel::~FormationModel(void)
{
}

FormationModel* FormationModel::getInstance()
{
	if (!mInstance)
	{
		mInstance = new FormationModel();
	}
	return mInstance;
}

void FormationModel::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void FormationModel::getFreeEquipmentList(vector<int>&  vcEquipmentList)
{
	vcEquipmentList.clear();
	for (unsigned int i = 0; i < m_vcFreeEquipment.size(); i++)
	{
		if (m_vcFreeEquipment.at(i) != FORMATION_EMPTY_EQUIPMENT)
		{
			vcEquipmentList.push_back(m_vcFreeEquipment.at(i));
		}
	}
}


void FormationModel::getFormation(vector<int>& vcFormation, bool bNeedRefresh)
{
	if (m_vcFormation.empty())
	{
		FormationController::getInstance()->handleFormation();
		return;
	}
	vcFormation.clear();
	vcFormation = m_vcFormation;

	// 刷新一次阵型，给需要的模块(比如竞技场);
	if (bNeedRefresh)
	{
		updateFormation();
	}
}

void FormationModel::getFormationWithoutEmptyPos(vector<int>& vcFormation)
{
	if (m_vcFormation.empty())
		return;

	vcFormation.clear();
	for(auto it : m_vcFormation)
	{
		if (it != FORMATION_EMPTY_POS)
		{
			vcFormation.push_back(it);
		}
	};
}


void FormationModel::getHeroEquipment(map<int, int>& mapHeroEquipment)
{
	mapHeroEquipment = m_mapHeroEquipment;
}


void FormationModel::updateFreeEquipment(vector<int>  vcEquipment)
{
	m_vcFreeEquipment = vcEquipment;
}

void FormationModel::setFormation(vector<int> vcFormation)
{
	if (m_nMode == F_MODE_TRAIL)
	{
		// 先备份，再赋值;
		m_vcFormationBackup = m_vcFormation;
		m_vcFormation = vcFormation;
	}
	else
	{
		// 先赋值，再备份;
		m_vcFormation = vcFormation;
		m_vcFormationBackup = m_vcFormation;
	}

}

void FormationModel::updateFcCoe( float fAtk, float fMdd, float fPdd, float fHp )
{
	m_yueliAtk = fAtk;
	m_yueliMdd = fMdd;
	m_yueliPdd = fPdd;
	m_yueliHp = fHp;
}

void FormationModel::updateFormation(vector<int> vcFormation)
{
	// 1.得到阵型数据;
	//m_vcFormation = vcFormation;
	setFormation(vcFormation);

	// 3.刷新;
	updateFormation();
}

float FormationModel::getDataFromDTSystem( int nId )
{
	DataTable* dtSys = DataManager::getInstance()->getDTToolSystem();
	assert(dtSys != nullptr);
	RowData* _dataAtk = dtSys->searchDataById(nId);
	if (_dataAtk != nullptr)
	{
		return _dataAtk->getFloatData("value");
	}
	return 0.0f;
}


void FormationModel::updateFormation()
{
	// 读表，得到相关数值;
	m_fAtkCoe = getDataFromDTSystem(10034);
	m_fPddCoe = getDataFromDTSystem(10035);
	m_fMddCoe = getDataFromDTSystem(10036);
	m_fHpCoe = getDataFromDTSystem(10037);
	m_fComboCoe = getDataFromDTSystem(10045);

	// 先计算最大追击，战斗值用到;
	updateCombo();

	// 1.通知UI刷新阵型;
	UI_FORMATION_PARAM uiParam;
	uiParam.vcFormation = m_vcFormation;
	uiParam.nFc = calcFcTeam();
	for (int i = 0; i < uiParam.vcFormation.size(); i++)
	{
		// 填充参数;
		UI_HERO_LIST_PARAM param;
		if (FORMATION_EMPTY_POS != uiParam.vcFormation.at(i))
		{
			constructUIHeroParam(uiParam.vcFormation.at(i), param);
		}
		uiParam.vcHeroData.push_back(param);
	}

	// 更新阵型;
	updateUI(OBS_PARAM_TYPE_FORMATION, (void*)&uiParam);

	// 2.更新Hero列表;
	updateHeroList();
}


void FormationModel::updateHeroList()
{
	// 1.查询所有Hero，并存储在未上阵列表;
	m_mapHeroOffField.clear();
	int nHeroCount = HeroModel::getInstance()->getHeroList()->size();
	for (int i = 0; i < nHeroCount; i++)
	{
		// 取出Id，并计算战斗值;
		int nHeroId = HeroModel::getInstance()->getHeroList()->at(i)->mHeroId;
		int nFc = calcFcSingle(nHeroId);

		// 试炼上阵等级限制;
		int nHeroLv = HeroModel::getInstance()->getHeroList()->at(i)->mLevel;
		DataTable* dtSystem = DataManager::getInstance()->getDTToolSystem();
		RowData* data = dtSystem->searchDataById(10056);
		assert(data != nullptr);
		int nNeedLv = data->getIntData("value");
		if (m_nMode == F_MODE_TRAIL && nHeroLv < nNeedLv)
		{
			continue;
		}

		m_mapHeroOffField.insert(pair<int, int>(nFc, nHeroId));
	}

	// 2.根据阵型数据过滤出已上阵Hero，并存储在已上阵列表;
	m_mapHeroOnField.clear();
	for (auto iter = m_vcFormation.begin(); iter != m_vcFormation.end(); iter++)
	{
		bool bFound = false;
		for (auto it = m_mapHeroOffField.begin(); it != m_mapHeroOffField.end(); it++)
		{
			// 挑选已上阵的Hero进行分类;
			if (*iter == it->second)
			{
				m_mapHeroOnField.insert(pair<int, int>(it->first, it->second));
				m_mapHeroOffField.erase(it);
				bFound = true;
				break;
			}
		}

		if (bFound)
			continue;
	}

	// 3.按规则构造出最终的Hero列表(已上阵优先/战斗值高优先);
	vector<UI_HERO_LIST_PARAM> vcHeroList;
	for (auto iter = m_mapHeroOffField.begin(); iter != m_mapHeroOffField.end(); iter++)
	{
		UI_HERO_LIST_PARAM  param;
		constructUIHeroParam(iter->second, param);
		vcHeroList.insert(vcHeroList.begin(), param);
	}
	for (auto it = m_mapHeroOnField.begin(); it != m_mapHeroOnField.end(); it++)
	{
		UI_HERO_LIST_PARAM  param;
		constructUIHeroParam(it->second, param);
		vcHeroList.insert(vcHeroList.begin(), param);
	}

	// 4.通知UI刷新Hero列表;
	updateUI(OBS_PARAM_TYPE_HERO_LIST, (void*)&vcHeroList);

}

void FormationModel::updateSelected()
{
	// 阵型数据 + 选中Id + 选中Pos;
	vector<int> vcSelected;
	vcSelected = m_vcFormation;
	vcSelected.push_back(m_nCurSelectedHero);
	vcSelected.push_back(m_nCurSelectedPos);
	updateUI(OBS_PARAM_TYPE_SELECTED, (void*)&vcSelected);
}


void FormationModel::updateSkillInfo(int nHeroId)
{
	// 1.查找Hero的技能Id;
	vector<int> vcSkillId;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (nullptr == heroParam)
	{
		CCLOG("searchHeroById  FALSE!!!!!");
		return;
	}
	vcSkillId.push_back(heroParam->mNormalSkillId);
	vcSkillId.push_back(heroParam->mActiveSkillId);
	vcSkillId.push_back(heroParam->mRushSkillId);
	vcSkillId.push_back(heroParam->mBuffSkillId);
	vcSkillId.push_back(heroParam->mTriggerSkillId);

	// 2.查询技能详情<主，普，被动1，被动2，被动3>;
	vector<FORMATION_SKILL_PARAM>  vcSkillInfo;
	for (int i = 0; i < vcSkillId.size(); ++i)
	{
		// 从技能表读;
		RowData* dataSkill = DataManager::getInstance()->searchSkillById(vcSkillId.at(i));
		FORMATION_SKILL_PARAM info;
		if (nullptr != dataSkill)
		{
			info.nSkillId = vcSkillId.at(i);
			info.nSkillType = dataSkill->getIntData("sklTyp");
			info.nResId = dataSkill->getIntData("resId");
			info.strName = STR_ANSI(dataSkill->getStringData("name"));
			info.strDesc = STR_ANSI(dataSkill->getStringData("capTxt"));
			info.bEnabled = true;
		}
		else
		{
			// 默认值;
			info.nSkillId = FORMATION_INVALID_SKILL_ID;

			// 查英雄表，得到技能名称，无论开不开启;
			// 用templateId吧，nHeroId格式现在我已经不知道id是个什么jb玩意了，出问题别找我;
			// 11/02/2015 Phil @zcjoy;
			RowData* heroData = DataManager::getInstance()->searchCharacterById(heroParam->mTemplateId);
			if (heroData != nullptr)
			{
				// 按照插入顺序;
				string  strField("");
				if (i == 0)			// 普通技能;
				{
					strField.append("atkID");
				}
				else if (i == 1)	// 主动技能;
				{
					strField.append("a_sklID");
				}
				else if (i == 2)	// 被动1;
				{
					strField.append("p1_sklID");
				}
				else if (i == 3)	// 被动2;
				{
					strField.append("p2_sklID");
				}
				else if (i == 4)	// 被动3;
				{
					strField.append("p3_sklID");
				}

				// 得到配置的技能Id;
				int nTmpSkillId = heroData->getIntData(strField);

				// 然后再看英雄类型，如果是主角，技能更复杂一点，在这不说明了，我说不清;
				// 11/02/2015 Phil @zcjoy;
				if (heroParam->isMainHero())
				{
					RowData* _row = DataManager::getInstance()->searchTalentSkillById(nTmpSkillId);
					if (nullptr != _row)
					{
						string strSkilllllll = _row->getStringData("skillId");
						vector<int>  vcSkilllllll;
						StringFormat::parseCsvStringVec(strSkilllllll, vcSkilllllll);
						if (vcSkilllllll.size() != 0)
						{
							nTmpSkillId = vcSkilllllll.at(0);
						}
					}
				}

				// 然后从技能表读信息;
				RowData* dataSkill = DataManager::getInstance()->searchSkillById(nTmpSkillId);
				if (nullptr != dataSkill)
				{
					info.nSkillId = nTmpSkillId;
					info.nSkillType = dataSkill->getIntData("sklTyp");
					info.nResId = dataSkill->getIntData("resId");
					info.strName = STR_ANSI(dataSkill->getStringData("name"));
					info.strDesc = STR_ANSI(dataSkill->getStringData("capTxt"));
					info.bEnabled = false;
				}
			}
		}
		vcSkillInfo.push_back(info);
	}

	// 3.通知UI刷新技能描述;
	updateUI(OBS_PARAM_TYPE_SKILL_INFO, (void*)&vcSkillInfo);
}


void FormationModel::updateCombo()
{
	// 1.计算;
	//UI_COMBO_PARAM param;
	Combo_Skill_Param  comboParam;
	m_nMaxCombo = calcMaxCombo(comboParam);

	// 2.取头像;
	for (int i = 0; i < comboParam.vcComboList.size(); i++)
	{
		HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(comboParam.vcComboList.at(i));
		if (nullptr == heroParam)
		{
			CCLOG("searchHeroById  FALSE!!!!!");
			return;
		}
		int nResId = heroParam->mResId;
		comboParam.vcComboListIcon.push_back(nResId);
	}

	// 3.通知UI刷新连击Hero;
	updateUI(OBS_PARAM_TYPE_COMBO, (void*)&comboParam);
}


void FormationModel::updateFriends()
{
	// 查询所有小伙伴;
	vector<int> vcFriends;

	// 通知UI刷新小伙伴;
	updateUI(OBS_PARAM_TYPE_FRIENDS, (void*)&vcFriends);
}


void FormationModel::updateUI(OBS_PARAM_TYPE_FM type, void* data)
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}


void FormationModel::setHeroOnField(int nHeroId, HERO_POS nPos)
{
	// 先检查Hero是否已上阵;
	if (!isHeroOnField(nHeroId))
	{
		// 空位上阵或指定位置上阵;
		HERO_POS nFinalPos = nPos;
		if (nFinalPos == Hero_Pos_Invalid)
		{
			//nFinalPos = getFirstEmptyPos();
			if (m_nCurSelectedPos == Hero_Pos_Invalid || FORMATION_EMPTY_POS != getHeroAtPos(m_nCurSelectedPos))
			{
				nFinalPos = getFirstEmptyPos();
			}
			else
			{
				nFinalPos = m_nCurSelectedPos;
			}
			//nFinalPos = (FORMATION_EMPTY_POS == getHeroAtPos(m_nCurSelectedPos)) ? m_nCurSelectedPos : getFirstEmptyPos();
		}
		m_vcFormation.at(nFinalPos) = nHeroId;

		// 随机分配一件装备(可能为空);
		/*
		auto iter = m_mapHeroEquipment.find(nHeroId);
		assert(iter == m_mapHeroEquipment.end());
		m_mapHeroEquipment.insert(pair<int, int>(nHeroId, getEquipment()));
		*/

		// 更新连击数;
		updateCombo();

		// UI上阵;
		UI_FORMATION_PARAM uiParam;
		uiParam.vcFormation = m_vcFormation;
		uiParam.nFc = calcFcTeam();
		for (int i = 0; i < uiParam.vcFormation.size(); i++)
		{
			// 填充参数;
			UI_HERO_LIST_PARAM param;
			if (nHeroId == uiParam.vcFormation.at(i))
			{
				constructUIHeroParam(uiParam.vcFormation.at(i), param);
			}
			uiParam.vcHeroData.push_back(param);
		}
		updateUI(OBS_PARAM_TYPE_HERO_ON_FIELD, (void*)&uiParam);

		// 更新Hero列表;
		updateHeroList();

		// 更新Combo;
		//updateCombo();

		// 设置当前选中;
		setCurSelected(nHeroId, nFinalPos);
	}
}

void FormationModel::setHeroOffField(int nHeroId)
{
	vector<int> vcOriFormation;
	vcOriFormation = m_vcFormation;

	// 下阵;
	HERO_POS nFinalPos = Hero_Pos_Invalid;
	HERO_POS nOriPos = Hero_Pos_Invalid;
	for (int i = 0; i < m_vcFormation.size(); i++)
	{
		if (nHeroId == m_vcFormation.at(i))
		{
			// Hero下阵;
			m_vcFormation.at(i) = FORMATION_EMPTY_POS;
			nOriPos = (HERO_POS)i;

			// 下阵一个英雄，默认选中空出来的那个位置;
			nFinalPos = (HERO_POS)i;
			break;
		}
	}

	// 更新连击数;
	updateCombo();

	// UI下阵;
	UI_FORMATION_PARAM uiParam;
	uiParam.vcFormation = m_vcFormation;
	uiParam.nFc = calcFcTeam();
	for (int i = 0; i < vcOriFormation.size(); i++)
	{
		// 填充参数;
		UI_HERO_LIST_PARAM param;
		if (nHeroId == vcOriFormation.at(i))
		{
			constructUIHeroParam(vcOriFormation.at(i), param);
			
			// 修正阵位的值，把下阵前的阵位传过去;
			param.nPos = nOriPos;
		}
		uiParam.vcHeroData.push_back(param);
	}
	updateUI(OBS_PARAM_TYPE_HERO_OFF_FIELD, (void*)&uiParam);

	// 更新Hero列表;
	updateHeroList();

	// 设置当前选中;
	setCurSelected(nHeroId, nFinalPos);
}


void FormationModel::swapFormation(HERO_POS nPos1, HERO_POS nPos2)
{
	if (m_vcFormation.empty()
		|| nPos1 == Hero_Pos_Invalid || nPos2 == Hero_Pos_Invalid)
		return;

	// 交换阵位;
	int nHeroId1 = m_vcFormation.at(nPos1);
	int nHeroId2 = m_vcFormation.at(nPos2);
	swap(m_vcFormation.at(nPos1), m_vcFormation.at(nPos2));

	// 更新连击数;
	updateCombo();

	// UI交换阵位;
	UI_SWAP_FORMATION_PARAM uiParam;
	uiParam.nFc = calcFcTeam();
	constructUIHeroParam(nHeroId1, uiParam.uiOriHero);
	constructUIHeroParam(nHeroId2, uiParam.uiDstHero);
	updateUI(OBS_PARAM_TYPE_HERO_SWAP, (void*)&uiParam);

	// 设置当前选中;
	setCurSelected(FORMATION_EMPTY_POS, nPos2);

	// 更新技能信息;
	updateSkillInfo(m_nCurSelectedHero);
}


int FormationModel::getHeroOnFieldCount()
{
	int nCount = 0;
	for (auto iter = m_vcFormation.begin(); iter != m_vcFormation.end(); iter++)
	{
		if (*iter != FORMATION_EMPTY_POS)
		{
			nCount++;
		}

		CC_BREAK_IF(nCount >= FORMATION_MAX_HERO_COUNT);
	}

	return nCount;
}


bool FormationModel::isHeroOnField(int nHeroId)
{
	if (FORMATION_EMPTY_POS == nHeroId)
	{
		return false;
	}

	for (auto iter = m_vcFormation.begin(); iter != m_vcFormation.end(); iter++)
	{
		if (*iter == nHeroId)
		{
			return true;
		}
	}

	return false;
}


int FormationModel::getHeroAtPos(HERO_POS nPos)
{
	if (m_vcFormation.empty() || nPos == Hero_Pos_Invalid)
		return FORMATION_EMPTY_POS;

	// 返回当前位置的Hero，可能是空值;
	return m_vcFormation.at(nPos);
}


HERO_POS FormationModel::getPosOfHero(int nHeroId)
{
	for (int i = 0; i < m_vcFormation.size(); i++)
	{
		if (m_vcFormation.at(i) == nHeroId)
		{
			return (HERO_POS)i;
		}
	}

	return Hero_Pos_Invalid;
}


void FormationModel::setCurSelected(int nHero, HERO_POS nPos)
{
	if (m_vcFormation.empty())
		return;

	// 初始化一下;
	//m_nCurSelectedPos = Hero_Pos_Invalid;
	m_nCurSelectedHero = FORMATION_EMPTY_POS;

	// 从阵型中选中;
	if (nHero == FORMATION_EMPTY_POS && nPos != Hero_Pos_Invalid)
	{
		// 位置一定有;
		m_nCurSelectedPos = nPos;

		// 列表中不一定有对应的英雄，因为选中的可能是阵型空位;
		m_nCurSelectedHero = getHeroAtPos(nPos);
	}
	// 从列表中选中;
	else if (nHero != FORMATION_EMPTY_POS && nPos == Hero_Pos_Invalid)
	{
		// 一定有选中的英雄;
		m_nCurSelectedHero = nHero;

		// 阵型中不一定有对应的位置，因为选中的可能是列表中未上阵的英雄;
		for (int i = 0; i < m_vcFormation.size(); i++)
		{
			// 查找英雄在阵型中的位置;
			if (nHero == m_vcFormation.at(i))
			{
				m_nCurSelectedPos = (HERO_POS)i;
				break;
			}
		}

		// 如果英雄未上阵，显示当前选中的空位或者第一个空位;
		if (FORMATION_EMPTY_POS != getHeroAtPos(m_nCurSelectedPos)
			&& !isHeroOnField(nHero))
		{
			m_nCurSelectedPos = getFirstEmptyPos();
		}
	}
	// 正常选中;
	else if (nHero != FORMATION_EMPTY_POS && nPos != Hero_Pos_Invalid)
	{
		m_nCurSelectedHero = nHero;
		m_nCurSelectedPos = nPos;
	}
	else
	{
		return;
	}

	// 更新选中UI;
	updateSelected();
}


int FormationModel::getEquipment()
{
	/*
	// 过滤出可用的装备列表;
	vector<int> vcValidEquipment;
	for (unsigned int i = 0; i < m_vcFreeEquipment.size(); i++)
	{
		if (FORMATION_EMPTY_EQUIPMENT != m_vcFreeEquipment.at(i))
		{
			vcValidEquipment.push_back(m_vcFreeEquipment.at(i));
		}
	}

	if (vcValidEquipment.empty())
	{
		log("getEquipment  FALSE!!!!! no more equipment");
		return FORMATION_EMPTY_EQUIPMENT;
	}
	*/
	if (0 == m_vcFreeEquipment.size())
	{
		return FORMATION_EMPTY_EQUIPMENT;
	}

	// 随机取出一件装备;
	int nIndex = rand()%m_vcFreeEquipment.size();
	int nEquipmentId = m_vcFreeEquipment.at(nIndex);

	// 从装备列表中去掉这件装备;
	//m_vcEquipment.at(nIndex) = FORMATION_EMPTY_EQUIPMENT;
	auto iter = m_vcFreeEquipment.begin();
	for ( ; iter != m_vcFreeEquipment.end(); )
	{
		if (*iter == nEquipmentId)
		{
			m_vcFreeEquipment.erase(iter++);
			break;
		}
		else
		{
			++iter;
		}
	}
	return nEquipmentId;
}


HERO_POS FormationModel::getFirstEmptyPos()
{
	HERO_POS nPos = Hero_Pos_Invalid;

	for (int i = 0; i < m_vcFormation.size(); i++)
	{
		if (FORMATION_EMPTY_POS == m_vcFormation.at(i))
		{
			nPos = (HERO_POS)i;
			break;
		}
	}

	return nPos;
}


//////////////////////////////////// Combo相关; /////////////////////////////////////

int FormationModel::calcMaxCombo(Combo_Skill_Param& comboParam)
{
	comboParam.clear();
	int nMaxCombo = 0;

	// 去除阵型列表的空位;
	vector<int>  vcTmpFormation;
	for (int i = 0; i < m_vcFormation.size(); i++)
	{
		if (FORMATION_EMPTY_POS != m_vcFormation.at(i))
		{
			vcTmpFormation.push_back(m_vcFormation.at(i));
		}
	}

	// 两条主线;
	//   1. 由任意Hero的普攻发起第一次攻击，造成连续追打，计算次数;
	//   2. 由任意Hero的主动技能发起第一次攻击，造成连续追打，计算次数;
	//   阵型的Combo数量 = 两组数据取最大值;
	// 额外计算;
	//   1. 连击数触发被动3;
	// 最终排序;
	//   1. 被动3的连击序列单独存放，最后附在追打连击序列之后;


	// 每回合连击数(触发被动3)清零;
	m_nMaxHitCount = 0;

	//////////////////////////////  计算逻辑首发Hero普攻或主攻; ///////////////////////////////
	vector<Combo_Skill_Param>  vcResult;
	for (unsigned int i = 0; i < vcTmpFormation.size(); i++)
	{
		m_nLogicFirstHeroOnRush = vcTmpFormation.at(i);

		///// 1...首发普攻;
		m_nMaxHitCount = 0;
		Combo_Skill_Param  comboSkillParam;
		comboSkillParam.nIndex = i;
		resetHeroRushRecord(vcTmpFormation);
		resetHeroTriggerRecord(vcTmpFormation);
		startAtk(m_nLogicFirstHeroOnRush, AbstractSkill::SKILL_TYPE::NORMAL_SKILL, comboSkillParam);

		// 去掉重复Id(追打自身的情况);
		// 改需求，不去掉重复的了(by Phil 12/16/2014 @zcjoy);
		vcResult.push_back(comboSkillParam);
		int nTmpSize = comboSkillParam.nMaxCombo;

		///// 2...首发主攻;
		m_nMaxHitCount = 0;
		comboSkillParam.clear();
		comboSkillParam.nIndex = i;
		resetHeroRushRecord(vcTmpFormation);
		resetHeroTriggerRecord(vcTmpFormation);
		startAtk(m_nLogicFirstHeroOnRush, AbstractSkill::SKILL_TYPE::ACTIVE_SKILL, comboSkillParam);

		// 去掉重复Id(追打自身的情况);
		// 改需求，不去掉重复的了(by Phil 12/16/2014 @zcjoy);
		if (comboSkillParam.nMaxCombo >= nTmpSize)
		{
			vcResult.push_back(comboSkillParam);
		}
		
	}

	//////////////////////////////  取最大值; ///////////////////////////////
	if (vcResult.size() > 0)
	{
		/*
		std::sort(vcResult.begin(), vcResult.end(), [=](Combo_Skill_Param param1, Combo_Skill_Param param2)->bool{
			return param1.nMaxCombo > param2.nMaxCombo;
		});
		*/
		std::sort(vcResult.begin(), vcResult.end(), CC_CALLBACK_2(FormationModel::sortComboList, this));
		comboParam = *(vcResult.begin());
	}

	return comboParam.nMaxCombo;
}


void FormationModel::startAtk(int nHeroId, AbstractSkill::SKILL_TYPE nSkillType, Combo_Skill_Param& comboSkillParam)
{
	// 首发参数;
	comboSkillParam.nFirstSkillType = nSkillType;

	// 1. 读取普攻或主攻属性;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (nullptr == heroParam)
	{
		CCLOG("ERROR! ---> startAtk():heroParam not exists");
		return;
	}
	int nSkillId = -1;
	if (AbstractSkill::SKILL_TYPE::NORMAL_SKILL == nSkillType)
	{
		nSkillId = heroParam->mNormalSkillId;
	}
	else if (AbstractSkill::SKILL_TYPE::ACTIVE_SKILL == nSkillType)
	{
		nSkillId = heroParam->mActiveSkillId;
	}
	RowData* skillData = DataManager::getInstance()->searchSkillById(nSkillId);
	if (nullptr == skillData)
	{
		CCLOG("ERROR! ---> startAtk():skillData not exists");
		return;
	}

	// 2. 判断是否触发被动3;
	int nTriggerHeroId = isStartTrigger(nSkillId);
	if (-1 != nTriggerHeroId)
	{
		comboSkillParam.addHero(nHeroId, AbstractSkill::SKILL_TYPE::TRIGGER_SKILL);
	}

	// 3. 读取普攻或主攻结果，并尝试寻找追打;
	int typ = -1;
	if (AbstractSkill::SKILL_TYPE::NORMAL_SKILL == nSkillType)
	{
		typ = skillData->getIntData("staTyp");
	}
	else if (AbstractSkill::SKILL_TYPE::ACTIVE_SKILL == nSkillType)
	{
		typ = skillData->getIntData("addTpy");
	}
	if (-1 != typ)
	{
		// 添加结果;
		comboSkillParam.addHero(nHeroId, AbstractSkill::SKILL_TYPE::RUSH_SKILL,
			AbstractSkill::RESULT_STATE::NONE_STATE, getSkillState(typ));

		// 寻找追打;
		startRush(nHeroId, typ, comboSkillParam);
	}
}


void FormationModel::startRush(int nHeroId, int nPstType, Combo_Skill_Param& comboSkillParam)
{
	// 根据前置类型查找Hero，计算追打结果;
	int nNextHeroId = -1;
	int nRushResult = -1;

	// 先判断自己能否追打(优先);
	bool bSelfRush = false;
	if (isHeroCanRush(nHeroId, nPstType, nRushResult))
	{
		nNextHeroId = nHeroId;
		bSelfRush = true;
	}
	else
	{
		// 查找下一个满足条件的Hero;
		nNextHeroId = getNextHeroCanRush(nHeroId, nPstType, nRushResult);
	}

	// 若有;
	if (-1 != nNextHeroId && -1 != nRushResult)
	{
		// Combo数不等于Hero数，可能追打自身;
		// 不过滤重复值，就一定是Combo数等于Hero数;
		//vcComboHeroList.push_back(nNextHeroId);
		comboSkillParam.addHero(nNextHeroId, AbstractSkill::SKILL_TYPE::RUSH_SKILL,
			getSkillState(nPstType), getSkillState(nRushResult));
		setHeroRushRecord(nNextHeroId);

		// 判断是否触发被动3;
		HeroParam* param = HeroModel::getInstance()->searchHeroById(nNextHeroId);
		if (nullptr == param)
		{
			CCLOG("searchHeroById  FALSE!!!!!");
			return;
		}
		int nTriggerHeroId = isStartTrigger(param->mRushSkillId);
		if (-1 != nTriggerHeroId)
		{
			//vcTriggerHeroList.push_back(nTriggerHeroId);
			comboSkillParam.addHero(nNextHeroId, AbstractSkill::SKILL_TYPE::TRIGGER_SKILL);
		}

		// 结束控制;
		//   条件1: 未找到能追打的Hero;
		//   条件2：已循环至队首;
		//if ((nNextHeroId == m_nLogicFirstHeroOnRush/* || nNextHeroId == -1*/)
		//	&& !bSelfRush)
		//{
		//	return;
		//}

		// 继续查找NextHero的追打技能;
		startRush(nNextHeroId, nRushResult, comboSkillParam);
	}
	else
	{
		return;
	}
}


/********************************************************************
 *  函数名称: isHeroCanRush;
 *  功能描述: 判断Hero能否追打指定的前置状态，同时返回追打结果;
 *  参数列表: [in](int)nHeroId : 需要判断的英雄Id;
 *			  [in](int)nPreType : 指定的前置状态;
 *			  [out](int&)nRushResult : 追打结果;
 *  返回值  : (bool)能否追打;
 ********************************************************************/
bool FormationModel::isHeroCanRush(int nHeroId, int nPreType, int& nRushResult)
{
	// 每回合只能追打1次;
	auto iter = m_mapHeroRushRecord.find(nHeroId);
	if (iter != m_mapHeroRushRecord.end())
	{
		if (iter->second)
		{
			return false;
		}
	}

	// 读取Hero追打技能(被动1)并尝试追打nPreType;
	HeroParam* param = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (nullptr == param)
	{
		CCLOG("searchHeroById  FALSE!!!!!");
		return false;
	}
	int nRushSkillId = param->mRushSkillId;
	RowData* skillData = DataManager::getInstance()->searchSkillById(nRushSkillId);
	bool bRet = false;
	if (skillData != nullptr && nPreType == skillData->getIntData("preTyp"))
	{
		nRushResult = skillData->getIntData("pstTyp");
		bRet = true;
	}

	return bRet;
}


/********************************************************************
 *  函数名称: getNextHeroCanRush;
 *  功能描述: 根据英雄Id和后置状态，查找下一个能追打的英雄，同时返回追打结果;
 *  参数列表: [in](int)nPreHeroId : 指定的英雄Id;
 *			  [in](int)nPstType : 指定的后置状态;
 *			  [out](int&)nRushResult : 追打结果;
 *  返回值  : (int)可以追打该后置状态的英雄Id;
 ********************************************************************/
int FormationModel::getNextHeroCanRush(int nPreHeroId, int nPstType, int& nRushResult)
{
	for (unsigned int k = 0; k < m_vcFormation.size(); k++)
	{
		int nNextHeroId = m_vcFormation.at(k);
		if (nNextHeroId == FORMATION_EMPTY_POS)
		{
			continue;
		}

		// 条件1: 本回合未追打过;
		auto iter = m_mapHeroRushRecord.find(nNextHeroId);
		if (iter != m_mapHeroRushRecord.end())
		{
			if (false == iter->second)
			{
				// 条件2: 可追打给定的前置;
				if (isHeroCanRush(nNextHeroId, nPstType, nRushResult))
				{
					return nNextHeroId;
				}
			}
		}
	}

	return -1;
}


void FormationModel::setHeroRushRecord(int nHeroId, bool bRushOver)
{
	auto iter = m_mapHeroRushRecord.find(nHeroId);
	if (iter != m_mapHeroRushRecord.end())
	{
		iter->second = bRushOver;
	}
}


void FormationModel::resetHeroRushRecord(vector<int> vcFormation)
{
	m_mapHeroRushRecord.clear();
	for (unsigned int i = 0; i < vcFormation.size(); i++)
	{
		m_mapHeroRushRecord.insert(pair<int, bool>(vcFormation.at(i), false));
	}
}


void FormationModel::setHeroTriggerRecord(int nHeroId, bool bTriggerOver)
{
	auto iter = m_mapHeroTriggerRecord.find(nHeroId);
	if (iter != m_mapHeroTriggerRecord.end())
	{
		iter->second = bTriggerOver;
	}
}


void FormationModel::resetHeroTriggerRecord(vector<int> vcFormation)
{
	m_mapHeroTriggerRecord.clear();
	for (unsigned int i = 0; i < vcFormation.size(); i++)
	{
		m_mapHeroTriggerRecord.insert(pair<int, bool>(vcFormation.at(i), false));
	}
}


void FormationModel::filterComboList(vector<int>& vcComboList)
{
	vector<int> vcDst;
	vcDst.push_back(vcComboList.at(0));
	if (1 == vcComboList.size())
		return;

	for (unsigned int i = 1; i < vcComboList.size(); i++)
	{
		if (vcComboList.at(i) != vcComboList.at(i-1))
		{
			vcDst.push_back(vcComboList.at(i));
		}
	}
	vcComboList = vcDst;
}


int FormationModel::isStartTrigger(int nSkillId)
{
	m_nMaxHitCount += calcHitCount(nSkillId);
	return getTriggerHero();
}


int FormationModel::calcHitCount(int nSkillId)
{
	int nCount = 0;
	RowData* skillData = DataManager::getInstance()->searchSkillById(nSkillId);
	if (nullptr != skillData)
	{
		int nRng = skillData->getIntData("mRngTyp");
		switch (nRng)
		{
		case AbstractSkill::RANGE::RANDOM_1:
		case AbstractSkill::RANGE::RANDOM_2:
		case AbstractSkill::RANGE::RANDOM_3:
		case AbstractSkill::RANGE::RANDOM_4:
			{
				nCount += nRng;
			}
			break;

		case AbstractSkill::RANGE::HORIZONTAL:
		case AbstractSkill::RANGE::VERTICAL:
			{
				nCount += 3;
			}
			break;

		case AbstractSkill::RANGE::ALL:
			{
				nCount += getHeroOnFieldCount();
			}
			break;

		default:
			break;
		}
	}

	return nCount;
}


int FormationModel::getTriggerHero()
{
	for (unsigned int i = 0; i < m_vcFormation.size(); i++)
	{
		if (FORMATION_EMPTY_POS == m_vcFormation.at(i))
			continue;

		// 是否共用回合内追打名额;
		/*
		auto iter = m_mapHeroRushRecord.find(m_vcFormation.at(i));
		if (iter != m_mapHeroRushRecord.end())
		{
			if (true == iter->second)
				continue;
		}
		*/

		// 读取Hero连击技能(被动3)并判断;
		HeroParam* param = HeroModel::getInstance()->searchHeroById(m_vcFormation.at(i));
		if (nullptr == param)
		{
			CCLOG("searchHeroById  FALSE!!!!!");
			return -1;
		}
		int nTriggerSkillId = param->mTriggerSkillId;
		RowData* skillData = DataManager::getInstance()->searchSkillById(nTriggerSkillId);
		if (nullptr != skillData && skillData->getIntData("triTyp") == 4)
		{
			// 1. Hero的被动3在本轮追打中是否已被触发过;
			// 2. 由combo次数判断是否触发被动3追打;
			bool bFound = false;
			auto iter = m_mapHeroTriggerRecord.find(m_vcFormation.at(i));
			if (iter != m_mapHeroTriggerRecord.end() && iter->second)
			{
				bFound = true;
			}
			if ( !bFound && m_nMaxHitCount >= skillData->getIntData("triAmn"))
			{
				// 记录被动3追打并返回;
				setHeroTriggerRecord(m_vcFormation.at(i));
				return m_vcFormation.at(i);
			}
		}
	}

	return FORMATION_EMPTY_POS;
}


void FormationModel::procUploadFormation(bool bSucceed)
{
	updateUI(OBS_PARAM_TYPE_UPLOAD, (void*)(&bSucceed));
	resetFormation();
}

int FormationModel::calcFcSingle(int nHeroId)
{
	// 计算Hero战斗力;
	//int nFc = getBattlePowerPartA(nHeroId) + getBattlePowerPartB(nHeroId);
	int nFc = HeroModel::getInstance()->searchHeroById(nHeroId)->mFighting;
	return nFc;
}

int FormationModel::calcFcTeam()
{
	int nFc = 0;

	for (int i = 0; i < m_vcFormation.size(); ++i)
	{
		if (FORMATION_EMPTY_POS == m_vcFormation.at(i))
		{
			continue;
		}

		if (m_nMode == F_MODE_TRAIL)
		{
			auto iter = m_mapTrailAttr.find(m_vcFormation.at(i));
			if (iter != m_mapTrailAttr.end())
			{
				nFc += iter->second.fFc;
			}
		}
		else
		{
			nFc += calcFcSingle(m_vcFormation.at(i));
		}
	}

	// 普通模式下，战斗力不可能为0，如果为0，就直接从英雄模块取，再取到0，错的地方就多了，这里处理不了;
	if (m_nMode != F_MODE_TRAIL && nFc <= 0)
	{
		nFc = HeroModel::getInstance()->getTotalFighting();
	}

	updateFcTeam(nFc);
	return nFc;
}

float FormationModel::getBattlePowerPartA( int nHeroId )
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (heroParam == nullptr)
	{
		CCLOG("searchHeroById  FALSE!!!!!");
		return 0;
	}

	// 被动1伤害系数;
	float fDmgCoe = 0.0f;
	RowData* dataSkill = DataManager::getInstance()->searchSkillById(heroParam->mRushSkillId);
	if (nullptr != dataSkill)
	{
		fDmgCoe = dataSkill->getFloatData("dmgCoe");
	}

	// '''英雄总攻击属性*攻击价值*(1+暴击概率*暴击系数+连击概率*被动1伤害系数)'''
	float fCritP = ((float)heroParam->mCritP)/100.0f;
	float fCritCoe = ((float)heroParam->mCritCoe)/100.0f;
	float fComboP = ((float)heroParam->mComboP)/100.0f;
	return getHeroAttriValue(heroParam, "atk") * m_fAtkCoe * (1.0f + fCritP * fCritCoe + fComboP * fDmgCoe);
}

float FormationModel::getBattlePowerPartB( int nHeroId )
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (heroParam == nullptr)
	{
		CCLOG("searchHeroById  FALSE!!!!!");
		return 0;
	}

	// '''魔防*防御价值+物防*防御价值+血量*血量价值*(1+闪避概率)'''
	float fMissP = ((float)heroParam->mMissP) / 100.0f;
	return getHeroAttriValue(heroParam, "mdd") * m_fMddCoe + getHeroAttriValue(heroParam, "pdd") * m_fPddCoe
		+ getHeroAttriValue(heroParam, "hp") * m_fHpCoe * (1.0f + fMissP);
}

float FormationModel::getHeroAttriValue( HeroParam* heroParam, string strName )
{
	if (nullptr == heroParam)
	{
		return 0.0f;
	}

	// 是否上阵;
	float nIsOnField = isHeroOnField(heroParam->mHeroId) ? 1.0f : 0.0f;

	float fValue = 0.0f;
	if (strName.compare("atk") == 0)
	{
		fValue = heroParam->mAtk + nIsOnField * m_yueliAtk;
	}
	else if (strName.compare("hp") == 0)
	{
		fValue = heroParam->mHp + nIsOnField * m_yueliHp;
	}
	else if (strName.compare("mdd") == 0)		// 魔防;
	{
		fValue = heroParam->mMDef + nIsOnField * m_yueliMdd;
	}
	else if (strName.compare("pdd") == 0)		// 物防;
	{
		fValue = heroParam->mPDef + nIsOnField * m_yueliPdd;
	}

	return fValue;
}

void FormationModel::updateHeroEquipment( map<int, int> mapHeroEquipment )
{
	if (mapHeroEquipment.size() == 0)
	{
		return;
	}

	m_mapHeroEquipment = mapHeroEquipment;
}

void FormationModel::updateFcTeam(int nFcTeam)
{
	m_nFcTeam = nFcTeam;
}

void FormationModel::updateFcSingleA( map<int, float> mapFcSingleA )
{
	m_mapFcSingleA = mapFcSingleA;
}

void FormationModel::updateFcSingleB( map<int, float> mapFcSingleB )
{

	m_mapFcSingleB = mapFcSingleB;
}

void FormationModel::constructUIHeroParam( int nHeroId, UI_HERO_LIST_PARAM& param )
{
	// 不需要查询的参数;
	param.nHeroId = nHeroId;
	param.nPos = getPosOfHero(nHeroId);
	param.bIsOnField = isHeroOnField(nHeroId);

	// 血量;
	if (m_nMode == F_MODE_TRAIL)
	{
		HeroParam* _heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
		if (_heroParam != nullptr)
		{
			param.fHpPercent = ((float)(_heroParam->mHpCur) / (float)(_heroParam->mHp)) * 100.0f;
		}
	}

	// 需要查询英雄数据的参数;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(nHeroId);
	if (nullptr != heroParam)
	{
		param.nResId = heroParam->mResId;
		param.nStar  = heroParam->mHeroStars;
		param.nHeroId = heroParam->mHeroId;
		param.nLv = heroParam->mLevel;

		// 查询被动1技能的前置和后置;
		RowData* dataSkill = DataManager::getInstance()->searchSkillById(heroParam->mRushSkillId);
		if (nullptr != dataSkill)
		{
			param.nPreState  = getSkillState(dataSkill->getIntData("preTyp"));
			param.nNextState = getSkillState(dataSkill->getIntData("pstTyp"));
		}
		else
		{
			param.nPreState  = AbstractSkill::NONE_STATE;
			param.nNextState = AbstractSkill::NONE_STATE;
			CCLOG("Not Found Skill Data id = %d", heroParam->mRushSkillId);
		}
	}
}

void FormationModel::setFormationMode( Formation_Mode mode )
{
	 m_nMode = mode;
	 if (m_nMode == F_MODE_TRAIL)
	 {
		 m_vcFormation.clear();
	 }
}

void FormationModel::setTrailAttr( map<int, Trail_Formation_Attr>  mapTrailAttr )
{
	// 试炼属性;
	m_mapTrailAttr = mapTrailAttr;

	// 设置阵型数据;

	/*
	vector<int>  vcTrailFormation;
	for (auto it : m_mapTrailAttr)
	{
		vcTrailFormation.push_back(it.first);
	}
	*/

	// 更新阵型;
	//updateFormation(vcTrailFormation);
}

void FormationModel::resetFormation()
{
	// 试炼模式下退出后还原阵型数据;
	if (m_nMode == F_MODE_TRAIL)
	{
		m_vcFormation = m_vcFormationBackup;
	}
}

bool FormationModel::sortComboList( const Combo_Skill_Param& param1, const Combo_Skill_Param& param2 )
{
	bool  bRet = false;

	// 1.优先显示最大数量;
	if (param1.nMaxCombo > param2.nMaxCombo)
	{
		bRet = true;
	}
	// 2.数量相同;
	else if (param1.nMaxCombo == param2.nMaxCombo)
	{
		if (param1.nFirstSkillType != param2.nFirstSkillType)
		{
			// 2.1 优先显示主动技能首发序列;
			if (param1.nFirstSkillType == AbstractSkill::ACTIVE_SKILL)
			{
				bRet = true;
			}
		}
		// 2.2 首发技能类型相同，显示首发位置序号较小的序列;
		else
		{
			if (param1.nIndex < param2.nIndex)
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

bool FormationModel::isFormationChanged()
{
	if (m_vcFormation.size() != m_vcFormationBackup.size())
		return true;

	for (int i = 0; i < FORMATION_MAX_POS_COUNT; ++i)
	{
		if (m_vcFormation.at(i) != m_vcFormationBackup.at(i))
		{
			return true;
		}
	}
	return false;
}

int FormationModel::getFcTeam()
{
	return calcFcTeam();
}
