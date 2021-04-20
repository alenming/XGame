#include "LayerSmithy.h"
#include "SmithyModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "Widget/LayerCommHeadInfo.h"
#include "LayerEquipSource.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/SoundUtils.h"
#include "Temp/CustomGoldTips.h"
#include "LayerAddMaterial.h"
#include "DataManager/DataManager.h"
#include "Widget/LayerHeroRelation.h"

LayerSmithy::LayerSmithy()
	: m_nCurHeroId(-1)
	, m_itemType(EQUIP_ITEM_TYPE_NONE)
	, m_nCurSelToolId(-1)
	, m_nCurSelOpType(SMITHY_OP_TYPE_NONE)
	, m_pController(nullptr)
	, m_bIsFirstEnter(true)
	, m_nHorseBookExpPreview(0)
	, m_bIsCL(false)
{
	SmithyModel::getInstance()->addObserver(this);
	m_pController = new SmithyController();

	// 初始化UI装备列表;
	for (unsigned int i = 1; i <= 6; ++i)
		m_mapEquipment.insert(make_pair(i, nullptr));

	// 初始化面板数据对应Id;
	for (unsigned int i = 1; i <= 3; ++i)
		m_mapOpPanelWithId.insert(make_pair(i, -1));

	m_vcQHMaterials.clear();
	
}

LayerSmithy::~LayerSmithy()
{
	SmithyModel::getInstance()->removeObserver(this);
	CC_SAFE_DELETE(m_pController);
}
//
void LayerSmithy::onEnter()
{
	Layer::onEnter();

	// 重复进入时，刷新;
	if (m_bIsFirstEnter)
	{
		m_bIsFirstEnter = false;

		// 音效;
		SoundUtils::playSoundEffect("quanping");

		// 进场特效;
		playEnterAnimation();
	}
	else
	{
		// 1.进阶材料;
		auto iter = m_mapOpPanelWithId.find(m_nCurSelOpType);
		if (iter != m_mapOpPanelWithId.end())
		{
			iter->second = -1;
			updateOpPanel();
		}

		// 2.进阶标识;
		updateSingleEquipmentInfo(m_nCurSelToolId);
	}
}

void LayerSmithy::onExit()
{
	Layer::onExit();
}


bool LayerSmithy::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}
	
	initUI();

	return true;
}


void LayerSmithy::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIEquip.ExportJson"));
	m_ui.pRoot->setContentSize(size);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
			{
				this->onBtnBack(sender, type);
			},
		HEAD_INFO_TYPE_BKS_SHOP);
	this->addChild(_layerCommHeadInfo, 9);

	//////////// 左面板;
	m_ui.pnlLeft = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Left"));

	// 角色名/等级/战斗力;
	m_ui.txtRoleName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Role_Name"));
	m_ui.txtRoleName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRoleName->setString("");
	m_ui.txtRoleLv = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Role_Lv"));
	m_ui.txtRoleLv->setFontName(FONT_FZZHENGHEI);
	//m_ui.txtRoleLv->enableOutline(Color4B(Color3B(0x78, 0x0, 0x0)), 2);
	setTextAddStroke(m_ui.txtRoleLv, Color3B(0x78, 0x0, 0x0),2);
	m_ui.txtRoleLv->setString("");
	m_ui.txtFc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_FC"));
	m_ui.txtFc->setFontName(FONT_FZZHENGHEI);
	//m_ui.txtFc->enableOutline(Color4B(Color3B(0x7D, 0x3F, 0x1C)), 2);
	setTextAddStroke(m_ui.txtFc, Color3B(0x7D, 0x3F, 0x1C),2);
	m_ui.txtFc->setString("");

	// 血量/攻击/物防/法防;
	m_ui.txtHP = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_HP"));
	m_ui.txtHP->setFontName(FONT_FZZHENGHEI);
	m_ui.txtHP->setString("");
	m_ui.imgAtk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Image_gongji2"));
	m_ui.txtAtk = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Atk"));
	m_ui.txtAtk->setFontName(FONT_FZZHENGHEI);
	m_ui.txtAtk->setString("");
	m_ui.txtPD = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_PD"));
	m_ui.txtPD->setFontName(FONT_FZZHENGHEI);
	m_ui.txtPD->setString("");
	m_ui.txtMD = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_MD"));
	m_ui.txtMD->setFontName(FONT_FZZHENGHEI);
	m_ui.txtMD->setString("");

	// 装备列表 - 动态创建;

	//////////// 右面板;
	m_ui.pnlRight = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Right"));
	m_ui.cbPageQH = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(CheckBox_Page_QH)));
	m_ui.cbPageQH->addEventListener(CC_CALLBACK_2(LayerSmithy::onCheckBoxClicked, this, CheckBox_Page_QH));
	m_ui.cbPageCL = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(CheckBox_Page_CL)));
	m_ui.cbPageCL->addEventListener(CC_CALLBACK_2(LayerSmithy::onCheckBoxClicked, this, CheckBox_Page_CL));
	//m_ui.cbPageJJ = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(CheckBox_Page_JJ)));
	//m_ui.cbPageJJ->addEventListener(CC_CALLBACK_2(LayerSmithy::onCheckBoxClicked, this, CheckBox_Page_JJ));
	//m_ui.cbPageXL = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(CheckBox_Page_XL)));
	//m_ui.cbPageXL->addEventListener(CC_CALLBACK_2(LayerSmithy::onCheckBoxClicked, this, CheckBox_Page_XL));

	// 强化/进阶/淬炼;
	initUIQH();
	initUIJJ();
	initUICL();
}

void LayerSmithy::initUIQH()
{
	// 装备名称;
	m_ui.uiQH.pnlQH = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pnlRight, "Pnl_QH"));
	m_ui.uiQH.txtEquName = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName("Lab_Equ_Name"));
	m_ui.uiQH.txtEquName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtEquName->setString("");

	// 装备强化等级;
	m_ui.uiQH.txtLvDesc = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName("Lab_Lv_Desc"));
	m_ui.uiQH.txtLvDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtLvPre = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName("Lab_Lv_Pre"));
	m_ui.uiQH.txtLvPre->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtLvPre->setString("");
	m_ui.uiQH.imgArrow = dynamic_cast<ImageView*>(m_ui.uiQH.pnlQH->getChildByName("Img_Arrow"));
	m_ui.uiQH.txtLvDst = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName("Lab_Lv_Dst"));
	m_ui.uiQH.txtLvDst->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtLvDst->setString("");
	m_ui.uiQH.txtMaxLv = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName("Lab_MAXLV"));
	m_ui.uiQH.txtMaxLv->setFontName(FONT_FZZHENGHEI);
	//m_ui.uiQH.txtMaxLv->enableOutline(Color4B(Color3B(0x23, 0x1E, 0x48)));
	setTextAddStroke(m_ui.uiQH.txtMaxLv, Color3B(0x23, 0x1E, 0x48),2);
	m_ui.uiQH.txtMaxLv->setString("");

	// 强化属性;
	ostringstream oss;
	for (unsigned int i = 0; i < 2; ++i)
	{
		// 属性名称;
		oss.str("");
		oss << "Lab_Attr_" << i+1;
		m_ui.uiQH.txtBaseAttr[i] = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName(oss.str()));
		m_ui.uiQH.txtBaseAttr[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiQH.txtBaseAttr[i]->setString("");

		// 当前属性值;
		oss.str("");
		oss << "Lab_Attr_Value_" << i+1;
		m_ui.uiQH.txtBaseAttrValue[i] = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName(oss.str()));
		m_ui.uiQH.txtBaseAttrValue[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiQH.txtBaseAttrValue[i]->setString("");

		// 属性增量预览;
		oss.str("");
		oss << "Lab_Attr_Add_" << i+1;
		m_ui.uiQH.txtBaseAttrAdd[i] = dynamic_cast<Text*>(m_ui.uiQH.pnlQH->getChildByName(oss.str()));
		m_ui.uiQH.txtBaseAttrAdd[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiQH.txtBaseAttrAdd[i]->setString("");
	}

	///////////////////////////////// 普通强化消耗; ////////////////////////////////////
	m_ui.uiQH.pnlQHCost_Normal = dynamic_cast<Layout*>(m_ui.uiQH.pnlQH->getChildByName("Pnl_QH_Cost_Normal"));
	m_ui.uiQH.txtCostX1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQH, "Lab_X1_Cost"));
	m_ui.uiQH.txtCostX1->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtCostX1->setString("");
	m_ui.uiQH.btnQH_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQH, STRING(Btn_QH_1)));
	m_ui.uiQH.btnQH_1->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_QH_1));
	m_ui.uiQH.btnQH_N = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQH, STRING(Btn_QH_N)));
	m_ui.uiQH.btnQH_N->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_QH_N));

	///////////////////////////////// 马书强化消耗; ////////////////////////////////////
	m_ui.uiQH.pnlQHCost_HorseBook = dynamic_cast<Layout*>(m_ui.uiQH.pnlQH->getChildByName("Pnl_QH_Cost_HorseBook"));
	m_ui.uiQH.progressBarExp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "ProgressBar_Exp"));
	m_ui.uiQH.progressBarExpPreview = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "ProgressBar_Exp_0"));
	m_ui.uiQH.txtExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "Lab_Exp"));
	m_ui.uiQH.txtExp->setFontName(FONT_FZZHENGHEI);
	m_ui.uiQH.txtExp->setString("");
	m_ui.uiQH.pnlQHMaterial = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "Pnl_QH_Material"));
	m_ui.uiQH.btnQH_AutoAdd = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "Btn_QH_Auto_Add"));
	m_ui.uiQH.btnQH_AutoAdd->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_QH_Auto_Add));
	m_ui.uiQH.btnQH_HorseBook = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiQH.pnlQHCost_HorseBook, "Btn_QH_HorseBook"));
	m_ui.uiQH.btnQH_HorseBook->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_QH_HorseBook));
}

void LayerSmithy::initUIJJ()
{
	m_ui.uiJJ.pnlJJ = dynamic_cast<Layout*>(m_ui.pnlRight->getChildByName("Pnl_JJ"));
	m_ui.uiJJ.pnlJJ->setVisible(false);

	// 进阶预览;
	m_ui.uiJJ.txtEquName = dynamic_cast<Text*>(m_ui.uiJJ.pnlJJ->getChildByName("Lab_Equ_Name"));
	m_ui.uiJJ.txtEquName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiJJ.txtEquDstName = dynamic_cast<Text*>(m_ui.uiJJ.pnlJJ->getChildByName("Lab_Equ_Dst_Name"));
	m_ui.uiJJ.txtEquDstName->setFontName(FONT_FZZHENGHEI);

	// 进阶属性;
	ostringstream oss;
	for (unsigned int i = 0; i < 2; ++i)
	{
		// 属性名称;
		oss.str("");
		oss << "Lab_Attr_" << i+1;
		m_ui.uiJJ.txtAttr[i] = dynamic_cast<Text*>(m_ui.uiJJ.pnlJJ->getChildByName(oss.str()));
		m_ui.uiJJ.txtAttr[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiJJ.txtAttr[i]->setString("");

		// 当前属性值;
		oss.str("");
		oss << "Lab_Attr_Value_" << i+1;
		m_ui.uiJJ.txtAttrValue[i] = dynamic_cast<Text*>(m_ui.uiJJ.pnlJJ->getChildByName(oss.str()));
		m_ui.uiJJ.txtAttrValue[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiJJ.txtAttrValue[i]->setString("");

		// 属性增量预览;
		oss.str("");
		oss << "Lab_Attr_Add_" << i+1;
		m_ui.uiJJ.txtAttrAdd[i] = dynamic_cast<Text*>(m_ui.uiJJ.pnlJJ->getChildByName(oss.str()));
		m_ui.uiJJ.txtAttrAdd[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiJJ.txtAttrAdd[i]->setString("");
	}
	
	//////////////////////// 普通进阶消耗; /////////////////////////////
	m_ui.uiJJ.pnlJJCost_Normal = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, "Pnl_JJ_Cost_Normal"));
	m_ui.uiJJ.pnlJJCost_Normal->setVisible(true);

	//////////////////////// 马书进阶消耗; /////////////////////////////
	m_ui.uiJJ.pnlJJCost_HorseBook = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, "Pnl_JJ_Cost_HorseBook"));
	m_ui.uiJJ.imgScoreIcon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJCost_HorseBook, "Img_Score_Icon"));
	m_ui.uiJJ.txtScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJCost_HorseBook, "Lab_Score"));
	m_ui.uiJJ.txtScore->setFontName(FONT_FZZHENGHEI);
	m_ui.uiJJ.txtTipArena = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJCost_HorseBook, "Lab_Tips_Arena"));
	m_ui.uiJJ.txtTipArena->setFontName(FONT_FZZHENGHEI);
	m_ui.uiJJ.txtTipTrail = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJCost_HorseBook, "Lab_Tips_Trail"));
	m_ui.uiJJ.txtTipTrail->setFontName(FONT_FZZHENGHEI);
	m_ui.uiJJ.pnlJJCost_HorseBook->setVisible(true);

	m_ui.uiJJ.imgCost = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, "Img_Coin"));
	m_ui.uiJJ.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, "Lab_Cost"));
	m_ui.uiJJ.txtCost->setFontName(FONT_FZZHENGHEI);
	m_ui.uiJJ.txtCost->setString("");

	m_ui.uiJJ.txtLv = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, "Lab_Lv"));
	m_ui.uiJJ.txtLv->setFontName(FONT_FZZHENGHEI);

	// 进阶;
	m_ui.uiJJ.btnJJ = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiJJ.pnlJJ, STRING(Btn_JJ)));
	m_ui.uiJJ.btnJJ->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_JJ));

}

