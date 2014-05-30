#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using namespace std;

//The attributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces that will be used
SDL_Surface *message_image = NULL;
SDL_Surface *background = NULL;
SDL_Surface *circulo = NULL;
SDL_Surface *rectangulo = NULL;
SDL_Surface *triangulo = NULL;
SDL_Surface *screen = NULL;

SDL_Surface *load_image( std::string filename )
{
    return IMG_Load( filename.c_str() );
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}


int main(int argc, char **argv)
{
	IPaddress ip,*remoteip;
	TCPsocket server,client;
	client=NULL;
	char message[1024];
	int len;
	Uint32 ipaddr;
	Uint16 port;

	/* initialize SDL */
	if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
	{
		printf("SDL_Init: %s\n",SDL_GetError());
		exit(1);
	}

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return 1;
    }

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		printf("SDLNet_Init: %s\n",SDLNet_GetError());
		exit(2);
	}

	/* get the port from the commandline */
	port=(Uint16)strtol("3000",NULL,0);
	cout<<"Esperando cliente, conectado en el puerto "<<port<<endl;
	cout.flush();

	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1)
	{
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		exit(3);
	}

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		exit(4);
	}


    //Load the images
    message_image = load_image( "hello.bmp" );
    background = load_image( "background.bmp" );
    circulo = load_image( "circulo.png" );
    rectangulo = load_image( "rectangulo.png" );
    triangulo = load_image( "triangulo.png" );

    //Apply the message to the screen

    bool cliente_encontrado=false;
    bool quit=false;
    SDL_Event event;
	while(!quit)
	{
        if( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }


        //Apply the background to the screen
        apply_surface( 0, 0, background, screen );
        apply_surface( 320, 0, background, screen );
        apply_surface( 0, 240, background, screen );
        apply_surface( 320, 240, background, screen );

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        //SDL_Delay(17); /*sleep 1/10th of a second */

		/* try to accept a connection */
		client=SDLNet_TCP_Accept(server);
		if(!client)
		{
		     /* no connection accepted */
			/*printf("SDLNet_TCP_Accept: %s\n",SDLNet_GetError()); */

			continue;
		}

		/* get the clients IP and port number */
		remoteip=SDLNet_TCP_GetPeerAddress(client);
		if(!remoteip)
		{
			printf("SDLNet_TCP_GetPeerAddress: %s\n",SDLNet_GetError());
			//continue;
			exit(66);
		}

		/* print out the clients IP and port number */
		ipaddr=SDL_SwapBE32(remoteip->host);
		printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			remoteip->port);

//		/* read the buffer from client */
		len=SDLNet_TCP_Recv(client,message,1024);
		SDLNet_TCP_Close(client);
		if(!len)
		{
			printf("SDLNet_TCP_Recv: %s\n",SDLNet_GetError());
			//continue;
			exit(54);
		}

		/* print out the message */
		printf("Received: %.*s\n",len,message);
		break;
	}

	/* shutdown SDL_net */
	SDLNet_Quit();

    quit=false;
	while(!quit)
	{

        if( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }


        //Apply the background to the screen
        apply_surface( 0, 0, background, screen );
        apply_surface( 320, 0, background, screen );
        apply_surface( 0, 240, background, screen );
        apply_surface( 320, 240, background, screen );

        if(message[0]=='c')
        {
            apply_surface( 180, 140, circulo, screen );
        }
        if(message[0]=='r')
        {
            apply_surface( 180, 140, rectangulo, screen );
        }
        if(message[0]=='t')
        {
            apply_surface( 180, 140, triangulo, screen );
        }

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }
        SDL_Delay(100);
	}

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}
