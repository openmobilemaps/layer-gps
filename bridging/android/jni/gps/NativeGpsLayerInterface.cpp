// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from layer_gps.djinni

#include "NativeGpsLayerInterface.h"  // my header
#include "Marshal.hpp"
#include "NativeCoord.h"
#include "NativeGpsLayerCallbackInterface.h"
#include "NativeGpsMode.h"
#include "NativeLayerInterface.h"

namespace djinni_generated {

NativeGpsLayerInterface::NativeGpsLayerInterface() : ::djinni::JniInterface<::GpsLayerInterface, NativeGpsLayerInterface>("io/openmobilemaps/mapscore/shared/gps/GpsLayerInterface$CppProxy") {}

NativeGpsLayerInterface::~NativeGpsLayerInterface() = default;


CJNIEXPORT void JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        delete reinterpret_cast<::djinni::CppProxyHandle<::GpsLayerInterface>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_create(JNIEnv* jniEnv, jobject /*this*/, jobject j_handler)
{
    try {
        DJINNI_FUNCTION_PROLOGUE0(jniEnv);
        auto r = ::GpsLayerInterface::create(::djinni_generated::NativeGpsLayerCallbackInterface::toCpp(jniEnv, j_handler));
        return ::djinni::release(::djinni_generated::NativeGpsLayerInterface::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_native_1setMode(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jobject j_mode)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerInterface>(nativeRef);
        ref->setMode(::djinni_generated::NativeGpsMode::toCpp(jniEnv, j_mode));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_native_1updatePosition(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, ::djinni_generated::NativeCoord::JniType j_position, jdouble j_horizontalAccuracyM)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerInterface>(nativeRef);
        ref->updatePosition(::djinni_generated::NativeCoord::toCpp(jniEnv, j_position),
                            ::djinni::F64::toCpp(jniEnv, j_horizontalAccuracyM));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_native_1updateHeading(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jfloat j_angleHeading)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerInterface>(nativeRef);
        ref->updateHeading(::djinni::F32::toCpp(jniEnv, j_angleHeading));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT ::djinni_generated::NativeLayerInterface::JniType JNICALL Java_io_openmobilemaps_mapscore_shared_gps_GpsLayerInterface_00024CppProxy_native_1asLayerInterface(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerInterface>(nativeRef);
        auto r = ref->asLayerInterface();
        return ::djinni::release(::djinni_generated::NativeLayerInterface::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

}  // namespace djinni_generated
