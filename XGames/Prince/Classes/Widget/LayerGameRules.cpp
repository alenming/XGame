#include "LayerGameRules.h"
#include "Common/Common.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"

#define LABEL_HEIGHT	64.0f			//标签加上它与规则说明文本之间所占的高度
#define TEXT_HEIGHT		32.0f			//一行文本加上其所占行间距的高度
#define GAP_HEIGHT		40.0f			//上一段规则说明和下一段标签之间的间隔

LayerGameRules::LayerGameRules()
{
	m_pRoot = nullptr;
	m_scrollPanel = nullptr;
	m_pTitleTxt = nullptr;
	m_pText = nullptr;
	m_pScrollBar = nullptr;
	m_vecDes.clear();
}

LayerGameRules::~LayerGameRules()
{

}

LayerGameRules* LayerGameRules::create(int ruleTableId)
{
	LayerGameRules *pRet = new LayerGameRules();
	if (pRet != nullptr && pRet->initWithId(ruleTableId))
	{	
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}

bool LayerGameRules::initWithId(int ruleTableId)
{
	if(!ModalLayer::init())
	{
		return false;
	}

	//解析标签描述和玩法描述
	parseDesByRuleId(ruleTableId);

	initUI();

	return true;
}

void LayerGameRules::initUI()
{
	// 加载玩法说明UI;
	string strFileName;
	strFileName.assign("Image/UIHelp/UIHelp.ExportJson");

	// Root;
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_pRoot);

	//关闭按钮
	Button* closeBtn = (Button*)(Helper::seekWidgetByName(m_pRoot, "Btn_Close"));
	closeBtn->addTouchEventListener(CC_CALLBACK_2(LayerGameRules::onBtnClick, this));

	//滚动面板
	m_scrollPanel = (ui::ScrollView*)(Helper::seekWidgetByName(m_pRoot, "ScrollView_Help"));
	m_scrollPanel->addEventListener(CC_CALLBACK_2(LayerGameRules::onScrollViewEvent, this));

	//标题
	m_pTitleTxt = (Text*)(Helper::seekWidgetByName(m_pRoot, "Lab_Tga"));
	m_pTitleTxt->setVisible(false);

	//规则文本
	m_pText = (Text*)(Helper::seekWidgetByName(m_pRoot, "Lab_Rule"));
	m_pText->setVisible(false);

	//滚动条
	ImageView* imgScrollBg = (ImageView*)(Helper::seekWidgetByName(m_pRoot, "Img_di2"));
	m_pScrollBar = CustomScrollBar::create();
	m_pScrollBar->initScrollBar(m_scrollPanel->getContentSize().height);
	m_pScrollBar->setPosition(m_scrollPanel->getPosition() + Point(-6, 0));
	imgScrollBg->addChild(m_pScrollBar);
	m_pScrollBar->setScrollBarLengthPercent(1.0f);
	m_pScrollBar->setScrollBarPercentY(0.0f);

	//初始化滚动面板
	initScrollPanel();
}

void LayerGameRules::initScrollPanel()
{

	//计算文本高度
	m_scrollPanel->setInnerContainerSize(getContentTextSize());

	m_pScrollBar->setScrollBarLengthPercent(m_scrollPanel->getContentSize().height / m_scrollPanel->getInnerContainerSize().height);
	m_pScrollBar->setScrollBarPercentY(0.0f);

	//设置内容
	setContent();
}

//计算文本区域大小
Size LayerGameRules::getContentTextSize()
{
	Size inneSize;
	inneSize.width = m_scrollPanel->getInnerContainerSize().width;
	inneSize.height = 0.0f;

	for(int i=0; i<m_vecDes.size(); i++)
	{
		Text* playDesTxt = (Text*)m_pText->clone();

		//玩法文本
		int lines = 0;
		string playDesStr = string("");

		for(string str : m_vecDes.at(i).playDes)
		{
			Text* checkLines = Text::create(str, FONT_FZZHENGHEI, 24);
			lines += (int)checkLines->getContentSize().width / (int)m_pText->getContentSize().width;
			lines += (int)checkLines->getContentSize().width % (int)m_pText->getContentSize().width ? 1 : 0;
			if(playDesStr == "")
			{
				playDesStr = str;
			}
			else
			{
				playDesStr = playDesStr + string("\n\n") + str;
				++lines;												//空行+1
			}
		}
		inneSize.height = inneSize.height + lines*TEXT_HEIGHT + LABEL_HEIGHT + GAP_HEIGHT;	
	}
	
	return inneSize;
}

