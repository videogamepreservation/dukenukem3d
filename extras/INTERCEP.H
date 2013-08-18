//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment

This file is NOT part of Duke Nukem 3D version 1.5 - Atomic Edition
However, it is either an older version of a file that is, or is
some test code written during the development of Duke Nukem 3D.
This file is provided purely for educational interest.

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#import <appkit/appkit.h>

@interface NXShape:Object
{
    NXZone             *zone;
    void               *_impl;
}


// Init, copy, and free
- init;
- initFromRect:(NXRect *)rect;
- free;
- copyFromZone:(NXZone *)zone;

// Querying
- (BOOL)isEqual:shape;
- (BOOL)isEmpty;
- rectEnumerator;

// Changing
- offsetShape:(NXPoint *)distance;
- differenceWithShape:shape;
- unionWithShape:shape;
- intersectWithShape:shape;

@end


@protocol NXShapeEnumerator
- (NXRect *)nextRect;
@end

@interface NXInterceptorClient:Object
{
    void               *context;
    id                  interceptedRects;
    id                  listLock;
    id                  portLock;
    struct cthread     *handlingThread;
    char                _reserved0;
    unsigned int        _padding[8];
    void               *_private;
}

/*
 * Class methods
 */

+ initialize;

/*
 * Create and destroy
 */

- init;
- free;

/*
 * Query
 */

- (int)interceptorPort;

/*
 * Handling
 */

- setHandlingThread:(struct cthread *)thread;
- (struct cthread *)handlingThread;
- startHandlingThread;

typedef struct {
    msg_header_t	header;
    msg_type_t		type;
    int			a,b,c,d,e[16];
} dumb_message_t;

typedef struct {
    msg_header_t	header;
    msg_type_t		type;
    int			a;
} dumb_reply_t;

- (BOOL)handleInterceptorMessage:(dumb_message_t *)message
	withReply:(dumb_reply_t *)reply;

/*
 * Private
 */
 
- (void *)_context;
- _removeInterceptedRect:fp16;
- _addInterceptedRect:fp16 returnedScreenRect:(NXRect *)rect
	returnedFlags:(int *)fp24;

@end


@interface NXInterceptedRect:Object
{
    NXRect              rect;
    NXRect              screenRect;
    int                 windowNumber;
    id                  screen;
    unsigned int        uniqueID;
    id                  target;
    unsigned int        flags;
    id                  rectLock;
    BOOL                isTotallyVisible;
    BOOL                isTotallyObscured;
    BOOL                moveInProgress;
    BOOL                isLocked;
    id                  tmpBitmap;
    NXInterceptorClient *interceptorClient;
    unsigned int        _ir_padding[8];
    void               *_ir_private;
}


/*
 * Create and destroy
 */

- initForRect:(NXRect *)rect inWindow:(int)window
	onFramebuffer:frameBuffer forClient:client;
- free;

/*
 * Query
 */
 
- framebuffer;
- (BOOL)isTotallyObscured;
- (BOOL)isTotallyVisible;
- getRect:(NXRect *)rect;
- (int)windowNumber;
- (int)uniqueID;
- currentClipList:(NXRect *)rects count:(int)count;
- currentScreenRect:(NXRect *)rect;

/*
 * Action
 */

- compositeBits:bits withOp:(int)compositeOp;
- target;
- setTarget:target;

/*
 * Locking
 */
 
- lockRect;
- unlockRect;
- (BOOL)isLocked;

/*
 * Private
 */
 
- _handleMsg:(dumb_message_t *)fp16 withReply:(dumb_reply_t *)fp20;
- (unsigned int)_flags;

@end



#import <sys/types.h>
#import <mach/message.h>

@interface NXSimpleBitmap:Object
{
    BOOL                isPlanar;
    BOOL                hasAlpha;
    int                 bitsPerSample;
    int                 samplesPerPixel;
    int                 bitsPerPixel;
    int                 bytesPerRow;
    int                 bytesPerPlane;
    int                 numPlanes;
    int                 pixelsWide;
    int                 pixelsHigh;
    int                 colorSpace;
    void               *data[5];
    unsigned int        _bm_padding[8];
    void               *_bm_private;
}



/*
 * Create and destroy
 */

- initDataPlanes:(unsigned char **)planes pixelsWide:(int)width
	pixelsHigh:(int)height bitsPerSample:(int)bps samplesPerPixel:(int)spp
	hasAlpha:(BOOL)alpha isPlanar:(BOOL)isPlanar
	colorSpace:(NXColorSpace)colorSpace bytesPerRow:(int)rBytes
	bitsPerPixel:(int)pBits;
- initData:(unsigned char *)data pixelsWide:(int)width
	pixelsHigh:(int)height bitsPerSample:(int)bps samplesPerPixel:(int)spp
	hasAlpha:(BOOL)alpha isPlanar:(BOOL)isPlanar
	colorSpace:(NXColorSpace)colorSpace bytesPerRow:(int)rBytes
	bitsPerPixel:(int)pBits;
