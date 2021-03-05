// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from layer_gps.djinni

#import "MCCoord.h"
#import "MCGpsMode.h"
#import "MCLayerInterface.h"
#import <Foundation/Foundation.h>
@class MCGpsLayerInterface;
@protocol MCGpsLayerCallbackInterface;


@interface MCGpsLayerInterface : NSObject

+ (nullable MCGpsLayerInterface *)create:(nullable id<MCGpsLayerCallbackInterface>)handler;

- (void)setMode:(MCGpsMode)mode;

- (void)updatePosition:(nonnull MCCoord *)position
   horizontalAccuracyM:(double)horizontalAccuracyM;

- (void)updateHeading:(float)angleHeading;

- (nullable id<MCLayerInterface>)asLayerInterface;

@end
