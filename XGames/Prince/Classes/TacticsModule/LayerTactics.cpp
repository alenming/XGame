#include "LayerTactics.h"
#include "Utils/SoundUtils.h"
#include "Utils/StringFormat.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Common/Common.h"
#include "UI/UIDef.h"
#include "ModelTactics.h"
#include "TacticsController.h"
#include "ToolModule/ToolDef.h"
#include "Temp/CustomTips.h"
#include "Utils/StringFormat.h"
#include "GuideCenter/GuideManager.h"
#include "SmithyModule/LayerEquipSource.h"
#include "LayerAddMat.h"
#include "RobModule/SceneRob.h"


//阵法列表之间子项间距
#define GAP_TACTICS_LIST		125.0f
//阵法图标tag
#define TACTICS_ICON_TAG		1001

//添加在按钮上的材料道具TAG
#define MATTERIAL_ADDED_TAG		1101
//排升级材料ID起始值
#define MATTERIAL_ID_BEGIN		10016

LayerTactics::LayerTactics()
{
	ModelTactics::getInstance()->addObserver(this);
	m_bIsTacticsMatEnough = false;
	m_vRowUpgradeMatId.clear();
	m_vSeledMat.clear();
	m_iCostCoin = 0;
	m_iIdx = 0;
	m_iCurRefineTacticsKind = 0;
	m_eCurRowType = FRONT_ROW_TYPE;
	mPlayAniRow = FRONT_ROW_TYPE;
	m_bIsRowAniEnd = true;
	m_bIsCurRefineView = false;
	m_iCurTacticsKind = -1;
	for(int i=0; i<NUM_TACTICS_LIST; i++)
	{
		m_activiedTacs[i] = false;
	}
}

LayerTactics::~LayerTactics()
{
	ModelTactics::getInstance()->removeObserver(this);
}


LayerTactics* LayerTactics::create( bool isPush )
{
	LayerTactics* _layer = new LayerTactics();
	if (_layer->init(isPush))
	{
		_layer->autorelease();
		return _layer;
	}
	else
	{
		delete _layer;
		return nullptr;
	}
}

bool LayerTactics::init(bool isPush)
{
	if(!Layer::init())
	{
		return false;
	}

	m_bIsPushScene = isPush;

	initUI();

	if(ModelTactics::getInstance()->isInittedFromSvr())
	{
		updateTacticsDataList();
	}
	
	updateDataFightPower();

	return true;
}

void LayerTactics::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		if(!ModelTactics::getInstance()->isInittedFromSvr())
		{
			TacticsController::getInstance()->sendGetTacticsListMsg();
		}
		else
		{	
			if(m_bIsCurRefineView)
			{
				updateDataRefineView();		//刷新阵法精炼材料数量
			}
		}
		this->getScheduler()->unschedule("LayerTactics::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerTactics::sendMsg");
}

void LayerTactics::onExit()
{
	Layer::onExit();
}

void LayerTactics::initUI()
{
	//加载阵法主界面
	string strFileName;
	strFileName.assign("Image/UiZhenFa/UiZhenFa.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();		
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_ZHENFA);
	m_ui.pRoot->addChild(_layerCommHeadInfo);

	initRowUpMaterialData();
	initMainTacticsUI();
	initRowUpgradeUI();
	initRefineUI();
	
	//切入动画
	rowEnterAni();
	tacticsEnterAni();
}

void LayerTactics::initMainTacticsUI()
{
	//战斗力
	m_ui.txtFightPower = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Fc"));
	m_ui.txtFightPower->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtFightPower, Color3B(0x7d, 0x3f, 0x1c), 3);
	
	//阵法属性
	m_ui.txtTacticsAttributes = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Count"));
	m_ui.txtTacticsAttributes->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtTacticsAttributes, Color3B(0x76, 0x11, 0x00), 3);

	//掠夺入口
	m_ui.btnRobMaterial = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Button_lueduo"));
	m_ui.btnRobMaterial->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Rob_Mat));
	//Text* txtRob = (Text*)m_ui.btnRobMaterial->getChildByName("Label_wenben");
	//txtRob->setFontName(FONT_FZZHENGHEI);

	//前中后排按钮
	m_ui.btnFrontRow = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Front"));
	m_ui.btnFrontRow->setHighlighted(true);
	m_ui.btnFrontRow->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Front_Row));
	m_ui.btnFrontRow->setEnabled(false);		//高亮时置不可点击

	m_ui.btnMidRow = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Middle"));
	m_ui.btnMidRow->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Mid_Row));

	m_ui.btnBackRow = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Back"));
	m_ui.btnBackRow->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Back_Row));

	//前中后排等级
	m_ui.txtFrontLv = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Front_Level"));
	m_ui.txtFrontLv->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtFrontLv, Color3B(0x80, 0x1a, 0x1a), 2);
	m_ui.txtMidLv = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Middle_Level"));
	m_ui.txtMidLv->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtMidLv, Color3B(0x80, 0x1a, 0x1a), 2);
	m_ui.txtBackLv = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Back_Level"));
	m_ui.txtBackLv->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtBackLv, Color3B(0x80, 0x1a, 0x1a), 2);

	//阵法列表
	Layout* zhenFa = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_item"));
	m_ui.scrollTacticsList = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_List"));
	Size size(0, 0);
	if(NUM_TACTICS_LIST <= 5)
	{
		size.width = m_ui.scrollTacticsList->getInnerContainerSize().width;
	}
	else
	{
		size.width = zhenFa->getContentSize().width*NUM_TACTICS_LIST + (GAP_TACTICS_LIST - zhenFa->getContentSize().width)*(NUM_TACTICS_LIST-1) + 20;
	}
	size.height = m_ui.scrollTacticsList->getInnerContainerSize().height;
	m_ui.scrollTacticsList->setInnerContainerSize(size);

	Layout* tacticsPanel = zhenFa;
	for(int i=0; i<NUM_TACTICS_LIST; i++)
	{
		sTactics_List tacticsPnl;
		tacticsPnl.pnl = tacticsPanel;
		tacticsPnl.btnTactics = (Button*)tacticsPanel->getChildByName("Btn_zhen");
		tacticsPnl.btnTactics->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnSelTacticsClicked, this, i));
		tacticsPnl.imgFlag = (ImageView*)tacticsPanel->getChildByName("Img_flag");
		tacticsPnl.txtLevel = (Text*)tacticsPanel->getChildByName("Lab_ZhenLv");
		tacticsPnl.txtLevel->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(tacticsPnl.txtLevel, Color3B(0x79, 0x03, 0x03), 2);
		
		tacticsPnl.pnl->setPosition(ccpAdd(zhenFa->getPosition(), Point(GAP_TACTICS_LIST*i, 0)));
		tacticsPnl.pnl->setAnchorPoint(Point::ZERO);
		if(i != 0)
		{
			m_ui.scrollTacticsList->addChild(tacticsPnl.pnl);
		}
		m_ui.sTacticsList.push_back(tacticsPnl);
		tacticsPanel = (Layout*)zhenFa->clone();
	}


	//开启阵法提示
	m_ui.txtOpenTips = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Tips"));
	m_ui.txtOpenTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtOpenTips, Color3B::BLACK, 2);
}

void LayerTactics::initRowUpgradeUI()
{
	//排视图
	m_ui.imgRowView = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_rowLayer"));

	//当前选中排
	m_ui.imgRow = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Position"));
	m_ui.txtRowLv = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Level"));
	m_ui.txtRowLv->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtRowLv, Color3B(0x80, 0x1a, 0x1a), 2);

	//升级所需经验
	m_ui.txtRowLvUpExp = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Exp"));
	m_ui.txtRowLvUpExp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtRowLvUpExp, Color3B(0x4e, 0x2a, 0x20), 2);

	m_ui.barExpProgress = (LoadingBar*)(Helper::seekWidgetByName(m_ui.pRoot, "ProgressBar"));
	m_ui.barExpProgressPreview = (LoadingBar*)(Helper::seekWidgetByName(m_ui.pRoot, "ProgressBar_preview"));
	m_ui.barExpProgressPreview->setPercent(0.0);

	//排强化属性
	m_ui.sRowData.clear();
	for(int i=0; i<4; i++)
	{
		sRow_Attributes_Data rowData;
		rowData.imgRowAttributesImg = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, string("Img_Attributes")+_TO_STR(i+1)));
		rowData.txtRowAttributesKind = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, string("Lab_Attributes")+_TO_STR(i+1)));
		rowData.txtRowAttributesKind->setFontName(FONT_FZZHENGHEI);
		rowData.txtRowAttributesValue = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, string("Lab_Value")+_TO_STR(i+1)));
		rowData.txtRowAttributesValue->setFontName(FONT_FZZHENGHEI);
		rowData.txtRowAttributesValueUp = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, string("Lab_ValueUp")+_TO_STR(i+1)));
		rowData.txtRowAttributesValueUp->setFontName(FONT_FZZHENGHEI);
		rowData.imgRowUpgradeFlag = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, string("Img_Up")+_TO_STR(i+1)));
		m_ui.sRowData.push_back(rowData);
	}

	//排升级消耗的银币
	m_ui.txtRowLvUpCoin = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Coin"));
	m_ui.txtRowLvUpCoin->setFontName(FONT_FZZHENGHEI);
	
	//四个添加升级材料按钮
	m_ui.btnPropAdd.clear();
	for(int i=0; i<4; i++)
	{		
		Button* btn = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, string("Btn_Item")+_TO_STR(i+1)));
		btn->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_PropAdd1+i));
		m_ui.btnPropAdd.push_back(btn);
	}
	//四个取消升级材料按钮
	m_ui.btnPropCancel.clear();
	for(int i=0; i<4; i++)
	{		
		Button* btn = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, string("Btn_Close")+_TO_STR(i+1)));
		btn->setVisible(false);
		btn->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_PropCancel1+i));
		m_ui.btnPropCancel.push_back(btn);
	}

	//自动添加和强化
	m_ui.btnAutoAdd = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Auto"));
	m_ui.btnAutoAdd->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_AutoAdd));
	m_ui.btnUpgrade = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Str"));
	m_ui.btnUpgrade->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Upgrade));
}

