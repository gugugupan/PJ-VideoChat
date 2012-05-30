#include "camera.h"

CvCapture *camera_cap ;

void set_camera()
{
    camera_cap = cvCreateCameraCapture( -1 ) ;
}

void destroy_camera()
{
    cvReleaseCapture( &camera_cap ) ;
}

IplImage *get_camera_image()
{
    return cvQueryFrame( camera_cap ) ;
}
