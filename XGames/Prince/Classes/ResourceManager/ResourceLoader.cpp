#include "ResourceLoader.h"
#include "Utils/ResourceUtils.h"

ResourceLoader* ResourceLoader::mInstance = nullptr;

ResourceLoader::ResourceLoader()
	: mAsyncAddCount(0)
	, mAsyncLoadedCount(0)
{

}

ResourceLoader::~ResourceLoader()
{

}

ResourceLoader* ResourceLoader::getInstance()
{
	if (!mInstance)
	{
		mInstance = new ResourceLoader();		
	}

	return mInstance;
}

void ResourceLoader::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}
void ResourceLoader::startAsyncLoading(const AsyncLoadedCallBack& callBack)
{
	mAsyncLoadedCallBack = callBack;

	//启动加载图片定时器，每帧只加载一张图片
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(ResourceLoader::loadImage), this, 0, false);
}

void ResourceLoader::loadImage(float dt)
{
	if (mAsyncAddCount < mVecResPath.size())
	{
		Director::getInstance()->getTextureCache()->addImageAsync(mVecResPath.at(mAsyncAddCount),
			CC_CALLBACK_1(ResourceLoader::onLoadedImageAsyncCallBack, this));
		mAsyncAddCount++;
	}
	else
	{
		//如果资源列表为空，则直接结束异步加载
		if (mVecResPath.empty())
		{
			onLoadedImageAsyncCallBack(nullptr);
		}

		Director::getInstance()->getScheduler()->unschedule(
			schedule_selector(ResourceLoader::loadImage), this);
	}
}

void ResourceLoader::onLoadedImageAsyncCallBack(Texture2D* texture)
{
	mAsyncLoadedCount++;
	if (mAsyncLoadedCount >= mVecResPath.size())
	{
		//log("--------CachedTextureInfo----------");
		//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());

		clearAsync();
		//异步加载结束,执行回调
		mAsyncLoadedCallBack();
	}
}

void ResourceLoader::clearAsync()
{
	mAsyncAddCount = 0;
	mAsyncLoadedCount = 0;
	mVecResPath.clear();
}

void ResourceLoader::addAsyncUIRes(string name)
{
	string rootPath = ResourceUtils::getUIPath(name);
	stringstream ss;

	for(int i = 0; ;i++)
	{
		ss.clear();
		ss.str("");
		ss << rootPath << name << i << ".png";

		if (!addAsyncImageRes(ss.str()))
		{
			return;
		}
	}
}

bool ResourceLoader::addAsyncImageRes(string path)
{
	if (FileUtils::getInstance()->isFileExist(path))
	{
		mVecResPath.push_back(path);
		return true;
	}
	return false;
}

void ResourceLoader::addUIResourceAsync(string name, const AsyncLoadedCallBack& callback)
{
	addAsyncUIRes(name);
	startAsyncLoading(callback);
}

void ResourceLoader::removeUIResource(string name)
{
// 	string rootPath = ResourceUtils::getUIPath(name);
// 	stringstream ssImage;
// 	stringstream ssPlist;
// 
// 	for(int i = 0; ;i++)
// 	{
// 		ssImage.clear();
// 		ssPlist.clear();
// 		ssImage.str("");
// 		ssPlist.str("");
// 		ssImage << rootPath << name << i << ".png";
// 		ssPlist << rootPath << name << i << ".plist";
// 		if (FileUtils::getInstance()->isFileExist(ssImage.str()))
// 		{
// 			SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(ssPlist.str());
// 			Director::getInstance()->getTextureCache()->removeTextureForKey(ssImage.str());
// 		}
// 		else
// 		{
// 			return;
// 		}
// 	}
}