void LayerTactics::initRefineUI()
{
	//精炼视图
	m_ui.imgRefineView = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_refineLayer"));
	m_ui.pnlTactics = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_ZhenFa"));
	m_ui.imgTacticsIcon = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_ZhenFaIcon"));
	m_ui.txtTacticsName = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_TacticsName"));
	m_ui.txtTacticsName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtTacticsName, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtQuality = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_quality"));
	m_ui.txtQuality->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtQuality, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtUnactivated = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Activation"));
	m_ui.txtUnactivated->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtUnactivated, Color3B(0x21, 0x02, 0x02), 2);
	
	//阵法属性和升级属性
	m_ui.txtAttribute1 = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Attribute1"));
	m_ui.txtAttribute1->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtAttribute1, Color3B(0x1d, 0x2f, 0x07), 2);
	m_ui.txtAttribute2 = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Attribute2"));
	m_ui.txtAttribute2->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtAttribute2, Color3B(0x1d, 0x2f, 0x07), 2);

	//消耗银币
	m_ui.txtNextLvCoin = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_CountCoin"));
	m_ui.txtNextLvCoin->setFontName(FONT_FZZHENGHEI);
	
	//材料、数量
	m_ui.pnlMaterial1 = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Item1"));
	m_ui.pnlMaterial2 = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Item2"));
	m_ui.txtMaterialCount = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_CountMaterial"));
	m_ui.txtMaterialCount->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtMaterialCount, Color3B(0x1d, 0x2f, 0x07), 2);

	//其它需要描边设置字体的文本
	Text* txtTips3 = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Tips3"));
	txtTips3->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtTips3, Color3B::BLACK, 2);
	m_ui.txtNeedMaterailTips = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Tips4"));
	m_ui.txtNeedMaterailTips->setFontName(FONT_FZZHENGHEI);

	//使用/精炼按钮
	m_ui.btnUse = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Use"));
	m_ui.btnUse->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Use));
	m_ui.btnRefine = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Refine"));
	m_ui.btnRefine->addTouchEventListener(CC_CALLBACK_2(LayerTactics::onBtnClicked, this, Btn_Refine));
}