void LayerSmithy::initUICL()
{
	m_ui.uiCL.pnlCL = dynamic_cast<Layout*>(m_ui.pnlRight->getChildByName("Pnl_CL"));
	m_ui.uiCL.pnlCL->setVisible(false);

	// 预览;
	m_ui.uiCL.txtEquName = dynamic_cast<Text*>(m_ui.uiCL.pnlCL->getChildByName("Lab_Equ_Name"));
	m_ui.uiCL.txtEquName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiCL.txtEquDstName = dynamic_cast<Text*>(m_ui.uiCL.pnlCL->getChildByName("Lab_Equ_Dst_Name"));
	m_ui.uiCL.txtEquDstName->setFontName(FONT_FZZHENGHEI);

	// 淬炼属性;
	ostringstream oss;
	for (unsigned int i = 0; i < 2; ++i)
	{
		// 属性名称;
		oss.str("");
		oss << "Lab_Attr_" << i+1;
		m_ui.uiCL.txtAttr[i] = dynamic_cast<Text*>(m_ui.uiCL.pnlCL->getChildByName(oss.str()));
		m_ui.uiCL.txtAttr[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiCL.txtAttr[i]->setString("");

		// 当前属性值;
		oss.str("");
		oss << "Lab_Attr_Value_" << i+1;
		m_ui.uiCL.txtAttrValue[i] = dynamic_cast<Text*>(m_ui.uiCL.pnlCL->getChildByName(oss.str()));
		m_ui.uiCL.txtAttrValue[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiCL.txtAttrValue[i]->setString("");

		// 属性增量预览;
		oss.str("");
		oss << "Lab_Attr_Add_" << i+1;
		m_ui.uiCL.txtAttrAdd[i] = dynamic_cast<Text*>(m_ui.uiCL.pnlCL->getChildByName(oss.str()));
		m_ui.uiCL.txtAttrAdd[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.uiCL.txtAttrAdd[i]->setString("");
	}

	// 淬炼说明;
	m_ui.uiCL.txtCLTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCL.pnlCL, "Lab_CL_Title"));
	m_ui.uiCL.txtCLTitle->setFontName(FONT_FZZHENGHEI);
	m_ui.uiCL.txtCLTitle->setString("");
	m_ui.uiCL.txtCLDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCL.pnlCL, "Lab_CL_Desc"));
	m_ui.uiCL.txtCLDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.uiCL.txtCLDesc->setString("");

	// 淬炼消耗;
	m_ui.uiCL.pnlCLCost = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiCL.pnlCL, "Pnl_CL_Cost"));
	m_ui.uiCL.btnCL = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiCL.pnlCL, "Btn_CL"));
	m_ui.uiCL.btnCL->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_CL));

	// 淬炼降级;
	m_ui.uiCL.btnCLDegrade = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiCL.pnlCL, "Btn_CL_Degrade"));
	m_ui.uiCL.btnCLDegrade->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onBtnClicked, this, Btn_CL_Degrade));

	///////////////////////////////////// 淬炼降级; //////////////////////////////////////

	m_ui.uiCLDegrade.pnlCLDegrade = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIDowngrade.ExportJson"));
	this->addChild(m_ui.uiCLDegrade.pnlCLDegrade, 10);
	m_ui.uiCLDegrade.pnlCLDegrade->setVisible(false);
	m_ui.uiCLDegrade.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Img_Bg"));
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Lab_Downgrade"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Lab_Cost"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Lab_Tips"))->setFontName(FONT_FZZHENGHEI);
	m_ui.uiCLDegrade.txtCostGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "lab_Gold_1"));
	m_ui.uiCLDegrade.txtCostGold->setFontName(FONT_FZZHENGHEI);
	m_ui.uiCLDegrade.txtCostGold->setString("");
	m_ui.uiCLDegrade.btnOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Btn_Confirm"));
	m_ui.uiCLDegrade.btnOK->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			int nRet = m_pController->handleCLDegrade(m_nCurSelToolId, m_paramCL.nCostGold);
			if (nRet == 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10007));
			}
		}
	});

	auto _closeCallback = [=](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.uiCLDegrade.pnlCLDegrade->setVisible(false);
		}
	};
	m_ui.uiCLDegrade.btnCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "Btn_Cancel"));
	m_ui.uiCLDegrade.btnCancel->addTouchEventListener(_closeCallback);
	m_ui.uiCLDegrade.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiCLDegrade.pnlCLDegrade, "But_Close"));
	m_ui.uiCLDegrade.btnClose->addTouchEventListener(_closeCallback);
}

void LayerSmithy::onBtnBack( Ref* sender, cocos2d::ui::Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		// 音效;
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		Director::getInstance()->popScene();
	}
}

Color3B LayerSmithy::getEquipQuaColor( TOOL_ITEM_QUA qua )
{
	assert(qua >= 1 && qua <= TOTAL_ITEM_QUA_COUNT);
	return g_EquipColorWithQua[qua-1];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void LayerSmithy::initSmithyData( const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, const SMITHY_OP_TYPE opType )
{
	assert(nHeroId != -1 && nToolId != -1 && equipType != EQUIP_ITEM_TYPE_NONE);

	// 保存参数;
	m_nCurHeroId = nHeroId;
	//SmithyModel::getInstance()->setCurHeroId(m_nCurHeroId);

	// 更新角色信息;
	updateRoleInfo();

	// 选中装备;
	selectEquipment(equipType, nToolId);

	// 初始化面板类型;
	//m_nCurSelOpType = SMITHY_OP_TYPE_QH;
	if (opType == SMITHY_OP_TYPE_NONE)
	{
		selectPage(SMITHY_OP_TYPE_QH);
	}
	else
	{
		selectPage(opType);
	}
}

void LayerSmithy::selectEquipment( const EQUIP_ITEM_TYPE itemType, const int nToolId )
{
	if (nToolId == m_nCurSelToolId)
	{
		return;
	}

	// 记录当前选中装备;
	m_itemType = itemType;
	m_nCurSelToolId = nToolId;
	for (auto iter : m_mapEquipment)
	{
		if (nullptr != iter.second)
		{
			iter.second->setSelected(iter.first == itemType);
		}
	}

	// 重置面板数据标识;
	for (auto iter = m_mapOpPanelWithId.begin(); iter != m_mapOpPanelWithId.end(); ++iter)
		iter->second = -1;

	// 刷新;
	updateOpPanel();
}

void LayerSmithy::onCheckBoxClicked( Ref* sender, CheckBox::EventType type, int nWidgetName )
{
	// 不区分选中和未选中事件;
	switch (nWidgetName)
	{
	case CheckBox_Page_QH:
		{
			// 检查当前选中道具需要强化或是进阶;
			selectPage(SmithyModel::getInstance()->checkOpType(m_nCurSelToolId));
		}
		break;
		
	case CheckBox_Page_CL:
		{
			selectPage(SMITHY_OP_TYPE_CL);
		}
		break;

	default:
		break;
	}
}

void LayerSmithy::selectPage( SMITHY_OP_TYPE opType )
{
	// 刷新按钮，假状态;
	m_ui.cbPageQH->setSelectedState(m_nCurSelOpType == SMITHY_OP_TYPE_QH || m_nCurSelOpType == SMITHY_OP_TYPE_JJ);
	m_ui.cbPageCL->setSelectedState(m_nCurSelOpType == SMITHY_OP_TYPE_CL);

	if (opType == m_nCurSelOpType)
	{
		return;
	}

	// 音效;
	SoundUtils::playSoundEffect("dianji");

	// 有等级限制;
	if (opType == SMITHY_OP_TYPE_QH)
	{
		if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_STRENGTHEN))
			return;
	}
	else if (opType == SMITHY_OP_TYPE_CL)
	{
		if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL))
			return;
	}

	// 保存当前选中面板类型;
	m_nCurSelOpType = opType;

	// 1. 刷新面板数据;
	updateOpPanel();

	// 2. 显示面板;
	//m_ui.uiQH.pnlQH->setVisible(opType == SMITHY_OP_TYPE_QH);
	//m_ui.uiJJ.pnlJJ->setVisible(opType == SMITHY_OP_TYPE_JJ);
	//m_ui.uiCL.pnlCL->setVisible(opType == SMITHY_OP_TYPE_CL);
}

void LayerSmithy::updateOpPanel(bool bForceUpdate/* = false*/)
{
	// 是否强制刷新当前页面;
	if (bForceUpdate)
	{
		auto iter = m_mapOpPanelWithId.find(m_nCurSelOpType);
		assert(iter != m_mapOpPanelWithId.end());
		iter->second = -1;
	}

	if (m_nCurSelOpType == SMITHY_OP_TYPE_QH || m_nCurSelOpType == SMITHY_OP_TYPE_JJ)
	{
		m_nCurSelOpType = SmithyModel::getInstance()->checkOpType(m_nCurSelToolId);
	}

	// 再刷新一次按钮，真实状态;
	m_ui.cbPageQH->setSelectedState(m_nCurSelOpType == SMITHY_OP_TYPE_QH || m_nCurSelOpType == SMITHY_OP_TYPE_JJ);
	m_ui.cbPageCL->setSelectedState(m_nCurSelOpType == SMITHY_OP_TYPE_CL);

	// 显示面板;
	m_ui.uiQH.pnlQH->setVisible(m_nCurSelOpType == SMITHY_OP_TYPE_QH);
	m_ui.uiJJ.pnlJJ->setVisible(m_nCurSelOpType == SMITHY_OP_TYPE_JJ);
	m_ui.uiCL.pnlCL->setVisible(m_nCurSelOpType == SMITHY_OP_TYPE_CL);

	// 刷新红点显示;
	updateReddot();

	// 刷新;
	switch (m_nCurSelOpType)
	{
	case SMITHY_OP_TYPE_QH:
		updateQHInfo();
		break;

	case SMITHY_OP_TYPE_JJ:
		updateJJInfo();
		break;

	case SMITHY_OP_TYPE_CL:
		updateCLInfo();
		break;

	default:
		break;
	}
}

