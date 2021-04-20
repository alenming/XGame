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

	//����key,128λ��Կ
	void setKey(uint32_t key[]);

	//�����㷨
	void encrypt(const char* src, size_t size);

	//�����㷨
	void decrypt(const char* src, size_t size);

	//��ȡ������
	char* getBuffer();

	//��ȡ����������
	size_t getBufferSize();

	//�������������
	void clear();

	//���ü�������
	void setLoop(int loop);

private:

	//����
	void encrypt(char src[]);

	//����
	void decrypt(char src[]);
	
	//�����㷨��64λ���ģ�128λ��Կ
	void encrypt (uint32_t v[], uint32_t k[]);

	//�����㷨��64λ���ģ�128λ��Կ
	void decrypt (uint32_t v[], uint32_t k[]);

	//��4��charתΪ 32λ�޷�������
    uint32_t charToUint32(char c1, char c2, char c3, char c4);

	//�������޷�������ת����char
	void uint32ToChar(uint32_t n1, uint32_t n2, char src[]);

private:

	//128λ��Կ
	uint32_t mKey[4];

	//���ܡ����ܽ��
	char* mResultBuffer;

	//���ܡ��������ݳ���
	size_t mResultBufferSize;

	//��������
	int mLoop;

};


#endif //__TEACRYPT_H__

