// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#import "MCGpsLayerInterface+Private.h"
#import "MCGpsLayerInterface.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#import "MCCoord+Private.h"
#import "MCGpsCourseInfo+Private.h"
#import "MCGpsLayerCallbackInterface+Private.h"
#import "MCGpsMode+Private.h"
#import "MCGpsStyleInfo+Private.h"
#import "MCLayerInterface+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface MCGpsLayerInterface ()

- (id)initWithCpp:(const std::shared_ptr<::GpsLayerInterface>&)cppRef;

@end

@implementation MCGpsLayerInterface {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::GpsLayerInterface>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::GpsLayerInterface>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

+ (nullable MCGpsLayerInterface *)create:(nonnull MCGpsStyleInfo *)styleInfo {
    try {
        auto objcpp_result_ = ::GpsLayerInterface::create(::djinni_generated::GpsStyleInfo::toCpp(styleInfo));
        return ::djinni_generated::GpsLayerInterface::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setCallbackHandler:(nullable id<MCGpsLayerCallbackInterface>)handler {
    try {
        _cppRefHandle.get()->setCallbackHandler(::djinni_generated::GpsLayerCallbackInterface::toCpp(handler));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setMode:(MCGpsMode)mode {
    try {
        _cppRefHandle.get()->setMode(::djinni::Enum<::GpsMode, MCGpsMode>::toCpp(mode));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setModeWithRotationReset:(MCGpsMode)mode
                   resetRotation:(BOOL)resetRotation {
    try {
        _cppRefHandle.get()->setModeWithRotationReset(::djinni::Enum<::GpsMode, MCGpsMode>::toCpp(mode),
                                                      ::djinni::Bool::toCpp(resetRotation));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setResetRotationOnMapInteraction:(BOOL)resetRotation {
    try {
        _cppRefHandle.get()->setResetRotationOnMapInteraction(::djinni::Bool::toCpp(resetRotation));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (MCGpsMode)getMode {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getMode();
        return ::djinni::Enum<::GpsMode, MCGpsMode>::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)enableHeading:(BOOL)enable {
    try {
        _cppRefHandle.get()->enableHeading(::djinni::Bool::toCpp(enable));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)updatePosition:(nonnull MCCoord *)position
   horizontalAccuracyM:(double)horizontalAccuracyM {
    try {
        _cppRefHandle.get()->updatePosition(::djinni_generated::Coord::toCpp(position),
                                            ::djinni::F64::toCpp(horizontalAccuracyM));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)updateHeading:(float)angleHeading {
    try {
        _cppRefHandle.get()->updateHeading(::djinni::F32::toCpp(angleHeading));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setDrawPoint:(BOOL)enable {
    try {
        _cppRefHandle.get()->setDrawPoint(::djinni::Bool::toCpp(enable));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setDrawHeading:(BOOL)enable {
    try {
        _cppRefHandle.get()->setDrawHeading(::djinni::Bool::toCpp(enable));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)enablePointRotationInvariant:(BOOL)enable {
    try {
        _cppRefHandle.get()->enablePointRotationInvariant(::djinni::Bool::toCpp(enable));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setFollowInitializeZoom:(nullable NSNumber *)zoom {
    try {
        _cppRefHandle.get()->setFollowInitializeZoom(::djinni::Optional<std::optional, ::djinni::F32>::toCpp(zoom));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)updateStyle:(nonnull MCGpsStyleInfo *)styleInfo {
    try {
        _cppRefHandle.get()->updateStyle(::djinni_generated::GpsStyleInfo::toCpp(styleInfo));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)enableCourse:(BOOL)enable {
    try {
        _cppRefHandle.get()->enableCourse(::djinni::Bool::toCpp(enable));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)updateCourse:(nonnull MCGpsCourseInfo *)courseInfo {
    try {
        _cppRefHandle.get()->updateCourse(::djinni_generated::GpsCourseInfo::toCpp(courseInfo));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nullable id<MCLayerInterface>)asLayerInterface {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->asLayerInterface();
        return ::djinni_generated::LayerInterface::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto GpsLayerInterface::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto GpsLayerInterface::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<MCGpsLayerInterface>(cpp);
}

} // namespace djinni_generated

@end
