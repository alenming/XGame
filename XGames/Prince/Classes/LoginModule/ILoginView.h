#ifndef ILoginView_h__
#define ILoginView_h__

#include "LoginDef.h"

class ILoginView
{
public:

	virtual void	onRecvLoginData(int nResult, const char* szMsg) = 0;

	virtual void	onRecvRegisterData(int nResult, const char* szMsg) = 0;

	virtual void	onRecvModifyPwdData(int nResult, const char* szMsg) = 0;

	virtual void	onRecvActivateData(int nResult, const char* szMsg) = 0;

	virtual void	onRecvTryGameData(int nResult, const char* szMsg) = 0;

	virtual void	onRecvCreateRoleData() = 0;

};

#endif // ILoginView_h__
