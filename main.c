#include <stdio.h>
#include <stdlib.h>
#include "format.h"
#include <string.h>

/*  A  B  C  D  E  F  G  H -x->
 *1 00 01 02 03 04 05 06 07
 *2 10 11 12 13 14 15 16 17
 *3 20 21 22 23 24 25 26 27
 *4 30 31 32 33 34 35 36 37
 *5 40 41 42 43 44 45 46 47
 *6 50 51 52 53 54 55 56 57
 *7 60 61 62 63 64 65 66 67
 *8 70 71 72 73 74 75 76 77
 *y
 *V
 */


typedef struct pattern{
	tuple King[9];
	tuple Queen[9];
	tuple Rook[5];
	tuple Bishop[5];
	tuple Knight[9];
	tuple attackWPawn[3];
	tuple attackBPawn[3];
} pattern;

pattern patterns = {	/*King*/	{/*rec,size*/{0,9}, {1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}},
			/*Queen*/	{/*rec,size*/{1,9}, {1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}},
			/*Rook*/	{/*rec,size*/{1,5}, {1,0},{0,1},{0,-1},{-1,0}},
			/*Bishop*/	{/*rec,size*/{1,5}, {1,1},{1,-1},{-1,1},{-1,-1}},
			/*Knight*/	{/*rec,size*/{0,9}, {2,1},{2,-1},{1,2},{1,-2},{-1,2},{-1,-2},{-2,1},{-2,-1}},
			/*WPawn*/	{/*rec,size*/{0,3}, {-1,-1},{-1,1}},
			/*BPawn*/	{/*rec,size*/{0,3}, {1,-1},{1,1}}
};

int toMove(board* b){
	return (b->otherData & 0x10)>0;
}

int getColorFromBoard(board* b, short x, short y){
	return b->data[x][y]==EMPTY?-1:((b->data[x][y] & 0x8) > 0);
}

int getColorFromPiece(int piece){
	return piece==EMPTY?-1:((piece & 0x8) > 0);
}

char getPieceFromBoard(board* b, short x, short y){
	return b->data[x][y] & 0x7;
}

char getPieceFromPiece(int piece){
	return piece & 0x7;
}

board* advanceMoveOnCopy(board* b, short startx, short starty, short endx, short endy){
	board* copy = malloc(sizeof(board));
	memcpy(copy,b,sizeof(board));
	copy->data[endx][endy] = copy->data[startx][starty];
	copy->data[startx][starty] = 0x0;
	return copy;
}



int recursiveWalk(board* b, short startx, short starty, short endx, short endy, int color, tuple direction){
	if(!(startx<8 && 0<=startx && starty<8 && starty >= 0)){
		return 0;
	}
	if(getColorFromPiece(b->data[startx][starty])==color){
		return 0;
	}
	if(startx == endx && starty == endy){
		return 1;
	}
	
	if(b->data[startx][starty] == EMPTY){
		return recursiveWalk(b,startx+direction.x,starty+direction.y,endx,endy,color,direction);
	}

}

int walk(board* b, short startx, short starty, short endx, short endy, int color){
	return ( (startx<8 && 0 <= startx && starty<8 && starty>=0) && (startx == endx) && (starty == endy) && (getColorFromBoard(b,startx,starty)!=color));
}

/*
 * */
int reachable(board* b, short startx, short starty, short endx, short endy, int enableRW, int patternsize, tuple* patterns){
	if(!(endx<8 && endy<8 && endx>=0 && endy>=0)){
		return 0;
	}

	if(enableRW){
		for(int i=1;i<patternsize;++i){
			if(recursiveWalk(b,startx+patterns[i].x,starty+patterns[i].y,endx,endy,getColorFromPiece(b->data[startx][starty]),patterns[i])){
				return 1;
			}
		}
		return 0;
	}else{
		for(int i=1;i<patternsize;++i){
			if(walk(b,startx+patterns[i].x,starty+patterns[i].y,endx,endy,getColorFromPiece(b->data[startx][starty]))){
				return 1;
			}
		}
		return 0;
	}
}

