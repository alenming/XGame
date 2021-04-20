#ifndef DataManager_h__
#define DataManager_h__

#include "cocos2d.h"
#include "DataTable.h"
#include "Utils/Tea/TEACrypt.h"
#include <string>
#include <vector>
#include <map>

USING_NS_CC;
using namespace std;

class DataManager
{

public:

	DataManager();
	~DataManager();

public:

	//singleton
	static DataManager* getInstance();
	static void destroyInstance();

	//��ѯ��ɫ����
	RowData* searchCharacterById(int id);

	//��ȡ��ɫ���ݱ�
	DataTable* getDTCharacter();

	//��ѯ��ɫ�Ǽ�����
	RowData* searchStarUpById(int id, int curStars);
	
	RowData* searchStarUpById(int id);
	//��ѯ��������
	RowData* searchSkillById(int id);

	//��ѯbuff����
	RowData* searchBuffById(int id);

	//��ѯMONSTER����
	RowData* searchMonsterById(int id);

	//��ѯ������Ч��
	RowData* searchSkillEffectById(int id);

	//��ѯ�ؿ���
	RowData* searchChapterById(int id);

	//��ѯ���߱�
	RowData* searchToolById(int id);

	//��ѯװ����
	RowData* searchEquipmentReflById(int id);

	//��ѯ����ϵͳ���ݱ�
	RowData* searchToolSystemById(int id);

	//��ѯ����¯�����������ݱ�
	RowData* searchRefineFurnaceById(int id);

	//��ѯ����¯������������;
	RowData* searchRefineFurnaceSellById(int id);

	//��ѯ�ؿ�id
	RowData* searchBarrierById(int id);

	// ��ȡ�½ڱ����������;
	DataTable* getDTChapter() { return &mChapterCache; };

	// ��ȡ�ؿ������������;
	DataTable* getDTBarrier() { return &mBarrierCache; };

	// ��ȡ���߱����������;
	DataTable* getDTTool() { return &mToolCache; };

	// ��ȡװ��ϴ���������ݱ����������;
	DataTable* getDTEquipmentRef() { return &mEquipmentRefCache; };

	// ��ȡ����ϵͳ���ݱ����������;
	DataTable* getDTToolSystem() { return &mToolSystemCache; };

	// ��ȡ����¯���ݱ����������;
	DataTable* getDTRefineFurnace() { return &mRefineFurnaceCache; };
	//��ȡ����¯�������ñ���������;
	DataTable* getDTRefineFurnaceSell() { return &mRefineFurnaceSellCache; };
	//�������ñ�
	DataTable* getDTExperience() { return &mExperienceCache; };
	//���龭���
	DataTable* getDTHorseBookExp() { return &mHorseBookExpCache; };
	// �����·���������
	DataTable* getDTTaskNewServer() { return &mTaskNewServerCache; };
	// �����������̳Ǳ�����
	DataTable* getDTArenaStore() { return &mArenaStoreCache; };

	//��Ե�԰ױ�
	DataTable* getDTLoveTalk() { return &mLoveTalkCache; };
	//��Ե�����
	DataTable* getDTLovePlay() { return &mLovePlayCache; };
	//��Ե���ߵ����
	DataTable* getDTLoveLevel() { return &mLoveLevelCache; };
	//��Ե���ߵ����
	DataTable* getDTLoveFinger() { return &mLoveFingerCache; };
	//��Ե�ʼ���
	DataTable* getDTLoveHero() { return &mLoveHeroCache; };
	//��Ե�ʴ��
	DataTable* getDTLoveQue() { return &mLoveQueCache; };
	//��ԵѰ����
	DataTable* getDYLoveTreasure() {return &mLoveTreasure;};
	//��Ե����
	DataTable* getDTLoveAtrribute() {return &mLoveAttribute;};
	//��ѯ�Թ�����������
	DataTable* getDTMazeBase() { return &mMazeBaseCache; };
	//��ѯ�Թ���ͼ������
	DataTable* getDTMazeBaotu() { return &mMazeBaotuCache; };
	//��ѯ�Թ��¼�������
	DataTable* getDTMazeThing() { return &mMazeThingCache; };
	//��ѯ�Թ�Box����
	DataTable* getDTMazeBox() {return &mMazeBoxCache; };
	//��ѯvip�̳Ǳ�����
	DataTable* getDTShopVip(){ return &mShopVipCache; };
	// ��ѯѵ��λ�������ñ�;
	DataTable* getDTTraining() { return &mTrainingCache; };

	// ��ѯ���ܿ��ŵȼ�����������;
	DataTable* getDTFunctionLevel() { return &mFunctionLevelCache; };

	// ��ѯ�ȼ��������ñ�;
	DataTable* getDTHeroExp() { return &mHeroExpCache; };

