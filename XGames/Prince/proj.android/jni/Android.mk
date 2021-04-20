LOCAL_PATH := $(call my-dir)
COCOS_PATH := $(LOCAL_PATH)/../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp

LOCAL_WHOLE_STATIC_LIBRARIES := prince_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,XGames\Prince)
