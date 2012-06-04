#include "coder.h"

void encode( IplImage *frame , uchar *data )
{
    int len = 0 ;
    for ( int y = 0 ; y < frame -> height ; y ++ )
    {
        uchar *img_ptr = ( uchar* ) ( frame -> imageData + y * frame -> widthStep ) ;
        for ( int x = 0 ; x < frame -> width ; x ++ )
        {
            data[ len ++ ] = ( ( int ) img_ptr[ 3 * x ] * 256 + ( int ) img_ptr[ 3 * x ] + 1 ) * 256 + ( int ) img_ptr[ 3 * x + 2 ] ;
            //data[ len ++ ] = img_ptr[ 3 * x ] ;
            //data[ len ++ ] = img_ptr[ 3 * x + 1 ] ;
            //data[ len ++ ] = img_ptr[ 3 * x + 2 ] ;
        }
    }
}

void decode( uchar *data , IplImage *frame )
{
    int len = 0 ;
    for ( int y = 0 ; y < frame -> height ; y ++ )
    {
        uchar *img_ptr = ( uchar* ) ( frame -> imageData + y * frame -> widthStep ) ;
        for ( int x = 0 ; x < frame -> width ; x ++ )
        {
            img_ptr[ 3 * x ] = ( uchar ) ( data[ len ] / 256 ) / 256 ;
            img_ptr[ 3 * x + 1 ] = ( uchar ) ( data[ len ] / 256 ) % 256 ;
            img_ptr[ 3 * x + 2 ] = ( uchar ) data[ len ] % 256 ;
            len ++ ;
        }
    }
}