void LayerTactics::rowEnterAni()
{
	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.5f;

	// 面板飞入;
	Layout* upPnl = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Up"));

	Margin _oriMargin = upPnl->getLayoutParameter()->getMargin();
	Margin _margin = _oriMargin;
	_margin.left += shiftPosX;
	upPnl->getLayoutParameter()->setMargin(_margin);
	auto seq1 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(-shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			upPnl->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	upPnl->runAction(seq1);
}

void LayerTactics::tacticsEnterAni()
{
	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.5f;

	Margin _oriMargin = m_ui.pnlTactics->getLayoutParameter()->getMargin();
	Margin _margin = _oriMargin;
	_margin.right += shiftPosX;
	m_ui.pnlTactics->getLayoutParameter()->setMargin(_margin);
	auto seq2 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			m_ui.pnlTactics->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	m_ui.pnlTactics->runAction(seq2);
}

void LayerTactics::update(float dt)
{
	if(m_bIsPushScene)
	{
		Director::getInstance()->popScene();
	}
	else
	{
		Director::getInstance()->replaceScene(MainCityScene::create());
	}

	this->unscheduleUpdate();
}

void LayerTactics::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case Btn_Front_Row:
			{
				m_eCurRowType = FRONT_ROW_TYPE;
				m_ui.btnFrontRow->setHighlighted(true);
				m_ui.btnFrontRow->setEnabled(false);
				m_ui.btnMidRow->setEnabled(true);
				m_ui.btnMidRow->setHighlighted(false);
				m_ui.btnBackRow->setEnabled(true);
				m_ui.btnBackRow->setHighlighted(false);
				switchRowAndRefineView(1);
			}
			break;

		case Btn_Mid_Row:
			{
				m_eCurRowType = MIDDLE_ROW_TYPE;
				m_ui.btnMidRow->setHighlighted(true);
				m_ui.btnMidRow->setEnabled(false);
				m_ui.btnFrontRow->setEnabled(true);
				m_ui.btnFrontRow->setHighlighted(false);
				m_ui.btnBackRow->setEnabled(true);
				m_ui.btnBackRow->setHighlighted(false);
				switchRowAndRefineView(1);
			}
			break;

		case Btn_Back_Row:
			{
				m_eCurRowType = BACK_ROW_TYPE;
				m_ui.btnBackRow->setHighlighted(true);
				m_ui.btnBackRow->setEnabled(false);
				m_ui.btnFrontRow->setEnabled(true);
				m_ui.btnFrontRow->setHighlighted(false);
				m_ui.btnMidRow->setEnabled(true);
				m_ui.btnMidRow->setHighlighted(false);
				switchRowAndRefineView(1);
			}
			break;

		case Btn_PropAdd1:
		case Btn_PropAdd2:
		case Btn_PropAdd3:
		case Btn_PropAdd4:
			{
				//若无材料，弹出最低级道具获取途径
				bool haveProp = false;
				for(int i=0; i<m_vRowUpgradeMatId.size(); i++)
				{
					const TOOL_INFO* info = ToolModel::getInstance()->GetToolInfoBytemplateId(m_vRowUpgradeMatId.at(i));
					if(info)
					{
						haveProp = true;
						break;
					}
					continue;
				}
				if(!haveProp)
				{
					LayerEquipSource* pSmithy = LayerEquipSource::create();
					this->addChild(pSmithy);
					pSmithy->showSrc(m_vRowUpgradeMatId.at(0));
				}
				else
				{
					LayerAddMat* layer = LayerAddMat::create(&m_vSeledMat, m_eCurRowType);
					this->addChild(layer);
				}
			}
			break;

		case Btn_PropCancel1:
		case Btn_PropCancel2:
		case Btn_PropCancel3:
		case Btn_PropCancel4:
			{
				UIToolHeader* matterail = (UIToolHeader*)m_ui.btnPropAdd.at(nWidgetName-Btn_PropCancel1)->getChildByTag(MATTERIAL_ADDED_TAG);

				if(!m_vSeledMat.empty())
				{
					int isDeled = false;
					m_vSeledMat.erase(
						std::remove_if(m_vSeledMat.begin(), m_vSeledMat.end(), 
						[&](int item)
					{
						if (item == matterail->getToolId() && !isDeled)
						{
							m_iCostCoin -= ModelTactics::getInstance()->getRowUpMaterialPrice(item);
							isDeled = true;
							return true;
						}
						return false;
					}	
					), m_vSeledMat.end());
				}	
				m_ui.btnPropAdd.at(nWidgetName-Btn_PropCancel1)->removeAllChildrenWithCleanup(true);
				m_ui.btnPropAdd.at(nWidgetName-Btn_PropCancel1)->setEnabled(true);
				m_ui.btnPropCancel.at(nWidgetName-Btn_PropCancel1)->setVisible(false);
			}
			//强化银币
			m_ui.txtRowLvUpCoin->setString(_TO_STR(m_iCostCoin));
			showRowNextLevelAttributes();
			break;

		case Btn_AutoAdd:
			{
				//遍历背包材料数据
				m_iCostCoin = 0;
				m_vSeledMat.clear();

				for(int i=0; i<m_ui.btnPropAdd.size(); i++)
				{
					m_ui.btnPropAdd.at(i)->removeAllChildrenWithCleanup(true);
				}

				int m = 0;
				for(int i=0; i<m_vRowUpgradeMatId.size(); i++)
				{					
					const TOOL_INFO* info = ToolModel::getInstance()->GetToolInfoBytemplateId(m_vRowUpgradeMatId.at(i));
					if(info)
					{		
						int loops = info->nstack > 4 ? 4 : info->nstack;
						for(int j=0; j<loops; j++)
						{
							UIToolHeader* rowUpMaterial = UIToolHeader::create(m_vRowUpgradeMatId.at(i));
							rowUpMaterial->setToolId(m_vRowUpgradeMatId.at(i));
							rowUpMaterial->setTag(MATTERIAL_ADDED_TAG);
							if(m < m_ui.btnPropAdd.size())
							{
								m_ui.btnPropAdd.at(m)->addChild(rowUpMaterial);
								//按钮上的材料道具淡入
								rowUpMaterial->setOpacity(0);
								rowUpMaterial->runAction(FadeIn::create(0.5f));
								m_ui.btnPropAdd.at(m)->setEnabled(false);		//添加材料后不能点击，需先点击去掉材料按钮
								m_ui.btnPropCancel.at(m)->setVisible(true);
								rowUpMaterial->setPosition(Point(6, 6));
								rowUpMaterial->setAnchorPoint(Point(0, 0));
								int matId = m_vRowUpgradeMatId.at(i);
								m_vSeledMat.push_back(matId);
								m_iCostCoin += ModelTactics::getInstance()->getRowUpMaterialPrice(m_vRowUpgradeMatId.at(i));
								m++;
							}						
						}
					}					
				}			
				if(m_vSeledMat.empty())
				{
					//提示没有材料
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20128));
				}
			}
			//强化银币
			m_ui.txtRowLvUpCoin->setString(_TO_STR(m_iCostCoin));
			showRowNextLevelAttributes();
			break;

		case Btn_Upgrade:
			{
				if(!m_bIsRowAniEnd)
				{
					break;			//排强化动画结束前，不处理强化操作
				}
				bool isAddedProp = false;
				sCurRowData curRowData = ModelTactics::getInstance()->getCurRowData(m_eCurRowType);	
				for(int i=0; i<m_ui.btnPropAdd.size(); i++)
				{
					if(m_ui.btnPropAdd.at(i)->getChildrenCount() != 0)
					{
						isAddedProp = true;
						break;
					}
				}
				if(!isAddedProp)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20129));		
				}
				else if(curRowData.mRowLvUpExp == -1)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20130));
				}
				else if(MainModel::getInstance()->getMainParam()->mCoin < m_iCostCoin)
				{
					//银币不足
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
				else
				{
					int size = 0;

					//整理成服务器接收的形式
					vector<sRowUpMaterial> vecItemList;
					vecItemList.clear();
					for(int i=0; i<m_vRowUpgradeMatId.size(); i++)
					{		
						int nId = m_vRowUpgradeMatId.at(i);
						sRowUpMaterial data;
						data.id = nId;
						data.amount = 0;
						for(int j=0; j<m_vSeledMat.size(); j++)
						{
							if(m_vSeledMat.at(j) == nId)
								data.amount++;
						}
						if(data.amount != 0)
							vecItemList.push_back(data);
					}
					//发送前先记录已激活的阵法和排相关属性
					vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
					for(int i=0; i<vecTacticsList.size(); i++)
					{
						m_activiedTacs[i] = vecTacticsList.at(i).mIsActivated;	
					}

					m_sCurRowAtr.curRowLevel = curRowData.mRowLevel;
					if(m_ui.sRowData.size() == 4)
					{
						m_sCurRowAtr.curRowHp = atoi(m_ui.sRowData.at(0).txtRowAttributesValue->getString().c_str());
						m_sCurRowAtr.curRowAtk = atoi(m_ui.sRowData.at(1).txtRowAttributesValue->getString().c_str());
						m_sCurRowAtr.curRowPdd = atoi(m_ui.sRowData.at(2).txtRowAttributesValue->getString().c_str());
						m_sCurRowAtr.curRowMdd = atoi(m_ui.sRowData.at(3).txtRowAttributesValue->getString().c_str());
					}
					
					mPlayAniRow = m_eCurRowType;
					TacticsController::sendUpgradeRowMsg(vecItemList, m_eCurRowType);	
				}
			}
			break;

		case Btn_Use:
			{
				if(ModelTactics::getInstance()->getTacticsDataList().at(m_iCurTacticsKind).mIsActivated)
				{
					if(ModelTactics::getInstance()->getTacticsDataList().at(m_iCurTacticsKind).mIsUsing)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20131));
					}
					else
					{
						TacticsController::sendUseTacticsMsg(ModelTactics::getInstance()->getTacticsDataList().at(m_iCurTacticsKind).mRowId);
					}					
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20132));
				}
			}
			break;

		case Btn_Refine:
			{
				sTacticsData tacData = ModelTactics::getInstance()->getTacticsDataList().at(m_iCurTacticsKind);

				if(!tacData.mIsActivated)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20132));
				}
				else if(tacData.mLevel >= TACTICS_MAX_LEVEL)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20133));
				}
				else if(!m_bIsTacticsMatEnough)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20134));
				}
				else if(MainModel::getInstance()->getMainParam()->mCoin < tacData.mLvUpCoin)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20135));
				}
				else
				{
					int levelLimit[3] = {0};
					for(int i=0; i<tacData.mLvUpLimit.size(); i++)
					{
						levelLimit[i] = tacData.mLvUpLimit.at(i);
					}
					if(ModelTactics::getInstance()->getCurRowData(FRONT_ROW_TYPE).mRowLevel < levelLimit[0] ||
						ModelTactics::getInstance()->getCurRowData(MIDDLE_ROW_TYPE).mRowLevel < levelLimit[1] ||
						ModelTactics::getInstance()->getCurRowData(BACK_ROW_TYPE).mRowLevel < levelLimit[2])
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20136));
					}
					else
					{
						//记录当前精炼的阵法
						m_iCurRefineTacticsKind = m_iCurTacticsKind;
						//阵法精炼
						TacticsController::sendUpgradeTacticsMsg(tacData.mRowId);
					}
				}
			}
			break;
		case Btn_Rob_Mat:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ROB))
				{
					Director::getInstance()->pushScene(SceneRob::create(true));
				}
			}		
			break;

		default:
			break;
		}
	}
}

void LayerTactics::onBtnSelTacticsClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");

		Button* btn = (Button*)ref;
		Layout* tactics = (Layout*)btn->getParent();

		for(int i=0; i<m_ui.sTacticsList.size(); i++)
		{
			m_ui.sTacticsList.at(i).btnTactics->setEnabled(true);
			m_ui.sTacticsList.at(i).btnTactics->setHighlighted(false);
		}
		m_ui.sTacticsList.at(nWidgetName).btnTactics->setEnabled(false);
		m_ui.sTacticsList.at(nWidgetName).btnTactics->setHighlighted(true);
		m_iCurTacticsKind = nWidgetName;
		switchRowAndRefineView(2);
	}
}

void LayerTactics::updateRowView()
{
	sCurRowData curRowData = ModelTactics::getInstance()->getCurRowData(m_eCurRowType);

	//主界面排相关文本
	m_ui.txtFrontLv->setString(_TO_STR(ModelTactics::getInstance()->getCurRowData(FRONT_ROW_TYPE).mRowLevel) + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtFrontLv, Color3B(0x80, 0x1a, 0x1a), 2);
	m_ui.txtMidLv->setString(_TO_STR(ModelTactics::getInstance()->getCurRowData(MIDDLE_ROW_TYPE).mRowLevel) + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtMidLv, Color3B(0x80, 0x1a, 0x1a), 2);
	m_ui.txtBackLv->setString(_TO_STR(ModelTactics::getInstance()->getCurRowData(BACK_ROW_TYPE).mRowLevel) + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtBackLv, Color3B(0x80, 0x1a, 0x1a), 2);

	//排强化界面
	m_ui.imgRow->loadTexture("Image/UiZhenFa/icon/img_pai" + _TO_STR(curRowData.mRowType) + ".png");
	m_ui.txtRowLv->setString(_TO_STR(curRowData.mRowLevel) + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtRowLv, Color3B(0x80, 0x1a, 0x1a), 2);
	if(curRowData.mRowLevel < ROW_MAX_LEVEL)
	{
		m_ui.txtRowLvUpExp->setString(_TO_STR(curRowData.mRowCurExp) + string("/") + _TO_STR(curRowData.mRowLvUpExp));
		m_ui.barExpProgress->setPercent(((float)curRowData.mRowCurExp/(float)curRowData.mRowLvUpExp)*100);
	}
	else
	{
		m_ui.txtRowLvUpExp->setString(DataManager::getInstance()->searchCommonTexdtById(20137));
		m_ui.barExpProgress->setPercent(100.0f);
	}

	setTextAddStroke(m_ui.txtRowLvUpExp, Color3B(0x4e, 0x2a, 0x20), 2);

	for(int i=0; i<m_ui.btnPropAdd.size(); i++)
	{
		m_ui.btnPropAdd.at(i)->removeAllChildrenWithCleanup(true);
		m_ui.btnPropAdd.at(i)->setEnabled(true);
		m_ui.btnPropCancel.at(i)->setVisible(false);
	}
	m_iCostCoin = 0;

	if(curRowData.mRowID == 100 || curRowData.mRowID == 200 || curRowData.mRowID == 300)
	{
		for(int i=0; i<m_ui.sRowData.size(); i++)
		{	
			m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(0));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString("");
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setVisible(false);
			m_ui.sRowData.at(i).imgRowUpgradeFlag->setVisible(false);
		}
		unLoadPreviewProgressBar();
	}
	else
	{
		showRowNextLevelAttributes();
	}

	//强化银币
	m_ui.txtRowLvUpCoin->setString(_TO_STR(m_iCostCoin));
}

