#include "TowerModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "HeroModule/HeroModel.h"
#include "VIPModule/VipModel.h"
#include "MainModule/MainModel.h"

TowerModel* TowerModel::m_pInstance = nullptr;

TowerModel::TowerModel(void)
	: m_bTriggerBuff(false)
	, m_bIsTowerSweeping(false)
	, m_nTreasureBoxId(-1)
	, m_nTreasureId(-1)
	, m_nTreasureCount(0)
{
	m_mapTrailOp.clear();
	while (!m_queTreasureRefreshData.empty())
		m_queTreasureRefreshData.pop();

	while (!m_queTreasureOpenData.empty())
		m_queTreasureOpenData.pop();
}


TowerModel::~TowerModel(void)
{
}

TowerModel* TowerModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new TowerModel;
	}
	return m_pInstance;
}

void TowerModel::destroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

/*
int TowerModel::getRefreshTreasureCost()
{
	// 查询宝藏配置表，取得刷新宝藏的消耗;
	RowData* _row = DataManager::getInstance()->searchTrailTreasureById(m_nTreasureBoxId);
	assert(_row != nullptr);
	int nRefreshGold = _row->getIntData("refreshcost");
	return nRefreshGold;
}*/

void TowerModel::updateTrailMainInfo( TrailMainInfo info )
{
	m_trailMainInfo = info;
	MainModel::getInstance()->setTrialScore(m_trailMainInfo.nScore);
	DataTable* _table = DataManager::getInstance()->getDTTrail();
	m_trailMainInfo.nTotalFloor = _table->getTableData().size();

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_TRAIL_MAIN_INFO, (void*)(&m_trailMainInfo));
}

void TowerModel::getBuffInfo( int nBuffId, string& name, string& desc )
{
	RowData* _row = DataManager::getInstance()->searchBuffById(nBuffId);
	assert(_row != nullptr);
	name.clear();
	name = STR_ANSI(_row->getStringData("name"));
	desc.clear();
	desc = STR_ANSI(_row->getStringData("capTxt"));
}

TOWER_FIGHT_TYPE TowerModel::getFightType( TOWER_TYPE nTowerType, int nFloor )
{
	// 爬塔每一层都是战斗;
	if (nTowerType == TOWER_TYPE_TOWER)
	{
		return TFT_FIGHT;
	}

	// 试炼查表;
	RowData* _row = DataManager::getInstance()->searchTrailById(nFloor);
	assert(_row != nullptr);
	TOWER_FIGHT_TYPE fightType;
	int nType = _row->getIntData("type");
	if (nType == 1)
	{
		fightType = TFT_FIGHT;
	}
	else if (nType == 2)
	{
		fightType = TFT_REWARD;
	}
	return fightType;
}

std::string TowerModel::getClearCondition( int nFloor )
{
	int nBarrierId = towerFloor2Id(nFloor);
	auto iter = m_mapBarrierTower.find(nBarrierId);
	assert(iter != m_mapBarrierTower.end());
	return iter->second.strDescription;
}

int TowerModel::getOpResId( TOWER_TYPE type, int nFloor )
{
	if (type == TOWER_TYPE_TOWER)
	{
		// 取BossShow;
		int nId = towerFloor2Id(nFloor);
		auto iter = m_mapBarrierTower.find(nId);
		assert(iter != m_mapBarrierTower.end());
		return iter->second.nBossShowId;
	}
	else if (type == TOWER_TYPE_TRAIL)
	{
		// 所有英雄随机;
		Vector<HeroParam*>* heroList = HeroModel::getInstance()->getHeroList();
		assert(heroList != nullptr && heroList->size() > 0);
		int n = rand()%heroList->size();
		return heroList->at(n)->mResId;
	}

	// 默认鲁智深;
	return 41003;
}

void TowerModel::updateTrailFloor()
{
	updateUI(OBS_PARAM_TYPE_TRAIL_FLOOR, (void*)(&(m_trailMainInfo.nCurFloor)));
}