void LayerSmithy::updateQHInfo()
{
	auto iter = m_mapOpPanelWithId.find(SMITHY_OP_TYPE_QH);
	assert(iter != m_mapOpPanelWithId.end());

	// reset;
	if (iter->second == -1)
	{
		// 清理所有;
		for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=](MaterialHeader* _material)->void {
			_material->removeMaterial();
			_material->removeFromParent();
		});
		m_vcQHMaterials.clear();

		// 刷新数据;
		m_paramQH.clear();
		SmithyModel::getInstance()->getQHParam(m_nCurSelToolId, m_paramQH);

		// 更新UI;
		Node* _equ = m_ui.uiQH.pnlQH->getChildByName("tag_QH_Equ_Icon");
		if (_equ != nullptr)
		{
			_equ->removeFromParent();
			_equ = nullptr;
		}
		int nTemplateId = ToolModel::getInstance()->GetToolInfo(m_paramQH.nEquId)->ntemplateId;
		_equ = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramQH.nEquIcon));
		((UIToolHeader*)_equ)->setIconQua(m_paramQH.nEquQua);
		((UIToolHeader*)_equ)->setToolId(nTemplateId);
		((UIToolHeader*)_equ)->setLvlWithoutBg(m_paramQH.nPreLv);
		RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(nTemplateId);
		CCASSERT(pEquipment != nullptr, "");
		((UIToolHeader*)_equ)->setJJLvl(m_paramQH.nEquQua, pEquipment->getIntData("stars"));
		_equ->setPosition(Vec2(113.0f, 535.0f));
		_equ->setName("tag_QH_Equ_Icon");
		m_ui.uiQH.pnlQH->addChild(_equ);
		m_ui.uiQH.txtEquName->setString(m_paramQH.strEquName);
		m_ui.uiQH.txtEquName->setColor(getEquipQuaColor(m_paramQH.nEquQua));
		m_ui.uiQH.txtLvPre->setString(_TO_STR(m_paramQH.nPreLv));
		m_ui.uiQH.txtLvDst->setString(_TO_STR(m_paramQH.nDstLv));
		m_ui.uiQH.txtLvDst->setVisible(m_paramQH.nDstLv > m_paramQH.nPreLv);
		m_ui.uiQH.imgArrow->setVisible(m_ui.uiQH.txtLvDst->isVisible());
		ostringstream oss;
		oss << "(" << DataManager::getInstance()->searchCommonTexdtById(20174) << m_paramQH.nMaxLv << ")";
		m_ui.uiQH.txtMaxLv->setString(oss.str());
		oss.str("");

		// 基础属性;
		for (unsigned int i = 0; i < 2; ++i)
		{
			if (m_paramQH.vcBase.size() > i)
			{
				oss.str("");

				char szNum1[32], szNum2[32];
				memset(&szNum1, 0, 32);
				memset(&szNum2, 0, 32);
				if (m_paramQH.vcBase.at(i).attrType >= ATTR_TYPE_HP_COE)
				{
					sprintf(szNum1, "%0.2lf%%", m_paramQH.vcBase.at(i).fAttrValue * 100.0f);
					sprintf(szNum2, "%0.2lf%%", m_paramQH.vcBase.at(i).fAttrAdd * 100.0f);
				}
				else
				{
					sprintf(szNum1, "%d", (int)(m_paramQH.vcBase.at(i).fAttrValue));
					sprintf(szNum2, "%d", (int)(m_paramQH.vcBase.at(i).fAttrAdd));
				}

				m_ui.uiQH.txtBaseAttr[i]->setString(m_paramQH.vcBase.at(i).strAttrName);
				m_ui.uiQH.txtBaseAttrValue[i]->setString(szNum1);
				string str = (m_paramQH.vcBase.at(i).fAttrAdd > 0.000001f) ? "+" : "";
				str.append(szNum2);
				m_ui.uiQH.txtBaseAttrAdd[i]->setString(str);
				m_ui.uiQH.txtBaseAttrAdd[i]->setVisible(m_paramQH.vcBase.at(i).fAttrAdd > 0.000001f);
				m_ui.uiQH.txtBaseAttrAdd[i]->setOpacity(255);

				// 长度适配;
				m_ui.uiQH.txtBaseAttrValue[i]->setPosition(Point(m_ui.uiQH.txtBaseAttr[i]->getContentSize().width + 10.0f, 0.0f)
					+ m_ui.uiQH.txtBaseAttr[i]->getPosition());
				m_ui.uiQH.txtBaseAttrAdd[i]->setPosition(Point(m_ui.uiQH.txtBaseAttrValue[i]->getContentSize().width + 20.0f, 0.0f)
					+ m_ui.uiQH.txtBaseAttrValue[i]->getPosition());
			}
			else
			{
				m_ui.uiQH.txtBaseAttr[i]->setString("");
				m_ui.uiQH.txtBaseAttrValue[i]->setString("");
				m_ui.uiQH.txtBaseAttrAdd[i]->setString("");
			}
		}

		////////////////////////// 普通强化; //////////////////////////
		m_ui.uiQH.pnlQHCost_Normal->setVisible(m_itemType < EQUIP_ITEM_TYPE_HORSE/* && !m_paramQH.bFinal*/);
		m_ui.uiQH.txtCostX1->setString(_TO_STR(m_paramQH.nCost));

		// 货币颜色变化(不够的话显示红色);
		MainParam* _mainParam = MainModel::getInstance()->getMainParam();
		Color3B _color = (m_paramQH.nCost > _mainParam->mCoin) ? Color3B(0xF5, 0x20, 0x20) : Color3B(0xFF, 0xFF, 0xFF);
		m_ui.uiQH.txtCostX1->setColor(_color);

		////////////////////////// 马书强化; //////////////////////////
		m_ui.uiQH.pnlQHCost_HorseBook->setVisible(m_itemType >= EQUIP_ITEM_TYPE_HORSE/* && !m_paramQH.bFinal*/);
		oss.str("");
		oss << m_paramQH.nExp << "/" << m_paramQH.nMaxExp;
		m_ui.uiQH.txtExp->setString(oss.str());
		m_ui.uiQH.txtExp->setVisible(true);
		m_ui.uiQH.progressBarExp->setPercent(100.0f * ((float)m_paramQH.nExp/(float)m_paramQH.nMaxExp));
		m_ui.uiQH.progressBarExpPreview->setPercent(m_ui.uiQH.progressBarExp->getPercent());
		m_ui.uiQH.progressBarExpPreview->setVisible(true);
		m_ui.uiQH.pnlQHMaterial->removeAllChildren();
		for (int i = 0; i < 4; ++i)
		{
			SelectedMaterialParam smp;
			MaterialHeader* _material = MaterialHeader::create(CC_CALLBACK_1(LayerSmithy::modifyMaterialCallback, this), smp);
			_material->setPosition(Vec2(70.0f + 110.0f*i, 60.0f));
			m_ui.uiQH.pnlQHMaterial->addChild(_material);
			m_vcQHMaterials.push_back(_material);
		}
		m_ui.uiQH.btnQH_HorseBook->setEnabled(true);

		// 是否强化至最高等级;
		if (m_paramQH.bFinal)
		{
			// 内容隐藏;
			Vector<Node*> vcChildren;
			if (m_itemType < EQUIP_ITEM_TYPE_HORSE)
				vcChildren = m_ui.uiQH.pnlQHCost_Normal->getChildren();
			else
				vcChildren = m_ui.uiQH.pnlQHCost_HorseBook->getChildren();

			for_each(vcChildren.begin(), vcChildren.end(), [=](Node* _node){
				_node->setVisible(_node->getName().compare("Image_qianghuaxiaohao") == 0
					|| _node->getName().compare("Image_xian2") == 0);
			});

			// 标识;
			ImageView* imgFinal = dynamic_cast<ImageView*>(m_ui.uiQH.pnlQH->getChildByName("img_qh_final"));
			if (imgFinal != nullptr)
			{
				imgFinal->setVisible(true);
			}
			else
			{
				ImageView* imgFinal = ImageView::create("Image/UIEquip/icon/img_yiqianghuadaozuigaoji.png");
				imgFinal->setName("img_qh_final");
				imgFinal->setPosition(Vec2(375.0f, 135.0f));
				m_ui.uiQH.pnlQH->addChild(imgFinal);
			}
		}
		else
		{
			// 内容显示;
			Vector<Node*> vcChildren;
			if (m_itemType < EQUIP_ITEM_TYPE_HORSE)
				vcChildren = m_ui.uiQH.pnlQHCost_Normal->getChildren();
			else
				vcChildren = m_ui.uiQH.pnlQHCost_HorseBook->getChildren();

			for_each(vcChildren.begin(), vcChildren.end(), [=](Node* _node){
				_node->setVisible(true);
			});

			ImageView* imgFinal = dynamic_cast<ImageView*>(m_ui.uiQH.pnlQH->getChildByName("img_qh_final"));
			if (imgFinal != nullptr)
			{
				imgFinal->removeFromParent();
				imgFinal = nullptr;
			}
		}

		// 关联Id;
		iter->second = m_nCurSelToolId;
	}
}

