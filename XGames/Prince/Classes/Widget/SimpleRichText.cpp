#include "SimpleRichText.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"


SimpleRichText::SimpleRichText()
{

}

SimpleRichText::~SimpleRichText()
{

}

void SimpleRichText::setString( const string& richText, int defaultFontSize, Color3B defaultFontColor )
{
	if (richText.empty())
	{
		return;
	}
	
	//清空富文本元素
	for (size_t i = 0; i < mVecRichElement.size(); i++)
	{
		removeElement(mVecRichElement.at(i));
	}
	mVecRichElement.clear();

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
			RichElementText* element = nullptr;
			string& subStr = vecSubStr.at(i);

			if (subStr.empty())
			{
				continue;
			}

			string::size_type index = subStr.find("::");
			if (index == string::npos)
			{
				element = RichElementText::create(i, defaultFontColor, 255, subStr, FONT_FZZHENGHEI, defaultFontSize);
			}
			else
			{
				string colorStr = subStr.substr(0, index);
				string contentStr = subStr.substr(index + 2);
				Color3B color3B = StringFormat::parseCsvStringToColor(colorStr);
				element = RichElementText::create(i, color3B, 255, contentStr, FONT_FZZHENGHEI, defaultFontSize);
			}

			if (element)
			{
				pushBackElement(element);
				mVecRichElement.push_back(element);
			}
		}

		//最后一行不加换行
		if (j < lines.size() - 1)
		{
			RichElementNewLine* element = RichElementNewLine::create();
			pushBackElement(element);
			mVecRichElement.push_back(element);
		}
	}
}