//调整坐标
void LayerGameRules::setContent()
{
	//分割线条
	ImageView* imgLine = (ImageView*)(Helper::seekWidgetByName(m_pRoot, "Img_TgaLine"));
	imgLine->setVisible(false);

	int lastPartLines = 0;			//上一段规则所占的行数
	float yPos = 0.0f;
	for(int i=0; i<m_vecDes.size(); i++)
	{
		
		Text* titleTxt = (Text*)m_pTitleTxt->clone();
		ImageView* imgLines = (ImageView*)imgLine->clone();
		Text* playDesTxt = (Text*)m_pText->clone();

		//标签文本
		titleTxt->setString(m_vecDes.at(i).labelDes);
		//玩法文本
		int lines = 0;
		string playDesStr = string("");

		for(string str : m_vecDes.at(i).playDes)
		{
			Text* checkLines = Text::create(str, FONT_FZZHENGHEI, 24);
			lines += (int)checkLines->getContentSize().width / (int)playDesTxt->getContentSize().width;
			lines += (int)checkLines->getContentSize().width % (int)playDesTxt->getContentSize().width ? 1 : 0;
			if(playDesStr == "")
			{
				playDesStr = str;
			}
			else
			{
				playDesStr = playDesStr + string("\n\n") + str;
				++lines;
			}
		}
		playDesTxt->setContentSize(Size(playDesTxt->getContentSize().width, lines*TEXT_HEIGHT));

		titleTxt->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		playDesTxt->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
		playDesTxt->setString(playDesStr);
		if(i == 0)
		{
			//第一段规则及其标签坐标
			titleTxt->setPosition(Vec2(m_pTitleTxt->getPositionX()-40, m_scrollPanel->getInnerContainerSize().height - 40));			
		}
		else
		{
			//其他规则及其标签坐标
			titleTxt->setPosition(Vec2(m_pTitleTxt->getPositionX()-40, yPos - (lastPartLines*TEXT_HEIGHT+GAP_HEIGHT)));		
		}
		imgLines->setPosition(Vec2(titleTxt->getPositionX(), titleTxt->getPositionY()-30));
		playDesTxt->setPosition(Vec2(m_pText->getPositionX()-40, titleTxt->getPositionY() - 40));
		yPos = playDesTxt->getPositionY();

		lastPartLines = lines;
		m_scrollPanel->addChild(titleTxt, 1);
		titleTxt->setVisible(true);
		titleTxt->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(titleTxt, Color3B(0x7f, 0x68, 0x0e), 2);
		m_scrollPanel->addChild(playDesTxt, 1);
		playDesTxt->setVisible(true);
		playDesTxt->setFontName(FONT_FZZHENGHEI);
		m_scrollPanel->addChild(imgLines, 1);
		imgLines->setVisible(true);
		setTextAddStroke(playDesTxt, Color3B(0x79, 0x11, 0x11), 2);
	}
}

void LayerGameRules::onBtnClick( Ref* ref, Widget::TouchEventType type)
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		this->dispose();
	}
}

void LayerGameRules::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if((ui::ScrollView*)pSender == m_scrollPanel)
	{
		switch (type)
		{
		case ui::ScrollView::EventType::SCROLLING:
			{
				{
					Size size = m_scrollPanel->getInnerContainerSize();
					Vec2 vec = m_scrollPanel->getInnerContainer()->getPosition();

					//滚动条重设进度
					float percentY = -vec.y / (size.height - m_scrollPanel->getContentSize().height);
					m_pScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
				}
			}
			break;

		default:
			break;
		}
	}
}

void LayerGameRules::parseDesByRuleId(int ruleTableId)
{
	//查询规则标签数据表
	RowData* ruleData = DataManager::getInstance()->searchRulesLabelById(ruleTableId);

	if(ruleData == nullptr)
	{
		return;
	}

	string labelIdStr = ruleData->getStringData("label");
	string playDesIdStr = ruleData->getStringData("ruleId");

	if(!labelIdStr.empty())
	{
		vector<string> vecStr;
		vecStr.clear();
		StringFormat::parseCsvStringVec(labelIdStr, vecStr);
		for(int i=0; i<vecStr.size(); i++)
		{
			int labelId = atoi(vecStr.at(i).c_str());
			RowData* labelDesData = DataManager::getInstance()->searchRulesTextById(labelId);
			//获得标签描述
			string labelStr = labelDesData->getStringData("rule");
			sLabelAndPlayDes des;
			des.playDes.clear();
			des.labelDes = labelStr;
			m_vecDes.push_back(des);
		}
	}

	if(!playDesIdStr.empty())
	{
		vector<string> vecStr;
		vecStr.clear();
		StringFormat::parseCsvStringVec(playDesIdStr, vecStr);
		for(int i=0; i<vecStr.size(); i++)
		{
			string ss = STR_ANSI(vecStr.at(i));
			int nIndex = vecStr.at(i).find(';', 0);
			if(nIndex == string::npos)
			{
				int playDesId = atoi(vecStr.at(i).c_str());
				RowData* ruleDesData = DataManager::getInstance()->searchRulesTextById(playDesId);
				//获得玩法描述
				string playDesStr = ruleDesData->getStringData("rule");
				string sss = STR_ANSI(playDesStr);
				m_vecDes.at(i).playDes.push_back(playDesStr);
			}
			else
			{
				vector<int> vecIdStr;
				vecIdStr.clear();
				StringFormat::parseCsvStringVecBySemi(vecStr.at(i), vecIdStr);
				for(int playDesId : vecIdStr)
				{
					RowData* ruleDesData = DataManager::getInstance()->searchRulesTextById(playDesId);
					//获得玩法描述
					string playDesStr = ruleDesData->getStringData("rule");
					string ssss = STR_ANSI(playDesStr);
					(m_vecDes.at(i).playDes).push_back(playDesStr);
				}
			}
		}
	}
}