void TowerModel::updateBuff( int nCurFloor )
{
	/*// 查表;
	RowData* _row = DataManager::getInstance()->searchTrailById(nCurFloor);
	assert(_row != nullptr);
	string strBuff = _row->getStringData("buffId");
	StringFormat::parseCsvStringMap(strBuff, m_mapBuff);*/

	// 服务器下发buff;
	vector<UI_BUFF_PARAM>  vcBuffParam;
	m_mapBuffState.clear();
	for (int nIndex = 0; nIndex < m_vcBuff.size(); ++nIndex)
	{
		// 初始化buff兑换状态;
		m_mapBuffState.insert(pair<int, bool>(m_vcBuff.at(nIndex).first, false));

		// 构造buff参数;
		UI_BUFF_PARAM _param;
		_param.nIndex = nIndex;
		_param.nBuffId = m_vcBuff.at(nIndex).first;
		_param.nPrice = m_vcBuff.at(nIndex).second;

		// desc查buff表;
		RowData* __row = DataManager::getInstance()->searchBuffById(_param.nBuffId);
		assert(__row != nullptr);
		_param.strBuffDesc = __row->getStringData("capTxt");
		_param.strBuffName = __row->getStringData("name");

		vcBuffParam.push_back(_param);
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_TRAIL_BUFF_CONTENT, (void*)(&vcBuffParam));

	// 然后再根据各个buff的价格刷新一次是否可兑换;
	//updateUIBuffState();
}

void TowerModel::updateUIBuffState()
{
	// ;
	updateUI(OBS_PARAM_TYPE_TRAIL_BUFF_STATE, (void*)(&m_mapBuffState));

	// buff状态;
	//map<int, bool>  mapBuffState;

	// 条件;
	// 1. 价格小于剩余星星数量(废弃);
	// 2. 本轮次未兑换过;
	//int nStar = m_trailMainInfo.nStar;
	//for (auto iter = m_mapBuff.begin(); iter != m_mapBuff.end(); ++iter)
	//{
	//	auto _it = m_mapBuffState.find(iter->first);
	//	assert(_it != m_mapBuffState.end());
	//	bool bEnabled = (/*(iter->second <= nStar) && */!_it->second) ? true : false;
	//	mapBuffState.insert(pair<int, bool>(iter->first, bEnabled));
	//}

	//updateUI(OBS_PARAM_TYPE_TRAIL_BUFF_STATE, (void*)(&mapBuffState));
}

void TowerModel::updateUI( OBS_PARAM_TYPE_TOWER type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}

int TowerModel::getBuffRange( int nBuffId )
{
	// buff生效范围(0-全体，1-单体);
	// 9011 - 0
	// 9022 - 0
	// 9033 - 0
	// 9044 - 0
	// 9055 - 1
	// (写死，默认全体);
	return (nBuffId == 9055) ? 1 : 0;
}

void TowerModel::setBuffTriggered( bool bIsTriggered )
{
	m_bTriggerBuff = bIsTriggered;

	// 重置参数;
	if (!bIsTriggered)
	{
		m_vcBuff.clear();
		m_mapBuffState.clear();
	}
}

void TowerModel::updateBuffState( int nBuffId, bool bUsed /*= true*/ )
{
	// 1. 刷新数据;
	auto iter = m_mapBuffState.find(nBuffId);
	assert(iter != m_mapBuffState.end());
	iter->second = bUsed;

	// 2. 刷新UI;
	updateUIBuffState();
}

void TowerModel::updateStarCount( int nCount, map<string, float>  mapBuff )
{
	// 更新星星数量;
	m_trailMainInfo.nStar = nCount;

	// 更新已兑换buff列表;
	m_trailMainInfo.mapBuffEffect = mapBuff;

	// 更新主界面所有信息;
	updateTrailMainInfo(m_trailMainInfo);
}

void TowerModel::updateGold( int nGold )
{
	m_trailMainInfo.nGold = nGold;

	// 更新主界面所有信息;
	updateTrailMainInfo(m_trailMainInfo);
}

