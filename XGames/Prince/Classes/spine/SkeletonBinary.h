#ifndef SPINE_SKELETON_BINARY_H_
#define SPINE_SKELETON_BINARY_H_

#include <spine/SkeletonJson.h>
#include <stdio.h>
#include "Json.h"
#include <spine/extension.h>
#include <spine/AtlasAttachmentLoader.h>
#include <string>
#include <vector>
#include <algorithm>

extern "C" void _spSkeletonJson_setError (spSkeletonJson* self, Json* root, const char* value1, const char* value2);
extern "C" spSkeletonData* spSkeletonJson_readSkeletonBinaryFile (spSkeletonJson* self, const char* path);

static const int TIMELINE_SCALE = 0;
static const int TIMELINE_ROTATE = 1;
static const int TIMELINE_TRANSLATE = 2;
static const int TIMELINE_ATTACHMENT = 3;
static const int TIMELINE_COLOR = 4;
static const int TIMELINE_FLIPX = 5;
static const int TIMELINE_FLIPY = 6;
// static const int CURVE_LINEAR = 0;
static const int CURVE_STEPPED = 1;
static const int CURVE_BEZIER = 2;

namespace Spine 
{
	typedef std::string String;
	typedef signed char sbyte;
	typedef unsigned char byte;

	class BufferedStream
	{
	public:
		BufferedStream(void* BufferData, int Length) : _MyBuffer(BufferData), _MyLength(Length), _MyPosition(0) {}

		inline byte ReadByte()
		{
			if (_MyPosition>=_MyLength)
				return (byte)(-1);
			const byte* s = (const byte*)_MyBuffer + _MyPosition;

			byte ret = *s;

			_MyPosition++;
			return ret;
		}

	private:
		void * _MyBuffer;
		size_t _MyLength;
		size_t _MyPosition;
	};

	class SkeletonBinary 
	{

	public:

		SkeletonBinary(){}
		~SkeletonBinary(){}

	public:

		spSkeletonData* Read(spSkeletonJson* self, BufferedStream& input);

	private:

		spSkin* ReadSkin(BufferedStream& input, const String& skinName, bool nonessential);
		spAttachment* ReadAttachment (BufferedStream& input, spSkin* skin, const String& attachmentName, bool nonessential);

		void TO_ARRAY(float** outArray, int* outLength, const std::vector<float>& lList);
		void TO_ARRAY(float** outArray, const std::vector<float>& lList);
		void TO_ARRAY(int** outArray, int* outLength, const std::vector<int>& lList);
		char* TO_STR(const String& value);
		std::vector<float> ReadFloatArray(BufferedStream& input, float scale);
		std::vector<int> ReadShortArray(BufferedStream& input);
		std::vector<int> ReadIntArray(BufferedStream& input);

		void ReadAnimation(const String& name, BufferedStream& input, spSkeletonData* skeletonData, int animationIndex);
		void ReadCurve(BufferedStream& input, int frameIndex, spCurveTimeline* timeline);

		sbyte ReadSByte(BufferedStream& input);
		bool ReadBoolean(BufferedStream& input);
		float ReadFloat(BufferedStream& input);
		int ReadInt(BufferedStream& input);
		int ReadInt(BufferedStream& input, bool optimizePositive);
		String ReadString(BufferedStream& input);
		void ReadUtf8_slow(BufferedStream& input, String&chars, int charCount, int b);

	public:

		float Scale;
		spAttachmentLoader* attachmentLoader;
	};
};

#endif //SPINE_SKELETON_BINARY_H_