	// ��ѯ�����������ñ�;
	DataTable* getDTLevelUpGuide() { return &mTableLevelUpGuide; };

	// ��ѯ�½��������ݱ�;
	DataTable* getDTChapterPos() { return &mTableChapterPos; };

	// ��ѯӢ�۽�������
	RowData* searchHeroAdvanceById(int heroTempletId, int advLvl);

	// ��ѯӢ�۽�������
	RowData* searchHeroAdvanceById(int AdvanceById);

	// ��ѯӢ��Ե������
	RowData* searchHeroFateById(int id);
	DataTable* getDTHeroFate(){ return &mHeroFateCache; };
	// ��ѯ��������
	RowData* searchExperienceById(int id);
	// ��ѯ���龭������
	RowData* searchHorseBookExpById(int id);
	// ��ѯ�����·���������
	RowData* searchTaskNewServerById(int id);

	// ��ѯ�����������̳Ǳ�����
	RowData* searchArenaStoreById(int id);

	//��ѯ��Ե�԰ױ�����
	RowData* searchLoveTalkById(int id);
	//��ѯ��Ե���������
	RowData* searchLovePlayById(int id);
	//��ѯ��Ե���ߵ��������
	RowData* searchLoveLevelById(int id);
	//��ѯ��Ե�Ⱦ�����
	RowData* searchLoveFingerById(int id);
	//��ѯ��Ե�ʼ�����
	RowData* searchLoveHeroById(int id);
	//��ѯ��Ե�ʴ�����
	RowData* searchLoveQueById(int id);
	//��ѯ��ԵѰ������
	RowData* searchLoveTreasureByid(int id);
	//��ѯ��Ե��������
	RowData* searchLoveAtrributeById(int id);
	//��ѯ��������
	RowData* searchStarSpriteAttriById(int id);
	//��ѯ���鼼�ܱ�
	RowData* searchStarSpriteSkillById(int id);
	
	//��ѯ�Թ�����������
	RowData* searchMazeBaseById(int id);
	//��ѯ�Թ���ͼ������
	RowData* searchMazeBaotuById(int id);
	//��ѯ�Թ��¼�������
	RowData* searchMazeThingById(int id);
	//��ѯ�Թ�Box���ݱ�����
	RowData* searchMazeBoxById(int id);

	//��ѯvip�̳Ǳ�����
	RowData* searchShopVipById(int id);


	// ��ѯѵ��λ������������;
	RowData* searchTrainingById(int id);

	// ��ѯ�ȼ��������ñ�;
	RowData* searchHeroExp(int id);

	//��ѯ����������
	RowData* searchGuideById(int id);
	//��ȡ����������
	DataTable* getDTGuide();
	//��ѯ�Ի���
	RowData* searchGuideChatById(int id);

	//��ѯ�ȼ����ű�
	RowData* searchFunctionLevelById(int id);

	//��ѯ�ȼ���
	RowData* searchTalentSkillById(int id);

	// ��ѯ�����������ñ�;
	RowData* searchLevelUpGuide(int id);

	DataTable* getDTTrail() { return &mTableTrail; };

	// ��ѯ�������ñ�;
	RowData* searchTrailById(int id);

	// ��ѯ�����������ñ�;
	RowData* searchTrailTreasureById(int id);

	//��ѯ�����
	RowData* searchTaskById(int id);

	//��ѯ���ʻ
	RowData* searchBestActById(int id);

	//��ȡ�����
	DataTable* getDTTask(){ return &mTableTask; };

	//��ȡ���ʻ
	DataTable* getDTBestAct() {return &mTableBestAct; };

	//��ѯս��������
	RowData* searchFightBg(int id);

	// ��ѯ�½��������ݱ�;
	RowData* searchChapterPosById(int id);

	//�ǩ��������
	DataTable* getDTSign(){ return &mTableSign; };

	//�����base��
	DataTable* getDTStarBase(){ return &mTableStarBase; };

	//����ռ�Ǳ�
	DataTable* getDTStar(){ return &mTableStar; };

	//�������ݰ�
	DataTable* getArenaReword() {return &mTableArenaReword; };

	//�������¯���ݰ�
	DataTable* getSmelter() {return &mTableSmelter;};

	// ��ѯ����base�����ݱ�;
	RowData* searchStarBaseById(int id);

	// ��ѯռ�����ݱ�;
	RowData* searchStarById(int id);

	// ��ѯ�������ݱ�;
	RowData* searchArenaRewordById(int id);

	//��ȡVIP��������
	DataTable* getDTVip();

	//��ѯVIP��Ȩ����;
	RowData* searchVipDescriptionById(int id);
	
	//��ѯ�淨�����ǩid�͹�������id
	RowData* searchRulesLabelById(int id);

	//��ѯ��ǩ�ı����淨���������ı�
	RowData* searchRulesTextById(int id);