void printBoard(board* b){
	printf("   A  B  C  D  E  F  G  H\n");
	printf("   ______________________\n");
	for(int i=7;i>=0;--i){
		printf("%d |",i+1);
		for(int j=0;j<8;++j){
			printf("%02d ",b->data[j][i]);
		}
		printf("\n");
	}
	printf("to Move: %s\n",toMove(b)?"black":"white");
	printf("00/000: ws:%d wl:%d bs:%d bl:%d\n",b->otherData&0x1>0,b->otherData&0x2>0,b->otherData&0x4>0,b->otherData&0x8>0);
}

/*checks wether or not on a certain field a certain piece is positioned
 * */
int collide(board* b, int x, int y, tuple direction, char piece){
	return( (x+direction.x<8 && x+direction.x>=0) && (y+direction.y<8 && y+direction.y>=0)
		&& (b->data[x+direction.x][y+direction.y] == piece));
}

/*checks wether or not in a certain direction a certain piece is positioned
 * */
int recursiveCollide(board* b, int x, int y, tuple direction, char piece){
	if(!(x+direction.x<8 && x+direction.x>=0 && y+direction.y<8 && y+direction.y>=0)){
		return 0;
	}

	if(b->data[x+direction.x][y+direction.y]==piece){
		return 1;
	}

	if(b->data[x+direction.x][y+direction.y]==EMPTY){
		return recursiveCollide(b,x+direction.x,y+direction.y,direction,piece);
	}else{
		return 0;
	}
}

/*checks if a square is attacked by any piece of a certain color
 * possible values for color: 0x1(black), 0x0(white)
 *
 * */
int isAttacked(board* b, short x, short y,char color){
	if(color != 0 && color != 1){
		return 0;
	}
	char colorprefix = color?0x8:0x0;
	/*PAWNS*/

	/*Pawn movement isnt self-invers, so i had to improvise Kappa*/
	/*attacked by black pawn*/
	if(color){
		for(int i=1;i<patterns.attackWPawn[0].y;++i){
			if(collide(b,x,y,patterns.attackWPawn[i],BPAWN)){
				return 1;
			}
		}
	}else{
	/*attacked by white pawn*/
		for(int i=1;i<patterns.attackBPawn[0].y;++i){
			if(collide(b,x,y,patterns.attackBPawn[i],WPAWN)){
				return 1;
			}
		}
	}
	/*KNIGHTS*/
	for(int i=1;i<patterns.Knight[0].y;++i){
		if(collide(b,x,y,patterns.Knight[i],colorprefix|WKNIGHT)){
			return 1;
		}
	}

	/*BISHOP*/
	for(int i=1;i<patterns.Bishop[0].y;++i){
		if(recursiveCollide(b,x,y,patterns.Bishop[i],colorprefix|WBISHOP)){
			return 1;
		}
	}
	/*ROOK*/
	for(int i=1;i<patterns.Rook[0].y;++i){
		if(recursiveCollide(b,x,y,patterns.Rook[i],colorprefix|WROOK)){
			return 1;
		}
	}
	/*QUEEN*/
	for(int i=1;i<patterns.Queen[0].y;++i){
		if(recursiveCollide(b,x,y,patterns.Queen[i],colorprefix|WQUEEN)){
			return 1;
		}
	}
	/*KING*/
	for(int i=1;i<patterns.King[0].y;++i){
		if(collide(b,x,y,patterns.King[i],colorprefix|WKING)){
			return 1;
		}
	}
	return 0;

}

int checkCheck(board* b, int color, int freeAfter){
	//find King
	tuple Kingpos = {-1,-1};
	int colorprefix = 8*color;//only works if color = 1 or color = 0
	for(int i=0;i<8;++i){
		for(int j=0;j<8;++j){
			if(b->data[i][j]==(colorprefix|WKING)){
				Kingpos.x = i;
				Kingpos.y = j;
				break;
			}
		}
		if(Kingpos.x != -1){
			break;
		}
	}
	if(Kingpos.x == -1){
		if(freeAfter){free(b);}
		return 0;
	}
	if(isAttacked(b,Kingpos.x,Kingpos.y,1-color)){
		if(freeAfter){free(b);}
		return 1;	
	}else{
		if(freeAfter){free(b);}
		return 0;
	}
}

//TODO testing
/*returns 	0, if illegal move,
 *		1, if legal move
 *
 *undefined behaviour is expected, if entered board is already in illegal state
 * */
