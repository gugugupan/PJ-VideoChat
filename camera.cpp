#include "camera.h"

CvCapture *camera_cap ;

void set_camera( int auth )
{
    if ( auth || 1 )
        camera_cap = cvCreateCameraCapture( -1 ) ;
    else
        camera_cap = cvCaptureFromFile( "test.avi" ) ;
}

void destroy_camera()
{
    cvReleaseCapture( &camera_cap ) ;
}

IplImage *get_camera_image()
{
    return cvQueryFrame( camera_cap ) ;
}
