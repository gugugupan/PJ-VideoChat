#ifndef ___CAMERA___
#define ___CAMERA___

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

    extern void set_camera() ;
    extern void destroy_camera() ;
    extern IplImage *get_camera_image() ;

#endif