int checkMove(board* b, short startx, short starty, short endx, short endy){
	
	//invalid input
	if(!(0<=startx && 0<= starty && 0<=endx && 0<=endy && startx<8 && starty<8 && endx<8 && endy<8)){return 0;}

	//wrong player
	if(toMove(b) != getColorFromBoard(b,startx,starty)){return 0;}
			
	//check otherwise legal move for correct pattern
	switch (getPieceFromBoard(b,startx,starty)){
		case /*EMPTY*/	0x0:	return 0;

		case /*PAWN*/	0x1:	if(getColorFromBoard(b,startx,starty)==WHITE){
						//WHITE
						
						//move
						if(getPieceFromBoard(b,startx,starty+1)==EMPTY){
							if(walk(b,startx,starty+1,endx,endy,0) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
								return 1;
							}

							//doublemove
							if(starty == 1 && getPieceFromBoard(b,startx,starty+2)==EMPTY){
								if (walk(b,startx,starty+2,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
									return 1;
								}
							}
						}

						//capture
						if(walk(b,startx-1,starty+1,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
							return 1;
						}
						if(walk(b,startx+1,starty+1,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
							return 1;
						}

						//enpassant
						if(b->enpassant.y!=0 && walk(b,startx+1,starty,b->enpassant.x,b->enpassant.y,WHITE) && (endx==b->enpassant.x) && (endy==b->enpassant.y+1)){
							board* enpassantcopy = advanceMoveOnCopy(b,startx,starty,endx,endy);
							enpassantcopy->data[b->enpassant.x][b->enpassant.y] = 0;
							if(!checkCheck(enpassantcopy,WHITE,1)){
								return 1;
							}
						}
						if(b->enpassant.y!=0 && walk(b,startx-1,starty,b->enpassant.x,b->enpassant.y,WHITE) && (endx==b->enpassant.x) && (endy==b->enpassant.y+1)){
							board* enpassantcopy = advanceMoveOnCopy(b,startx,starty,endx,endy);
							enpassantcopy->data[b->enpassant.x][b->enpassant.y] = 0;
							if(!checkCheck(enpassantcopy,WHITE,1)){
								return 1;
							}
						}

						return 0;

					}else if(getColorFromBoard(b,startx,starty)==BLACK){
						//BLACK

						//move
						if(getPieceFromBoard(b,startx,starty-1)==EMPTY){
							if(walk(b,startx,starty-1,endx,endy,0) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
								return 1;
							}

							//doublemove
							if(starty == 6 && getPieceFromBoard(b,startx,starty-2)==EMPTY){
								if (walk(b,startx,starty-2,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
									return 1;
								}
							}
						}

						//capture
						if(walk(b,startx+1,starty-1,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
							return 1;
						}
						if(walk(b,startx-1,starty-1,endx,endy,WHITE) && !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),WHITE,1)){
							return 1;
						}

						//enpassant
						if(b->enpassant.y!=0 && walk(b,startx+1,starty,b->enpassant.x,b->enpassant.y,WHITE) && (endx==b->enpassant.x) && (endy==b->enpassant.y-1)){
							board* enpassantcopy = advanceMoveOnCopy(b,startx,starty,endx,endy);
							enpassantcopy->data[b->enpassant.x][b->enpassant.y] = 0;
							if(!checkCheck(enpassantcopy,WHITE,1)){
								return 1;
							}
						}
						if(b->enpassant.y!=0 && walk(b,startx-1,starty,b->enpassant.x,b->enpassant.y,WHITE) && (endx==b->enpassant.x) && (endy==b->enpassant.y-1)){
							board* enpassantcopy = advanceMoveOnCopy(b,startx,starty,endx,endy);
							enpassantcopy->data[b->enpassant.x][b->enpassant.y] = 0;
							if(!checkCheck(enpassantcopy,WHITE,1)){
								return 1;
							}
						}

						return 0;
					}

		case /*KNIGHT*/ 0x2:	if(reachable(b,startx,starty,endx,endy,patterns.Knight[0].x,patterns.Knight[0].y,patterns.Knight)
						&& !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),getColorFromBoard(b,startx,starty),1)){
						return 1;
					}else{
						return 0;
					}
		case /*BISHOP*/	0x3:	if(reachable(b,startx,starty,endx,endy,patterns.Bishop[0].x,patterns.Bishop[0].y,patterns.Bishop)
						&& !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),getColorFromBoard(b,startx,starty),1)){
						return 1;
					}else{
						return 0;
					}
		case /*ROOK*/	0x4:	if(reachable(b,startx,starty,endx,endy,patterns.Rook[0].x,patterns.Rook[0].y,patterns.Rook)
						&& !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),getColorFromBoard(b,startx,starty),1)){
						return 1;
					}else{
						return 0;
					} 
		case /*QUEEN*/	0x5:	if(reachable(b,startx,starty,endx,endy,patterns.Queen[0].x,patterns.Queen[0].y,patterns.Queen)
						&& !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),getColorFromBoard(b,startx,starty),1)){
						return 1;
					}else{
						return 0;
					}
		case /*KING*/	0x6:	if(reachable(b,startx,starty,endx,endy,patterns.King[0].x,patterns.King[0].y,patterns.King)
						&& !checkCheck(advanceMoveOnCopy(b,startx,starty,endx,endy),getColorFromBoard(b,startx,starty),1)){
						return 1;
					}
					//weiß rochade
					//kurz
					if(b->otherData&0x1>0){
						if(endx == 6 && endy == 0 &&  b->data[5][0]==EMPTY && b->data[6][0]==EMPTY){
							if((!isAttacked(b,4,0,BLACK)) && (!isAttacked(b,5,0,BLACK)) && (!isAttacked(b,6,0,BLACK))){
								return 1;
							}
						}
					}
					//lang
					if(b->otherData&0x2>0){
						if(endx==2 && endy == 0 && b->data[1][0] == EMPTY && b->data[2][0]==EMPTY && b->data[3][0]==EMPTY){
							if(!isAttacked(b,2,0,BLACK) && !isAttacked(b,3,0,BLACK) && !isAttacked(b,4,0,BLACK)){
								return 1;
							}
						}
					}

					//schwarz rochade
					//kurz
					if(b->otherData&0x4>0){
						if(endx == 6 && endy == 7 &&  b->data[5][7]==EMPTY && b->data[6][7]==EMPTY){
							if(!isAttacked(b,4,7,WHITE) && !isAttacked(b,5,7,WHITE) && !isAttacked(b,6,7,WHITE)){
								return 1;
							}
						}
					}
					//lang
					if(b->otherData&0x8>0){
						if(endx==2 && endy == 7 && b->data[1][7] == EMPTY && b->data[2][7]==EMPTY && b->data[3][7]==EMPTY){
							if(!isAttacked(b,2,7,WHITE) && !isAttacked(b,3,7,WHITE) && !isAttacked(b,4,7,WHITE)){
								return 1;
							}
						}
					}
					return 0;
	}
}

