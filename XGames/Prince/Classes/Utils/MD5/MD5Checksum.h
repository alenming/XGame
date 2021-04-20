//
//  MD5Checksum.h

//

#ifndef __MD5Checksum__
#define __MD5Checksum__

#include "cocos2d.h"
USING_NS_CC;
/*****************************************************************************************
 
 
 *****************************************************************************************/
//typedef unsigned long uint32_t;
typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef unsigned char UCHAR;

#define ASSERT CC_ASSERT
#ifndef TRUE
    #define TRUE true
#endif
#ifndef FALSE
    #define FALSE false
#endif
class MD5Checksum
{
public:
    static std::string GetMD5OfString(std::string strString);
    //interface functions for the RSA MD5 calculation
    static std::string GetMD5(const std::string& strFilePath);
    
protected:
    //constructor/destructor
    MD5Checksum();
    virtual ~MD5Checksum() {};
    
    //RSA MD5 implementation
    void Transform(BYTE Block[64]);
    void Update(BYTE* Input, uint32_t nInputLen);
    std::string Final();
    inline uint32_t RotateLeft(uint32_t x, int n);
    inline void FF( uint32_t& A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t S, uint32_t T);
    inline void GG( uint32_t& A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t S, uint32_t T);
    inline void HH( uint32_t& A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t S, uint32_t T);
    inline void II( uint32_t& A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t S, uint32_t T);
    
    //utility functions
    inline void uint32_tToByte(BYTE* Output, uint32_t* Input, UINT nLength);
    inline void ByteTouint32_t(uint32_t* Output, BYTE* Input, UINT nLength);
    
private:
    BYTE  m_lpszBuffer[64];  //input buffer
    uint32_t m_nCount[2];   //number of bits, modulo 2^64 (lsb first)
    uint32_t m_lMD5[4];   //MD5 checksum
};

#endif /* defined(__MD5Checksum__) */
