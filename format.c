#include "format.h"
#include <stdlib.h>

/*
 *undefined behaviour for illegal FEN
 * */
board* FENtoBoard(char* FEN){
	int i=0;
	board* b = calloc(1,sizeof(board));
	tuple pointer = {0,7};
	char c = FEN[0];
	while(c != ' '){
		switch (c){
			case '1': pointer.x+=1;
				  break;
			case '2': pointer.x+=2;
				  break;
			case '3': pointer.x+=3;
				  break;
			case '4': pointer.x+=4;
				  break;
			case '5': pointer.x+=5;
				  break;
			case '6': pointer.x+=6;
				  break;
			case '7': pointer.x+=7;
				  break;
			case '8': pointer.x=0;
				  --pointer.y;
				  break;
			case 'P': b->data[pointer.x][pointer.y]=WPAWN;
				  ++pointer.x;
				  break;
			case 'B': b->data[pointer.x][pointer.y]=WBISHOP;
				  ++pointer.x;
				  break;
			case 'N': b->data[pointer.x][pointer.y]=WKNIGHT;
				  ++pointer.x;
				  break;
			case 'R': b->data[pointer.x][pointer.y]=WROOK;
				  ++pointer.x;
				  break;
			case 'Q': b->data[pointer.x][pointer.y]=WQUEEN;
				  ++pointer.x;
				  break;
			case 'K': b->data[pointer.x][pointer.y]=WKING;
				  ++pointer.x;
				  break;
			case 'p': b->data[pointer.x][pointer.y]=BPAWN;
				  ++pointer.x;
				  break;
			case 'b': b->data[pointer.x][pointer.y]=BBISHOP;
				  ++pointer.x;
				  break;
			case 'n': b->data[pointer.x][pointer.y]=BKNIGHT;
				  ++pointer.x;
				  break;
			case 'r': b->data[pointer.x][pointer.y]=BROOK;
				  ++pointer.x;
				  break;
			case 'q': b->data[pointer.x][pointer.y]=BQUEEN;
				  ++pointer.x;
				  break;
			case 'k': b->data[pointer.x][pointer.y]=BKING;
				  ++pointer.x;
				  break;
			default:  break;
		}
		if(pointer.x>=8){
			pointer.x=0;
			--pointer.y;
		}
		++i;
		c=FEN[i];
	}
	++i;
	if(FEN[i]=='b'){
		b->otherData = b->otherData | 0x10;
	}
	i+=2;
	c=FEN[i];
	while(c!=' '){
		switch (c){
			case 'K': b->otherData = (b->otherData | 0x1);
				  break;
			case 'Q': b->otherData = (b->otherData | 0x2);
				  break;
			case 'k': b->otherData = (b->otherData | 0x4);
				  break;
			case 'q': b->otherData = (b->otherData | 0x8);
			default:  break;
		}
		++i;
		c=FEN[i];
	}
	++i;
	c=FEN[i];
	while(c!=' ' && c!= 0){
		switch (c){
			case '-': break;
			case 'a': b->enpassant.x = 0;
				  break;
			case 'b': b->enpassant.x = 1;
				  break;
			case 'c': b->enpassant.x = 2;
				  break;
			case 'd': b->enpassant.x = 3;
				  break;
			case 'e': b->enpassant.x = 4;
				  break;
			case 'f': b->enpassant.x = 5;
				  break;
			case 'g': b->enpassant.x = 6;
				  break;
			case 'h': b->enpassant.x = 7;
				  break;
			case '3': b->enpassant.y = 3;
				  break;
			case '6': b->enpassant.y = 4;
			default:  break;
		}
		++i;
		c=FEN[i];
	}
	return b;
	//TODO impletment 50 rule
}