- free;

/*
 * Querying
 */

- (int)pixelsHigh;
- (int)pixelsWide;
- (int)colorSpace;
- (int)numPlanes;
- (int)bytesPerPlane;
- (int)bytesPerRow;
- (int)bitsPerSample;
- (int)bitsPerPixel;
- (int)samplesPerPixel;
- (BOOL)hasAlpha;
- (BOOL)isPlanar;
- getDataPlanes:(unsigned char **)data;
- (unsigned char *)data;

@end


@interface NXFramebuffer:NXSimpleBitmap
{
    NXInterceptorClient *interceptorClient;
    int                 screenNumber;
    NXRect		bounds;
    char                pixelEncoding[64];
    char                driver[80];
    int                 deviceSlot;
    int                 deviceUnit;
    void               *conversionTable;
    void               *inverseConversionTable;
    BOOL                isMapped;
    unsigned int        _fb_padding[8];
    void               *_fb_private;
}



/*
 * Create and destroy
 */
 
+ MegaPixelInstance;
- initFromScreen:(int)screen andMapIfPossible:(BOOL)doMap;
- free;

/*
 * Querying
 */
 
- (int)deviceSlot;
- (int)deviceUnit;
- (char *)driver;
- (char *)pixelEncoding;
- (void *)addressForPoint:(NXPoint *)point;
- (void *)inverseConversionTable;
- (void *)conversionTable;
- (int)screenNumber;
- screenBounds:(NXRect *)bounds;
- (BOOL)isMappable;

/*
 * Private
 */
 
- _remapForThread;
- _interceptorClient;

@end



@interface NXDirectBitmap:NXSimpleBitmap
{
    BOOL                isBuffered;
    BOOL                isDirectMapped;
    BOOL                isUnobscured;
    BOOL                isLocked;
    BOOL                depthMismatch;
    BOOL                drawToBuffer;
    BOOL                updateNeeded;
    NXFramebuffer      *framebuffer;
    int                 currentScreen;
    int                 newScreen;
    NXInterceptedRect  *interceptRect;
    NXInterceptorClient *interceptClient;
    void               *copyFunc;
    id                  window;
    int                 gWinNum;
    NXRect              rect;
    int                 _flushOnExposure;
    id                  _delegate;
    id                  _viewClip;
    unsigned int        _dbm_padding[5];
    void               *_dbm_private;
}


/*
 * Class methods
 */

+ minDepthForGray:(int)gray andColor:(int)color;

/*
 * Create and destroy
 */

- initForRect:(NXRect *)rect inWindow:window;
- init;
- free;

/*
 * Query
 */
 
- (BOOL)isDirectMapped;
- (BOOL)isBuffered;
- (int)bytesPerRow;
- (int)bytesPerPlane;
- (char *)pixelEncoding;
- getPixelEncoding:(char **)pixelEncoding;
- (char *)data;
- getDataPlanes:(char **)dataPlanes;
- (void *)inverseConversionTable;
- (void *)conversionTable;

/*
 * Change state
 */
 
- setDirectMapped:(BOOL)isDirectMapped;
- setBuffered:(BOOL)isBuffered;


/*
 * Action
 */
 
- updateState;
- updateForRect:(NXRect *)rect inWindow:window;
- flushIn:(NXRect *)rect;
- flush;


/*
 * Locking
 */ 
 
- lockBitmap;
- unlockBitmap;
- (BOOL)tryLockBitmap;

/*
 * Area methods
 */

- areaWindowFreed:area;
- areaChangedScreen:area from:(int)fromWindow to:(int)toWindow;
- areaIsInvalid:area;
- areaWasOrderedOut:area;
- areaWasOrderedIn:area;
- areaDidReveal:area inRect:(NXRect *)rect;
- areaWillObscure:area inRect:(NXRect *)rect;
- areaDidMove:area by:(NXPoint *)point;
- areaWillMove:area by:(NXPoint *)point;

/*
 * Private
 */
 
- _viewClip:clip;
- _setViewClip:clip;
- (BOOL)_isUnobscured;
- (NXPoint)_screenBoundsOrigin;
- _setFlushOnExposure:(BOOL)flush;
- _framebuffer;
- _setDelegate:delegate;
- _updateForRect:(NXRect *)rect inWinNum:(int)winNum onScreen:(int)screen;
- _updateBackingStoreForRect:(NXRect *)rect;
- (BOOL)_canUseDirectMapping;
- _updateBuffer;
- _initForRect:(NXRect *)rect inWinNum:(int)winNum onScreen:(int)screen;
- _mapFramebufferForScreen:(int)screen;
- _flushInShape:shape;
- (char *)_dataBuffer;

@end


