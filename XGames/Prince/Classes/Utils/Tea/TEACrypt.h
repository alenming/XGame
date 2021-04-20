#ifndef __TEACRYPT_H__
#define __TEACRYPT_H__

#include <stdio.h>
#include <stdint.h>

class TEACrypt
{

public:

	TEACrypt();
	~TEACrypt();

public:

	//设置key,128位密钥
	void setKey(uint32_t key[]);

	//加密算法
	void encrypt(const char* src, size_t size);

	//解密算法
	void decrypt(const char* src, size_t size);

	//获取计算结果
	char* getBuffer();

	//获取计算结果长度
	size_t getBufferSize();

	//清空运算结果缓存
	void clear();

	//设置加密轮数
	void setLoop(int loop);

private:

	//加密
	void encrypt(char src[]);

	//解密
	void decrypt(char src[]);
	
	//加密算法，64位明文，128位密钥
	void encrypt (uint32_t v[], uint32_t k[]);

	//解密算法，64位明文，128位密钥
	void decrypt (uint32_t v[], uint32_t k[]);

	//将4个char转为 32位无符号整型
    uint32_t charToUint32(char c1, char c2, char c3, char c4);

	//将两个无符号整型转换成char
	void uint32ToChar(uint32_t n1, uint32_t n2, char src[]);

private:

	//128位密钥
	uint32_t mKey[4];

	//加密、解密结果
	char* mResultBuffer;

	//加密、解密数据长度
	size_t mResultBufferSize;

	//加密轮数
	int mLoop;

};


#endif //__TEACRYPT_H__

