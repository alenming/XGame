#ifndef RobDef_h__
#define RobDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "FormationModule/CustomScrollBar.h"
#include "Widget/SimpleRichText.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include "UI/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include "PvpModule/PvpHeader.h"

using namespace std;
using namespace cocos2d::ui;

const int MAX_ROB_ENEMY_COUNT = 3;
const int MAX_ENEMY_HERO_COUNT = 5;
const int MAX_TOOL_CHIPS_COUNT = 6;
const int MAX_BOX_COUNT = 3;

const float MAX_TOOL_CHIPS_WIDTH = 64.0f;
const float MAX_TOOL_CHIPS_HEIGHT = 64.0f;

enum OBS_PARAM_TYPE_ROB
{
	OBS_PARAM_TYPE_ROB_MAIN_INFO,			// ��������Ϣ;
	OBS_PARAM_TYPE_ROB_REFRESH_ENEMY,		// ������Ϣ;
	OBS_PARAM_TYPE_ROB_RECOED,				// �Ӷ��¼;
	OBS_PARAM_TYPE_ROB_REST_TIME,			// ˢ����սʱ��;
	OBS_PARAM_TYPE_ROB_COMPOSE_RESULT,		// �ϳɽ��;
	OBS_PARAM_TYPE_ROB_TEN_RESULT,			// �Ӷ�10�ν��;
	OBS_PARAM_TYPE_ROB_PUSH_UPDATE,			// �Ӷ����͸���;
};


// ��������Ϣ;
typedef struct tagRob_Main_Info
{
	// ��սʣ��ʱ��;
	int nRestTime;

	// ��ս������;
	int nSignAmount;

	tagRob_Main_Info()
		: nRestTime(0)
		, nSignAmount(0)
	{
	}
}Rob_Main_Info;

typedef struct tagBW_CHIPS_ITEM_INFO
{
	int id;
	int amount;

	tagBW_CHIPS_ITEM_INFO()
		: id(-1)
		, amount(0)
	{
	}
}BW_CHIPS_ITEM_INFO;

typedef struct tagROB_PLAYER_INFO
{
	// ���Id;
	int  nPlayerId;

	// �Ӷ������;
	string strPlayerName;

	// �Ӷ��������;
	int nPlayerType;	// С�ڵ���0 Ϊ����;

	int petResId;

	// �������;
	vector<matrixData> vecMatrixIds;

	// ��ҵȼ�;
	int nLv;

	// ���ս����;
	int nTotalAtk;

	// �����Ƭ�ļ���;
	int nPro;

	bool operator == (const tagROB_PLAYER_INFO& info) const
	{
		return nPlayerId == info.nPlayerId;
	}

	bool operator == (const int& nPlayerId) const
	{
		return nPlayerId == nPlayerId;
	}

	tagROB_PLAYER_INFO()
		: nPlayerId(-1)
		, strPlayerName("")
		, nPlayerType(0)
		, petResId(-1)
		, nLv(1)
		, nTotalAtk(0)
		, nPro(0)
	{
		vecMatrixIds.clear();
	}
}ROB_PLAYER_INFO;

typedef struct tagROB_RESULT_INFO
{
	string strTip;
	int nOrder;
	int robFlag; //�Ӷ�����־	
	int succFlag; //ս�������־
	int nCoin;
	int nExp;
	int nEnergy;
	int avoidtime;

	//����
	//vector<ROB_RESULT_BOX> vecBoxList[3];

	// ������Ʒ<Id, Amount>;
	map<int, int>  mapDropItems;

	tagROB_RESULT_INFO()
		: strTip("")
		, nOrder(0)
		, robFlag(0)
		, succFlag(0)
		, nCoin(0)
		, nExp(0)
		, nEnergy(0)
		, avoidtime(0)
	{
		mapDropItems.clear();
	}
}ROB_RESULT_INFO;

typedef struct tagROB_TEN_RESULT
{
	string strTip;
	int robFlag; //�Ӷ�����־	
	int fragId;
	int fragAmount;
	string fragName;
	int succFlag; //ս�������־
	int getCoin;
	int getExp;
	int avoidtime;
	//
	int times; //���Ӷ�Ĵ���
	int getTimes; //ȷ�ϵ��ڼ��λ�ȡ����Ƭ, 0��δ���
	int costEnergy; //���ĵľ���

	// �Ӷ����б�;
	map<int, ROB_RESULT_INFO>  mapResult;

	tagROB_TEN_RESULT()
		: strTip("")
		, robFlag(0)
		, fragId(0)
		, fragAmount(0)
		, fragName("")
		, succFlag(0)
		, getCoin(0)
		, getExp(0)
		, avoidtime(0)
		, times(0)
		, getTimes(0)
		, costEnergy(0)
	{
		mapResult.clear();
	}
}ROB_TEN_RESULT;

typedef struct tagROB_RECORDS_INFO
{
	string strPlayerName;
	int petResId;
	int level;
	int id;
	int itemId;
	string strItemName;
	int amount;
	int succflag;
	int direction;	// 0:���Ӷ�  1:�Ӷ����;
	string strDate;

	tagROB_RECORDS_INFO()
		: strPlayerName("")
		, petResId(-1)
		, level(0)
		, id(-1)
		, itemId(-1)
		, strItemName("")
		, amount(0)
		, succflag(0)
		, direction(0)
		, strDate("")
	{
	}
}ROB_RECORDS_INFO;

