#ifndef ResourceLoader_h__
#define ResourceLoader_h__

#include <string>
#include <vector>

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

typedef function<void()> AsyncLoadedCallBack;

class ResourceLoader : public Ref
{

public:

	ResourceLoader();
	~ResourceLoader();

	static ResourceLoader* getInstance();
	static void destroyInstance();

public:

	//启动异步加载
	void startAsyncLoading(const AsyncLoadedCallBack& callBack);

	//添加需异步加载的UI资源
	void addAsyncUIRes(string name);

	//添加需异步加载的图片资源
	bool addAsyncImageRes(string path);

	//添加异步加载UI资源，并启动异步加载
	void addUIResourceAsync(string name, const AsyncLoadedCallBack& callback);

	//移除UI资源
	void removeUIResource(string name);

private:

	//每帧添加一张异步加载的图片
	void loadImage(float dt);

	//异步加载图片回调
	void onLoadedImageAsyncCallBack(Texture2D* texture);

	//清空信息
	void clearAsync();

private:

	static ResourceLoader* mInstance;

	//所有需异步加载的图片路径
	vector<string> mVecResPath;

	//当前已经添加的图片数量
	int mAsyncAddCount;

	//当前已经加载的图片数量
	int mAsyncLoadedCount;

	//异步加载完毕回调
	AsyncLoadedCallBack mAsyncLoadedCallBack;

};

#endif // ResourceLoader_h__
