// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni
#ifdef __cplusplus

#include "GpsStyleInfoInterface.h"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class MCGpsStyleInfoInterface;

namespace djinni_generated {

class GpsStyleInfoInterface
{
public:
    using CppType = std::shared_ptr<::GpsStyleInfoInterface>;
    using CppOptType = std::shared_ptr<::GpsStyleInfoInterface>;
    using ObjcType = MCGpsStyleInfoInterface*;

    using Boxed = GpsStyleInfoInterface;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

} // namespace djinni_generated

#endif
