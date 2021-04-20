#include "DataTable.h"
#include "CSVParser.h"
#include <cmath>
#include <cctype>


DataTable::DataTable()
	: mIsLoaded(false)
	, mTea(nullptr)
{

}

DataTable::~DataTable()
{

}

void DataTable::init(const char* tableName, TEACrypt* tea)
{
	mTableName = string(tableName);
	mTea = tea;
}

void DataTable::loadFile()
{
	//判定数据表是否已加载
	if (mIsLoaded)
	{
		return;
	}

	//读取二进制文件
	string path = "Data/" + mTableName;
	Data data = FileUtils::getInstance()->getDataFromFile(path);
	if (data.isNull())
	{
		CCLOG("!!error, data null");
		return;
	}

	const char *buffer = (char*)data.getBytes();
	size_t size = data.getSize();

	mTea->decrypt(buffer, size);

	//读取解密数据
	istringstream iss(mTea->getBuffer());
	bool isReadHead = false;
	vector<string> head;
	vector<string> record;
	string line;

	while(getline(iss, line)  && iss.good())
	{
		if (!isReadHead)
		{
			//读取表头
			csvline_populate(head, line, ',');
			this->setTableHead(head);
			isReadHead = true;
		}
		else
		{
			//读取数据
			csvline_populate(record, line, ',');
			this->addRowData(record);
		}
	}

	mTea->clear();

	mIsLoaded = true;
}

void DataTable::setTableHead(vector<string>& vecHead)
{
	this->mTableHead = mTableHead;

	for (int i = 0; i < vecHead.size(); i++)
	{
		mTableHead.insert(pair<string, int>(strim(vecHead[i]), i));
	}
}

void DataTable::addRowData(vector<string>& data)
{
	RowData rowData(this);
	rowData.setData(data);
	int id = rowData.getIntData("id");
	if (id >= 0)
	{
		mTableData.insert(pair<int, RowData>(id, rowData));
	}
}

RowData* DataTable::searchDataById(int id)
{
	loadFile();
	auto iter = mTableData.find(id);
	if (iter != mTableData.end())
	{
		return &((*iter).second);
	}
	return nullptr;
}

vector<RowData*> DataTable::searchAllDataById(int id)
{
	loadFile();
	vector<RowData*> vecData;
	for (auto iter = mTableData.begin(); iter != mTableData.end(); iter++)
	{
		int tempId = (*iter).first;
		if (tempId == id)
		{
			vecData.push_back(&((*iter).second));
		}
	}
	return vecData;
}

//获取整表数据
multimap<int, RowData>& DataTable::getTableData()
{
	loadFile();
	return mTableData;
}

int DataTable::getFieldIndex(string field)
{
	auto iter = mTableHead.find(field);
	if (iter != mTableHead.end())
	{
		return (*iter).second;
	}
	return -1;

}

string& DataTable::strim(string &s)   
{  
	if (s.empty())   
	{  
		return s;
	}  
	s.erase(0,s.find_first_not_of(" "));  
	s.erase(s.find_last_not_of(" ") + 1);  
	return s;  
}  


///////////////////////////////华丽丽的分割线////////////////////////////////////////////


RowData::RowData(DataTable* dataTable)
{
	this->mDataTable = dataTable;
};

RowData::~RowData()
{

}

//添加数据
void RowData::setData(vector<string>& data)
{
	this->mData = data;
}

//获取整型数据
int RowData::getIntData(string field)
{
	int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return atoi(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		CCLOG("Not Found Field %s", field.c_str());
		return -1;
	}
}

//获取浮点数据
float RowData::getFloatData(string field)
{
	int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return atof(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		CCLOG("Not Found Field %s", field.c_str());
		return -1.0f;
	}
}

//获取字符串数据
string RowData::getStringData(string field)
{
	int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		return mData.at(fieldIndex);
	}
	else
	{
		CCLOG("Not Found Field %s", field.c_str());
		return "";
	}
}

long long RowData::getLongData(string field)
{
	int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return atof(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		CCLOG("Not Found Field %s", field.c_str());
		return -1.0f;
	}
}