/******************************************************************************
* Spine Runtimes Software License
* Version 2.1
*
* Copyright (c) 2015, Esoteric Software
* All rights reserved.
*
* You are granted a perpetual, non-exclusive, non-sublicensable and
* non-transferable license to install, execute and perform the Spine Runtimes
* Software (the "Software") solely for internal use. Without the written
* permission of Esoteric Software (typically granted by licensing Spine), you
* may not (a) modify, translate, adapt or otherwise create derivative works,
* improvements of the Software or develop new applications using the Software
* or (b) remove, delete, alter or obscure any trademarks or any copyright,
* trademark, patent or other intellectual property or proprietary rights
* notices on or in the Software, including any copy thereof. Redistributions
* in binary or source form must include this license and terms.
*
* THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ESOTERIC SOFTARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "SkeletonBinary.h"


namespace Spine {


	spSkeletonData* SkeletonBinary::Read (spSkeletonJson* self, BufferedStream& input)
	{           

		float scale = this->Scale;
		attachmentLoader = self->attachmentLoader;
		auto skeletonData = spSkeletonData_create();
		skeletonData->hash = TO_STR(ReadString(input));
		skeletonData->version = TO_STR(ReadString(input));
		skeletonData->width = ReadFloat(input);
		skeletonData->height = ReadFloat(input);

		bool nonessential = ReadBoolean(input);

		if (nonessential) {
			ReadString(input);
			/*
			skeletonData->imagesPath = ReadString(input);
			if (skeletonData->imagesPath.Length == 0) skeletonData->imagesPath = nullptr;
			*/
		}

		// Bones.

		skeletonData->bonesCount = ReadInt(input, true);
		skeletonData->bones = MALLOC(spBoneData*, skeletonData->bonesCount );

		for (int i = 0; i < skeletonData->bonesCount ; i++) {
			String name = ReadString(input);
			spBoneData* parent = nullptr;
			int parentIndex = ReadInt(input, true) - 1;
			if (parentIndex != -1) parent = skeletonData->bones[parentIndex];
			spBoneData* boneData = spBoneData_create(name.c_str(), parent);
			boneData->x = ReadFloat(input) * scale;
			boneData->y = ReadFloat(input) * scale;
			boneData->scaleX = ReadFloat(input);
			boneData->scaleY = ReadFloat(input);
			boneData->rotation = ReadFloat(input);
			boneData->length = ReadFloat(input) * scale;
			boneData->flipX = ReadBoolean(input);
			boneData->flipY = ReadBoolean(input);
			boneData->inheritScale = ReadBoolean(input);
			boneData->inheritRotation = ReadBoolean(input);
			if (nonessential) ReadInt(input); // Skip bone color.
			// skeletonData->bones. bones.Add(boneData);

			skeletonData->bones[i] = boneData;
		}

		// IK constraints.

		skeletonData->ikConstraintsCount = ReadInt(input, true);
		skeletonData->ikConstraints = MALLOC(spIkConstraintData*, skeletonData->ikConstraintsCount);

		for (int i = 0; i < skeletonData->ikConstraintsCount; i++) {
			spIkConstraintData* ikConstraintData = spIkConstraintData_create(ReadString(input).c_str());
			ikConstraintData->bonesCount = ReadInt(input, true);
			ikConstraintData->bones = MALLOC(spBoneData*, ikConstraintData->bonesCount);

			for (int ii = 0; ii < ikConstraintData->bonesCount; ii++)
			{
				ikConstraintData->bones[ii] = skeletonData->bones[ReadInt(input, true)];
			}
			ikConstraintData->target = skeletonData->bones[ReadInt(input, true)];
			ikConstraintData->mix = ReadFloat(input);
			ikConstraintData->bendDirection = ReadSByte(input);

			skeletonData->ikConstraints[i] = ikConstraintData;
		}

		// Slots.

		skeletonData->slotsCount = ReadInt(input, true);
		skeletonData->slots = MALLOC(spSlotData*, skeletonData->slotsCount);

		for (int i = 0; i < skeletonData->slotsCount; i++) {
			String slotName = ReadString(input);
			spBoneData* boneData = skeletonData->bones[ReadInt(input, true)];
			spSlotData* slotData = spSlotData_create(slotName.c_str(), boneData);
			int color = ReadInt(input);
			slotData->r = ((color & 0xff000000) >> 24) / 255.0f;
			slotData->g = ((color & 0x00ff0000) >> 16) / 255.0f;
			slotData->b = ((color & 0x0000ff00) >> 8) / 255.0f;
			slotData->a = ((color & 0x000000ff)) / 255.0f;
			slotData->attachmentName = TO_STR(ReadString(input));
			slotData->blendMode = (spBlendMode) ReadInt(input, true);
			skeletonData->slots[i] = slotData;
		}

		// Default skin.
		spSkin* defaultSkin = ReadSkin(input, "default", nonessential);
		if (defaultSkin != nullptr) {
			skeletonData->defaultSkin = defaultSkin;
		}


		skeletonData->skinsCount = 1 + ReadInt(input, true);
		skeletonData->skins = MALLOC(spSkin*, skeletonData->skinsCount);

		skeletonData->skins[0] = skeletonData->defaultSkin;
		// Skins.
		for (int i = 1; i < skeletonData->skinsCount; i++)
			skeletonData->skins[i] = ReadSkin(input, ReadString(input), nonessential);

		// Events.
		skeletonData->eventsCount = ReadInt(input, true);
		skeletonData->events = MALLOC(spEventData*, skeletonData->eventsCount);

		for (int i = 0; i < skeletonData->eventsCount; i++) {
			spEventData* eventData = spEventData_create(ReadString(input).c_str());
			eventData->intValue = ReadInt(input, false);
			eventData->floatValue = ReadFloat(input);
			eventData->stringValue = TO_STR(ReadString(input));
			skeletonData->events[i] = (eventData);
		}

		// Animations.
		skeletonData->animationsCount = ReadInt(input, true);
		skeletonData->animations = MALLOC(spAnimation*, skeletonData->animationsCount);
		for (int i = 0; i < skeletonData->animationsCount; i++)
			ReadAnimation(ReadString(input), input, skeletonData, i);

		/*
		skeletonData->bones.TrimExcess();
		skeletonData->slots.TrimExcess();
		skeletonData->skins.TrimExcess();
		skeletonData->events.TrimExcess();
		skeletonData->animations.TrimExcess();
		skeletonData->ikConstraints.TrimExcess();
		*/

		return skeletonData;
	}


	/** @return May be nullptr. */
	spSkin* SkeletonBinary::ReadSkin (BufferedStream& input, const String& skinName, bool nonessential) {
		int slotCount = ReadInt(input, true);
		//if (slotCount == 0) return nullptr;
		spSkin* skin = spSkin_create(skinName.c_str());
		for (int i = 0; i < slotCount; i++) {
			int slotIndex = ReadInt(input, true);

			for (int ii = 0, nn = ReadInt(input, true); ii < nn; ii++) {
				String name = ReadString(input);
				spSkin_addAttachment(skin, slotIndex, name.c_str(), ReadAttachment(input, skin, name, nonessential));
			}
		}
		return skin;
	}

	spAttachment* SkeletonBinary::ReadAttachment (BufferedStream& input, spSkin* skin, const String& attachmentName, bool nonessential) {
		float scale = Scale;

		String name = ReadString(input);
		if (name.length() == 0) name = attachmentName;

		switch ((spAttachmentType)input.ReadByte()) {
		case SP_ATTACHMENT_REGION: {
			String path = ReadString(input);
			if (path.length() == 0) path = name;

			auto* attachment = spAttachmentLoader_newAttachment(attachmentLoader, skin, SP_ATTACHMENT_REGION, (attachmentName.c_str()), (path.c_str()));
			spRegionAttachment* region = SUB_CAST(spRegionAttachment, attachment);

			if (region == nullptr) return nullptr;
			region->path = TO_STR(path);
			region->x = ReadFloat(input) * scale;
			region->y = ReadFloat(input) * scale;
			region->scaleX = ReadFloat(input);
			region->scaleY = ReadFloat(input);
			region->rotation = ReadFloat(input);
			region->width = ReadFloat(input) * scale;
			region->height = ReadFloat(input) * scale;
			int color = ReadInt(input);
			region->r = ((color & 0xff000000) >> 24) / 255.0f;
			region->g = ((color & 0x00ff0000) >> 16) / 255.0f;
			region->b = ((color & 0x0000ff00) >> 8) / 255.0f;
			region->a = ((color & 0x000000ff)) / 255.0f;
			spRegionAttachment_updateOffset(region);
			return (spAttachment*)region;
								   }
		case SP_ATTACHMENT_BOUNDING_BOX: {

			auto* attachment = spAttachmentLoader_newAttachment(attachmentLoader, skin, SP_ATTACHMENT_BOUNDING_BOX, (attachmentName.c_str()), NULL);
			spBoundingBoxAttachment* box = SUB_CAST(spBoundingBoxAttachment, attachment);

			if (box == nullptr) return nullptr;
			TO_ARRAY(&box->vertices, &box->verticesCount, ReadFloatArray(input, scale));
			return (spAttachment*)box;
										 }
		case SP_ATTACHMENT_MESH: {
			String path = ReadString(input);
			if (path.length() == 0) path = name;

			auto* attachment = spAttachmentLoader_newAttachment(attachmentLoader, skin, SP_ATTACHMENT_MESH, (attachmentName.c_str()), path.c_str());


			spMeshAttachment* mesh = SUB_CAST(spMeshAttachment, attachment);
			if (mesh == nullptr) return nullptr;
			mesh->path = TO_STR(path);
			TO_ARRAY(&mesh->regionUVs, ReadFloatArray(input, 1));
			TO_ARRAY(&mesh->triangles, &mesh->trianglesCount, ReadShortArray(input)); // TODO: Convert to tri-strip 
			TO_ARRAY(&mesh->vertices, &mesh->verticesCount, ReadFloatArray(input, scale));
			spMeshAttachment_updateUVs(mesh);
			int color = ReadInt(input);
			mesh->r = ((color & 0xff000000) >> 24) / 255.0f;
			mesh->g = ((color & 0x00ff0000) >> 16) / 255.0f;
			mesh->b = ((color & 0x0000ff00) >> 8) / 255.0f;
			mesh->a = ((color & 0x000000ff)) / 255.0f;
			mesh->hullLength = ReadInt(input, true);
			if (nonessential) {
				TO_ARRAY(&mesh->edges, &mesh->edgesCount, ReadIntArray(input));
				mesh->width = ReadFloat(input) * scale;
				mesh->height = ReadFloat(input) * scale;
			}
			return (spAttachment*)mesh;
								 }
		case SP_ATTACHMENT_SKINNED_MESH: {

			// FIXME: Not tested
			String path = ReadString(input);
			if (path.length() == 0) path = name;

			auto* attachment = spAttachmentLoader_newAttachment(attachmentLoader, skin, SP_ATTACHMENT_SKINNED_MESH, (attachmentName.c_str()), path.c_str());
			spSkinnedMeshAttachment* mesh = SUB_CAST(spSkinnedMeshAttachment, attachment);

			if (mesh == nullptr) return nullptr;
			mesh->path = TO_STR(path);
			std::vector<float> uvs = ReadFloatArray(input, 1);
			std::vector<int> triangles = ReadShortArray(input);

			int vertexCount = ReadInt(input, true);
			std::vector<float> weights;
			weights.reserve(uvs.size() * 3 * 3);
			std::vector<int> bones;
			bones.reserve(uvs.size() * 3);

			for (int i = 0; i < vertexCount; i++) {
				int boneCount = (int)ReadFloat(input);
				bones.push_back(boneCount);
				for (int nn = i + boneCount * 4; i < nn; i += 4) {
					bones.push_back((int)ReadFloat(input));
					weights.push_back(ReadFloat(input) * scale);
					weights.push_back(ReadFloat(input) * scale);
					weights.push_back(ReadFloat(input));
				}
			}
			TO_ARRAY(&mesh->bones, &mesh->bonesCount, bones);
			TO_ARRAY(&mesh->weights, &mesh->weightsCount, weights);
			TO_ARRAY(&mesh->triangles, &mesh->trianglesCount, triangles);
			TO_ARRAY(&mesh->regionUVs, &mesh->uvsCount, uvs);

			spSkinnedMeshAttachment_updateUVs(mesh);

			int color = ReadInt(input);
			mesh->r = ((color & 0xff000000) >> 24) / 255.0f;
			mesh->g = ((color & 0x00ff0000) >> 16) / 255.0f;
			mesh->b = ((color & 0x0000ff00) >> 8) / 255.0f;
			mesh->a = ((color & 0x000000ff)) / 255.0f;
			mesh->hullLength = ReadInt(input, true) * 2;
			if (nonessential) {
				TO_ARRAY(&mesh->edges, &mesh->edgesCount, ReadIntArray(input));
				mesh->width = ReadFloat(input) * scale;
				mesh->height = ReadFloat(input) * scale;
			}
			return (spAttachment*)mesh;
										 }
		}
		return nullptr;
	}

	void SkeletonBinary::TO_ARRAY(float** outArray, int* outLength, const std::vector<float>& lList)
	{
		auto* ret = CALLOC(float, lList.size());
		*outLength = (int)lList.size();
		if (ret)
		{
			memcpy(ret, lList.data(), sizeof(float) * lList.size());
		}
		*outArray = ret;
		return;
	}

	void SkeletonBinary::TO_ARRAY(float** outArray, const std::vector<float>& lList)
	{
		auto*  ret = CALLOC(float, lList.size());
		if (ret)
		{
			memcpy(ret, lList.data(), sizeof(float) * lList.size());
		}

		*outArray = ret;
	}

	void SkeletonBinary::TO_ARRAY(int** outArray, int* outLength, const std::vector<int>& lList)
	{
		auto*  ret = MALLOC(int, lList.size());
		*outLength = (int)lList.size();
		if (ret)
			memcpy(ret, lList.data(), sizeof(float) * lList.size());
		*outArray = ret;
	}

	char* SkeletonBinary::TO_STR(const String& value)
	{
		if (value.length() == 0)
			return nullptr;
		char*  ret = MALLOC(char, value.size() + 1);
		if (ret)
			memcpy(ret, value.c_str(), value.size() + 1);
		return ret;
	}

	std::vector<float> SkeletonBinary::ReadFloatArray(BufferedStream& input, float scale) {
		int n = ReadInt(input, true);
		std::vector<float> array; array.reserve(n);
		if (scale == 1) {
			for (int i = 0; i < n; i++)
				array.push_back(ReadFloat(input));
		} else {
			for (int i = 0; i < n; i++)
				array.push_back(ReadFloat(input) * scale);
		}
		return array;
	}

	std::vector<int> SkeletonBinary::ReadShortArray (BufferedStream& input) {
		int n = ReadInt(input, true);
		std::vector<int> array; array.reserve(n);
		for (int i = 0; i < n; i++)
			array.push_back((input.ReadByte() << 8) + input.ReadByte());
		return array;
	}

	std::vector<int> SkeletonBinary::ReadIntArray (BufferedStream& input) {
		int n = ReadInt(input, true);
		std::vector<int> array; array.reserve(n);
		for (int i = 0; i < n; i++)
			array.push_back(ReadInt(input, true));
		return array;
	}

	void SkeletonBinary::ReadAnimation (const String& name,
		BufferedStream& input, spSkeletonData* skeletonData, int animationIndex) {
			std::vector<spTimeline*> timelines;
			float scale = this->Scale;
			float duration = 0;

			// Slot timelines.
			for (int i = 0, n = ReadInt(input, true); i < n; i++) {
				int slotIndex = ReadInt(input, true);

				for (int ii = 0, nn = ReadInt(input, true); ii < nn; ii++) {
					int timelineType = input.ReadByte();
					int frameCount = ReadInt(input, true);


					switch (timelineType) {
					case TIMELINE_COLOR: {
						spColorTimeline* timeline =  spColorTimeline_create(frameCount);
						timeline->slotIndex = slotIndex;
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
							float time = ReadFloat(input);
							int color = ReadInt(input);

							float r = ((color & 0xff000000) >> 24) / 255.0f;                                
							float g = ((color & 0x00ff0000) >> 16) / 255.0f;
							float b = ((color & 0x0000ff00) >> 8) / 255.0f;
							float a = ((color & 0x000000ff)) / 255.0f;

							spColorTimeline_setFrame(timeline, frameIndex, time, r, g, b, a);
							if (frameIndex < frameCount - 1) ReadCurve(input, frameIndex, (spCurveTimeline*)timeline);
						}



						timelines.push_back((spTimeline*)timeline);
						if (frameCount > 0)
							duration = std::max(duration, timeline->frames[frameCount * 5 - 5]);
						break;
										 }
					case TIMELINE_ATTACHMENT: {
						spAttachmentTimeline* timeline = spAttachmentTimeline_create(frameCount);
						timeline->slotIndex = slotIndex;
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
						{
							float fFrame = ReadFloat(input);
							String attachName = ReadString(input);
							spAttachmentTimeline_setFrame(timeline,
								frameIndex, fFrame, (attachName.length () ? attachName.c_str() : nullptr));
						}
						timelines.push_back((spTimeline*)timeline);
						if (frameCount > 0)
							duration = std::max(duration, timeline->frames[frameCount - 1]);
						break;
											  }
					}
				}
			}

			// Bone timelines.
			for (int i = 0, n = ReadInt(input, true); i < n; i++) {
				int boneIndex = ReadInt(input, true);

				for (int ii = 0, nn = ReadInt(input, true); ii < nn; ii++) {
					int timelineType = input.ReadByte();
					int frameCount = ReadInt(input, true);
					switch (timelineType) {
					case TIMELINE_ROTATE: {
						spRotateTimeline* timeline = spRotateTimeline_create(frameCount);
						timeline->boneIndex = boneIndex;
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
							float time = ReadFloat(input);
							float rot = ReadFloat(input);
							spRotateTimeline_setFrame(timeline, frameIndex, time, rot);
							if (frameIndex < frameCount - 1) ReadCurve(input, frameIndex, (spCurveTimeline*)timeline);
						}
						timelines.push_back((spTimeline*)timeline);
						if (frameCount > 0)
							duration = std::max(duration, timeline->frames[frameCount * 2 - 2]);
						break;
										  }


					case TIMELINE_TRANSLATE:
					case TIMELINE_SCALE: {
						spTranslateTimeline *timeline;
						float timelineScale = 1;
						if (timelineType == TIMELINE_SCALE)
							timeline = spScaleTimeline_create(frameCount);
						else {
							timeline = spTranslateTimeline_create(frameCount);
							timelineScale = scale;
						}
						timeline->boneIndex = boneIndex;
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {		
							float time = ReadFloat(input);
							float x = ReadFloat(input);
							float y = ReadFloat(input);
							spTranslateTimeline_setFrame(timeline, frameIndex, time, x * timelineScale,  y * timelineScale);
							if (frameIndex < frameCount - 1)
								ReadCurve(input, frameIndex, (spCurveTimeline*)timeline);
						}
						timelines.push_back((spTimeline*)timeline);
						if (frameCount > 0)
							duration = std::max(duration, timeline->frames[frameCount * 3 - 3]);
						break;
										 }
					case TIMELINE_FLIPX:
					case TIMELINE_FLIPY: {
						spFlipTimeline* timeline = spFlipTimeline_create(frameCount, timelineType == TIMELINE_FLIPX);
						timeline->boneIndex = boneIndex;
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
						{
							float time = ReadFloat(input);
							int flip = ReadBoolean(input);
							spFlipTimeline_setFrame(timeline, frameIndex, time, flip);
						}
						timelines.push_back((spTimeline*)timeline);
						if (frameCount > 0)
							duration = std::max(duration, timeline->frames[frameCount * 2 - 2]);
						break;
										 }
					}
				}
			}

			// IK timelines.
			for (int i = 0, n = ReadInt(input, true); i < n; i++) {
				int ikConstraintIndex = ReadInt(input, true);
				// spIkConstraintData* ikConstraint = skeletonData->ikConstraints[ikConstraintIndex];
				int frameCount = ReadInt(input, true);

				spIkConstraintTimeline* timeline = spIkConstraintTimeline_create(frameCount);
				timeline->ikConstraintIndex = ikConstraintIndex;
				for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {

					float time = ReadFloat(input);
					float mix = ReadFloat(input);
					int bendDirection = ReadSByte(input);

					spIkConstraintTimeline_setFrame(timeline, frameIndex, time, mix, bendDirection);
					if (frameIndex < frameCount - 1) ReadCurve(input, frameIndex, (spCurveTimeline*)timeline);
				}
				timelines.push_back((spTimeline*)timeline);
				duration = std::max(duration, timeline->frames[frameCount * 3 - 3]);
			}

			// FFD timelines.
			for (int i = 0, n = ReadInt(input, true); i < n; i++) {

				spSkin* skin = skeletonData->skins[ReadInt(input, true)];
				for (int ii = 0, nn = ReadInt(input, true); ii < nn; ii++) {
					int slotIndex = ReadInt(input, true);
					for (int iii = 0, nnn = ReadInt(input, true); iii < nnn; iii++) {
						String attName = ReadString(input);
						spAttachment* attachment = spSkin_getAttachment(skin, slotIndex, attName.length() ? attName.c_str() : nullptr);
						int frameCount = ReadInt(input, true);
						int verticesCount = 0;

						if (attachment->type == SP_ATTACHMENT_MESH)
							verticesCount = SUB_CAST(spMeshAttachment, attachment)->verticesCount;
						else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH)
							verticesCount = SUB_CAST(spSkinnedMeshAttachment, attachment)->weightsCount / 3 * 2;


						spFFDTimeline* timeline = spFFDTimeline_create(frameCount, verticesCount);


						timeline->slotIndex = slotIndex;
						timeline->attachment = attachment;
						float* tempVertices = MALLOC(float, verticesCount);
						for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
							float time = ReadFloat(input);

							float* frameVertices;

							int end = ReadInt(input, true);
							if (end == 0) {
								if (attachment->type == SP_ATTACHMENT_MESH)
									frameVertices = SUB_CAST(spMeshAttachment, attachment)->vertices;
								else
								{
									frameVertices = tempVertices;
									memset(frameVertices, 0, sizeof(float) * verticesCount);
								}
							} else {
								frameVertices = tempVertices;
								int start = ReadInt(input, true);
								memset(frameVertices, 0, sizeof(float) * start);

								end += start;
								int v = start;
								if (scale == 1) {
									for (; v < end; v++)
										frameVertices[v] = ReadFloat(input);
								} else {
									for (; v < end; v++)
										frameVertices[v] = ReadFloat(input) * scale;
								}

								memset(frameVertices + v, 0, sizeof(float) * (verticesCount - v));

								if (attachment->type == SP_ATTACHMENT_MESH) {
									float* meshVertices = ((spMeshAttachment*)attachment)->vertices;
									for (int v = 0, vn = verticesCount; v < vn; v++)
										frameVertices[v] += meshVertices[v];
								}
							}

							spFFDTimeline_setFrame(timeline, frameIndex, time, frameVertices); // spFFDTimeline_setFrame copy the vertices

							if (frameIndex < frameCount - 1) ReadCurve(input, frameIndex, SUPER(timeline));
						}

						FREE(tempVertices);

						timelines.push_back((spTimeline*)timeline);
						duration = std::max(duration, timeline->frames[frameCount - 1]);
					}
				}
			}

			// Draw order timeline.
			int drawOrderCount = ReadInt(input, true);
			if (drawOrderCount > 0) {
				spDrawOrderTimeline* timeline = spDrawOrderTimeline_create(drawOrderCount, skeletonData->slotsCount);

				int slotCount = skeletonData->slotsCount;
				for (int i = 0; i < drawOrderCount; i++) {

					int offsetCount = ReadInt(input, true);
					int* drawOrder = MALLOC(int, slotCount);
					for (int ii = slotCount - 1; ii >= 0; ii--)
						drawOrder[ii] = -1;
					int* unchanged = MALLOC(int, slotCount - offsetCount);
					int originalIndex = 0, unchangedIndex = 0;
					for (int ii = 0; ii < offsetCount; ii++) {
						int slotIndex = ReadInt(input, true);
						// Collect unchanged items.
						while (originalIndex != slotIndex)
							unchanged[unchangedIndex++] = originalIndex++;
						// Set changed items.
						drawOrder[originalIndex + ReadInt(input, true)] = originalIndex;
						originalIndex++;
					}
					// Collect remaining unchanged items.
					while (originalIndex < slotCount)
						unchanged[unchangedIndex++] = originalIndex++;
					// Fill in unchanged items.
					for (int ii = slotCount - 1; ii >= 0; ii--)
						if (drawOrder[ii] == -1) drawOrder[ii] = unchanged[--unchangedIndex];
					spDrawOrderTimeline_setFrame(timeline, i, ReadFloat(input), drawOrder);

					FREE(drawOrder); 
					FREE(unchanged); 
				}
				timelines.push_back((spTimeline*)timeline);
				duration = std::max(duration, timeline->frames[drawOrderCount - 1]);
			}

			// Event timeline.
			int eventCount = ReadInt(input, true);
			if (eventCount > 0) {
				spEventTimeline* timeline = spEventTimeline_create(eventCount);
				for (int i = 0; i < eventCount; i++) {
					float time = ReadFloat(input);
					spEventData* eventData = skeletonData->events[ReadInt(input, true)];
					spEvent* e =  spEvent_create(eventData);
					e->intValue = ReadInt(input, false);
					e->floatValue = ReadFloat(input);
					e->stringValue = ReadBoolean(input) ? TO_STR(ReadString(input).c_str()) : eventData->stringValue;

					spEventTimeline_setFrame(timeline, i, time, e);
				}
				timelines.push_back((spTimeline*)timeline);
				duration = std::max(duration, timeline->frames[eventCount - 1]);
			}

			skeletonData->animations[animationIndex] = spAnimation_create((name.c_str()), (int)timelines.size());
			skeletonData->animations[animationIndex]->duration = duration;

			for (int i = 0; i < (int)timelines.size(); i++)
			{
				skeletonData->animations[animationIndex]->timelines[i] = timelines[i];
			}
	}



	void SkeletonBinary::ReadCurve (BufferedStream& input, int frameIndex, spCurveTimeline* timeline) {
		switch (input.ReadByte()) {
		case CURVE_STEPPED:
			spCurveTimeline_setStepped(timeline, frameIndex);
			break;

		case CURVE_BEZIER:
			{
				float f1 = ReadFloat(input);
				float f2 = ReadFloat(input);
				float f3 = ReadFloat(input);
				float f4 = ReadFloat(input);
				spCurveTimeline_setCurve(timeline, frameIndex, f1, f2, f3, f4);
			}
			break;
		}
	}

	sbyte SkeletonBinary::ReadSByte (BufferedStream& input) {
		int value = input.ReadByte();
		return (sbyte)value;
	}

	bool SkeletonBinary::ReadBoolean (BufferedStream& input) {
		return input.ReadByte() != 0;
	}

	float SkeletonBinary::ReadFloat (BufferedStream& input) {
		union
		{
			int intValue;
			float floatValue;
		}t;

		t.intValue = (input.ReadByte() << 24) + (input.ReadByte() << 16) + (input.ReadByte() << 8) + input.ReadByte();
		return t.floatValue;
	}

	int SkeletonBinary::ReadInt (BufferedStream& input) {
		return (input.ReadByte() << 24) + (input.ReadByte() << 16) + (input.ReadByte() << 8) + input.ReadByte();
	}

	int SkeletonBinary::ReadInt (BufferedStream& input, bool optimizePositive) {
		int b = input.ReadByte();
		int result = b & 0x7F;
		if ((b & 0x80) != 0) {
			b = input.ReadByte();
			result |= (b & 0x7F) << 7;
			if ((b & 0x80) != 0) {
				b = input.ReadByte();
				result |= (b & 0x7F) << 14;
				if ((b & 0x80) != 0) {
					b = input.ReadByte();
					result |= (b & 0x7F) << 21;
					if ((b & 0x80) != 0) {
						b = input.ReadByte();
						result |= (b & 0x7F) << 28;
					}
				}
			}
		}
		return optimizePositive ? result : ((result >> 1) ^ -(result & 1));
	}

	String SkeletonBinary::ReadString(BufferedStream& input) {
		int charCount = ReadInt(input, true);
		switch (charCount) {
		case 0:
		case 1:
			return "";
		}
		charCount--;
		String chars;
		// Try to read 7 bit ASCII chars.
		int b = 0;
		while (chars.length() < charCount) {
			b = input.ReadByte();
			if (b > 127) break;
			chars.push_back(b);
		}
		// If a char was not ASCII, finish with slow path.
		if (chars.length() < charCount) ReadUtf8_slow(input, chars, charCount, b);
		return chars;
	}

	void SkeletonBinary::ReadUtf8_slow (BufferedStream& input, String&chars, int charCount, int b) {
		for (;;) {
			switch (b >> 4) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				chars.push_back((char) b);
				break;
			case 12:
			case 13:
				chars.push_back(((char) ((b & 0x1F) << 6) | (input.ReadByte() & 0x3F)));
				break;
			case 14:
				chars.push_back(((char) ((b & 0x0F) << 12) | ((input.ReadByte() & 0x3F) << 6) | (input.ReadByte() & 0x3F)));
				break;
			}
			if (chars.length() >= charCount) break;
			b = input.ReadByte() & 0xFF;
		}
	}

};

spSkeletonData* spSkeletonBinary_readSkeletonData (spSkeletonJson* self, const void* skel, int length) {

	Spine::SkeletonBinary handler;
	Spine::BufferedStream buffer ((void*)skel, length);
	handler.Scale = self->scale;
	handler.attachmentLoader = self->attachmentLoader;
	return handler.Read(self, buffer);
}

spSkeletonData* spSkeletonJson_readSkeletonBinaryFile (spSkeletonJson* self, const char* path) {
	int length;
	spSkeletonData* skeletonData;
	const char* skel = _spUtil_readFile(path, &length);
	if (!skel) {
		_spSkeletonJson_setError(self, 0, "Unable to read skeleton file: ", path);
		return 0;
	}

	skeletonData = spSkeletonBinary_readSkeletonData(self, skel, length);
	FREE(skel);
	return skeletonData;
}
