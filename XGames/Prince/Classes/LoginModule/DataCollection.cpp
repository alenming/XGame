#include "DataCollection.h"
#include "BossModule/BossModel.h"
#include "ChapterModule/BarrierModel.h"
#include "CommonBuy/CommonBuyModel.h"
#include "DataManager/DataManager.h"
#include "FightModule/FightModel.h"
#include "GuideCenter/GuideManager.h"
#include "HeroModule/HeroModel.h"
#include "HeroModule/FateModel.h"
#include "LevelUp/LevelUpController.h"
#include "LoginModule/LoginModel.h"
#include "LoveModule/LoveModel.h"
#include "MailModule/MailModel.h"
#include "MainModule/MainModel.h"
#include "MainTaskModule/MainTaskModel.h"
#include "MarvellousActive/MarvellousActiveModel.h"
#include "MazeModule/MazeModel.h"
#include "NewAreaActivityModule/NewAreaActivityModel.h"
#include "PlayerInfoModule/PlayerInfoModel.h"
#include "PvpModule/PvpModel.h"
#include "RefineryModule/RefineryModel.h"
#include "ResourceManager/ResourceLoader.h"
#include "RobModule/RobModel.h"
#include "Shopexc/ShopexcModel.h"
#include "SignModule/LayerSignModel.h"
#include "SmithyModule/SmithyModel.h"
#include "NewStarsCampModule/StarsCampModel.h"
#include "StarsSuModule/StarsSuModel.h"
#include "TacticsModule/ModelTactics.h"
#include "TakeCardModule/TakeCardModel.h"
#include "TakeCardModule/TakeCardController.h"
#include "ToolModule/ToolModel.h"
#include "TowerModule/TowerModel.h"
#include "Utils/SoundUtils.h"
#include "VIPModule/VipModel.h"
#include "MailModule/MailController.h"
#include "MainTaskModule/MainTaskController.h"
#include "SignModule/LayerSignController.h"
#include "ChapterModule/ChapterController.h"
#include "MazeModule/MazeController.h"
#include "FightModule/FightController.h"
#include "RobModule/RobController.h"
#include "Utils/LocalTimer.h"
#include "FriendModule/ChatController.h"
#include "FriendModule/FriendController.h"
#include "NewAreaActivityModule/NewAreaActivityController.h"
#include "LoginModule/LoginController.h"
#include "TacticsModule/TacticsController.h"
#include "VIPModule/VipController.h"
#include "RankTopModule/RankTopModel.h"
#include "OperateActModule/OperateActModel.h"
#include "FirstRecharge/FirstRechargeModel.h"
#include "OperateActModule/OperateActController.h"
#include "TalentModule/TalentModel.h"
#include "ChapterModule/ChapterModel.h"
#include "BossModule/BossActStateListener.h"
#include "ChapterModule/Activity/ActivityModel.h"
#include "ChapterModule/Activity/ActivityController.h"
#include "GroupModule/MyGroupModel.h"
#include "GroupModule/JoinGroupModel.h"
#include "GroupModule/MyGroupController.h"
#include "ArtifactModule/ArtifactModel.h"
#include "PvpModule/PvpController.h"
#include "UpgradeModule/UpgradeController.h"
#include "LimitTimeHeroModule/LimitTimeHeroModel.h"
#include "GoldActivity/GoldActivityController.h"
#include "GoldActivity/GoldActivityModel.h"
#include "GroupBossModule/GroupBossController.h"
#include "GroupBossModule/GroupBossModel.h"
#include "HeroRelationModule/HeroRelationModel.h"
#include "DoubleBonus/DoubleBonusModel.h"

void DataCollection::clearData()
{
	DataManager::destroyInstance();
	GuideManager::destroyInstance();
	LevelUpController::destroyInstance();
	ResourceLoader::destroyInstance();
	LocalTimer::destroyInstance();

	//销毁所有单例数据模型
	BossModel::destroyInstance();
	BarrierModel::destroyInstance();
	ChapterModel::destroyInstance();
	CommonBuyModel::destroyInstance();
	FightModel::destroyInstance();
	HeroModel::destroyInstance();
	FateModel::destroyInstance();
	LoginModel::destroyInstance();
	LoveModel::destroyInstance();
	MailModel::destroyInstance();
	MainModel::destroyInstance();
	MainTaskModel::destroyInstance();
	MarvellousActiveModel::destroyInstance();
	MazeModel::destroyInstance();
	NewAreaActivityModel::destroyInstance();
	PlayerInfoModel::destroyInstance();
	PvpModel::destroyInstance();
	PvpController::destroyInstance();
	RefineryModel::destroyInstance();
	RobModel::destroyInstance();
	ShopexcModel::destroyInstance();
	LayerSignModel::destroyInstance();
	SmithyModel::destroyInstance();
	StarsCampModel::destroyInstance();
	StarsSuModel::destroyInstance();
	ModelTactics::destroyInstance();
	TakeCardModel::destroyInstance();
	TakeCardController::destroyInstance();
	ToolModel::destroyInstance();
	TowerModel::destroyInstance();
	VipModel::destroyInstance();
	RankTopModel::destroyInstance();
	OperateActModel::destroyInstance();
	FirstRechargeModel::destroyInstance();
	TalentModel::destroyInstance();
	ActivityModel::destroyInstance();
	FriendModel::destroyInstance();
	MyGroupModel::destroyInstance();
	JoinGroupModel::destroyInstance();
	ArtifactModel::destroyInstance();
	LimitTimeHeroModel::destroyInstance();
	GroupBossModel::destroyInstance();
	HeroRelationModel::destroyInstance();

	//销毁所有单例controller
	MailController::destroyInstance();
	MainTaskController::destroyInstance();
	LayerSignController::destroyInstance();	
	ChapterController::destroyInstance();
	MazeController::destroyInstance();		//可以不是单例?
	TowerModel::destroyInstance();			//可以不是单例?
	FightController::destroyInstance();
	RobController::destroyInstance();
	VipController::destroyInstance();
	TacticsController::destroyInstance();
	BossActStateListener::destroyInstance();
	MyGroupController::destroyInstance();
	GroupBossController::destroyInstance();

	ChatController::destroyInstance();			//之前销毁有闪退问题，现在没有了？
 	FriendController::destroyInstance();
	NewAreaActivityController::destroyInstance();
	//LoginController::destroyInstance();		//不能销毁
	OperateActController::destroyInstance();
	ActivityController::destroyInstance();
	//UpgradeController::destroyInstance();		// 可不销毁，注销后检查更新时会再次被创建;
	GoldActivityModel::destroyInstance();
	GoldActivityController::destroyInstance();
	DoubleBonusModel::destoryInstance();
}