void LayerTactics::showRowNextLevelAttributes()
{
	sCurRowData curRowData = ModelTactics::getInstance()->getCurRowData(m_eCurRowType);
	
	int curTotalExp = curRowData.mRowCurExp;
	if(curRowData.mRowLevel >= ROW_MAX_LEVEL)
	{
		//达到最高级
		for(int i=0; i<m_ui.sRowData.size(); i++)
		{
			m_ui.sRowData.at(i).imgRowUpgradeFlag->setVisible(false);
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString(DataManager::getInstance()->searchCommonTexdtById(20137));
		}
		
		return;
	}
	for(int i=0; i<m_vSeledMat.size(); i++)
	{
		RowData* propData = DataManager::getInstance()->searchToolById(m_vSeledMat.at(i));
		if(propData)
		{
			curTotalExp += propData->getIntData("value");
		}
	}
	//这个变量  属性基数， 是用来计算跨等级的情况来显示强化后的属性预览
	int baseAttributes = 0;
	if(curTotalExp < curRowData.mRowLvUpExp)
	{
		//只有当当前添加的材料和当前拥有的经验总和满足升级到下一等级时才显示
		for(int i=0; i<m_ui.sRowData.size(); i++)
		{
			if(i == 0)
				m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeHp));
			else if(i == 1)
				m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeAtk));
			else if(i == 2)
				m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangePhyDef));
			else
				m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeMagicDef));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString("");
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setVisible(false);
			m_ui.sRowData.at(i).imgRowUpgradeFlag->setVisible(false);
			unLoadPreviewNextValue();
		}
		
		if(curTotalExp > curRowData.mRowCurExp)
		{
			setPreviewProgressBar(((float)curTotalExp/(float)curRowData.mRowLvUpExp)*100);	
		}
		else
		{
			unLoadPreviewProgressBar();
		}
	
		return;
	}
	else
	{
		//遍历，计算当前经验+材料经验能达到的排等级
		int rowId = curRowData.mRowID;
		for(int i=curRowData.mRowLevel; i<ROW_MAX_LEVEL; i++)
		{
			RowData* row = DataManager::getInstance()->searchTacticsById(rowId);
			if(curTotalExp >= row->getIntData("exp"))
			{
				baseAttributes++;
				rowId++;
				curTotalExp -= row->getIntData("exp");
			}
			else
			{
				break;
			}
		}
		
		setPreviewProgressBar(100.0);
	}
	
	//先获取下一级的血量，攻击，物防，法防的数值
	int attNextLvValue[4] = {0};
	if(curRowData.mRowLevel < ROW_MAX_LEVEL)
	{		
		RowData* row = DataManager::getInstance()->searchTacticsById(curRowData.mRowID+1);
		attNextLvValue[0] = row->getIntData("hp");
		attNextLvValue[1] = row->getIntData("atk");
		attNextLvValue[2] = row->getIntData("pdd");
		attNextLvValue[3] = row->getIntData("mdd");
	}
	for(int i=0; i<m_ui.sRowData.size(); i++)
	{
		if(i == 0)
		{
			m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeHp));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString(string("+") + _TO_STR((attNextLvValue[i] - curRowData.mRowRangeHp)*baseAttributes));
		}
		else if(i == 1)
		{
			m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeAtk));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString(string("+") + _TO_STR((attNextLvValue[i] - curRowData.mRowRangeAtk)*baseAttributes));
		}
		else if(i == 2)
		{
			m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangePhyDef));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString(string("+") + _TO_STR((attNextLvValue[i] - curRowData.mRowRangePhyDef)*baseAttributes));
		}
		else
		{
			m_ui.sRowData.at(i).txtRowAttributesValue->setString(_TO_STR(curRowData.mRowRangeMagicDef));
			m_ui.sRowData.at(i).txtRowAttributesValueUp->setString(string("+") + _TO_STR((attNextLvValue[i] - curRowData.mRowRangeMagicDef)*baseAttributes));
		}
		m_ui.sRowData.at(i).txtRowAttributesValueUp->setVisible(true);
		m_ui.sRowData.at(i).imgRowUpgradeFlag->setVisible(true);
		setPreviewNextValue();
	}	
}

void LayerTactics::setPreviewProgressBar(float percentValue)
{
	m_ui.barExpProgressPreview->setPercent(percentValue);
	FiniteTimeAction* fadeIn = FadeIn::create(0.5);
	FiniteTimeAction* fadeOut = FadeOut::create(0.5);
	m_ui.barExpProgressPreview->runAction(RepeatForever::create(Sequence::create(fadeIn, fadeOut, nullptr)));
}

void LayerTactics::unLoadPreviewProgressBar()
{
	m_ui.barExpProgressPreview->setPercent(0.0f);
	m_ui.barExpProgressPreview->stopAllActions();
}

void LayerTactics::setPreviewNextValue()
{	
	for(int i=0; i<m_ui.sRowData.size(); i++)
	{
		m_ui.sRowData.at(i).txtRowAttributesValueUp->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.5), FadeOut::create(0.5), nullptr)));
		m_ui.sRowData.at(i).imgRowUpgradeFlag->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.5), FadeOut::create(0.5), nullptr)));
	}
}

void LayerTactics::unLoadPreviewNextValue()
{
	for(int i=0; i<m_ui.sRowData.size(); i++)
	{
		m_ui.sRowData.at(i).txtRowAttributesValueUp->stopAllActions();
		m_ui.sRowData.at(i).imgRowUpgradeFlag->stopAllActions();
	}
}

void LayerTactics::switchRowAndRefineView(int type)
{
	//清空选中的材料
	m_vSeledMat.clear();

	//1. 排按钮   2.阵法按钮
	if(type == 1)
	{
		m_ui.imgRowView->setVisible(true);
		m_ui.imgRefineView->setVisible(false);
		
		vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
		for(int i=0; i<m_ui.sTacticsList.size(); i++)
		{
			m_ui.sTacticsList.at(i).btnTactics->setEnabled(true);
			m_ui.sTacticsList.at(i).btnTactics->setHighlighted(false);
		}

		m_bIsCurRefineView = false;
		updateRowView();
	}
	else if(type == 2)
	{
		m_ui.btnFrontRow->setEnabled(true);
		m_ui.btnFrontRow->setHighlighted(false);
		m_ui.btnMidRow->setEnabled(true);
		m_ui.btnMidRow->setHighlighted(false);
		m_ui.btnBackRow->setEnabled(true);
		m_ui.btnBackRow->setHighlighted(false);

		m_ui.imgRefineView->setVisible(true);
		m_ui.imgRowView->setVisible(false);
		//刷新精炼界面
		m_bIsCurRefineView = true;
		updateDataRefineView();
	}

	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.5f;

	// 切入特效;
	Layout* upPnl = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Up"));
	upPnl->setPositionX(upPnl->getPositionX() + shiftPosX);
	upPnl->runAction(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(-shiftPosX, 0.0f))));
}