// ��������;
typedef struct tagRob_Tool_Detail_Info
{
	int  nId;
	int  nTemplateId;
	int  nAmount;
	string  strName;
	string  strDesc;

	tagRob_Tool_Detail_Info()
		: nId(-1)
		, nTemplateId(-1)
		, nAmount(0)
		, strName("")
		, strDesc("")
	{
	}
}Rob_Tool_Detail_Info;

typedef struct tagROB_TREASURE_FATE_INFO
{
	string fateName;
	string fateInfo;
	int itemid;

	tagROB_TREASURE_FATE_INFO()
		: fateName("")
		, fateInfo("")
		, itemid(-1)
	{
	}
}ROB_TREASURE_FATE_INFO;


//////////////////////////////////// 

// �Ӷ�����������UI;
typedef struct tagUI_Rob_Result_Item
{
	Layout*		pRoot;
	ImageView*	imgIndex;
	Text*		txtCoin;
	Text*		txtExp;
	Text*		txtEnergy;

	tagUI_Rob_Result_Item()
	{
		memset(this, 0, sizeof(tagUI_Rob_Result_Item));
	}
}UI_Rob_Result_Item;

// �Ӷ���UI;
typedef struct tagUI_Rob_Result
{
	Layout*		pRoot;
	ImageView*	imgBg;
	Button*		btnClose;
	ui::ScrollView*	scrollviewResult;
	Button*		btnContinue;
	Button*		btnFinish;

	tagUI_Rob_Result()
	{
		memset(this, 0, sizeof(tagUI_Rob_Result));
	}
}UI_Rob_Result;

// �Ӷ���ֵ�������UI;
typedef struct tagUI_Rob_Item
{
	Layout*		pRoot;
	Text*		txtName;
	ImageView*	imgPro;
	Button*		btnRob;
	Button*		btnRob10;
	UIToolHeader*  player;

	tagUI_Rob_Item()
	{
		memset(this, 0, sizeof(tagUI_Rob_Item));
	}
}UI_Rob_Item;

// ѡ���Ӷ����UI;
typedef struct tagUI_Rob
{
	Layout*		pRoot;
	ImageView*	imgBg;
	Button*		btnClose;
	Text*		txtEnergy;
	ui::ScrollView*	scrollviewOpList;
	Button*		btnChange;

	tagUI_Rob()
	{
		memset(this, 0, sizeof(tagUI_Rob));
	}
}UI_Rob;

// ��սUI;
typedef struct tagUI_Rest
{
	Layout*		pRoot;
	ImageView*  imgBg;
	Text*		txtRestNum2;
	Button*		btnClose;
	Button*		btnOK;
	Button*		btnCancel;

	tagUI_Rest()
	{
		memset(this, 0, sizeof(tagUI_Rest));
	}
}UI_Rest;

// �Ӷ��¼��������UI;
typedef struct tagUI_Rob_Record_Item
{
	Layout*				pRoot;
	Button*				btnRobBack;
	SimpleRichText*		txtDesc;
	Text*				txtDate;
	UIToolHeader*		player;

	tagUI_Rob_Record_Item()
	{
		memset(this, 0, sizeof(tagUI_Rob_Record_Item));
	}
}UI_Rob_Record_Item;

// �Ӷ��¼UI;
typedef struct tagUI_Rob_Record
{
	Layout*		pRoot;
	ImageView*  imgBg;
	Button*		btnClose;
	ui::ScrollView*	scrollviewRecord;

	tagUI_Rob_Record()
	{
		memset(this, 0, sizeof(tagUI_Rob_Record));
	}
}UI_Rob_Record;

// ��������UI;
typedef struct tagUI_Detail
{
	Layout*		pRoot;
	ImageView*  imgBg;
	Button*		btnClose;
	UIToolHeader*  item;
	Text*		txtName;
	Text*		txtAmount;
	Text*		txtDesc;

	tagUI_Detail()
	{
		memset(this, 0, sizeof(tagUI_Detail));
	}
}UI_Detail;

// �Ӷ�������UI�ؼ�;
typedef struct tagUI_Rob_Main
{
	Layout*		pRoot;

	// �����б�;
	ui::ScrollView* scrollviewList;
	UI_Detail  uiDetail;

	// ѡ�б���;
	ImageView*  imgChipBg;
	Button*		btnCompose;
	Button*		btnRapidCompose;

	// �Ӷ�;
	UI_Rob		uiRob;
	UI_Rob_Result  uiRobResult;

	// �Ӷ��¼;
	Button*		btnRecord;
	UI_Rob_Record  uiRecord;

	// ��ս;
	Button*		btnRest;
	Text*		txtRestNum;
	ImageView*	imgTime;
	Text*		txtTime;
	UI_Rest		uiRest;

	tagUI_Rob_Main()
	{
		memset(this, 0, sizeof(tagUI_Rob_Main));
	}

}UI_Rob_Main;

#endif // RobDef_h__