	//��ȡ����������
	DataTable* getDTTactics();

	//��ѯ�����ݱ�
	RowData* searchTacticsById(int id);

	// ��ȡ֧������������;
	DataTable* getDTPay();

	// ��ѯ֧����;
	RowData* searchPayById(int id);

	//��ѯboss�������ݱ�
	RowData* searchBossRewardById(int id);

	//��ȡ��������������
	DataTable* getDTNewAreaActivity();

	//����ID��ѯ����������
	RowData* searchNewAreaActivityById(int id);

	//��ѯLoading��ʾ��
	RowData* searchLoadingTipsById(int id);

	//��ѯ����ê������
	RowData* searchCardAcPointById(int id);

	// ��ѯ��ؿ���������;
	RowData* searchActivityShowById(int id);

	//��ȡ��Ӫ���������
	DataTable* getDTOperateActivity();

	//��ѯ��ȡ��Ӫ�ĳ������
	RowData* searchOperateActivityById(int id);

	//��Ӫ�����������������
	DataTable* getDTOperateActivitySort();

	//������������
	DataTable* getTableEmblem();
	RowData* searchEmblemById(int id);

	//���Ұ��ɽ�������
	DataTable* getTableBuilding();
	RowData* searchBuildingById(int id);

	//���Ұ��ɾ�������
	DataTable* getTableDonate();
	RowData* searchDonateById(int id);

	//���Ұ����¼�
	DataTable* getTableEvent();
	RowData* searchEventById(int id);

	//���Ұ��ɻ
	DataTable* getGroupActivityInfo();
	RowData* searchGroupActivityInfoById(int id);

	//���ɻչʾ��Ʒ����
	DataTable* getGroupActivityShowInfo();
	RowData* searchGroupActivityshowInfoById( int id );

	//����Ӣ��ʯͷ
	DataTable* getTableHeroStone();
	RowData* searchHeroStoneById(int id);

	//�������Խ���
	DataTable* getTableAttbitureIntroduce();
	RowData* searchAttbitureIntroduceById(int id);

	//��������
	DataTable* getTableStarCampLinup();
	RowData* searchStarCampLinupById(int id);

	//�������
	DataTable* getDTArtifact();
	RowData* searchArtifactAttriById(int id);

	// ��ȡװ�����ݱ�����;
	DataTable* getDTEquipment() { return &mEquipmentCache; };
	RowData* searchEquipmentById(int id);

	// ��ȡװ���������ݱ�����;
	DataTable* getDTEquipmentAdvanced() { return &mEquipmentAdvancedCache; };
	RowData* searchEquipmentAdvancedById(int id);

	// ��ȡ�������ݱ�����;
	RowData* searchEquipmentQuenchById(int id);

	// ��ȡװ�����������;
	RowData* searchEquipmentExp(int id);

	//��ȡ�̵���������
	DataTable* getDTShop();
	RowData* searchShopById( int id );

	//�����湦�ܿ�����ʾ��
	DataTable* getDTOpenFunction();
	RowData* searchOpenFunctionById(int id);

	//����boss��
	DataTable* getDTGroupBoss();
	RowData* searchGroupBossById(int id);

	//�鿨Ԥ��
	DataTable* getDTShopCardView();

	//pvpshow
	DataTable* getTablePvpAllShow();

	//Ӣ�۹�ϵ��
	DataTable* getDTHeroRelation();
	RowData* searchHeroRelationById(int id);

	//������ʾ
	DataTable* getDTCommonTexdt();
	//��ȡ��ʾ�ı�����
	string searchCommonTexdtById(int id);

private:

	//��ʼ������
	void initData();

	//�����ݱ�
	void bindTable(DataTable& dataTable, const char* filepath);
	
private:

	static DataManager* mInstance;

	//Tea�ӽ�����Կ
	static uint32_t mKey[];

	//Tea�����㷨��
	TEACrypt mTea;

	//��ɫ��������
	DataTable mCharacterCache;
	//��ɫ���Ǳ�
	DataTable mHeroStarUpCache;

	//���ܱ�
	DataTable mSkillCache;

	//������Ч��
	DataTable mSkillEffectCache;

	//BUFF��
	DataTable mBuffCache;

	//MONSTER��
	DataTable mMonsterCache;


	//�½����ݱ�;
	DataTable mChapterCache;

	//�ؿ����ݱ�;
	DataTable mBarrierCache;

	//�������ݱ�;
	DataTable mToolCache;

	//װ��ϴ���������ݱ�;
	DataTable mEquipmentRefCache;

	//װ�����ݱ�;
	DataTable mEquipmentCache;
	//װ���������ݱ�;
	DataTable mEquipmentAdvancedCache;
	//����ϵͳ��
	DataTable mToolSystemCache;
	//Ӣ�۽��ױ�
	DataTable mHeroAdvanceCache;

