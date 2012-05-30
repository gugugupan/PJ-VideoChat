#include <iostream>
#include "camera.h"
#include "coder.h"
using namespace std;

int main()
{
    set_camera() ;
    IplImage *test_frame = get_camera_image() ;
    CvSize camera_size = cvGetSize( test_frame ) ; // Width = 640   Height = 480

    cvNamedWindow( "show" , 0 ) ;
    while ( 1 )
    {
        // Initial
        IplImage *frame ;
        IplImage *show_img ;
        uchar *data = ( uchar* ) malloc( sizeof( uchar ) * camera_size .height * camera_size .width * 3 ) ;

        // catch image from camera
        frame = get_camera_image() ; // depth = 8    nChannels = 3

        // encode image
        encode( frame , data ) ;

        // send data

        // recive data

        // decode image
        show_img = cvCreateImage( camera_size , frame -> depth , frame -> nChannels ) ;
        decode( data , show_img ) ;

        // show image
        cvShowImage( "show" , show_img ) ;

        // end
        char c = cvWaitKey( 50 ) ;
        if ( c == 'q' ) break ;

        // release
        cvReleaseImage( &show_img ) ;
        free( data ) ;
    }

    destroy_camera() ;
    cvDestroyWindow( "show" ) ;
}