bool TowerModel::isBuffAllApplied()
{
	// 查找未兑换并且足够兑换的buff;
	bool bFound = false;
	for (auto it = m_mapBuffState.begin(); it != m_mapBuffState.end(); ++it)
	{
		// 1.未兑换;
		if (!it->second)
		{
			auto iter = find_if(m_vcBuff.begin(), m_vcBuff.end(), [=](pair<int, int>  _pair){
				return _pair.first == it->first;
			});
			assert(iter != m_vcBuff.end());

			// 2.足够星星兑换;
			if (m_trailMainInfo.nStar >= iter->second)
			{
				bFound = true;
				break;
			}
		}
	}

	return !bFound;
}

/*
int TowerModel::getNextTreasureCost()
{
	// 查询宝藏配置表，取得开启宝藏的消耗;
	RowData* _row = DataManager::getInstance()->searchTrailTreasureById(m_nTreasureBoxId);
	assert(_row != nullptr);
	vector<int>  vcCost;
	StringFormat::parseCsvStringVec(_row->getStringData("cost"), vcCost);
	assert(vcCost.size() == 6);

	int nNextGold = -1;
	if (m_nTreasureOpendTimes < 6)
	{
		nNextGold = vcCost.at(m_nTreasureOpendTimes);
	}
	return nNextGold;
}*/

void TowerModel::onTreasureOpend( int nTreasureId, int nCount )
{
	// 保存开启的宝藏，等UI来取;
	m_nTreasureId = nTreasureId;
	m_nTreasureCount = nCount;
	CCASSERT(!m_queTreasureOpenData.empty(), "");
	m_queTreasureOpenData.pop();
}

void TowerModel::firstUpdateTreasure()
{
	assert(m_nTreasureBoxId != -1);
	refreshTreasure(m_nTreasureBoxId, true);
}

bool TowerModel::refreshTreasure( int nBoxId, bool bfirst /*= false*/ )
{
	// 重置当前开启的宝藏Id;
	m_nTreasureId = -1;
	m_nTreasureCount = 0;

	// 重置开启记录;
	while (!m_queTreasureOpenData.empty())
		m_queTreasureOpenData.pop();
	m_queTreasureOpenData = m_queTreasureOpenData_backup;

	// 是否扣减刷新次数;
	CCASSERT(!m_queTreasureRefreshData.empty(), "");
	if (!bfirst)
		m_queTreasureRefreshData.pop();

	// 更新一批宝藏数据;
	multimap<int, int> mapInfo;
	constructTreasureParam(nBoxId, mapInfo);
	updateUI(OBS_PARAM_TYPE_TRAIL_REFRESH_TREASURE, (void*)(&mapInfo));
	return true;
}

vector<int> TowerModel::isTreasureOpend()
{
	vector<int> vcRet;

	// 已开启成功;
	if (m_nTreasureId != -1)
	{
		vcRet.push_back(m_nTreasureId);
		vcRet.push_back(m_nTreasureCount);
		m_nTreasureId = -1;
		m_nTreasureCount = 0;
		return vcRet;
	}

	// 开启失败或服务器未返回;
	return vcRet;
}


