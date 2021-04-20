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

	//��ʼ��
	void init(const char* tableName, TEACrypt* tea);

	//�������ݱ�
	void loadFile();

	//ͨ��id��ѯ����
	RowData* searchDataById(int id);

	//ͨ��id��ѯ������idƥ�������
	vector<RowData*> searchAllDataById(int id);

	//��ȡ��ͷ�ֶ�����
	int getFieldIndex(string field);

	//��ȡ��������
	multimap<int, RowData>& getTableData();

private:

	//���ñ�ͷ
	void setTableHead(vector<string>& vecHead);

	//�������
	void addRowData(vector<string>& data);

	//ȥ���ַ�����β�ո�
	string& strim(string &s);

private:

	//������, ͨ��id�ֶ�����
	multimap<int, RowData> mTableData;

	//��ͷ, ͨ����ͷ�ַ�����ȡ��ͷ����
	map<string, int> mTableHead;

	//�Ƿ��Ѽ���
	bool mIsLoaded;

	//���ݱ�����
	string mTableName;

	//Tea�����㷨��
	TEACrypt* mTea;



};


class RowData
{

public:

	RowData(DataTable* dataTable);
	~RowData();

public:

	//�������
	void setData(vector<string>& data);

	//��ȡ��������
	int getIntData(string field);

	//��ȡ��������
	float getFloatData(string field);

	//��ȡ�ַ�������
	string getStringData(string field);

	//��ȡ����������
	long long getLongData(string field);

private:

	//������
	vector<string> mData;

	//�����ݣ����Ի�ȡ��ͷ
	DataTable* mDataTable;

};




#endif