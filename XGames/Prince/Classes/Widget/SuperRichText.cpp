#include "SuperRichText.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "Common/Common.h"

SuperRichText::SuperRichText()
{
	width = 0.0f;
}

SuperRichText::~SuperRichText()
{
	
}


SuperRichText* SuperRichText::create()
{
	SuperRichText* layout = new SuperRichText();
	if (layout && layout->init())
	{
		layout->autorelease();
		return layout;
	}
	CC_SAFE_DELETE(layout);
	return nullptr;
}

bool SuperRichText::init()
{
	if (Layout::init())
	{
		return true;
	}
	return false;
}

void SuperRichText::setString( const string& richText, int defaultFontSize, Color3B defaultFontColor )
{
	if (richText.empty())
	{
		return;
	}
	

	mVecRich.clear();

	//拆分每行数据
	vector<string> lines;
	StringFormat::split(lines, richText, "/n");
	for (int j = 0; j < lines.size(); j++)
	{
		//拆分子串
		vector<string> vecSubStr;
		StringFormat::split(vecSubStr, lines.at(j), "|");

		//构造富文本
		for (size_t i = 0; i < vecSubStr.size(); i++)
		{
			
			string& subStr = vecSubStr.at(i);

			if (subStr.empty())
			{
				continue;
			}

			string::size_type index = subStr.find("::"); //字体颜色专用
			if (index == string::npos)
			{
				index = subStr.find(";;"); //图片专用 哈哈
				if (index == string::npos)
				{
					Text* element = nullptr;
					element = Text::create(subStr,FONT_FZZHENGHEI,defaultFontSize);
					element->setColor(defaultFontColor);
					MyRichStruct item;
					item.type = MyRichType_Text;
					item.widget = element;

					mVecRich.push_back(item);
				}
				else
				{
					ImageView* element = nullptr;
					string fileName = subStr.substr(0, index);
					string contentStr = subStr.substr(index + 2); //个数
					int count = atoi(contentStr.c_str());
					for (int k = 0; k < count; k++)
					{
						element = ImageView::create(fileName);
						element->setScale(0.5f);
						MyRichStruct item;
						item.type = MyRichType_Img;
						item.widget = element;

						mVecRich.push_back(item);
					}
				}
			}
			else
			{
				Text* element = nullptr;
				string colorStr = subStr.substr(0, index);
				string contentStr = subStr.substr(index + 2);
				Color3B color3B = StringFormat::parseCsvStringToColor(colorStr);
				element = Text::create(contentStr, FONT_FZZHENGHEI, defaultFontSize);
				element->setColor(color3B);

				MyRichStruct item;
				item.type = MyRichType_Text;
				item.widget = element;

				mVecRich.push_back(item);
			}
		}
	}

	for (int i = 0; i < mVecRich.size(); i++)
	{
		MyRichStruct item = mVecRich.at(i);
		switch (item.type)
		{
		case MyRichType_Text:
			{
				Text* _text = dynamic_cast<Text*>(item.widget);
				_text->setTextHorizontalAlignment(TextHAlignment::LEFT);
				_text->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
				_text->setPosition(Vec2(width,this->getContentSize().height/2));
				width+=_text->getContentSize().width;
				this->addChild(_text);	
			}
			break;
		case MyRichType_Img:
			{
				ImageView* _img = dynamic_cast<ImageView*>(item.widget);
				_img->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
				_img->setPosition(Vec2(width,this->getContentSize().height/2+2));
				width+=_img->getContentSize().width*_img->getScale();
				this->addChild(_img);	
			}
			break;
		default:
			break;
		}
		
	}
}

void SuperRichText::setStroke( Color3B c3b,int size )
{
	for (int i = 0; i < mVecRich.size(); i++)
	{
		MyRichStruct item = mVecRich.at(i);
		if (item.type == MyRichType_Text)
		{
			Text* _text = dynamic_cast<Text*>(item.widget);
			setTextAddStroke(_text,c3b,size);
		}
	}
}
