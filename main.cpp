#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "camera.h"
#include "coder.h"
#include "SocketTransfer.h"

#define SERVER_STATE 0
#define CLIENT_STATE 1
using namespace std;

SocketTransfer *transfer ;
int program_state ;

void server_transfer_start()
{
    SocketTransfer::init() ;
    sockaddr_in serv_addr , cli_addr ;
    int serverSocket = socket( PF_INET , SOCK_STREAM , 0 ) ;
    int optval = 1;
    setsockopt( serverSocket , SOL_SOCKET , SO_REUSEADDR , &optval , sizeof optval ) ;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4399);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(serverSocket, 1);
    socklen_t cli_len = sizeof(cli_addr);
    int clientSocket = accept(serverSocket, (sockaddr *) &cli_addr, &cli_len);
    transfer = new SocketTransfer( clientSocket ) ;
}

void client_transfer_start( char *ip_addr )
{
    SocketTransfer::init();
    struct sockaddr_in serv_addr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname( ip_addr );
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr, (char *) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(4399);
    connect(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    transfer = new SocketTransfer(serverSocket);
}

int main( int argc , char *argv[] )
{
    bool first_turn = false ;

    // Intitial for transfer
    if ( strcmp( argv[ 1 ] , "-server" ) == 0 )
    {
        server_transfer_start() ;
        program_state = SERVER_STATE ;
    } else
    if ( strcmp( argv[ 1 ] , "-client" ) == 0 )
    {
        client_transfer_start( argv[ 2 ] ) ;
        program_state = CLIENT_STATE ;
        first_turn = true ;
    }

    set_camera( program_state ) ;
    IplImage *test_image = get_camera_image() ;
    CvSize image_size ;
    image_size .width = 320 ;
    image_size .height = 240 ;
    int transfer_len = image_size .height * image_size .width ;

    cvNamedWindow( "show" , 0 ) ;
    while ( 1 )
    {
        // Initial
        IplImage *frame    = cvCreateImage( image_size , test_image -> depth , test_image -> nChannels ) ;
        IplImage *show_img = cvCreateImage( image_size , test_image -> depth , test_image -> nChannels ) ;
        uchar *data = ( uchar* ) malloc( sizeof( uchar ) * transfer_len ) ;

        // catch image from camera
        cvResize( get_camera_image() , frame , CV_INTER_LINEAR ) ;

        cout << "GETOK" << endl;

        // encode image
        encode( frame , data ) ;

        // send data
        if ( first_turn )
            first_turn = false ;
        else {
            char *houjue = ( char *) malloc( sizeof( char ) * 4096 ) ;
            int i = 0 ;
            for ( int x = 0 ; x < image_size .height ; x ++ )
            {
                //cout << x << endl;
                strcpy( houjue , "" ) ;
                for ( int y = 0 ; y < image_size .width ; y ++ )
                    sprintf( houjue + strlen( houjue ) , "%d=" , data[ i ++ ] ) ;
                transfer -> send( houjue ) ;
            }
            free( houjue ) ;
        }

        cout << "SEND OK" << endl;

        // recive data
        char *houjue = ( char *) malloc( sizeof( char ) * 4096 ) ;
        int i = 0 ;
        for ( int x = 0 ; x < image_size .height ; x ++ )
        {
            transfer -> receive( houjue ) ;
            int xp = 0 ;
            for ( int y = 0 ; y < image_size .width ; y ++ )
            {
                data[ i ] = 0 ;
                while ( houjue[ xp ] != '=' )
                {
                    data[ i ] = data[ i ] * 10 + houjue[ xp ] - '0' ;
                    x ++ ;
                }
                i ++ ;
                xp ++ ;
            }
        }
        free( houjue ) ;

        cout << "RECIVE OK" << endl;

        // decode image
        decode( data , show_img ) ;

        // show image
        cvShowImage( "show" , show_img ) ;

        // end
        char c = cvWaitKey( 50 ) ;
        if ( c == 'q' ) break ;

        // release
        cvReleaseImage( &show_img ) ;
        cvReleaseImage( &frame ) ;
        free( data ) ;
    }

    destroy_camera() ;
    cvDestroyWindow( "show" ) ;
}
