// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni
#ifdef __cplusplus

#include "GpsLayerCallbackInterface.h"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@protocol MCGpsLayerCallbackInterface;

namespace djinni_generated {

class GpsLayerCallbackInterface
{
public:
    using CppType = std::shared_ptr<::GpsLayerCallbackInterface>;
    using CppOptType = std::shared_ptr<::GpsLayerCallbackInterface>;
    using ObjcType = id<MCGpsLayerCallbackInterface>;

    using Boxed = GpsLayerCallbackInterface;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

} // namespace djinni_generated

#endif
