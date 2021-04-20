#ifndef IMazeView_h__
#define IMazeView_h__

#include "MazeDef.h"


class IMazeView
{
public:
	virtual void	onGetAllMazesResult(const int& nResult) = 0;	
	virtual void	onRandomRoadResult(const int& nResult) = 0;

	virtual void	onOpenBoxResult(const int& boxId) = 0;
	virtual void    onOpenUnCompleteBoxResult(const int& boxId) = 0; 
	virtual void    onOpenBoxEndResult() = 0;
	virtual void	onRewardResult(Maze_ReWard_Show reward) = 0;
	virtual void	onRewardOpenCardResult(CHOOSEED_ITEM_REWARD reward) = 0;
	virtual void    gotoFirst() = 0;

protected:
private:
};

class IMazeLimitView
{
public:	
	virtual void	onLimitBuyResult(const int& nResult, const vector<TOOL_NEW_INFO>& vecNewTools) = 0;
};

#endif 
