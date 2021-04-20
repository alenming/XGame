#ifndef TypeDef_h__
#define TypeDef_h__

#ifndef _WIN32

#define far

#ifndef BYTE
typedef  unsigned char BYTE;
#endif

#ifndef DWORD
typedef  unsigned long DWORD;
#endif

#ifndef WORD
typedef  unsigned short WORD;
#endif

#ifndef UINT
typedef  unsigned int   UINT;
#endif

typedef int             BOOL;

typedef const void far  *LPCVOID;
typedef const char      *LPCSTR, *PCSTR;

typedef char            *LPSTR, *PSTR;

typedef void far        *LPVOID;
typedef LPCSTR LPCTSTR;

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#endif

#endif // TypeDef_h__