void main(){
	printf("0 = voreingestelltes FEN, 1 = eigenes FEN\n");
	char c;
	scanf("%c",&c);
	board* b;
	if(c == '0'){
		b = FENtoBoard("rnbqkbnr/pppppppp/8/8/3b4/8/PPPPPPPP/RNBQK2R w KQkq - ");
	}else{
		printf("FEN eingeben:\n");
		char* FEN = malloc(256*sizeof(char));
		scanf(" %s",FEN);
		b = FENtoBoard(FEN);
	}
	/*board* b = calloc(1,sizeof(board));
	b->data[0][0]=(char)WBISHOP;
	b->data[1][1]=(char)WPAWN;
	b->data[2][2]=(char)BBISHOP;
	b->data[1][4]=(char)BPAWN;
	b->data[0][4]=(char)WPAWN;
	b->enpassant.x = 1;
	b->enpassant.y = 4;*/
	printBoard(b);
	/*printf("a5xb6: %d\n",checkMove(b,0,4,1,5));
	printf("a5a6:  %d\n",checkMove(b,0,4,0,5));
	printf("b2xc3: %d\n",checkMove(b,1,1,2,2));
	printf("a1xc3: %d\n",checkMove(b,0,0,2,2));
	printf("b5xa4: %d\n",checkMove(b,1,4,0,3));*/
	int sx,sy,ex,ey;
	while(c!='q'){
		printf("enter move:\n");
		scanf("%d %d %d %d",&sx,&sy,&ex,&ey);
		printf("move Legal:%d\n",checkMove(b,sx,sy,ex,ey));
		printf("Quit? q=Yes, n=No\n");
		getchar();
		scanf("%c",&c);
	}
}
