#ifndef DataTable_h__
#define DataTable_h__

#include "cocos2d.h"
#include <vector>
#include <string>
#include <map>
#include "Utils/Tea/TEACrypt.h"

USING_NS_CC;
using namespace std;

class RowData;

class DataTable
{

public:

	DataTable();
	~DataTable();

public:

	//初始化
	void init(const char* tableName, TEACrypt* tea);

	//加载数据表
	void loadFile();

	//通过id查询数据
	RowData* searchDataById(int id);

	//通过id查询所有与id匹配的数据
	vector<RowData*> searchAllDataById(int id);

	//获取表头字段索引
	int getFieldIndex(string field);

	//获取整表数据
	multimap<int, RowData>& getTableData();

private:

	//设置表头
	void setTableHead(vector<string>& vecHead);

	//添加数据
	void addRowData(vector<string>& data);

	//去掉字符串首尾空格
	string& strim(string &s);

private:

	//表数据, 通过id字段索引
	multimap<int, RowData> mTableData;

	//表头, 通过表头字符串获取表头索引
	map<string, int> mTableHead;

	//是否已加载
	bool mIsLoaded;

	//数据表名称
	string mTableName;

	//Tea解密算法类
	TEACrypt* mTea;



};


class RowData
{

public:

	RowData(DataTable* dataTable);
	~RowData();

public:

	//添加数据
	void setData(vector<string>& data);

	//获取整型数据
	int getIntData(string field);

	//获取浮点数据
	float getFloatData(string field);

	//获取字符串数据
	string getStringData(string field);

	//获取长整型数据
	long long getLongData(string field);

private:

	//行数据
	vector<string> mData;

	//表数据，用以获取表头
	DataTable* mDataTable;

};




#endif