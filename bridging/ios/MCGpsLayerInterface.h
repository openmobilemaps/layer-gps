// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#import "MCCoord.h"
#import "MCGpsCourseInfo.h"
#import "MCGpsMode.h"
#import "MCLayerInterface.h"
#import <Foundation/Foundation.h>
@class MCGpsLayerInterface;
@class MCGpsStyleInfoInterface;
@protocol MCGpsLayerCallbackInterface;


@interface MCGpsLayerInterface : NSObject

+ (nullable MCGpsLayerInterface *)create:(nullable MCGpsStyleInfoInterface *)styleInfo;

- (void)setCallbackHandler:(nullable id<MCGpsLayerCallbackInterface>)handler;

- (void)setMode:(MCGpsMode)mode;

- (void)setModeWithRotationReset:(MCGpsMode)mode
                   resetRotation:(BOOL)resetRotation;

- (void)setResetRotationOnMapInteraction:(BOOL)resetRotation;

- (MCGpsMode)getMode;

- (void)enableHeading:(BOOL)enable;

- (void)updatePosition:(nonnull MCCoord *)position
   horizontalAccuracyM:(double)horizontalAccuracyM;

- (void)updateHeading:(float)angleHeading;

- (void)setDrawPoint:(BOOL)enable;

- (void)setDrawHeading:(BOOL)enable;

- (void)enablePointRotationInvariant:(BOOL)enable;

- (void)setFollowInitializeZoom:(nullable NSNumber *)zoom;

- (void)updateStyle:(nullable MCGpsStyleInfoInterface *)styleInfo;

- (void)enableCourse:(BOOL)enable;

- (void)updateCourse:(nonnull MCGpsCourseInfo *)courseInfo;

- (void)setRenderPassIndex:(int32_t)index;

- (nullable id<MCLayerInterface>)asLayerInterface;

@end
