// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#pragma once

#include "GpsMode.h"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeGpsMode final : ::djinni::JniEnum {
public:
    using CppType = ::GpsMode;
    using JniType = jobject;

    using Boxed = NativeGpsMode;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeGpsMode>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeGpsMode>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeGpsMode() : JniEnum("io/openmobilemaps/gps/shared/gps/GpsMode") {}
    friend ::djinni::JniClass<NativeGpsMode>;
};

} // namespace djinni_generated
