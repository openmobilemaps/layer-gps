// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from layer_gps.djinni

#import "MCColor.h"
#import "MCTextureHolderInterface.h"
#import <Foundation/Foundation.h>
@class MCGpsStyleInfoInterface;


@interface MCGpsStyleInfoInterface : NSObject

+ (nullable MCGpsStyleInfoInterface *)create:(nullable id<MCTextureHolderInterface>)pointTexture
                              headingTexture:(nullable id<MCTextureHolderInterface>)headingTexture
                               courseTexture:(nullable id<MCTextureHolderInterface>)courseTexture
                               accuracyColor:(nonnull MCColor *)accuracyColor;

- (nullable id<MCTextureHolderInterface>)getPointTexture;

- (nullable id<MCTextureHolderInterface>)getHeadingTexture;

- (nullable id<MCTextureHolderInterface>)getCourseTexture;

- (nonnull MCColor *)getAccuracyColor;

@end