void TowerModel::updateTrailOp( map<int, TrailOp> mapTrailOp )
{
	m_mapTrailOp = mapTrailOp;

	// 查表;
	RowData* _row = DataManager::getInstance()->searchTrailById(getTrailCurFloor());
	vector<int>  vcStar;
	vector<int>  vcScore;
	if (_row != nullptr)
	{
		string strStar = _row->getStringData("star");
		StringFormat::parseCsvStringVec(strStar, vcStar);

		string strScore = _row->getStringData("score");
		StringFormat::parseCsvStringVec(strScore, vcScore);
	}
	else
	{
		return;
	}

	assert(vcScore.size() == 3);
	assert(vcStar.size() == 3);

	// 构造UI参数;
	vector<UI_TOWER_OP_PARAM> vcParam;
	for (unsigned int i = 0; i < 3; ++i)
	{
		UI_TOWER_OP_PARAM uiParam;
		uiParam.nLv = i+1;
		auto iter = m_mapTrailOp.find(uiParam.nLv);
		assert(iter != m_mapTrailOp.end());
		uiParam.nType = iter->second.nType;
		uiParam.strOpName = iter->second.strName;
		uiParam.nOpLv = iter->second.nLv;
		uiParam.nOpFc = iter->second.nAtk;
		uiParam.vcOpFormation  = iter->second.vcFormation;
		for (int k = 0 ; k < uiParam.vcOpFormation.size(); ++k)
		{
			uiParam.vcOpFormationQua.push_back(uiParam.vcOpFormation.at(k).stars);
			if (uiParam.vcOpFormation.at(k).id == -1)
				continue;

			// 区分英雄和怪物;
			if (uiParam.nType > 0)
			{
				RowData* _row = DataManager::getInstance()->searchHeroAdvanceById(uiParam.vcOpFormation.at(k).id);
				assert(_row != nullptr);
				uiParam.vcOpFormationQua.at(k) = _row->getIntData("qua");
			}
			else if (uiParam.nType < 0)
			{
				RowData* _row = DataManager::getInstance()->searchMonsterById(uiParam.vcOpFormation.at(k).id);
				assert(_row != nullptr);
				uiParam.vcOpFormationQua.at(k) = _row->getIntData("qua");
			}
			else
			{
				assert(false);
			}
		}
		uiParam.nScore = vcScore.at(i);
		uiParam.nStar = vcStar.at(i);
		vcParam.push_back(uiParam);
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_TRAIL_OP, (void*)(&vcParam));
}

void TowerModel::updateTrailOpState( int nValidLv )
{
	updateUI(OBS_PARAM_TYPE_TRAIL_OP_STATE, (void*)(&nValidLv));
}

bool TowerModel::checkTrailFloorIsValid( int nFloor )
{
	RowData* _row = DataManager::getInstance()->searchTrailById(nFloor);
	return (_row != nullptr);
}

void TowerModel::updateTrailReward( map<int, int> mapReward )
{
	updateUI(OBS_PARAM_TYPE_TRAIL_REWARD, (void*)(&mapReward));
}

void TowerModel::constructTreasureParam( int nBoxId, multimap<int, int>& mapInfo )
{
	RowData* _row = DataManager::getInstance()->searchTrailTreasureById(nBoxId);
	assert(_row != nullptr);

	// 读表，取得宝藏配置数据;
	vector<vector<int> >  vcBox;
	string strBox = _row->getStringData("itemBox");
	StringFormat::parseCsvStringVec(strBox, vcBox);
	for (auto it : vcBox)
	{
		mapInfo.insert(pair<int, int>(it.at(0), it.at(1)));
	}
}

void TowerModel::resetTreasureData()
{
	m_nTreasureBoxId = -1;
	m_nTreasureId = -1;
	m_nTreasureCount = 0;
	while (!m_queTreasureRefreshData.empty())
		m_queTreasureRefreshData.pop();

	while (!m_queTreasureOpenData.empty())
		m_queTreasureOpenData.pop();
}

bool TowerModel::isLogicCurFloor( TOWER_TYPE nType, int nFloor )
{
	if (nType == TOWER_TYPE_TRAIL)
	{
		// 当前楼层可能有两次进入;
		// 第1次: 正常进入;
		// 第2次: 触发了buff或者宝藏;
		return (TowerModel::getInstance()->getTrailCurFloor() == nFloor);
	}
	else if (nType == TOWER_TYPE_TOWER)
	{
		return (TowerModel::getInstance()->getTowerCurFloor() == nFloor);
	}
}

void TowerModel::resetTrail( int date )
{
	// 重置层数;
	m_trailMainInfo.nCurFloor = 1;

	// 记录重置日期;
	//m_trailMainInfo.nTrailResetDate = date;

	// 更新UI;
	updateUI(OBS_PARAM_TYPE_TRAIL_RESET, (void*)(&date));
}

int TowerModel::getTrailCurFloor()
{
	 return m_trailMainInfo.nCurFloor;
}

