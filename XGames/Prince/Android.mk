LOCAL_PATH := $(call my-dir)
COCOS_PATH := $(LOCAL_PATH)/../..


###### princeģ�飬��Ϸ��ģ��
include $(CLEAR_VARS)
LOCAL_MODULE := prince_static
LOCAL_MODULE_FILENAME := libPrince

# ����Ŀ¼����Ŀ¼�ĺ���
define walk
    $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
# ����ClassesĿ¼
ALLFILES = $(call walk, $(LOCAL_PATH)/Classes)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST := $(filter %.cpp, $(ALLFILES))
FILE_LIST += $(filter %.c, $(ALLFILES))
				   
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Classes \
					$(COCOS_PATH)/cocos/ui \
					$(COCOS_PATH)/cocos/editor-support/cocostudio \
					$(COCOS_PATH)/extensions \
					$(COCOS_PATH)/cocos \
					$(COCOS_PATH)/external/curl/include/android/curl \
					$(COCOS_PATH)/external/unzip \
					$(LOCAL_PATH)/Classes/SDK \
					$(LOCAL_PATH)/Classes/Patcher \

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)
# $(call import-module,Box2D)
$(call import-module,editor-support/cocostudio)
$(call import-module,network)
$(call import-module,extensions)
