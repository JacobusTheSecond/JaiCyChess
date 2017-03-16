#ifndef __FORMAT__
#define __FORMAT__

enum {	WHITE	/*0x0*/,
	BLACK	/*0x1*/
};

enum {	EMPTY		/*0x0*/, 
	WPAWN		/*0x1*/, 
	WKNIGHT		/*0x2*/, 
	WBISHOP		/*0x3*/, 
	WROOK		/*0x4*/, 
	WQUEEN		/*0x5*/, 
	WKING		/*0x6*/, 
	PLACEHOLDER1	/*0x7*/,
        PLACEHOLDER2	/*0x8*/,	
	BPAWN		/*0x9*/, 
	BKNIGHT		/*0xA*/, 
	BBISHOP		/*0xB*/, 
	BROOK		/*0xC*/, 
	BQUEEN		/*0xD*/, 
	BKING		/*0xE*/
};

typedef struct tuple{
	short x;
	short y;
}tuple;

typedef struct board{
	char data[8][8];
	tuple enpassant;
	char otherData; // 0x1 = ws, 0x2 = wl, 0x4 = bs, 0x8 = bl, 0x10 = black to Move
} board;

extern board* FENtoBoard(char*);
#endif