void LayerSmithy::updateJJInfo()
{
	auto _it = m_mapOpPanelWithId.find(SMITHY_OP_TYPE_JJ);
	assert(_it != m_mapOpPanelWithId.end());

	// reset;
	if (_it->second == -1)
	{
		// 刷新数据;
		m_paramJJ.clear();
		SmithyModel::getInstance()->getJJParam(m_nCurSelToolId, m_paramJJ);

		// 刷新UI;
		Node* _pre = m_ui.uiJJ.pnlJJ->getChildByName("tag_JJ_Equ_Icon_1");
		if (_pre != nullptr)
		{
			_pre->removeFromParent();
			_pre = nullptr;
		}
		int nTemplateId = ToolModel::getInstance()->GetToolInfo(m_paramJJ.nEquId)->ntemplateId;
		_pre = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramJJ.nEquIcon));//(nTemplateId);
		((UIToolHeader*)_pre)->setToolId(nTemplateId);
		((UIToolHeader*)_pre)->setIconQua(m_paramJJ.nEquQua);
		RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(nTemplateId);
		CCASSERT(pEquipment != nullptr, "");
		((UIToolHeader*)_pre)->setJJLvl(m_paramJJ.nEquQua, pEquipment->getIntData("stars"));
		((UIToolHeader*)_pre)->setLvlWithoutBg(m_paramJJ.nEquAdvLv);
		_pre->setPosition(Vec2(175.0f, 535.0f));
		_pre->setName("tag_JJ_Equ_Icon_1");
		m_ui.uiJJ.pnlJJ->addChild(_pre, 2);
		m_ui.uiJJ.txtEquName->setString(m_paramJJ.strEquName);
		m_ui.uiJJ.txtEquName->setColor(getEquipQuaColor(m_paramJJ.nEquQua));

		Node* _dst = m_ui.uiJJ.pnlJJ->getChildByName("tag_JJ_Equ_Icon_2");
		if (_dst != nullptr)
		{
			_dst->removeFromParent();
			_dst = nullptr;
		}
		_dst = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramJJ.nEquDstIcon));//(m_paramJJ.nEquDstTemplateId);
		((UIToolHeader*)_dst)->setToolId(m_paramJJ.nEquDstTemplateId);
		((UIToolHeader*)_dst)->setIconQua(m_paramJJ.nEquDstQua);
		pEquipment = DataManager::getInstance()->searchEquipmentById(m_paramJJ.nEquDstTemplateId);
		CCASSERT(pEquipment != nullptr, "");
		((UIToolHeader*)_dst)->setJJLvl(m_paramJJ.nEquDstQua, pEquipment->getIntData("stars"));
		((UIToolHeader*)_dst)->setLvlWithoutBg(m_paramJJ.nEquDstAdvLv);
		_dst->setPosition(Vec2(415.0f, 535.0f));
		_dst->setName("tag_JJ_Equ_Icon_2");
		m_ui.uiJJ.pnlJJ->addChild(_dst, 2);
		m_ui.uiJJ.txtEquDstName->setString(m_paramJJ.strEquDstName);
		m_ui.uiJJ.txtEquDstName->setColor(getEquipQuaColor(m_paramJJ.nEquDstQua));

		//assert(m_paramJJ.vcBase.size() == 1);
		ostringstream oss;
		for (unsigned int i = 0; i < 2; ++i)
		{
			if (m_paramJJ.vcBase.size() > i)
			{
				m_ui.uiJJ.txtAttr[i]->setString(m_paramJJ.vcBase.at(i).strAttrName);

				char szNum1[32], szNum2[32];
				memset(&szNum1, 0, 32);
				memset(&szNum2, 0, 32);
				if (m_paramJJ.vcBase.at(i).attrType >= ATTR_TYPE_HP_COE)
				{
					sprintf(szNum1, "%0.2lf%%", m_paramJJ.vcBase.at(i).fAttrValue * 100.0f);
					sprintf(szNum2, "%0.2lf%%", m_paramJJ.vcBase.at(i).fAttrAdd * 100.0f);
				}
				else
				{
					sprintf(szNum1, "%d", (int)(m_paramJJ.vcBase.at(i).fAttrValue));
					sprintf(szNum2, "%d", (int)(m_paramJJ.vcBase.at(i).fAttrAdd));
				}

				m_ui.uiJJ.txtAttrValue[i]->setString(szNum1);

				oss.str("");
				if (m_paramJJ.vcBase.at(i).fAttrAdd > 0.000001f)
				{
					oss << "+";
				}

				oss << szNum2;//param.vcBase.at(i).fAttrAdd;
				m_ui.uiJJ.txtAttrAdd[i]->setString(oss.str());
				m_ui.uiJJ.txtAttrAdd[i]->setVisible(!m_paramJJ.bFinal);
				m_ui.uiJJ.txtAttrAdd[i]->setOpacity(255);

				// 长度适配;
				m_ui.uiJJ.txtAttrValue[i]->setPosition(Point(m_ui.uiJJ.txtAttr[i]->getContentSize().width + 10.0f, 0.0f)
					+ m_ui.uiJJ.txtAttr[i]->getPosition());
				m_ui.uiJJ.txtAttrAdd[i]->setPosition(Point(m_ui.uiJJ.txtAttrValue[i]->getContentSize().width + 20.0f, 0.0f)
					+ m_ui.uiJJ.txtAttrValue[i]->getPosition());
			}
			else
			{
				m_ui.uiJJ.txtAttr[i]->setString("");
				m_ui.uiJJ.txtAttrValue[i]->setString("");
				m_ui.uiJJ.txtAttrAdd[i]->setString("");
			}
		}

		// 进阶等级;
		if (HeroModel::getInstance()->getMainHeroLevel() < m_paramJJ.nNeedLv)
		{
			oss.str("");
			oss << DataManager::getInstance()->searchCommonTexdtById(20175) << m_paramJJ.nNeedLv;
			m_ui.uiJJ.txtLv->setString(oss.str());
			m_ui.uiJJ.txtLv->setColor(Color3B(0xD9, 0x2E, 0x2E));
			m_ui.uiJJ.txtLv->setVisible(true);
			m_ui.uiJJ.imgCost->setVisible(false);
			m_ui.uiJJ.txtCost->setVisible(false);
		}
		else
		{
			m_ui.uiJJ.txtLv->setVisible(false);
			m_ui.uiJJ.imgCost->setVisible(true);
			m_ui.uiJJ.txtCost->setVisible(true);
			m_ui.uiJJ.txtCost->setString(_TO_STR(m_paramJJ.nCost));
			m_ui.uiJJ.txtCost->setColor(MainModel::getInstance()->getMainParam()->mCoin > m_paramJJ.nCost ? Color3B::WHITE : Color3B(0xD9, 0x2E, 0x2E));
		}

		/////////////////////////// 普通进阶消耗; ////////////////////////////
		if (m_itemType < EQUIP_ITEM_TYPE_HORSE)
		{
			m_ui.uiJJ.pnlJJCost_Normal->setVisible(true);
			m_ui.uiJJ.pnlJJCost_HorseBook->setVisible(false);

			// 清理掉所有;
			m_ui.uiJJ.pnlJJCost_Normal->removeAllChildren();
			m_ui.uiJJ.pnlJJCost_Normal->setScale(1.0f);
			m_ui.uiJJ.pnlJJCost_Normal->setOpacity(255);

			// 重新加载材料;
			unsigned int index = 0;
			float fStartX = 40.0f;
			float fStartY = m_ui.uiJJ.pnlJJCost_Normal->getContentSize().height/2 - 5.0f;
			for (auto iter = m_paramJJ.mapTools.begin(); iter != m_paramJJ.mapTools.end(); ++iter, ++index)
			{
				UIToolHeader* _item = UIToolHeader::create(iter->first);
				_item->setToolId(iter->first);
				_item->setScale(0.75f);
				_item->setPosition(Vec2(fStartX + 95.0f * index, fStartY));
				oss.str("");
				oss << iter->second.at(0) << "/" << iter->second.at(1);
				_item->setBottomNum(oss.str());
				if (iter->second.at(0) >= iter->second.at(1))
				{
					// 材料足够，显示绿色数字;
					_item->setBottomNumColor(Color3B(0x10, 0xA9,0x1E));
					_item->setTouchEnabled(false);
				}
				else
				{
					// 材料不足，显示红色数字，并提供获取路径;
					_item->setBottomNumColor(Color3B(0xF5, 0x20, 0x20));
					_item->setTouchEnabled(true);
					_item->setAddCornerIcon(true);
					_item->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onGetTools, this, iter->first));
				}
				m_ui.uiJJ.pnlJJCost_Normal->addChild(_item);
			}

			//m_ui.uiJJ.txtCost->setVisible(!m_paramJJ.bFinal);
			m_ui.uiJJ.btnJJ->setEnabled(!m_paramJJ.bFinal);
			m_ui.uiJJ.btnJJ->setBright(!m_paramJJ.bFinal);
			//m_ui.uiJJ.imgCost->setVisible(!m_paramJJ.bFinal);

			if (m_paramJJ.bFinal)
			{
				/*if (m_ui.uiJJ.txtNone == nullptr)
				{
					string str("已达到最高进阶等级");
					m_ui.uiJJ.txtNone = Text::create(STR_UTF8(str), FONT_FZZHENGHEI, 40);
					m_ui.uiJJ.txtNone->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
					m_ui.uiJJ.txtNone->setColor(Color3B(0xB5, 0x71, 0x34));
					m_ui.uiJJ.txtNone->setPosition(
						Point(-m_ui.uiJJ.pnlJJCost_Normal->getContentSize().width/2, -m_ui.uiJJ.pnlJJCost_Normal->getContentSize().height/2 + 30.0f)
						+ m_ui.uiJJ.pnlJJCost_Normal->getPosition());
					m_ui.uiJJ.pnlJJCost_Normal->getParent()->addChild(m_ui.uiJJ.txtNone);
				}*/
			}
			else
			{
				if (m_ui.uiJJ.txtNone != nullptr)
				{
					m_ui.uiJJ.txtNone->removeFromParent();
					m_ui.uiJJ.txtNone = nullptr;
				}

				// 材料再跑一次淡入;
				m_ui.uiJJ.pnlJJCost_Normal->setOpacity(0);
				m_ui.uiJJ.pnlJJCost_Normal->runAction(FadeIn::create(0.3f));
			}
		}
		/////////////////////////// 马书进阶消耗; ////////////////////////////
		else
		{
			m_ui.uiJJ.pnlJJCost_Normal->setVisible(false);
			m_ui.uiJJ.pnlJJCost_HorseBook->setVisible(true);

			int nTotalScore = 0;
			int nNeedScore = 0;
			if (m_itemType == EQUIP_ITEM_TYPE_HORSE)
			{
				m_ui.uiJJ.imgScoreIcon->loadTexture("Image/Icon/global/img_shilianjifen_icon.png");
				nTotalScore = max(0, MainModel::getInstance()->getMainParam()->mTrialScore);
				nNeedScore = m_paramJJ.nTrialScore;
			}
			else if (m_itemType == EQUIP_ITEM_TYPE_BOOK)
			{
				m_ui.uiJJ.imgScoreIcon->loadTexture("Image/Icon/global/img_icon_rongyu.png");
				nTotalScore = max(0, MainModel::getInstance()->getMainParam()->mPvpScore);
				nNeedScore = m_paramJJ.nPvpScore;
			}

			oss.str("");
			oss << nTotalScore << "/" << nNeedScore;
			m_ui.uiJJ.txtScore->setString(oss.str());
			m_ui.uiJJ.txtScore->setColor((nTotalScore >= nNeedScore) ? Color3B(0x54, 0xDD, 0x51) : Color3B(0xD9, 0x2E, 0x2E));

			m_ui.uiJJ.txtTipArena->setVisible(m_itemType == EQUIP_ITEM_TYPE_BOOK);
			m_ui.uiJJ.txtTipTrail->setVisible(m_itemType == EQUIP_ITEM_TYPE_HORSE);
		}

		// 关联Id;
		_it->second = m_nCurSelToolId;
	}
}