void TowerModel::getBuffDstHeros(vector<HeroParam*>& vcDstHeroList)
{
	// 遍历英雄列表;
	Vector<HeroParam*>* vcHeroList = HeroModel::getInstance()->getHeroList();
	for (auto iter = vcHeroList->begin(); iter != vcHeroList->end(); ++iter)
	{
		// 取出所有残血的英雄;
		if ((*iter)->mHpCur < (*iter)->mHp)
		{
			vcDstHeroList.push_back(*iter);
		}
	}
}

void TowerModel::updateRanking( TOWER_TYPE nType, const UI_TRAIL_RANKING_PARAM& uiParam )
{
	OBS_PARAM_TYPE_TOWER  paramType = (nType == TOWER_TYPE_TRAIL) ? OBS_PARAM_TYPE_TRAIL_RANKING : OBS_PARAM_TYPE_TOWER_RANKING;
	updateUI(paramType, (void*)(&uiParam));
}

void TowerModel::updateTowerMainInfo( TowerMainInfo info )
{
	m_towerMainInfo = info;
	m_towerMainInfo.nTotalFloor = m_mapBarrierTower.size();

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_TOWER_MAIN_INFO, (void*)(&m_towerMainInfo));
}

void TowerModel::resetTower()
{
	// 重置层数;
	m_towerMainInfo.nCurFloor = 1;
	m_towerMainInfo.bEnd = false;

	// 剩余可重置次数(免费的);
	if (m_towerMainInfo.nLeftResetCount > 0)
	{
		m_towerMainInfo.nLeftResetCount--;
	}
	else
	{
		// Vip已重置次数;
		MainModel::getInstance()->getTradingStatus()->mTowerResetNum++;
	}

	// 更新UI;
	updateUI(OBS_PARAM_TYPE_TOWER_RESET, nullptr);
}

bool TowerModel::checkTowerFloorIsValid( int nFloor )
{
	//RowData* _row = DataManager::getInstance()->searchTrailById(nFloor);
	//return (_row != nullptr);

	auto iter = m_mapBarrierTower.find(towerFloor2Id(nFloor));
	return iter != m_mapBarrierTower.end();
}

void TowerModel::updateTowerFloor()
{
	updateUI(OBS_PARAM_TYPE_TOWER_FLOOR, (void*)(&(m_towerMainInfo.nCurFloor)));
}

int TowerModel::towerFloor2Id( int nFloor )
{
	int nId = nFloor + 200000;
	auto iter = m_mapBarrierTower.find(nId);
	assert(iter != m_mapBarrierTower.end());
	return nId;
}

int TowerModel::towerId2Floor( int nBarrierId )
{
	auto iter = m_mapBarrierTower.find(nBarrierId);
	assert(iter != m_mapBarrierTower.end());
	return (iter->first - 200000);
}

void TowerModel::updateTowerFloorInfo( map<int, BarrierInfo>  mapInfo )
{
	m_mapBarrierTower = mapInfo;
}

/*
void TowerModel::onTowerBuyChallengeTimes( bool bResult )
{
	updateUI(OBS_PARAM_TYPE_TOWER_BUY_CHALLENGE_TIMES, (void*)(&bResult));
}
*/

void TowerModel::setScore( int nScore )
{
	m_trailMainInfo.nScore = nScore;
	updateUI(OBS_PARAM_TYPE_TRAIL_SCORE, (void*)(&nScore));
}

void TowerModel::updateTowerRewardPreview()
{
	// <floor, <Id, Count> >;
	map<int, vector<int> >  mapRewardPreview;
	vector<RewardPreviewParam>  vcReward;

	// 查关卡数据;
	map<int, BarrierInfo>* mapInfo = TowerModel::getInstance()->getTowerFloorInfo();
	if (nullptr != mapInfo && !mapInfo->empty())
	{
		for (auto iter = mapInfo->begin(); iter != mapInfo->end(); ++iter)
		{
			RewardPreviewParam  param;
			param.nIndex = TowerModel::getInstance()->towerId2Floor(iter->first);
			param.mapReward = iter->second.mapRewardShow;
			if (param.mapReward.size() > 0)
			{
				vcReward.push_back(param);
			}
		}
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_TOWER_REWARD_SHOW, (void*)(&vcReward));
}

