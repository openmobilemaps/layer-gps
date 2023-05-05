// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#pragma once

#include "GpsStyleInfo.h"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeGpsStyleInfo final {
public:
    using CppType = ::GpsStyleInfo;
    using JniType = jobject;

    using Boxed = NativeGpsStyleInfo;

    ~NativeGpsStyleInfo();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeGpsStyleInfo();
    friend ::djinni::JniClass<NativeGpsStyleInfo>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("io/openmobilemaps/gps/shared/gps/GpsStyleInfo") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Lio/openmobilemaps/mapscore/shared/graphics/objects/TextureHolderInterface;Lio/openmobilemaps/mapscore/shared/graphics/objects/TextureHolderInterface;Lio/openmobilemaps/mapscore/shared/graphics/common/Color;)V") };
    const jfieldID field_pointTexture { ::djinni::jniGetFieldID(clazz.get(), "pointTexture", "Lio/openmobilemaps/mapscore/shared/graphics/objects/TextureHolderInterface;") };
    const jfieldID field_headingTexture { ::djinni::jniGetFieldID(clazz.get(), "headingTexture", "Lio/openmobilemaps/mapscore/shared/graphics/objects/TextureHolderInterface;") };
    const jfieldID field_accuracyColor { ::djinni::jniGetFieldID(clazz.get(), "accuracyColor", "Lio/openmobilemaps/mapscore/shared/graphics/common/Color;") };
};

} // namespace djinni_generated