void LayerSmithy::updateCLInfo()
{
	auto _it = m_mapOpPanelWithId.find(SMITHY_OP_TYPE_CL);
	assert(_it != m_mapOpPanelWithId.end());

	// reset;
	if (_it->second == -1)
	{
		// 刷新数据;
		m_paramCL.clear();
		SmithyModel::getInstance()->getCLParam(m_nCurSelToolId, m_itemType, m_paramCL);

		// 刷新UI;
		Node* _pre = m_ui.uiCL.pnlCL->getChildByName("tag_CL_Equ_Icon_1");
		if (_pre != nullptr)
		{
			_pre->removeFromParent();
			_pre = nullptr;
		}
		_pre = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquIcon));
		_pre->setPosition(Vec2(175.0f, 535.0f));
		_pre->setName("tag_CL_Equ_Icon_1");
		RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(m_paramCL.nEquTemplateId);
		CCASSERT(pEquipment != nullptr, "'");
		(dynamic_cast<UIToolHeader*>(_pre))->setLvlWithoutBg(m_paramCL.nEquAdvLv);
		(dynamic_cast<UIToolHeader*>(_pre))->setJJLvl(m_paramCL.nEquQua, pEquipment->getIntData("stars"));
		(dynamic_cast<UIToolHeader*>(_pre))->setIconQua(m_paramCL.nEquQua);
		(dynamic_cast<UIToolHeader*>(_pre))->showTianMingFlag(m_paramCL.bIsPreFate && (m_paramCL.nEquCLLevel > 0));
		(dynamic_cast<UIToolHeader*>(_pre))->setCLLevel(m_paramCL.nEquCLLevel);
		m_ui.uiCL.pnlCL->addChild(_pre, 2);
		m_ui.uiCL.txtEquName->setString(m_paramCL.strEquName);
		m_ui.uiCL.txtEquName->setColor(getEquipQuaColor(m_paramCL.nEquQua));

		Node* _dst = m_ui.uiCL.pnlCL->getChildByName("tag_CL_Equ_Icon_2");
		if (_dst != nullptr)
		{
			_dst->removeFromParent();
			_dst = nullptr;
		}
		_dst = UIToolHeader::create(ResourceUtils::getSmallIconPath(m_paramCL.nEquDstIcon));
		_dst->setPosition(Vec2(415.0f, 535.0f));
		_dst->setName("tag_CL_Equ_Icon_2");
		(dynamic_cast<UIToolHeader*>(_dst))->setLvlWithoutBg(m_paramCL.nEquAdvLv);
		(dynamic_cast<UIToolHeader*>(_dst))->setJJLvl(m_paramCL.nEquQua, pEquipment->getIntData("stars"));	// 与左侧相同;
		(dynamic_cast<UIToolHeader*>(_dst))->setIconQua(m_paramCL.nEquDstQua);
		(dynamic_cast<UIToolHeader*>(_dst))->showTianMingFlag(m_paramCL.bIsDstFate && (m_paramCL.nEquDstCLLevel > 0));
		(dynamic_cast<UIToolHeader*>(_dst))->setCLLevel(m_paramCL.nEquDstCLLevel);
		m_ui.uiCL.pnlCL->addChild(_dst, 2);
		m_ui.uiCL.txtEquDstName->setString(m_paramCL.strEquDstName);
		m_ui.uiCL.txtEquDstName->setColor(getEquipQuaColor(m_paramCL.nEquDstQua));

		//assert(m_paramCL.vcBase.size() == 1);
		ostringstream oss;
		for (int i = 0; i < 2; ++i)
		{
			if (m_paramCL.vcBase.size() > i)
			{
				m_ui.uiCL.txtAttr[i]->setString(m_paramCL.vcBase.at(i).strAttrName);

				char szNum1[32], szNum2[32];
				memset(&szNum1, 0, 32);
				memset(&szNum2, 0, 32);
				if (m_paramCL.vcBase.at(i).attrType >= ATTR_TYPE_HP_COE)
				{
					sprintf(szNum1, "%0.2lf%%", m_paramCL.vcBase.at(i).fAttrValue * 100.0f);
					sprintf(szNum2, "%0.2lf%%", m_paramCL.vcBase.at(i).fAttrAdd * 100.0f);
				}
				else
				{
					sprintf(szNum1, "%d", (int)(m_paramCL.vcBase.at(i).fAttrValue));
					sprintf(szNum2, "%d", (int)(m_paramCL.vcBase.at(i).fAttrAdd));
				}

				m_ui.uiCL.txtAttrValue[i]->setString(szNum1);

				oss.str("");
				if (m_paramCL.vcBase.at(i).fAttrAdd > 0.000001f)
				{
					oss << "+";
				}

				oss << szNum2;
				m_ui.uiCL.txtAttrAdd[i]->setString(oss.str());
				m_ui.uiCL.txtAttrAdd[i]->setVisible(!m_paramCL.bFinal);
				m_ui.uiCL.txtAttrAdd[i]->setOpacity(255);

				// 长度适配;
				m_ui.uiCL.txtAttrValue[i]->setPosition(Point(m_ui.uiCL.txtAttr[i]->getContentSize().width + 10.0f, 0.0f)
					+ m_ui.uiCL.txtAttr[i]->getPosition());
				m_ui.uiCL.txtAttrAdd[i]->setPosition(Point(m_ui.uiCL.txtAttrValue[i]->getContentSize().width + 20.0f, 0.0f)
					+ m_ui.uiCL.txtAttrValue[i]->getPosition());
			}
			else
			{
				m_ui.uiCL.txtAttr[i]->setString("");
				m_ui.uiCL.txtAttrValue[i]->setString("");
				m_ui.uiCL.txtAttrAdd[i]->setString("");
			}
		}

		// 天命相关;
		// 是否专属武器+马书;
		oss.str("");
		if (m_itemType < EQUIP_ITEM_TYPE_HORSE && !m_paramCL.bIsPreFate)
		{
			oss << DataManager::getInstance()->searchCommonTexdtById(20176);
			m_ui.uiCL.txtCLTitle->setString(oss.str());
			m_ui.uiCL.txtCLTitle->setColor(Color3B(0xE9, 0xF0, 0x35));
			setTextAddStroke(m_ui.uiCL.txtCLTitle, Color3B(0x02, 0x4C, 0x10), 2);
		}
		else
		{
			if (m_paramCL.nEquCLLevel == 0)
			{
				oss << DataManager::getInstance()->searchCommonTexdtById(20177);
				m_ui.uiCL.txtCLTitle->setString(oss.str());
				m_ui.uiCL.txtCLTitle->setColor(Color3B(0x76, 0xBB, 0xC3));
				setTextAddStroke(m_ui.uiCL.txtCLTitle, Color3B(0x1C, 0x49, 0x8D), 2);
			}
			else
			{
				oss << DataManager::getInstance()->searchCommonTexdtById(20178);
				m_ui.uiCL.txtCLTitle->setString(oss.str());
				m_ui.uiCL.txtCLTitle->setColor(Color3B(0xFF, 0xF9, 0x59));
				setTextAddStroke(m_ui.uiCL.txtCLTitle, Color3B(0xBA, 0x0B, 0x0B), 2);
			}
		}
		m_ui.uiCL.txtCLDesc->setString(m_paramCL.strTianMingDesc);

		m_ui.uiCL.btnCLDegrade->setEnabled(m_paramCL.nEquCLLevel > 0);
		m_ui.uiCL.btnCLDegrade->setBright(m_paramCL.nEquCLLevel > 0);

		/////////////////////////// 淬炼消耗; ////////////////////////////

		// 清理掉所有;
		m_ui.uiCL.pnlCLCost->removeAllChildren();
		m_ui.uiCL.pnlCLCost->setScale(1.0f);
		m_ui.uiCL.pnlCLCost->setOpacity(255);

		// 重新加载材料;
		unsigned int index = 0;
		float fStartX = 40.0f;
		float fStartY = m_ui.uiCL.pnlCLCost->getContentSize().height/2 + 5.0f;
		for (auto iter = m_paramCL.mapTools.begin(); iter != m_paramCL.mapTools.end(); ++iter, ++index)
		{
			UIToolHeader* _item = UIToolHeader::create(iter->first);
			_item->setToolId(iter->first);
			_item->setScale(0.75f);
			_item->setPosition(Vec2(fStartX + 95.0f * index, fStartY));
			oss.str("");
			oss << iter->second.at(0) << "/" << iter->second.at(1);
			_item->setBottomNum(oss.str());
			if (iter->second.at(0) >= iter->second.at(1))
			{
				// 材料足够，显示绿色数字;
				_item->setBottomNumColor(Color3B(0x10, 0xA9,0x1E));
				_item->setTouchEnabled(false);
			}
			else
			{
				// 材料不足，显示红色数字，并提供获取路径;
				_item->setBottomNumColor(Color3B(0xF5, 0x20, 0x20));
				_item->setTouchEnabled(true);
				_item->setAddCornerIcon(true);
				_item->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onGetTools, this, iter->first));
			}
			m_ui.uiCL.pnlCLCost->addChild(_item);
		}

		// 淬炼至最高等级;
		m_ui.uiCL.pnlCLCost->setVisible(!m_paramCL.bFinal);
		m_ui.uiCL.btnCL->setVisible(!m_paramCL.bFinal);
		m_ui.uiCL.btnCL->setEnabled(!m_paramCL.bFinal);
		if (m_paramCL.bFinal)
		{
			ImageView* imgFinal = dynamic_cast<ImageView*>(m_ui.uiCL.pnlCL->getChildByName("img_cl_final"));
			if (imgFinal != nullptr)
			{
				imgFinal->setVisible(true);
			}
			else
			{
				ImageView* imgFinal = ImageView::create("Image/UIEquip/icon/img_yicuiliandaozuigaoji.png");
				imgFinal->setName("img_cl_final");
				imgFinal->setPosition(Vec2(420.0f, 100.0f));
				m_ui.uiCL.pnlCL->addChild(imgFinal);
			}
		}
		else
		{
			ImageView* imgFinal = dynamic_cast<ImageView*>(m_ui.uiCL.pnlCL->getChildByName("img_cl_final"));
			if (imgFinal != nullptr)
			{
				imgFinal->removeFromParent();
				imgFinal = nullptr;
			}
		}

		// 关联Id;
		_it->second = m_nCurSelToolId;
	}
}

void LayerSmithy::updateRoleInfo()
{
	// 取英雄信息;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(m_nCurHeroId);
	assert(nullptr != heroParam);

	// 头像;
	UIToolHeader* _hero = UIToolHeader::create(heroParam->mTemplateId);
	_hero->setPosition(Vec2(100.0f, 540.0f));
	m_ui.pnlLeft->addChild(_hero, 2);

	// 名字和等级;
	m_ui.txtRoleName->setString(heroParam->getName());
	string str("LV");
	str.append(_TO_STR(heroParam->mLevel));
	m_ui.txtRoleLv->setString(STR_UTF8(str));
	setTextAddStroke(m_ui.txtRoleLv, Color3B(0x78, 0x0, 0x0),2);

	// 战斗力相关的数值属性;
	updateRoleFightInfo();

	// 装备列表;
	updateEquipmentInfo();
}

void LayerSmithy::updateRoleFightInfo()
{
	// 取英雄信息;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(m_nCurHeroId);
	assert(nullptr != heroParam);

	// 战斗力;
	m_ui.txtFc->setString(_TO_STR(heroParam->mFighting));
	setTextAddStroke(m_ui.txtFc, Color3B(0x7D, 0x3F, 0x1C),2);
	// 攻击类型图标;
	if (heroParam->mAtkTyp == HERO_ATK_MAG)
	{
		m_ui.imgAtk->loadTexture("Image/Icon/global/img_fagong.png");
	}

	// 血量/攻击/物防/法防;
	m_ui.txtHP->setString(_TO_STR(heroParam->mHp));
	m_ui.txtAtk->setString(_TO_STR(heroParam->mAtk));
	m_ui.txtPD->setString(_TO_STR(heroParam->mPDef));
	m_ui.txtMD->setString(_TO_STR(heroParam->mMDef));
}

void LayerSmithy::updateEquipmentInfo()
{
	// 取到当前最新的角色装备数据<type, id>;
	map<int, int>  mapEquipIdList;
	getEquipIdList(mapEquipIdList);

	// 与当前页面的数据比较并更新;
	int nIndex = 0;
	auto iter1 = mapEquipIdList.begin();
	auto iter2 = m_mapEquipment.begin();
	for (; iter1 != mapEquipIdList.end() && iter2 != m_mapEquipment.end(); ++iter1, ++iter2, ++nIndex)
	{
		assert(iter1->first == iter2->first);

		// 角色数据中该位置有装备(若无，则不考虑，因为不允许脱掉装备);
		if (iter1->second != -1)
		{
			int nTemplateId = ToolModel::getInstance()->GetToolInfo(iter1->second)->ntemplateId;
			RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(nTemplateId);
			// 新添加的装备;
			if (iter2->second == nullptr)
			{
				// 创建并添加装备至UI;
				int nCLId = SmithyModel::getInstance()->calcCLId(iter1->second);
				RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(nCLId);
				CCASSERT(_row != nullptr, "");
				UIToolHeader* _equip = UIToolHeader::create(ResourceUtils::getSmallIconPath(_row->getIntData("icon")));
				_equip->setToolId(nTemplateId);
				_equip->setScale(0.9f);
				_equip->setPosition(Vec2(90.0f + nIndex%3 * 118.0f, 208.0f - nIndex/3 * 123.0f));
				m_ui.pnlLeft->addChild(_equip, 2);
				iter2->second = _equip;
				_equip->setTouchEnabled(true);
				updateSingleEquipmentInfo(iter1->second);
				_equip->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onEquipmentClicked, this, (EQUIP_ITEM_TYPE)iter1->first, iter1->second));
			}
			else
			{
				// 判断是否需替换为新装备(templateId是否相同);
				if (iter2->second->getToolId() != nTemplateId)
				{
					// 旧的不去;
					Vec2 pos = iter2->second->getPosition();
					iter2->second->removeFromParent();
					iter2->second = nullptr;

					// 新的不来;
					int nCLId = SmithyModel::getInstance()->calcCLId(iter1->second);
					RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(nCLId);
					CCASSERT(_row != nullptr, "");
					UIToolHeader* _equip = UIToolHeader::create(ResourceUtils::getSmallIconPath(_row->getIntData("icon")));
					_equip->setToolId(nTemplateId);
					_equip->setScale(0.9f);
					_equip->setPosition(pos);
					m_ui.pnlLeft->addChild(_equip, 2);
					iter2->second = _equip;
					_equip->setTouchEnabled(true);
					updateSingleEquipmentInfo(iter1->second);
					_equip->addTouchEventListener(CC_CALLBACK_2(LayerSmithy::onEquipmentClicked, this, (EQUIP_ITEM_TYPE)iter1->first, iter1->second));
				}
				else
				{
					updateSingleEquipmentInfo(iter1->second);
				}
			}
		}
	} // end for
}

void LayerSmithy::updateSingleEquipmentInfo(const int nToolId)
{
	if (nToolId == -1)
		return;

	// 先取到对应的Icon;
	EQUIP_ITEM_TYPE itemType = SmithyModel::getInstance()->getEquType(nToolId);
	auto iter = m_mapEquipment.find(itemType);
	assert(iter != m_mapEquipment.end() && iter->second != nullptr);

	// 1. 强化等级变化(同一Id, 同一templateId);
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	if (nullptr != _info)
	{
		iter->second->setLvlWithoutBg(_info->nadvLvl);
	}
	else
		return;
	
	// 2. 进阶变化(进阶后是不同的templateId);
	iter->second->setIconQua(_info->nItemQua);
	iter->second->setAdvFlag(SmithyModel::getInstance()->checkEquState(nToolId));
	RowData* _row = DataManager::getInstance()->searchEquipmentById(_info->ntemplateId);
	CCASSERT(_row != nullptr, "'");
	iter->second->setJJLvl((TOOL_ITEM_QUA)_info->nItemQua, _row->getIntData("stars"));

	// 3. 更新淬炼后的装备图标;
	m_paramCL.clear();
	SmithyModel::getInstance()->getCLParam(nToolId, itemType, m_paramCL);
	ostringstream oss;
	oss << "Image/Icon/Small/" << m_paramCL.nEquIcon << ".png";
	iter->second->setMainIcon(oss.str());
	iter->second->showTianMingFlag(m_paramCL.bIsPreFate && (m_paramCL.nEquCLLevel > 0));
	iter->second->setCLLevel(m_paramCL.nEquCLLevel);

	// 4. 更新加锁状态;
	if (itemType >= EQUIP_ITEM_TYPE_HORSE)
	{
		int nLv = 80;
		int nId = -1;
		if (itemType == EQUIP_ITEM_TYPE_HORSE)
			nId = 10113;
		else if (itemType == EQUIP_ITEM_TYPE_BOOK)
			nId = 10114;

		// 读取开放等级;
		RowData* _row = DataManager::getInstance()->searchToolSystemById(nId);
		if (nullptr != _row)
			nLv = _row->getIntData("value");

		// 是否加锁;
		iter->second->lock(HeroModel::getInstance()->getMainHeroLevel() < nLv);
	}
}