void LayerTactics::updateTacticsDataList()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
	//阵法属性(目前UI设计是只有一条属性)
	if(ModelTactics::getInstance()->isUsedTactics())
	{
		for(int i=0; i<vecTacticsList.size(); i++)
		{
			if(vecTacticsList.at(i).mRowId == ModelTactics::getInstance()->getCurUsingTacticsData().mRowId)
			{
				m_iCurTacticsKind = i;
			}
		}
		//正使用的阵法置标志
		for(int i=0; i<m_ui.sTacticsList.size(); i++)
		{
			m_ui.sTacticsList.at(i).imgFlag->setVisible(false);
			m_ui.sTacticsList.at(i).btnTactics->setHighlighted(false);
			m_ui.sTacticsList.at(i).btnTactics->setEnabled(true);
		}
		//阵法属性
		sTacticsData curUsed = ModelTactics::getInstance()->getCurUsingTacticsData();
		if(curUsed.mRowId == vecTacticsList.at(m_iCurTacticsKind).mRowId)
		{
			m_ui.txtTacticsAttributes->setString(_F_TO_STR(curUsed.mAttrValue*100) + string("% ") + curUsed.mAttriName.ch_name);
			setTextAddStroke(m_ui.txtTacticsAttributes, Color3B(0x76, 0x11, 0x00), 3);	
		}	
	}
	else
	{
		m_ui.txtTacticsAttributes->setString(DataManager::getInstance()->searchCommonTexdtById(20138));
		setTextAddStroke(m_ui.txtTacticsAttributes, Color3B(0x76, 0x11, 0x00), 3);
	}
	for(int i=0; i<vecTacticsList.size(); i++)
	{
		UIToolHeader* tacticsItem = UIToolHeader::create(vecTacticsList.at(i).mIconId);
		m_ui.sTacticsList.at(i).btnTactics->removeAllChildren();
		tacticsItem->setAnchorPoint(Point(0.5, 0.5));
		tacticsItem->setScale(0.8);
		tacticsItem->setPosition(Point(3, 14));
		m_ui.sTacticsList.at(i).btnTactics->getParent()->addChild(tacticsItem, -1, TACTICS_ICON_TAG);

		if(vecTacticsList.at(i).mIsActivated)
		{
			m_ui.sTacticsList.at(i).txtLevel->setString(StringFormat::intNumChangeToCh(vecTacticsList.at(i).mLevel) +DataManager::getInstance()->searchCommonTexdtById(20139));
			m_ui.sTacticsList.at(i).txtLevel->setColor(Color3B(0xff, 0xfc, 0x22));
			setTextAddStroke(m_ui.sTacticsList.at(i).txtLevel, Color3B(0x79, 0x03, 0x03), 2);
			tacticsItem->setOpacity(255);
		}
		else
		{
			//越南的跟中文不一样,为了统一版本,跟着越南走
			//越南的 显示的意思是   级 + 1 + 开放  什么意思嘛!!!!中文	级激活
			//20329越南是 级  中文是 空串  20107  越南是开放  中文是 级激活
			m_ui.sTacticsList.at(i).txtLevel->setString(DataManager::getInstance()->searchCommonTexdtById(20329) + " " +
			_TO_STR(vecTacticsList.at(i).mLvUpLimit.at(0))  + " " + DataManager::getInstance()->searchCommonTexdtById(20107));
			m_ui.sTacticsList.at(i).txtLevel->setColor(Color3B(0x92, 0x7a, 0x70));
			setTextAddStroke(m_ui.sTacticsList.at(i).txtLevel, Color3B(0x21, 0x02, 0x02), 2);
			tacticsItem->setOpacity(200);
		}

		m_ui.sTacticsList.at(i).imgFlag->setVisible(vecTacticsList.at(i).mIsUsing ? true : false);
	}

	m_eCurRowType = FRONT_ROW_TYPE;
	updateRowView();
}

void LayerTactics::updateCurTacticsData()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();

	for(int i=0; i<m_ui.sTacticsList.size(); i++)
	{
		m_ui.sTacticsList.at(i).imgFlag->setVisible(false);
		m_ui.sTacticsList.at(i).btnTactics->setHighlighted(false);
		m_ui.sTacticsList.at(i).btnTactics->setEnabled(true);
		if(i == m_iCurTacticsKind)
		{
			m_ui.sTacticsList.at(i).imgFlag->setVisible(true);
			m_ui.sTacticsList.at(i).btnTactics->setHighlighted(true);
			m_ui.sTacticsList.at(i).btnTactics->setEnabled(false);
		}
	}

	//阵法属性
	sTacticsData curUsed = ModelTactics::getInstance()->getCurUsingTacticsData();
	if(curUsed.mRowId == vecTacticsList.at(m_iCurTacticsKind).mRowId)
	{
		m_ui.txtTacticsAttributes->setString(_F_TO_STR(curUsed.mAttrValue*100) + string("% ") + curUsed.mAttriName.ch_name);
		setTextAddStroke(m_ui.txtTacticsAttributes, Color3B(0x76, 0x11, 0x00), 3);	
	}

	updateDataRefineView();
}

void LayerTactics::updateTacticsQuality()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
	if(m_iCurTacticsKind < 0 || m_iCurTacticsKind >= vecTacticsList.size())
	{
		return;
	}
	m_ui.sTacticsList.at(m_iCurTacticsKind).txtLevel->setString(StringFormat::intNumChangeToCh(vecTacticsList.at(m_iCurTacticsKind).mLevel) + 
		DataManager::getInstance()->searchCommonTexdtById(20139));
	setTextAddStroke(m_ui.sTacticsList.at(m_iCurTacticsKind).txtLevel, Color3B(0x79, 0x03, 0x03), 2);
}

