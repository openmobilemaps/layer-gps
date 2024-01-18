// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#pragma once

#include "GpsStyleInfoInterface.h"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeGpsStyleInfoInterface final : ::djinni::JniInterface<::GpsStyleInfoInterface, NativeGpsStyleInfoInterface> {
public:
    using CppType = std::shared_ptr<::GpsStyleInfoInterface>;
    using CppOptType = std::shared_ptr<::GpsStyleInfoInterface>;
    using JniType = jobject;

    using Boxed = NativeGpsStyleInfoInterface;

    ~NativeGpsStyleInfoInterface();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeGpsStyleInfoInterface>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeGpsStyleInfoInterface>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeGpsStyleInfoInterface();
    friend ::djinni::JniClass<NativeGpsStyleInfoInterface>;
    friend ::djinni::JniInterface<::GpsStyleInfoInterface, NativeGpsStyleInfoInterface>;

};

} // namespace djinni_generated