void LayerSmithy::getEquipIdList( map<int, int>& mapEquipIdList )
{
	// 取英雄信息;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(m_nCurHeroId);
	assert(nullptr != heroParam);

	// 取出6种装备并写入列表;
	/*
		1-帽子  2-衣服  3-武器;
		4-耳环  5-战马  6-兵书;
	*/
	mapEquipIdList.clear();
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_HAT, heroParam->mHeaderId));
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_CLOTHES, heroParam->mBodyId));
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_WEAPON, heroParam->mWeaponId));
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_EARRINGS, heroParam->mEarringId));
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_HORSE, heroParam->mHorseId));
	mapEquipIdList.insert(pair<int, int>(EQUIP_ITEM_TYPE_BOOK, heroParam->mBookId));

	// end;
	assert(mapEquipIdList.size() == 6);
}

void LayerSmithy::onBtnClicked( Ref* sender, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_QH_1:
		case Btn_QH_N:
			{
				int nTimes = (nWidgetName == Btn_QH_1) ? 1 : 5;
				int nRet = m_pController->handleQH(m_nCurSelToolId, nTimes);
				if (nRet != 0)
				{
					string strMsg("");
					if (-1 == nRet || -4 == nRet)
					{
						strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20179));
					}
					else if (-2 == nRet)
					{
						strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20180));
					}
					else if (-3 == nRet)
					{
						LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
						Director::getInstance()->getRunningScene()->addChild(buyEnergy);
						return;
					}

					if (!strMsg.empty())
					{
						CustomTips::showUtf8(strMsg);
					}
				}
				else
				{
					// 音效;
					SoundUtils::playSoundEffect("yangcheng_2");
				}
			}
			break;

		case Btn_JJ:
			{
				int nRet = m_pController->handleJJ(m_nCurSelToolId);
				if (nRet != 0)
				{
					string strMsg("");
					if (-1 == nRet)
					{
						strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20179));
					}
					else if (-2 == nRet)
					{
						strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20181));
					}
					else if (-3 == nRet)
					{
						LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
						Director::getInstance()->getRunningScene()->addChild(buyEnergy);
						
						return;
					}
					else if (-4 == nRet || -6 == nRet)
					{
						//
						bool bAllPassed = true;
						for (auto it : m_paramJJ.mapTools)
						{
							if (it.second.at(0) < it.second.at(1))
							{
								if (!SmithyModel::getInstance()->isBarrierPassedWithTemplateId(it.first))
								{
									bAllPassed = false;
									break;
								}
							}
						}
						if (bAllPassed)
						{
							// 金币进阶;
							LayerAdvanced* _layer = LayerAdvanced::create(this);
							this->addChild(_layer, 10);
							return;
						}
						else
						{
							if (nRet == -4)
								strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20182));
							else if (nRet == -6)
								strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20183));
						}
					}
					else if (-5 == nRet)
					{
						strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20184));
					}

					if (!strMsg.empty())
					{
						CustomTips::showUtf8(strMsg);
					}
				}
				else
				{
					// 音效;
					SoundUtils::playSoundEffect("yangcheng_1");
				}
			}
			break;

		case Btn_QH_Auto_Add:
			{
				// 空位数量;
				//int nCount = std::count_if(m_vcQHMaterials.begin(), m_vcQHMaterials.end(),
				//	[=](MaterialHeader* _material)->bool{
				//		return _material->isEmpty();
				//});

				// 先预判可用数量，没有的话直接弹出获取途径(新增需求 04/15/2016 added by phil @zcjoy);
				vector<const TOOL_INFO*> vcAll = SmithyModel::getInstance()->getQHMaterial_HorseBook(m_nCurSelToolId, 99);
				if (vcAll.size() > 0)
				{
					// 当前已选的要从待选队列中剔除;
					vector<const TOOL_INFO*> vcManualSelected;

					// 计算空位数量，顺便把已选的给挑出来;
					int nCount = 0;
					for (int i = 0; i < m_vcQHMaterials.size(); ++i)
					{
						if (m_vcQHMaterials.at(i)->isEmpty())
						{
							nCount++;
						}
						else
						{
							vcManualSelected.push_back(m_vcQHMaterials.at(i)->getParam()->toolInfo);
						}
					}

					// 自动获取材料并添加显示;
					RowData* _row = nullptr;
					vector<SelectedMaterialParam>  vcAutoSelected;
					vector<const TOOL_INFO*> vcGet = SmithyModel::getInstance()->getQHMaterial_HorseBook(m_nCurSelToolId, nCount, &vcManualSelected);
					for (int i = 0; i < vcGet.size(); ++i)
					{
						_row = DataManager::getInstance()->searchToolById(vcGet.at(i)->ntemplateId);
						CCASSERT(_row != nullptr, "");
						SelectedMaterialParam smp;
						smp.toolInfo = vcGet.at(i);
						smp.nExp = _row->getIntData("value");
						smp.nLogicOrder = i;
						vcAutoSelected.push_back(smp);
					}
					addMaterialPageCallback(vcAutoSelected, false);
				}
				else
				{
					// 读道具表，确定道具种类;
					int nId = -1;
					if (m_itemType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_HORSE)
						nId = 10109;
					else if (m_itemType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_BOOK)
						nId = 10110;

					if (nId != -1)
					{
						vector<int>  vcMaterialType;
						RowData* _row = DataManager::getInstance()->searchToolSystemById(nId);
						CCASSERT(_row != nullptr, "");
						StringFormat::parseCsvStringVec(_row->getStringData("value"), vcMaterialType);

						// 对type排序;
						sort(vcMaterialType.begin(), vcMaterialType.end(), [=](int type1, int type2){
							RowData* _row1 = DataManager::getInstance()->searchToolById(type1);
							RowData* _row2 = DataManager::getInstance()->searchToolById(type2);
							if (_row1 != nullptr && _row2 != nullptr)
							{
								return _row1->getIntData("itemQua") <_row2->getIntData("itemQua");
							}
							return true;
						});

						if (vcMaterialType.size() > 0)
						{
							// 取品质最低的类型，弹出获取途径窗口;
							LayerEquipSource* _equipSource = LayerEquipSource::create();
							_equipSource->showSrc(vcMaterialType.at(0));
							this->addChild(_equipSource, 10);
						}
					}
				}
			}
			break;

		case Btn_QH_HorseBook:
			{
				// 取出道具参数;
				vector<int> vcMaterialTemplateIds;
				for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=, &vcMaterialTemplateIds](MaterialHeader* header){
					if (!header->isEmpty())
					{
						vcMaterialTemplateIds.push_back(header->getParam()->toolInfo->ntemplateId);
					}
				});
				if (vcMaterialTemplateIds.empty())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20185));
				}
				else
				{
					// 锁定所有材料;
					for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=](MaterialHeader* header){
						header->lock();
					});

					m_pController->handleQH_HorseBook(m_nCurSelToolId, &vcMaterialTemplateIds);
				}
			}
			break;

		case Btn_CL:
			{
				bool bOK = true;
				for (auto iter = m_paramCL.mapTools.begin(); iter != m_paramCL.mapTools.end(); ++iter)
				{
					if (iter->second.at(0) < iter->second.at(1))
					{
						bOK = false;
						break;
					}
				}
				if (bOK)
				{
					m_ui.uiCL.btnCL->setEnabled(false);
					m_pController->handleCL(m_nCurSelToolId);
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20182));
				}
			}
			break;

		case Btn_CL_Degrade:
			{
				m_ui.uiCLDegrade.txtCostGold->setString(_TO_STR(m_paramCL.nCostGold));
				m_ui.uiCLDegrade.pnlCLDegrade->setVisible(true);
				ActionCreator::runCommDlgAction(m_ui.uiCLDegrade.imgBg);
				SoundUtils::playSoundEffect("tanchuang");
			}
			break;

		default:
			break;
		}
	}
}

void LayerSmithy::onGetTools( Ref* sender, Widget::TouchEventType type, int nToolId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		LayerEquipSource* _equipSource = LayerEquipSource::create();
		_equipSource->showSrc(nToolId);
		this->addChild(_equipSource, 10);
	}
}

void LayerSmithy::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_EQUIPMENT:
		break;

	case OBS_PARAM_TYPE_EQU_QH_RESULT:
		{
			TOOL_CHANGE_INFO* _changeInfo = (TOOL_CHANGE_INFO*)(param->updateData);
			assert(_changeInfo != nullptr);

			waitRefresh();
			updateRoleFightInfo();
			//updateSingleEquipmentInfo(_changeInfo->nid);
			updateEquipmentInfo();

			// 马书强化结果，进度条满的时候播放动画;
			if (m_itemType >= EQUIP_ITEM_TYPE_HORSE && (m_nHorseBookExpPreview >= m_paramQH.nMaxExp))
			{
				ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anizhenxingqianghua/Anizhenxingqianghua.ExportJson");
				Armature* _animation = Armature::create("Anizhenxingqianghua");
				_animation->getAnimation()->play("Animation2");
				_animation->getAnimation()->setMovementEventCallFunc([=](Armature *armature, MovementEventType movementType, const std::string& movementID){
					if (movementType == COMPLETE)
					{
						m_ui.uiQH.progressBarExp->setPercent(100.0f);
						armature->removeFromParent();
						m_ui.uiQH.progressBarExpPreview->setVisible(false);
					}
				});
				_animation->setPosition(m_ui.uiQH.progressBarExp->getPosition());
				m_ui.uiQH.progressBarExp->getParent()->addChild(_animation, m_ui.uiQH.progressBarExp->getLocalZOrder()+1);
				m_ui.uiQH.txtExp->setVisible(false);
			}

			// 强化等级;
			int nAdvLv = _changeInfo->nadvLvl;
			if (nAdvLv >= 1)
			{
				// 播放动画;
				playQHAnimation(nAdvLv, _changeInfo->bIsCrit);
			}
			else
			{
				// 这个地方禁用再启用主要是针对马书强化并且未升级的情况;
				// 更新前禁用，更新后启用;
				m_ui.uiQH.btnQH_HorseBook->setEnabled(false);
				updateOpPanel();
			}
		}
		break;

	case OBS_PARAM_TYPE_EQU_JJ_RESULT:
		{
			TOOL_CHANGE_INFO* _changeInfo = (TOOL_CHANGE_INFO*)(param->updateData);
			assert(_changeInfo != nullptr);
			waitRefresh();
			updateRoleFightInfo();
			//updateSingleEquipmentInfo(_changeInfo->nid);
			updateEquipmentInfo();

			if (m_itemType < EQUIP_ITEM_TYPE_HORSE)
			{
				playJJAnimation();
			}
			else
			{
				updateOpPanel();
			}
		}
		break;

		// 淬炼/淬炼降级，共用此参数;
	case OBS_PARAM_TYPE_EQU_CL_RESULT:
	case OBS_PARAM_TYPE_EQU_CL_DEGRADE_RESULT:
		{
			m_ui.uiCLDegrade.pnlCLDegrade->setVisible(false);
			TOOL_CHANGE_INFO* _changeInfo = (TOOL_CHANGE_INFO*)(param->updateData);
			assert(_changeInfo != nullptr);
			waitRefresh();
			//updateOpPanel();
			updateRoleFightInfo();
			//updateSingleEquipmentInfo(_changeInfo->nid);
			updateEquipmentInfo();

			playCLAnimation((OBS_PARAM_TYPE_SM)param->id);
		}
		break;

	default:
		break;
	}
}

void LayerSmithy::onEquipmentClicked( Ref* sender, Widget::TouchEventType type, EQUIP_ITEM_TYPE itemType, int nToolId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		// 音效;
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		selectEquipment(itemType, nToolId);
	}
}