void LayerTactics::updateDataRefineView()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
	if(m_iCurTacticsKind < 0 || m_iCurTacticsKind >= vecTacticsList.size())
	{
		return;
	}

	//阵法属性
	sTacticsData curUsed = ModelTactics::getInstance()->getCurUsingTacticsData();
	if(curUsed.mRowId == vecTacticsList.at(m_iCurTacticsKind).mRowId)
	{
		m_ui.txtTacticsAttributes->setString(_F_TO_STR(curUsed.mAttrValue*100) + string("% ") + curUsed.mAttriName.ch_name);
		setTextAddStroke(m_ui.txtTacticsAttributes, Color3B(0x76, 0x11, 0x00), 3);	
	}
		
	UIToolHeader* toolIcon = UIToolHeader::create(vecTacticsList.at(m_iCurTacticsKind).mIconId);
	m_ui.imgTacticsIcon->removeAllChildren();
	m_ui.imgTacticsIcon->addChild(toolIcon);
	toolIcon->setAnchorPoint(Point(0, 0));

	m_ui.txtTacticsName->setString(vecTacticsList.at(m_iCurTacticsKind).mName);
	setTextAddStroke(m_ui.txtTacticsName, Color3B(0x79, 0x03, 0x03), 2);
	if(vecTacticsList.at(m_iCurTacticsKind).mIsActivated)
	{
		m_ui.txtUnactivated->setVisible(false);
		m_ui.txtQuality->setVisible(true);
		m_ui.txtQuality->setString(DataManager::getInstance()->searchCommonTexdtById(20140) + m_ui.sTacticsList.at(m_iCurTacticsKind).txtLevel->getString());
		setTextAddStroke(m_ui.txtQuality, Color3B(0x79, 0x03, 0x03), 2);
	}
	else
	{
		m_ui.txtQuality->setVisible(false);
		m_ui.txtUnactivated->setVisible(true);
		int lv = vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(0);
		
		string str = DataManager::getInstance()->searchCommonTexdtById(10099) +"("+
		DataManager::getInstance()->searchCommonTexdtById(20142) + TO_STR(lv)
		+ DataManager::getInstance()->searchCommonTexdtById(20101) + ")";
		m_ui.txtUnactivated->setString(str);
		setTextAddStroke(m_ui.txtUnactivated, Color3B(0x21, 0x02, 0x02), 2);
	}
	m_ui.txtAttribute1->setString(_F_TO_STR((vecTacticsList.at(m_iCurTacticsKind).mAttrValue)*100) + 
		string("% ") + vecTacticsList.at(m_iCurTacticsKind).mAttriName.ch_name);
	setTextAddStroke(m_ui.txtAttribute1, Color3B(0x1d, 0x2f, 0x07), 2);
	//每级增加属性
	m_ui.txtAttribute2->setString(_F_TO_STR((vecTacticsList.at(m_iCurTacticsKind).mNextAttrVal)*100) + string("% ") + 
		vecTacticsList.at(m_iCurTacticsKind).mAttriName.ch_name);
	setTextAddStroke(m_ui.txtAttribute2, Color3B(0x1d, 0x2f, 0x07), 2);

	if(vecTacticsList.at(m_iCurTacticsKind).mLevel < TACTICS_MAX_LEVEL)
	{
		m_ui.txtNextLvCoin->setString(_TO_STR(vecTacticsList.at(m_iCurTacticsKind).mLvUpCoin));
		m_ui.txtNextLvCoin->getParent()->getChildByName("Img_Coin")->setVisible(true);
	}
	else
	{
		m_ui.txtNextLvCoin->setString("");
		m_ui.txtNextLvCoin->getParent()->getChildByName("Img_Coin")->setVisible(false);
	}

	//创建省级材料图标
	int totalNum = 0;
	UIToolHeader* speMaterial = UIToolHeader::create(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId1);
	speMaterial->setToolId(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId1);
	speMaterial->setTipsEnabled(true);
	speMaterial->setName("SPE_MATERIAL");
	const TOOL_INFO* info1 = ToolModel::getInstance()->GetToolInfoBytemplateId(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId1);
	if(info1 != nullptr)
	{
		speMaterial->setNumEx(info1->nstack);
		totalNum += info1->nstack;
	}
	else
	{
		speMaterial->setNumEx(0);
	}
	speMaterial->setScale(0.8);
	m_ui.pnlMaterial1->removeAllChildren();
	m_ui.pnlMaterial1->addChild(speMaterial);
	speMaterial->setAnchorPoint(Point(0, 0));

	UIToolHeader* comMaterial = UIToolHeader::create(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId2);
	comMaterial->setToolId(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId2);
	comMaterial->setTipsEnabled(true);
	comMaterial->setName("COM_MATERIAL");
	const TOOL_INFO* info2 = ToolModel::getInstance()->GetToolInfoBytemplateId(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialId2);
	if(info2 != nullptr)
	{
		comMaterial->setNumEx(info2->nstack);
		totalNum += info2->nstack;
	}
	else
	{
		comMaterial->setNumEx(0);
	}
	comMaterial->setScale(0.8);
	m_ui.pnlMaterial2->removeAllChildren();
	m_ui.pnlMaterial2->addChild(comMaterial);
	comMaterial->setAnchorPoint(Point(0, 0));

	//升级材料和数量
	if(!vecTacticsList.at(m_iCurTacticsKind).mIsActivated)
	{
		m_ui.txtNeedMaterailTips->setString(DataManager::getInstance()->searchCommonTexdtById(20143));
		sCurRowData curRowData1 = ModelTactics::getInstance()->getCurRowData(FRONT_ROW_TYPE);
		sCurRowData curRowData2 = ModelTactics::getInstance()->getCurRowData(MIDDLE_ROW_TYPE);
		sCurRowData curRowData3 = ModelTactics::getInstance()->getCurRowData(BACK_ROW_TYPE);
		int min = curRowData1.mRowLevel <= curRowData2.mRowLevel ? curRowData1.mRowLevel : curRowData2.mRowLevel;
		min = min <= curRowData3.mRowLevel ? min : curRowData3.mRowLevel;
		m_ui.txtMaterialCount->setString(_TO_STR(min) + string("/") + _TO_STR(vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(0)));
		m_ui.txtMaterialCount->setColor(Color3B(0xff, 0x09, 0x09));
		setTextAddStroke(m_ui.txtMaterialCount, Color3B(0x1d, 0x2f, 0x07), 2);
	}
	else
	{
		bool isCanUpgrade = false;
		for(int j=0; j<vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.size(); j++)
		{
			if(ModelTactics::getInstance()->getCurRowData(FRONT_ROW_TYPE).mRowLevel >= vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(j) &&
			   ModelTactics::getInstance()->getCurRowData(MIDDLE_ROW_TYPE).mRowLevel >= vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(j) &&
			   ModelTactics::getInstance()->getCurRowData(BACK_ROW_TYPE).mRowLevel >= vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(j))
			{
				isCanUpgrade = true;
			}
		}

		if(!isCanUpgrade)
		{
			m_ui.txtNeedMaterailTips->setString(DataManager::getInstance()->searchCommonTexdtById(20143));
			m_ui.txtMaterialCount->setString(_TO_STR(vecTacticsList.at(m_iCurTacticsKind).mLvUpLimit.at(0)));
			m_ui.txtMaterialCount->setColor(Color3B(0xff, 0x09, 0x09));
			setTextAddStroke(m_ui.txtMaterialCount, Color3B(0x1b, 0x01, 0x01), 2);
			m_ui.txtMaterialCount->setVisible(true);
		}
		else
		{		
			if(vecTacticsList.at(m_iCurTacticsKind).mLevel <= TACTICS_MAX_LEVEL)
			{
				m_ui.txtNeedMaterailTips->setString(DataManager::getInstance()->searchCommonTexdtById(20144));
				m_ui.txtMaterialCount->setVisible(true);
				m_ui.txtMaterialCount->setString(_TO_STR(totalNum) + string("/") + _TO_STR(vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialCnt1));
				if(totalNum < vecTacticsList.at(m_iCurTacticsKind).mLvUpMaterialCnt1)
				{
					m_ui.txtMaterialCount->setColor(Color3B(0xff, 0x09, 0x09));
					setTextAddStroke(m_ui.txtMaterialCount, Color3B(0x1b, 0x01, 0x01), 2);
					m_bIsTacticsMatEnough = false;
				}
				else
				{
					m_ui.txtMaterialCount->setColor(Color3B(0x54, 0xdd, 0x51));
					setTextAddStroke(m_ui.txtMaterialCount, Color3B(0x1d, 0x2f, 0x07), 2);
					m_bIsTacticsMatEnough = true;
				}
				m_ui.txtMaterialCount->setVisible(true);
			}
			if(vecTacticsList.at(m_iCurTacticsKind).mLevel >= TACTICS_MAX_LEVEL)
			{
				m_ui.txtNeedMaterailTips->setString(DataManager::getInstance()->searchCommonTexdtById(20145));
				m_ui.txtMaterialCount->setVisible(false);
			}
		}
	}
}

void LayerTactics::initRowUpMaterialData()
{
	ModelTactics* pInstance = ModelTactics::getInstance();
	m_vRowUpgradeMatId.clear();
	for(int i=0; i<pInstance->m_vRowMaterialCostCoin.size(); i++)
	{
		int nId = pInstance->m_vRowMaterialCostCoin.at(i).id;
		m_vRowUpgradeMatId.push_back(nId);
	}
}

void LayerTactics::updateDataFightPower()
{
	m_ui.txtFightPower->setString(_TO_STR(FormationModel::getInstance()->getFcTeam()));	
	setTextAddStroke(m_ui.txtFightPower, Color3B(0x7d, 0x3f, 0x1c), 3);
}

void LayerTactics::updateNewActivedTactics()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();

	for(int i=0; i<vecTacticsList.size(); i++)
	{
		if(vecTacticsList.at(i).mIsActivated)
		{
			if(m_activiedTacs[i])                       //已经激活的直接设置激活状态
			{
				m_ui.sTacticsList.at(i).txtLevel->setString(StringFormat::intNumChangeToCh(vecTacticsList.at(i).mLevel) + 
					DataManager::getInstance()->searchCommonTexdtById(20139));
				m_ui.sTacticsList.at(i).txtLevel->setColor(Color3B(0xff, 0xfc, 0x22));
				setTextAddStroke(m_ui.sTacticsList.at(i).txtLevel, Color3B(0x79, 0x03, 0x03), 2);
				m_ui.sTacticsList.at(i).btnTactics->getParent()->getChildByTag(TACTICS_ICON_TAG)->setOpacity(255);
			}
			else                                       //新激活的需要播放动画再设置状态
			{
				m_iIdx = i;
				startNewTacActiviedAni();			
			}
		}
		else
		{
			//越南的跟中文不一样,为了统一版本,跟着越南走
			//越南的 显示的意思是   级 + 1 + 开放  什么意思嘛!!!!中文	级激活
			//20329越南是 级  中文是 空串  20107  越南是开放  中文是 级激活
			m_ui.sTacticsList.at(i).txtLevel->setString(DataManager::getInstance()->searchCommonTexdtById(20329) + " " +
				_TO_STR(vecTacticsList.at(i).mLvUpLimit.at(0))  + " " + DataManager::getInstance()->searchCommonTexdtById(20107));
			m_ui.sTacticsList.at(i).txtLevel->setColor(Color3B(0x92, 0x7a, 0x70));
			setTextAddStroke(m_ui.sTacticsList.at(i).txtLevel, Color3B(0x21, 0x02, 0x02), 2);
			m_ui.sTacticsList.at(i).btnTactics->getParent()->getChildByTag(TACTICS_ICON_TAG)->setOpacity(200);
			
			//else
			//{
			//	//中文片显示  级激活
			//	m_ui.sTacticsList.at(i).txtLevel->setString(_TO_STR(vecTacticsList.at(i).mLvUpLimit.at(0)) +
			//		DataManager::getInstance()->searchCommonTexdtById(20107));
			//	m_ui.sTacticsList.at(i).txtLevel->setColor(Color3B(0x92, 0x7a, 0x70));
			//	setTextAddStroke(m_ui.sTacticsList.at(i).txtLevel, Color3B(0x21, 0x02, 0x02), 2);
			//	m_ui.sTacticsList.at(i).btnTactics->getParent()->getChildByTag(TACTICS_ICON_TAG)->setOpacity(200);
			//}
		}
	}

}

