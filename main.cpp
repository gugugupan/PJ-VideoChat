#include <iostream>
#include "camera.h"
using namespace std;

int main()
{
    set_camera() ;
    IplImage *test_frame = get_camera_image() ;
    CvSize camera_size = cvGetSize( test_frame ) ; // Width = 640   Height = 480

    cvNamedWindow( "show" , 0 ) ;
    while ( 1 )
    {
        // catch image from camera
        IplImage *frame = get_camera_image() ;
        //cvShowImage( "show" , frame ) ;

        // encode image

        // send data

        // recive data

        // decode image

        // show image

        // end
        char c = cvWaitKey( 50 ) ;
        if ( c == 'q' ) break ;
    }

    destroy_camera() ;
    cvDestroyWindow( "show" ) ;
}