	//Ӣ��Ե�ֱ�
	DataTable mHeroFateCache;

	//����¯�̵����ñ�
	DataTable mRefineFurnaceCache;
	//����¯�������ñ�
	DataTable mRefineFurnaceSellCache;
	//�������ñ�
	DataTable mExperienceCache;
	//���龭���
	DataTable mHorseBookExpCache;
	//�·������
	DataTable mTaskNewServerCache;
	//�����������̳Ǳ�
	DataTable mArenaStoreCache;

	//��Ե�԰ױ�
	DataTable mLoveTalkCache;
	//��Ե�����
	DataTable mLovePlayCache;
	//��Ե���ߵ����
	DataTable mLoveLevelCache;
	//��Ե�ȾƱ�
	DataTable mLoveFingerCache;
	//��Ե�ʼ�
	DataTable mLoveHeroCache;
	//��Ե�ʴ�
	DataTable mLoveQueCache;
	//��ԵѰ��
	DataTable mLoveTreasure;
	//��Ե����
	DataTable mLoveAttribute;
	//����������
	DataTable mStarAdvCache;
	//�������Ա�
	DataTable mStarAttriCache;
	//���鼼�ܱ�
	DataTable mStarSkillCache;
	//�Թ�����������
	DataTable mMazeBaseCache;
	//�Թ���ͼ������
	DataTable mMazeBaotuCache;
	//�Թ��¼�
	DataTable mMazeThingCache;
	//�Թ�Box����
	DataTable mMazeBoxCache;

	//vip�̳Ǳ�����
	DataTable mShopVipCache;

	// ѵ��λ�������ñ�;
	DataTable mTrainingCache;

	// �ȼ����ñ�;
	DataTable mHeroExpCache;

	//����������
	DataTable mGuideCache;
	//�����Ի��Լ�����Ի���
	DataTable mGuideChatCache;

	//�ȼ����ű�
	DataTable mFunctionLevelCache;

	//���Ǽ��ܱ�
	DataTable mTalentSkillCache;

	// ����������;
	DataTable mTableLevelUpGuide;

	// �������ñ�;
	DataTable mTableTrail;

	// �����������ñ�;
	DataTable mTableTrailTreasure;

	//�������ñ�
	DataTable mTableTask;

	//���ʻ���ñ�
	DataTable mTableBestAct;

	//ս��������
	DataTable mTableBgColor;

	// �½��������ݱ�;
	DataTable mTableChapterPos;

	//ǩ��������
	DataTable mTableSign;

	// �½��������ݱ�;
	DataTable mTableStarBase;

	// �½��������ݱ�;
	DataTable mTableStar;

	// �������ݱ�;
	DataTable mTableArenaReword;

	//Vip���ݱ�
	DataTable mTableVip;

	//�淨����ID��
	DataTable mTableRulesId;
	//�淨�����ı���
	DataTable mTableRulesDes;

	//�����ݱ�
	DataTable mTableTactics;

	// ֧�����ݱ�;
	DataTable mTablePay;

	//boss������
	DataTable mTableBossReward;

	//����¯��
	DataTable mTableSmelter;

	//�������
	DataTable mTableNewAreaActivity;

	//loading��ʾ�����ݱ�
	DataTable mTableLoadingTips;

	//��Ƭê������
	DataTable mTableCardAcPoint;

	// ��ؿ���������;
	DataTable mTableActivityShow;

	//��Ӫ�����
	DataTable mTableOperateActivity;

	//��Ӫ���������
	DataTable mTableOperateActivitySort;

	//��������
	DataTable mTableEmblem;

	//���ɽ���
	DataTable mTableBuilding;

	//���ɾ���
	DataTable mTableDonate;

	//�����¼���
	DataTable mTableEvent;

	//���ɻ����
	DataTable mTableGroupActivity;

	//���ɻչʾ����
	DataTable mTableGroupActShow;

	//Ӣ��ʯͷ��
	DataTable mTableHeroStone;

	//���Խ��ܱ�
	DataTable mTableAttributeIntruduce;

	//�������α�
	DataTable mTableStarsCampLinup;

	//�������
	DataTable mTableArtifact;

	// ������;
	DataTable mTableQuench;

	// װ�������;
	DataTable mTableEquipmentExp;

	//�̵���
	DataTable mTableShop;

	//�����湦�ܿ�����ʾ��
	DataTable mTableOpenFunc;

	//����boss��
	DataTable mTableGroupBoss;

	//�鿨Ԥ��
	DataTable mTableShopCardView;

	//pvpShow
	DataTable nTablePvpAllShow;

	//Ӣ�۹�ϵ��
	DataTable mTableHeroRelation;

	//�ı���ʾ�ַ�
	DataTable mTableCommonText;
};



#endif