void LayerTactics::startNewTacActiviedAni()
{
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
		newTacActiviedIconAni();		
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anizhenfajihuo/Anizhenfajihuo.ExportJson");
	Armature* ani = Armature::create("Anizhenfajihuo");
	ani->getAnimation()->play("JiHuoWenZi");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ImageView* imgTitle = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_nameListTitle"));
	ani->setPosition(imgTitle->getContentSize() / 2);
	imgTitle->addChild(ani);
}

void LayerTactics::newTacActiviedIconAni()
{
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
		setTacticsActiviedState();	
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anizhenfajihuo_Za/Anizhenfajihuo_Za.ExportJson");
	Armature* ani = Armature::create("Anizhenfajihuo_Za");
	ani->getAnimation()->play("Ani_1");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ani->setPosition(m_ui.sTacticsList.at(m_iIdx).btnTactics->getParent()->getContentSize() / 2);
	ani->setAnchorPoint(Point(1, 1));
	m_ui.sTacticsList.at(m_iIdx).btnTactics->getParent()->addChild(ani);
}

void LayerTactics::setTacticsActiviedState()
{
	vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
	m_ui.sTacticsList.at(m_iIdx).txtLevel->setString(StringFormat::intNumChangeToCh(vecTacticsList.at(m_iIdx).mLevel) +
	DataManager::getInstance()->searchCommonTexdtById(20139));
	m_ui.sTacticsList.at(m_iIdx).txtLevel->setColor(Color3B(0xff, 0xfc, 0x22));
	setTextAddStroke(m_ui.sTacticsList.at(m_iIdx).txtLevel, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.sTacticsList.at(m_iIdx).btnTactics->getParent()->getChildByTag(TACTICS_ICON_TAG)->setOpacity(255);
}

void LayerTactics::playUpgradeTacAni()
{
	///////// 浮动特效;
	Layout* _parent = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Up"));

	// 起始坐标;
	float fStart_x = 220.0f;
	float fStart_y = 400.0f;

	// 帧间距(秒/帧);
	float fSpace = 1.0f/25.0f;

	// 属性值变化;
	static int count = 0;
	auto callback = [=](float delta)
	{
		if(m_iCurRefineTacticsKind == ATTR_KIND_TIAN_GANG)
		{
			if (++count > 2)
			{
				count = 0;
				Director::getInstance()->getScheduler()->unschedule("attrValue", this);
				return;
			}
		}
		else 
		{
			if (++count > 1)
			{
				count = 0;
				Director::getInstance()->getScheduler()->unschedule("attrValue", this);
				return;
			}
		}

		// 单条属性值;
		Layout* _pnlAttr = Layout::create();
		_pnlAttr->setContentSize(Size(200.0f, 50.0f));
		if(m_iCurRefineTacticsKind == ATTR_KIND_TIAN_GANG)
		{
			_pnlAttr->setPosition(Vec2(fStart_x + 15.0f, fStart_y + (count-1)*40));
		}
		else
		{
			_pnlAttr->setPosition(Vec2(fStart_x + 15.0f, fStart_y));
		}	
		_pnlAttr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_parent->addChild(_pnlAttr);

		vector<sTacticsData> vecTacticsList = ModelTactics::getInstance()->getTacticsDataList();
		sTacticsData curSelTac;
		if(m_iCurRefineTacticsKind < vecTacticsList.size())
			curSelTac = vecTacticsList.at(m_iCurRefineTacticsKind);

		//如果是防御阵法，额外加一个法防属性
		if(m_iCurRefineTacticsKind == ATTR_KIND_TIAN_GANG)
		{
			if(count == 1)
			{
				ImageView* imgWuFang = ImageView::create(curSelTac.mAttriName.icon_path);
				imgWuFang->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				imgWuFang->setPosition(Vec2(0.0f, -5.0f));
				_pnlAttr->addChild(imgWuFang);

				Text* txtAttrWu = Text::create(DataManager::getInstance()->searchCommonTexdtById(10010) + (" +  ") 
					+ _F_TO_STR((curSelTac.mAttrValue)*100) + string("%"), FONT_FZZHENGHEI, 24);
				txtAttrWu->setColor(Color3B(0x8E, 0xEF, 0x3F));
				setTextAddStroke(txtAttrWu, Color3B::BLACK, 1);
				txtAttrWu->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				txtAttrWu->setPosition(Point(45.0f, 4.0f) + imgWuFang->getPosition());
				_pnlAttr->addChild(txtAttrWu);
			}			
			else if(count == 2)
			{
				ImageView* imgFaFang = ImageView::create("Image/Icon/global/img_mofang.png");
				imgFaFang->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				imgFaFang->setPosition(Vec2(0.0f, -imgFaFang->getContentSize().height - 6.0f));
				_pnlAttr->addChild(imgFaFang);

				Text* txtAttrFa = Text::create(DataManager::getInstance()->searchCommonTexdtById(10011) + (" +  ") 
					+ _F_TO_STR((curSelTac.mAttrValue)*100) + string("%"), FONT_FZZHENGHEI, 24);
				txtAttrFa->setColor(Color3B(0x8E, 0xEF, 0x3F));
				setTextAddStroke(txtAttrFa, Color3B::BLACK, 1);
				txtAttrFa->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				txtAttrFa->setPosition(Point(45.0f, 4.0f) + imgFaFang->getPosition());
				_pnlAttr->addChild(txtAttrFa);
			}		
		}
		else
		{
			ImageView* img = ImageView::create(curSelTac.mAttriName.icon_path);
			img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			img->setPosition(Vec2(0.0f, 0.0f));
			_pnlAttr->addChild(img);

			Text* txtAttr = Text::create(curSelTac.mAttriName.ch_name + (" +  ") 
				+ _F_TO_STR((curSelTac.mAttrValue)*100) + string("%"), FONT_FZZHENGHEI, 24);
			txtAttr->setColor(Color3B(0x8E, 0xEF, 0x3F));
			setTextAddStroke(txtAttr, Color3B::BLACK, 1);
			txtAttr->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtAttr->setPosition(Point(45.0f, 4.0f) + img->getPosition());
			_pnlAttr->addChild(txtAttr);
		}

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

	// 阵法精炼品阶+N;
	Layout* _pnlQh = Layout::create();
	_pnlQh->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_pnlQh->setContentSize(Size(255.0f, 50.0f));
	_pnlQh->setPosition(Vec2(fStart_x + 15.0f, fStart_y - 20.0f));
	_parent->addChild(_pnlQh);
	ImageView* imgQH = ImageView::create("Image/UiZhenFa/icon/img_zhenfajinglian.png");
	imgQH->setPosition(Vec2(_pnlQh->getContentSize().width/2 - 20.0f, _pnlQh->getContentSize().height/2));
	_pnlQh->addChild(imgQH);
	Text* txtQHN = Text::create(_TO_STR(1), FONT_FZZHENGHEI, 40);
	txtQHN->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	txtQHN->setColor(Color3B(0xF3, 0xDE, 0x98));
	setTextAddStroke(txtQHN, Color3B(0xE3, 0x1C, 0x03), 1);
	txtQHN->setPosition(Vec2(imgQH->getContentSize().width-24, imgQH->getContentSize().height/2));
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

void LayerTactics::playRowProgressBarAni()
{
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
		playUpgradeRowAni();
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anizhenxingqianghua/Anizhenxingqianghua.ExportJson");
	Armature* ani = Armature::create("Anizhenxingqianghua");
	ani->getAnimation()->play("Animation1");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ani->setPosition(m_ui.barExpProgress->getContentSize()/2);
	ani->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.barExpProgress->addChild(ani, 99);
	m_bIsRowAniEnd = false;									//开始播放动画
}

void LayerTactics::playUpgradeRowAni()
{
	//如果本次强化没有升级，则不需播放强化+N动画
	sCurRowData curRowData = ModelTactics::getInstance()->getCurRowData(mPlayAniRow);	
	if(curRowData.mRowLevel <= m_sCurRowAtr.curRowLevel)
	{
		sCurRowAtr att;
		m_sCurRowAtr = att;		//清空记录
		m_bIsRowAniEnd = true;	//排强化动画结束
		return;
	}

	///////// 浮动特效;
	Layout* _parent = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Up"));

	// 起始坐标;
	float fStart_x = 220.0f;
	float fStart_y = 400.0f;

	// 帧间距(秒/帧);
	float fSpace = 1.0f/25.0f;

	// 属性值变化;
	static int count = 0;
	auto callback = [=](float delta)
	{
		if (++count > 4)
		{
			count = 0;
			Director::getInstance()->getScheduler()->unschedule("attrValue", this);
			return;
		}

		// 单条属性值;
		Layout* _pnlAttr = Layout::create();
		_pnlAttr->setContentSize(Size(200.0f, 50.0f));
		_pnlAttr->setPosition(Vec2(fStart_x + 15.0f, fStart_y + 40 - count*10));
		_pnlAttr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_parent->addChild(_pnlAttr);

		if(count == 1)
		{
			//血量
			ImageView* imgHp = ImageView::create("Image/Icon/global/img_xueliang.png");
			imgHp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			imgHp->setPosition(Vec2(0.0f, -imgHp->getContentSize().height - 6.0f));
			_pnlAttr->addChild(imgHp);

			Text* txtAttrHp = Text::create(DataManager::getInstance()->searchCommonTexdtById(10008) + (" +  ") 
				+ _TO_STR(curRowData.mRowRangeHp - m_sCurRowAtr.curRowHp), FONT_FZZHENGHEI, 24);
			txtAttrHp->setColor(Color3B(0x8E, 0xEF, 0x3F));
			setTextAddStroke(txtAttrHp, Color3B::BLACK, 1);
			txtAttrHp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtAttrHp->setPosition(Point(45.0f, 4.0f) + imgHp->getPosition());
			_pnlAttr->addChild(txtAttrHp);
		}
		else if(count == 2)
		{
			//攻击
			ImageView* imgAtk = ImageView::create("Image/Icon/global/img_gongji.png");
			imgAtk->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			imgAtk->setPosition(Vec2(0.0f, -imgAtk->getContentSize().height - 6.0f));
			_pnlAttr->addChild(imgAtk);

			Text* txtAttrAtk = Text::create(DataManager::getInstance()->searchCommonTexdtById(10009) + " +  " 
				+ _TO_STR(curRowData.mRowRangeAtk - m_sCurRowAtr.curRowAtk), FONT_FZZHENGHEI, 24);
			txtAttrAtk->setColor(Color3B(0x8E, 0xEF, 0x3F));
			setTextAddStroke(txtAttrAtk, Color3B::BLACK, 1);
			txtAttrAtk->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtAttrAtk->setPosition(Point(45.0f, 4.0f) + imgAtk->getPosition());
			_pnlAttr->addChild(txtAttrAtk);
		}
		else if(count == 3)
		{
			//物防
			ImageView* imgPdd = ImageView::create("Image/Icon/global/img_wufang.png");
			imgPdd->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			imgPdd->setPosition(Vec2(0.0f, -imgPdd->getContentSize().height - 6.0f));
			_pnlAttr->addChild(imgPdd);

			Text* txtAttrPdd = Text::create(DataManager::getInstance()->searchCommonTexdtById(10010) + (" +  ") 
				+ _TO_STR(curRowData.mRowRangePhyDef - m_sCurRowAtr.curRowPdd), FONT_FZZHENGHEI, 24);
			txtAttrPdd->setColor(Color3B(0x8E, 0xEF, 0x3F));
			setTextAddStroke(txtAttrPdd, Color3B::BLACK, 1);
			txtAttrPdd->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtAttrPdd->setPosition(Point(45.0f, 4.0f) + imgPdd->getPosition());
			_pnlAttr->addChild(txtAttrPdd);
		}
		else if(count == 4)
		{
			//法防
			ImageView* imgMdd = ImageView::create("Image/Icon/global/img_mofang.png");
			imgMdd->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			imgMdd->setPosition(Vec2(0.0f, -imgMdd->getContentSize().height - 6.0f));
			_pnlAttr->addChild(imgMdd);

			Text* txtAttrMdd = Text::create(DataManager::getInstance()->searchCommonTexdtById(10011) + (" +  ") 
				+ _TO_STR(curRowData.mRowRangeMagicDef - m_sCurRowAtr.curRowMdd), FONT_FZZHENGHEI, 24);
			txtAttrMdd->setColor(Color3B(0x8E, 0xEF, 0x3F));
			setTextAddStroke(txtAttrMdd, Color3B::BLACK, 1);
			txtAttrMdd->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtAttrMdd->setPosition(Point(45.0f, 4.0f) + imgMdd->getPosition());
			_pnlAttr->addChild(txtAttrMdd);
		}

		// 属性值动画序列;
		Sequence* seq_attr = Sequence::create(
			EaseSineOut::create(Spawn::create(ScaleTo::create(5*fSpace, 1.5f), MoveBy::create(5*fSpace, Vec2(0.0f, 21.0f)), nullptr)),
			EaseSineIn::create(Spawn::create(ScaleTo::create(5*fSpace, 1.0f), MoveBy::create(fSpace, Vec2(0.0f, 5.0f)), nullptr)),
			MoveBy::create(20*fSpace, Vec2(0.0f, 102.0f)),
			Spawn::create(FadeOut::create(5*fSpace), MoveBy::create(5*fSpace, Vec2(0.0f, 22.0f)), nullptr),
			CallFuncN::create([=](Node* pSender){
				_pnlAttr->removeFromParent();
				m_bIsRowAniEnd = true;	//排强化动画结束
		}),
			nullptr);
		_pnlAttr->runAction(seq_attr);
	};

	// 强化等级+N;
	Layout* _pnlQh = Layout::create();
	_pnlQh->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_pnlQh->setContentSize(Size(255.0f, 50.0f));
	_pnlQh->setPosition(Vec2(fStart_x + 15.0f, fStart_y - 20.0f));
	_parent->addChild(_pnlQh);
	ImageView* imgQH = ImageView::create("Image/UIEquip/icon/img_qianghuadengji+.png");
	imgQH->setPosition(Vec2(_pnlQh->getContentSize().width/2 - 20.0f, _pnlQh->getContentSize().height/2));
	_pnlQh->addChild(imgQH);
	Text* txtQHN = Text::create(_TO_STR(curRowData.mRowLevel - m_sCurRowAtr.curRowLevel), FONT_FZZHENGHEI, 30);
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

void LayerTactics::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_TACTICS_MAIN_LIST:
		{
			updateTacticsDataList();
			updateDataFightPower();
		}
		break;

	case nMAIN_CMD_TACTICS_USE_TACTICS:
		{
			updateCurTacticsData();
			updateDataFightPower();
		}
		break;

	case nMAIN_CMD_TACTICS_ROW_UPGRADE:
		{
			m_vSeledMat.clear();

			//播放特效
			playRowProgressBarAni();

			updateRowView();
			updateDataFightPower();
			//排升级后刷新是否有新阵法被激活
			updateNewActivedTactics();
		}
		break;

	case nMAIN_CMD_TACTICS_UPGRADE:
		{
			//播放文字动画
			playUpgradeTacAni();

			//更新阵法列表的阵法品质
			updateTacticsQuality();
			updateDataRefineView();
			updateDataFightPower();
		}
		break;

	default:
		break;
	}
}

void LayerTactics::updateSelExpPropView()
{
	int m = 0;
	m_iCostCoin = 0;
	//先清空所有按钮上材料
	for(int i=0; i<4; i++)
	{
		m_ui.btnPropAdd.at(i)->removeAllChildrenWithCleanup(true);
		m_ui.btnPropAdd.at(i)->setEnabled(true);
		m_ui.btnPropCancel.at(i)->setVisible(false);
	}
	//排序
	std::sort(m_vSeledMat.begin(), m_vSeledMat.end(), 
		[](const int& p1, const int& p2)->bool
	{
		//按材料id从小到大排序
		return p1 < p2;
	});
	//镶嵌
	for(int i=0; i<m_vSeledMat.size(); i++)
	{				
		UIToolHeader* rowUpMaterial = UIToolHeader::create(m_vSeledMat.at(i));
		if(rowUpMaterial == nullptr)
		{
			continue;
		}
		rowUpMaterial->setTag(MATTERIAL_ADDED_TAG);
		rowUpMaterial->setToolId(m_vSeledMat.at(i));
		m_ui.btnPropAdd.at(i)->addChild(rowUpMaterial);
		m_ui.btnPropAdd.at(i)->setEnabled(false);		//添加材料后不能点击，需先点击去掉材料按钮
		m_ui.btnPropCancel.at(i)->setVisible(true);
		m_iCostCoin += ModelTactics::getInstance()->getRowUpMaterialPrice(m_vSeledMat.at(i));
		rowUpMaterial->setPosition(Point(6, 6));
		rowUpMaterial->setAnchorPoint(Point(0, 0));							
	}	

	//强化银币
	m_ui.txtRowLvUpCoin->setString(_TO_STR(m_iCostCoin));
	showRowNextLevelAttributes();
}





