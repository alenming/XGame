#include <jni.h>
#include "bspatch.h"

#ifndef _Included_com_zcjoy_prince_Update
#define _Included_com_zcjoy_prince_Update
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com.zcjoy.prince.Update
 * Method:    patcher
 * Signature: ()V
 */
JNIEXPORT jint JNICALL Java_com_zcjoy_prince_Update_patcher
  (JNIEnv *, jobject, jstring argv1, jstring argv2, jstring argv3);

#ifdef __cplusplus
}
#endif
#endif