void LayerSmithy::waitRefresh()
{
	// 标记所有页面刷新;
	for (auto iter = m_mapOpPanelWithId.begin(); iter != m_mapOpPanelWithId.end(); ++iter)
		iter->second = -1;
}

void LayerSmithy::playQHAnimation(int nAdvLv, bool bCrit)
{
	m_ui.uiQH.btnQH_1->setEnabled(false);
	m_ui.uiQH.btnQH_N->setEnabled(false);
	m_ui.uiQH.btnQH_HorseBook->setEnabled(false);

	// 1.播放图标动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Aniqianghua/Aniqianghua.ExportJson");
	Armature* _armature = Armature::create("Aniqianghua");
	_armature->setPosition(Vec2(113.0f, 535.0f));
	m_ui.uiQH.pnlQH->addChild(_armature, 9);
	_armature->getAnimation()->play("Ani_QH");
	_armature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerSmithy::movementEventCallFunc, this));

	///////// 浮动特效;
	Layout* _parent = m_ui.uiQH.pnlQH;
	
	// 起始坐标;
	float fStart_x = 290.0f;
	float fStart_y = 400.0f;

	// 帧间距(秒/帧);
	float fSpace = 1.0f/25.0f;

	// 属性值变化;
	static int count = 0;
	static bool bOver = false;
	auto callback = [=](float delta)
	{
		if (++count > m_paramQH.vcBase.size())
		{
			count = 0;
			bOver = true;
			Director::getInstance()->getScheduler()->unschedule("attrValue", this);
			return;
		}

		// 单条属性值;
		Layout* _pnlAttr = Layout::create();
		_pnlAttr->setContentSize(Size(200.0f, 50.0f));
		_pnlAttr->setPosition(Vec2(fStart_x + 15.0f, fStart_y));
		_pnlAttr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_parent->addChild(_pnlAttr, 9);
		ImageView* img = ImageView::create(getAttrIcon(m_paramQH.vcBase.at(count-1).attrType));
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setPosition(Vec2(0.0f, 0.0f));
		_pnlAttr->addChild(img);

		char szNum1[32];
		memset(&szNum1, 0, 32);;
		if (m_paramQH.vcBase.at(count-1).attrType >= ATTR_TYPE_HP_COE)
		{
			sprintf(szNum1, "%0.2lf%%", m_paramQH.vcBase.at(count-1).fAttrAdd * nAdvLv * 100.0f);
		}
		else
		{
			sprintf(szNum1, "%d", (int)(m_paramQH.vcBase.at(count-1).fAttrAdd * nAdvLv));
		}

		ostringstream oss;
		oss << m_paramQH.vcBase.at(count-1).strAttrName << " +" << szNum1;
		Text* txtAttr = Text::create(oss.str(), FONT_FZZHENGHEI, 24);
		txtAttr->setColor(Color3B(0x8E, 0xEF, 0x3F));
		setTextAddStroke(txtAttr, Color3B::BLACK, 1);
		txtAttr->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		txtAttr->setPosition(Point(45.0f, 0.0f) + img->getPosition());
		_pnlAttr->addChild(txtAttr);
		oss.str("");

		// 单条属性值动画序列;
		Sequence* seq_attr = Sequence::create(
			EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
			EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(fSpace, Vec2(0.0f, 5.0f)), nullptr)),
			MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
			Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
			CallFuncN::create([=](Node* pSender){
				_pnlAttr->removeFromParent();
				if (bOver)
				{
					updateOpPanel();
					bOver = false;
				}
		}),
			nullptr);
		_pnlAttr->runAction(seq_attr);
	};

	// 暴击;
	if (bCrit)
	{
		ImageView* imgCrit = ImageView::create("Image/UIEquip/icon/img_baoji.png");
		imgCrit->setPosition(Vec2(fStart_x, fStart_y + 40.0f));
		_parent->addChild(imgCrit, 9);

		// 暴击的动画序列;
		Sequence* seq_crit = Sequence::create(
			EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
			EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(5*fSpace, Vec2(0.0f, 5.0f)), nullptr)),
			MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
			Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
			CallFuncN::create([=](Node* pSender){
				imgCrit->removeFromParent();
		}),
			nullptr);
		imgCrit->runAction(seq_crit);
	}

	// 强化等级+N;
	Layout* _pnlQh = Layout::create();
	_pnlQh->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_pnlQh->setContentSize(Size(255.0f, 50.0f));
	_pnlQh->setPosition(Vec2(fStart_x + 15.0f, fStart_y - 20.0f));
	_parent->addChild(_pnlQh, 9);
	ImageView* imgQH = ImageView::create("Image/UIEquip/icon/img_qianghuadengji+.png");
	imgQH->setPosition(Vec2(_pnlQh->getContentSize().width/2 - 20.0f, _pnlQh->getContentSize().height/2));
	_pnlQh->addChild(imgQH);
	Text* txtQHN = Text::create(_TO_STR(nAdvLv), FONT_FZZHENGHEI, 50);
	txtQHN->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	txtQHN->setColor(Color3B(0xF3, 0xDE, 0x98));
	setTextAddStroke(txtQHN, Color3B(0xE3, 0x1C, 0x03), 1);
	txtQHN->setPosition(Vec2(imgQH->getContentSize().width, imgQH->getContentSize().height/2));
	_pnlQh->addChild(txtQHN);

	// 强化等级的动画序列;
	Sequence* seq_QH = Sequence::create(
		EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
		EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(5*fSpace, Vec2(0.0f, 5.0f)), nullptr)),
		CallFuncN::create([this, callback, fSpace](Node* pSender){
			Director::getInstance()->getScheduler()->schedule(callback, this, 6*fSpace, kRepeatForever, 0.000001f, false, "attrValue");
	}),
		MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
		Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
		CallFuncN::create([=](Node* pSender){
			_pnlQh->removeFromParent();
	}),
		nullptr);
	_pnlQh->runAction(seq_QH);
}

void LayerSmithy::playJJAnimation()
{
	if (m_itemType >= EQUIP_ITEM_TYPE_HORSE)
		return;

	m_ui.uiJJ.btnJJ->setEnabled(false);

	// 材料个数;
	int nItemCount = m_ui.uiJJ.pnlJJCost_Normal->getChildrenCount();
	if (nItemCount <= 0 || nItemCount > 3)
		return;

	// 选择动画;
	ostringstream oss;
	oss << "Ani_JJ_" << nItemCount;

	// 1.播放动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniJinjie/AniJinjie.ExportJson");
	Armature* _armature = Armature::create("AniJinjie");
	_armature->setPosition(Vec2(175.0f, 535.0f));	// 左侧装备的位置;
	m_ui.uiJJ.pnlJJ->addChild(_armature, 3);
	_armature->getAnimation()->play(oss.str());
	_armature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerSmithy::movementEventCallFunc, this));

	// 2.物品特效;
	m_ui.uiJJ.pnlJJCost_Normal->runAction(ScaleTo::create(0.3f, 1.5f));
	m_ui.uiJJ.pnlJJCost_Normal->runAction(FadeOut::create(0.3f));

	///////// 浮动特效;
	Layout* _parent = m_ui.uiJJ.pnlJJ;

	// 起始坐标;
	float fStart_x = 290.0f;
	float fStart_y = 400.0f;

	// 帧间距(秒/帧);
	float fSpace = 1.0f/25.0f;

	// 属性值变化;
	static int count = 0;
	auto callback = [=](float delta)
	{
		if (++count > m_paramJJ.vcBase.size())
		{
			count = 0;
			Director::getInstance()->getScheduler()->unschedule("attrValue", this);
			return;
		}

		// 单条属性值;
		Layout* _pnlAttr = Layout::create();
		_pnlAttr->setContentSize(Size(200.0f, 50.0f));
		_pnlAttr->setPosition(Vec2(fStart_x + 15.0f, fStart_y));
		_pnlAttr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_parent->addChild(_pnlAttr, 3);
		ImageView* img = ImageView::create(getAttrIcon(m_paramJJ.vcBase.at(count-1).attrType));
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setPosition(Vec2(0.0f, 0.0f));
		_pnlAttr->addChild(img);

		char szNum1[32];
		memset(&szNum1, 0, 32);;
		if (m_paramJJ.vcBase.at(count-1).attrType >= ATTR_TYPE_HP_COE)
		{
			sprintf(szNum1, "%0.2lf%%", m_paramJJ.vcBase.at(count-1).fAttrAdd * 100.0f);
		}
		else
		{
			sprintf(szNum1, "%d", (int)(m_paramJJ.vcBase.at(count-1).fAttrAdd));
		}

		ostringstream oss;
		oss << m_paramJJ.vcBase.at(count-1).strAttrName << " +" << szNum1;
		Text* txtAttr = Text::create(oss.str(), FONT_FZZHENGHEI, 24);
		txtAttr->setColor(Color3B(0x8E, 0xEF, 0x3F));
		setTextAddStroke(txtAttr, Color3B::BLACK, 1);
		txtAttr->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		txtAttr->setPosition(Point(45.0f, 0.0f) + img->getPosition());
		_pnlAttr->addChild(txtAttr);
		oss.str("");

		// 单条属性值动画序列;
		Sequence* seq_attr = Sequence::create(
			EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
			EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(fSpace, Vec2(0.0f, 5.0f)), nullptr)),
			MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
			Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
			CallFuncN::create([=](Node* pSender){
				_pnlAttr->removeFromParent();
		}),
			nullptr);
		_pnlAttr->runAction(seq_attr);
	};

	// 进阶成功;
	ImageView* imgJJ = ImageView::create("Image/UIEquip/icon/img_jinjiechenggong.png");
	imgJJ->setPosition(Vec2(fStart_x + 15.0f, fStart_y - 20.0f));
	_parent->addChild(imgJJ, 3);

	// 进阶成功动画序列;
	Sequence* seq_JJ = Sequence::create(
		EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
		EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(5*fSpace, Vec2(0.0f, 5.0f)), nullptr)),
		CallFuncN::create([this, callback, fSpace](Node* pSender){
			Director::getInstance()->getScheduler()->schedule(callback, this, 6*fSpace, kRepeatForever, 0.000001f, false, "attrValue");
	}),
		MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
		Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
		CallFuncN::create([=](Node* pSender){
			imgJJ->removeFromParent();
	}),
		nullptr);
	imgJJ->runAction(seq_JJ);
}

void LayerSmithy::playCLAnimation(OBS_PARAM_TYPE_SM clType)
{
	if (clType == OBS_PARAM_TYPE_EQU_CL_DEGRADE_RESULT)
	{
		updateOpPanel();
		return;
	}

	// 材料个数;
	int nItemCount = m_ui.uiCL.pnlCLCost->getChildrenCount();
	if (nItemCount <= 0 || nItemCount > 2)
		return;

	// 选择动画;
	ostringstream oss;
	oss << "Ani_CuiLian_XG" << nItemCount;

	// 物品替换动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_CuiLianTeXiao_XG/Ani_CuiLianTeXiao_XG.ExportJson");
	Armature* _armature = Armature::create("Ani_CuiLianTeXiao_XG");
	_armature->setPosition(Vec2(175.0f, 535.0f));	// 左侧装备的位置;
	m_ui.uiCL.pnlCL->addChild(_armature, 3);
	_armature->getAnimation()->play(oss.str());
	_armature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerSmithy::movementEventCallFunc, this));
	_armature->getAnimation()->setFrameEventCallFunc(bind(&LayerSmithy::frameEventCallFunc, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ));
	
	m_bIsCL = true;
}

