// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#include "NativeGpsLayerCallbackInterface.h"  // my header
#include "NativeCoord.h"
#include "NativeGpsMode.h"

namespace djinni_generated {

NativeGpsLayerCallbackInterface::NativeGpsLayerCallbackInterface() : ::djinni::JniInterface<::GpsLayerCallbackInterface, NativeGpsLayerCallbackInterface>("io/openmobilemaps/gps/shared/gps/GpsLayerCallbackInterface$CppProxy") {}

NativeGpsLayerCallbackInterface::~NativeGpsLayerCallbackInterface() = default;

NativeGpsLayerCallbackInterface::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeGpsLayerCallbackInterface::JavaProxy::~JavaProxy() = default;

void NativeGpsLayerCallbackInterface::JavaProxy::modeDidChange(::GpsMode c_mode) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeGpsLayerCallbackInterface>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_modeDidChange,
                           ::djinni::get(::djinni_generated::NativeGpsMode::fromCpp(jniEnv, c_mode)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeGpsLayerCallbackInterface::JavaProxy::onPointClick(const ::Coord & c_coordinate) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeGpsLayerCallbackInterface>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onPointClick,
                           ::djinni::get(::djinni_generated::NativeCoord::fromCpp(jniEnv, c_coordinate)));
    ::djinni::jniExceptionCheck(jniEnv);
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_gps_shared_gps_GpsLayerCallbackInterface_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        delete reinterpret_cast<::djinni::CppProxyHandle<::GpsLayerCallbackInterface>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_gps_shared_gps_GpsLayerCallbackInterface_00024CppProxy_native_1modeDidChange(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jobject j_mode)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerCallbackInterface>(nativeRef);
        ref->modeDidChange(::djinni_generated::NativeGpsMode::toCpp(jniEnv, j_mode));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_io_openmobilemaps_gps_shared_gps_GpsLayerCallbackInterface_00024CppProxy_native_1onPointClick(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, ::djinni_generated::NativeCoord::JniType j_coordinate)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::GpsLayerCallbackInterface>(nativeRef);
        ref->onPointClick(::djinni_generated::NativeCoord::toCpp(jniEnv, j_coordinate));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

} // namespace djinni_generated