int TowerModel::getSweepSingleTime()
{
	// 查表;
	return 30;
}

void TowerModel::onCountDown( int nSec )
{
	updateUI(OBS_PARAM_TYPE_TOWER_SWEEP_COUNT_DOWN, (void*)(&nSec));
}

void TowerModel::updateSweepSettlement( TowerSweepSettlement* result )
{
	updateUI(OBS_PARAM_TYPE_TOWER_SWEEP_SETTLEMENT, (void*)result);
	m_towerMainInfo.bSweepResult = false;
}

int TowerModel::getNextValidVip( int& nNextValue )
{
	// 当前VIP属性;
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = getCurMaxTimes();
	int nNextVipLv = nCurVipLv;
	nNextValue = nCurValue;

	// 下一级;
	int nLoopLv = nNextVipLv;
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	RowData* _rowNext = _dt->searchDataById(++nLoopLv);
	while (_rowNext != nullptr)
	{
		// 下一级的值;
		nNextValue = _rowNext->getIntData("tower");
		if (nNextValue > nCurValue)
		{
			nNextVipLv = nLoopLv;
			break;
		}
		else
			_rowNext = _dt->searchDataById(++nLoopLv);
	}

	return nNextVipLv;
}

int TowerModel::getCurVipLeftTimes()
{
	return getCurMaxTimes() - MainModel::getInstance()->getTradingStatus()->mTowerResetNum;
}

int TowerModel::getCurMaxTimes()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData("tower");
	}
	return nCurValue;
}

int TowerModel::getTowerResetCost()
{
	int nNextTime = MainModel::getInstance()->getTradingStatus()->mTowerResetNum + 1;
	RowData* _row = DataManager::getInstance()->searchToolSystemById(10075);
	assert(_row != nullptr);
	vector<int>  vcPrice;
	StringFormat::parseCsvStringVec(_row->getStringData("value"), vcPrice);
	assert(!vcPrice.empty());
	int nIndex = (nNextTime > vcPrice.size() ? vcPrice.size()-1 : nNextTime-1);
	return vcPrice.at(nIndex);
}

void TowerModel::updateTrailCurFloorNum( int delta /*= 1*/ )
{
	//if (delta <= 0)
	//	return;

	if (checkTrailFloorIsValid(m_trailMainInfo.nCurFloor+delta))
	{
		m_trailMainInfo.nCurFloor += delta;
	}
	else
	{
		m_trailMainInfo.bEnd = true;
	}
}

int TowerModel::getBuffPrice( int nBuffId )
{
	if (m_vcBuff.empty())
	{
		return 0;
	}

	auto it = find_if(m_vcBuff.begin(), m_vcBuff.end(), [=](pair<int, int> _pair){
		return _pair.first == nBuffId;
	});

	if (it != m_vcBuff.end())
	{
		return it->second;
	}

	return 0;
}

void TowerModel::checkUISweepSettlement()
{
	bool bDone = true;
	updateUI(OBS_PARAM_TYPE_TOWER_UI_SWEEP_SETTLEMENT, (void*)(&bDone));
}

int TowerModel::getTreasureOpendTimes()
{
	int nTimes = 6-m_queTreasureOpenData.size();
	nTimes = max(0, nTimes);
	nTimes = min(nTimes, 6);
	return nTimes;
}

int TowerModel::getNextTreasureCost()
{
	// 附加次数判定;
	int nCost = -1;
	if (getTreasureOpendTimes() < 6)
	{
		CCASSERT(!m_queTreasureOpenData.empty(), "");
		nCost = m_queTreasureOpenData.front();
	}
	return nCost;
}

int TowerModel::getRefreshTreasureCost()
{
	int nCost = -1;
	if (!m_queTreasureRefreshData.empty())
	{
		nCost = m_queTreasureRefreshData.front();
	}
	return nCost;
}
