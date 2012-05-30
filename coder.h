#ifndef ___CODER___
#define ___CODER___

#include <cv.h>
#include <cxcore.h>

    extern void encode( IplImage *frame , uchar *data ) ;
    extern void decode( uchar *data , IplImage *frame ) ;

#endif