void LayerSmithy::movementEventCallFunc( Armature *armature, MovementEventType movementType, const string& movementID )
{
	if (movementType == COMPLETE)
	{
		if (movementID.compare("Ani_QH") == 0)
		{
			armature->removeFromParent();
			m_ui.uiQH.btnQH_1->setEnabled(true);
			m_ui.uiQH.btnQH_N->setEnabled(true);
			//updateOpPanel();
		}
		else if (movementID.find("Ani_JJ") != string::npos)
		{
			armature->removeFromParent();
			m_ui.uiJJ.btnJJ->setEnabled(true);
			updateOpPanel();
		}
		else if (movementID.compare("Ani_XL") == 0)
		{
			armature->removeFromParent();
		}

		//判断是否要调用缘分弹窗，主角不调用
		//兵书，马书，武器淬炼开启天命时弹缘分窗

		HeroParam* hero = HeroModel::getInstance()->searchHeroById(m_nCurHeroId);
		if(hero == nullptr || hero->isMainHero() || !m_bIsCL)
		{
			return;
		}
		// 刷新数据;
		m_paramCL.clear();
		SmithyModel::getInstance()->getCLParam(m_nCurSelToolId, m_itemType, m_paramCL);
		if(m_itemType == EQUIP_ITEM_TYPE_BOOK || m_itemType == EQUIP_ITEM_TYPE_HORSE || m_itemType == EQUIP_ITEM_TYPE_WEAPON)
		{
			if(m_paramCL.bIsPreFate && m_paramCL.nEquCLLevel == 1)
			{
				this->addChild(LayerHeroRelation::create(hero->mTemplateId, false, nullptr, true, m_nCurSelToolId, m_itemType), 99);
				m_bIsCL = false;
			}
		}
	}
}

void LayerSmithy::frameEventCallFunc( Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex )
{
	if (frameEventName.compare("change") == 0)		// 更新淬炼数据;
	{
		updateOpPanel();

		// 播放天命文字变化的遮罩动画;
		/*ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_CuiLianTeXiao_TM/Ani_CuiLianTeXiao_TM.ExportJson");
		Armature* _armature = Armature::create("Ani_CuiLianTeXiao_TM");
		_armature->setPosition(Vec2(175.0f, 535.0f));	// "可激活/已激活天命"文字标题位置;
		m_ui.uiCL.pnlCL->addChild(_armature, 3);
		_armature->getAnimation()->play("Ani_CuiLian_TM");
		_armature->getAnimation()->setFrameEventCallFunc(bind(&LayerSmithy::frameEventCallFunc, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ));*/
	}
	else if (frameEventName.compare("changeTitle") == 0)
	{
		//updateOpPanel();
	}
}

void LayerSmithy::playEnterAnimation()
{
	// 左/右面板移动距离和时间;
	float shiftPosX_L = 400.0f;
	float shiftPosX_R = 630.0f;
	float shiftTime = 0.4f;

	// 左面板飞入;
	m_ui.pnlLeft->setPosition(Point(-shiftPosX_L, 0.0f) + m_ui.pnlLeft->getPosition());
	auto action1 = EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX_L, 0.0f)));
	m_ui.pnlLeft->runAction(action1);

	// 右面板飞入;
	auto callBack = [](Node* pSender){ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_EQUIP_INFO); };
	m_ui.pnlRight->setPosition(Point(shiftPosX_R, 0.0f) + m_ui.pnlRight->getPosition());

		auto seq = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(-shiftPosX_R, 0.0f))),
		CallFuncN::create(callBack), nullptr);

	m_ui.pnlRight->runAction(seq);
}

std::string LayerSmithy::getAttrIcon( SMITHY_EQU_ATTR_TYPE attrType )
{
	string strIconName = "";
	switch (attrType)
	{
	case ATTR_TYPE_HP:
	case ATTR_TYPE_HP_COE:
		strIconName.assign("Image/Icon/global/img_xueliang.png");
		break;
	case ATTR_TYPE_ATK:
	case ATTR_TYPE_ATK_COE:
		{
			HeroParam* _hero = HeroModel::getInstance()->searchHeroById(m_nCurHeroId);
			CCASSERT(_hero != nullptr, "");
			if (_hero->mAtkTyp == HERO_ATK_PHY)		// 物理攻击;
			{
				strIconName.assign("Image/Icon/global/img_gongji.png");
			}
			else if (_hero->mAtkTyp == HERO_ATK_MAG)	// 法术攻击;
			{
				strIconName.assign("Image/Icon/global/img_fagong.png");
			}
		}
		break;
	case ATTR_TYPE_PDD:
	case ATTR_TYPE_PDD_COE:
		strIconName.assign("Image/Icon/global/img_wufang.png");
		break;
	case ATTR_TYPE_MDD:
	case ATTR_TYPE_MDD_COE:
		strIconName.assign("Image/Icon/global/img_mofang.png");
		break;

	case ATTR_TYPE_CRI_PRB:
		strIconName.assign("Image/Icon/global/img_shuxing_baoji.png");
		break;
	case ATTR_TYPE_CRI_RES:
		strIconName.assign("Image/Icon/global/img_shuxing_baokang.png");
		break;
	case ATTR_TYPE_HIT:
		strIconName.assign("Image/Icon/global/img_shuxing_mingzhong.png");
		break;
	case ATTR_TYPE_MISS:
		strIconName.assign("Image/Icon/global/img_shuxing_shanbi.png");
		break;
	default:
		break;
	}

	return strIconName;
}

void LayerSmithy::modifyMaterialCallback( MaterialClickEvent event )
{
	if (event == MCE_ADD)
	{
		// 先预判可用数量，没有的话直接弹出获取途径(新增需求 04/15/2016 added by phil @zcjoy);
		vector<const TOOL_INFO*> vcAll = SmithyModel::getInstance()->getQHMaterial_HorseBook(m_nCurSelToolId, 99);
		if (vcAll.size() > 0)
		{
			vector<SelectedMaterialParam> vcSelectedMaterials;
			for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=, &vcSelectedMaterials](MaterialHeader* _header){
				if (!_header->isEmpty())
				{
					vcSelectedMaterials.push_back(*(_header->getParam()));
				}
			});

			LayerAddMaterial* _layer = LayerAddMaterial::create(m_nCurSelToolId, CC_CALLBACK_2(LayerSmithy::addMaterialPageCallback, this),
				vcSelectedMaterials);
			this->addChild(_layer, 10);
		}
		else
		{
			// 读道具表，确定道具种类;
			int nId = -1;
			if (m_itemType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_HORSE)
				nId = 10109;
			else if (m_itemType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_BOOK)
				nId = 10110;

			if (nId != -1)
			{
				vector<int>  vcMaterialType;
				RowData* _row = DataManager::getInstance()->searchToolSystemById(nId);
				CCASSERT(_row != nullptr, "");
				StringFormat::parseCsvStringVec(_row->getStringData("value"), vcMaterialType);

				// 对type排序;
				sort(vcMaterialType.begin(), vcMaterialType.end(), [=](int type1, int type2){
					RowData* _row1 = DataManager::getInstance()->searchToolById(type1);
					RowData* _row2 = DataManager::getInstance()->searchToolById(type2);
					if (_row1 != nullptr && _row2 != nullptr)
					{
						return _row1->getIntData("itemQua") <_row2->getIntData("itemQua");
					}
					return true;
				});

				if (vcMaterialType.size() > 0)
				{
					// 取品质最低的类型，弹出获取途径窗口;
					LayerEquipSource* _equipSource = LayerEquipSource::create();
					_equipSource->showSrc(vcMaterialType.at(0));
					this->addChild(_equipSource, 10);
				}
			}
		}
	}
	else if (event == MCE_DEL)
	{
		updateQHExp();
	}
}

void LayerSmithy::addMaterialPageCallback( vector<SelectedMaterialParam> vcTools, bool bClean /*= false*/ )
{
	if (vcTools.empty())
		return;

	// 清理所有;
	if (bClean)
	{
		for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=](MaterialHeader* _material)->void {
			if (!_material->isEmpty())
				_material->removeMaterial();
		});
	}

	// 重新排序;
	// 按品质排序;
	sort(vcTools.begin(), vcTools.end(), [=](SelectedMaterialParam selected1, SelectedMaterialParam selected2)->bool {
		return selected1.toolInfo->nItemQua < selected2.toolInfo->nItemQua;
	});

	// 下一帧执行;
	// 是因为上一步清理时已经执行了帧延迟，会导致在同一帧内把已经添加的"x"按钮给删除掉;
	getScheduler()->schedule([=](float dt)->void {
		this->getScheduler()->unschedule("LayerSmithy::addMaterialPageCallback", this);

		// 重新添加至空位;
		for (int i = 0; i < min(4, (int)vcTools.size()); ++i)
		{
			auto it = find_if(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=](MaterialHeader* header)->bool {
				return (header->isEmpty());
			});
			if (it != m_vcQHMaterials.end())
			{
				(*it)->updateMaterial(vcTools.at(i));
			}
		}

		// 刷新经验;
		updateQHExp();
	},
		this, 0, 0, 0, false, "LayerSmithy::addMaterialPageCallback");
}

void LayerSmithy::updateQHExp()
{
	int nExpAdded = 0;
	for_each(m_vcQHMaterials.begin(), m_vcQHMaterials.end(), [=, &nExpAdded](MaterialHeader* header){
		if (!header->isEmpty())
		{
			nExpAdded += header->getParam()->nExp;
		}
	});
	float percent = (float)(m_paramQH.nExp + nExpAdded)/(float)m_paramQH.nMaxExp;
	if (percent > 1.000001f)
		percent = 1.0f;
	
	m_ui.uiQH.progressBarExpPreview->setPercent(percent*100.0f);
	if (m_ui.uiQH.progressBarExpPreview->getPercent() > m_ui.uiQH.progressBarExp->getPercent())
	{
		m_ui.uiQH.progressBarExpPreview->runAction(RepeatForever::create(
			Sequence::create(EaseSineIn::create(FadeIn::create(0.5f)), EaseSineOut::create(FadeOut::create(0.5f)), nullptr)));
	}
	else
	{
		m_ui.uiQH.progressBarExpPreview->stopAllActions();
		m_ui.uiQH.progressBarExpPreview->setPercent(m_ui.uiQH.progressBarExp->getPercent());
	}

	ostringstream oss;
	oss << m_paramQH.nExp + nExpAdded << "/" << m_paramQH.nMaxExp;
	m_ui.uiQH.txtExp->setString(oss.str());
	m_nHorseBookExpPreview = m_paramQH.nExp + nExpAdded;
}

void LayerSmithy::updateReddot()
{
	// 先刷新强化/进阶标签;
	SMITHY_OP_TYPE opType = SmithyModel::getInstance()->checkOpType(m_nCurSelToolId);
	switch (opType)
	{
	case SMITHY_OP_TYPE_QH:
		{
			int nRet = SmithyModel::getInstance()->isEquQHValid(m_nCurSelToolId);
			showPageReddot(m_ui.cbPageQH, (nRet == 0));
		}
		break;
	case SMITHY_OP_TYPE_JJ:
		{
			int nRet = SmithyModel::getInstance()->isEquJJValid(m_nCurSelToolId);
			showPageReddot(m_ui.cbPageQH, (nRet == 0));
		}
		break;
	case SMITHY_OP_TYPE_CL:
		break;
	case SMITHY_OP_TYPE_NONE:
		break;
	default:
		break;
	}

	// 再刷新淬炼标签;
	int nRet = SmithyModel::getInstance()->isEquCLValid(m_nCurSelToolId);
	showPageReddot(m_ui.cbPageCL, (nRet == 0));
}

void LayerSmithy::showPageReddot( CheckBox* cbPage, bool bShow )
{
	if (cbPage == nullptr)
		return;

	if (bShow)
	{
		ImageView* imgReddot = dynamic_cast<ImageView*>(cbPage->getChildByName("img_flag_red_dot"));
		if (nullptr == imgReddot)
		{
			imgReddot = ImageView::create("Image/Icon/global/img_dian.png");
			imgReddot->setName("img_flag_red_dot");
			cbPage->addChild(imgReddot, 2);
		}

		bool bSelected = cbPage->getSelectedState();
		if (bSelected)
		{
			imgReddot->setPosition(Vec2(cbPage->getContentSize().width - imgReddot->getContentSize().width/2.0f, 
				cbPage->getContentSize().height - imgReddot->getContentSize().height/2.0f));
		}
		else
		{
			imgReddot->setPosition(Vec2(cbPage->getContentSize().width - imgReddot->getContentSize().width/2.0f - 10.0f, 
				cbPage->getContentSize().height - imgReddot->getContentSize().height/2.0f));
		}
	}
	else
	{
		ImageView* imgReddot = dynamic_cast<ImageView*>(cbPage->getChildByName("img_flag_red_dot"));
		if (nullptr != imgReddot)
		{
			imgReddot->removeFromParent();
			imgReddot = nullptr;
		}
	}
}
