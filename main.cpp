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

    set_camera() ;
    IplImage *test_frame = get_camera_image() ;
    CvSize camera_size = cvGetSize( test_frame ) ; // Width = 640   Height = 480
    int transfer_len = camera_size .height * camera_size .width * 3 ;

    cvNamedWindow( "show" , 0 ) ;
    while ( 1 )
    {
        // Initial
        IplImage *frame ;
        IplImage *show_img ;
        uchar *data = ( uchar* ) malloc( sizeof( uchar ) * camera_size .height * camera_size .width * 3 ) ;

        // catch image from camera
        frame = get_camera_image() ;

        // encode image
        encode( frame , data ) ;

        // send data
        if ( first_turn )
            first_turn = false ;
        else {
            char *houjue = ( char *) malloc( sizeof( char ) * 10 ) ;
            for ( int i = 0 ; i < transfer_len ; i ++ )
            {
                sprintf( houjue , "%d=" , data[ i ] ) ;
                transfer -> send( houjue ) ;
            }
            free( houjue ) ;
        }

        cout << "SEND OK" << endl;

        // recive data
        char *houjue = ( char *) malloc( sizeof( char ) * 10 ) ;
        for ( int i = 0 ; i < transfer_len ; i ++ )
        {
            //cout << i << endl;
            transfer -> receive( houjue ) ;
            //cout << "TRANSFER OK" << endl;
            data[ i ] = 0 ; int x = 0 ;
            while ( houjue[ x ] != '=' )
            {
                data[ i ] = data[ i ] * 10 + houjue[ x ] - '0' ;
                x ++ ;
            }
            //cout << "END" << endl;
        }
        free( houjue ) ;

        cout << "RECIVE OK" << endl;

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
