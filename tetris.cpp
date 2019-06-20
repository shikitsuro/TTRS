//リファレンス
//https://dxlib.xsrv.jp/dxfunc.html
//
//
////memo
//未実装
//ghost SRS
//gravity
//gameover
////
////////////////////////////////////////////////////////////////
// include,define                                             //
////////////////////////////////////////////////////////////////

//include
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>

//フィールドサイズ(200*400)
#define LEFT_1P  80 
#define RIGHT_1P 240
#define UP_1P    60
#define DOWN_1P  440
#define LEFT_2P  400
#define RIGHT_2P 560
#define UP_2P    60
#define DOWN_2P  440
//ホールドサイズ(40*40)
#define hLEFT_1P  20
#define hRIGHT_1P 60
#define hUP_1P 60
#define hDOWN_1P  100
#define hLEFT_2P  580
#define hRIGHT_2P 620
#define hUP_2P 60
#define hDOWN_2P  100
//ネクストサイズ(40*40)
#define nLEFT_1P  320-60
#define nRIGHT_1P 320-20
#define nUP_1P(a) (60+50*a)
#define nDOWN_1P(a) (100+50*a)
#define nLEFT_2P  320+20
#define nRIGHT_2P 320+60
#define nUP_2P(a) (60+50*a)
#define nDOWN_2P(a) (100+50*a)
//フレーム数
#define WAIT (1000/60)

/////////////////////////////////////////////////////////////////
// グローバル変数                                              //
/////////////////////////////////////////////////////////////////

//フラグ管理
//0 :左右移動の待機時間1P
//1 :ハードドロップ暴発回避
//2
//~:未実装
int flag[100] = { 0 };

//ゲーム関連
struct Player{
	int x;
	int y;
	int rota;//向き//B->+ A->-
	int now;//現在のミノ//XSZLJIOT//nowが8のとき進める
	int hold;
	int next[5];
	int nexttable[7];//7回で更新
	int tablepoint;
	int prehold;//use hold yet?
	int prerota; //use rotation yet?
	int premove;//use move yet?
}typedef Player;
struct Field {
	int field_1[50][12];//バイナリ
	int field_2[50][12];//color格納
	int hold[4][4];
	int next[5][4][4];
}typedef Field;
struct Img {
	int back;
	int field;
	int mino;
	int unit[8];
	int ghost[8];
}typedef Img;

Player p1;
Field f1;
Img img;
int object[9][4][4] = {
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},{//s
		{0,0,0,0},
		{0,0,0,0},
		{1,1,0,0},
		{0,1,1,0}
	},{//z
		{0,0,0,0},
		{0,0,0,0},
		{0,2,2,0},
		{2,2,0,0}
	},{//l
		{0,0,0,0},
		{0,0,0,0},
		{3,3,3,0},
		{0,0,3,0}
	},{//j
		{0,0,0,0},
		{0,0,0,0},
		{4,4,4,0},
		{4,0,0,0}
	},{//i
		{0,0,0,0},
		{0,0,0,0},
		{5,5,5,5},
		{0,0,0,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{0,6,6,0},
		{0,6,6,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{7,7,7,0},
		{0,7,0,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	}
};
int object_bin[9][4][4] = {
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},{//s
		{0,0,0,0},
		{0,0,0,0},
		{1,1,0,0},
		{0,1,1,0}
	},{//z
		{0,0,0,0},
		{0,0,0,0},
		{0,1,1,0},
		{1,1,0,0}
	},{//l
		{0,0,0,0},
		{0,0,0,0},
		{1,1,1,0},
		{0,0,1,0}
	},{//j
		{0,0,0,0},
		{0,0,0,0},
		{1,1,1,0},
		{1,0,0,0}
	},{//i
		{0,0,0,0},
		{0,0,0,0},
		{1,1,1,1},
		{0,0,0,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{0,1,1,0},
		{0,1,1,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{1,1,1,0},
		{0,1,0,0}
	},{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	}
};


/////////////////////////////////////////////////////////////////
// プロトタイプ宣言                                            //
/////////////////////////////////////////////////////////////////
void Init(Player *p0,Field *f0);
void LoadData();
void DrawBack();
void DrawField();
void DrawMino();
void Drowsample();

void makenext(Player *p0);
void usenext(Player *p0);
void nextcontact(Player *p0, Field *f0);
void operate(Player *p0, Field *f0);
void SRS(Player *p0, Field *f0);
int rotation(int now ,int rota,int a[4][4],int i,int j);
int able(Player* p0, Field* f0);
void fieldcontact(Player *p0, Field *f0);
void clear(Player* p0, Field* f0);
void drop(Player* p0, Field* f0);
void lineclear(Player* p0, Field* f0);


/////////////////////////////////////////////////////////////////
// メイン関数                                                  //
/////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	SetMainWindowText("Game Fighters");// タイトルを 変更
	ChangeWindowMode(TRUE);// ウインドウモードに変更
	if (DxLib_Init() == -1)return -1;// エラーが起きたら直ちに終了
	SetDrawScreen(DX_SCREEN_BACK);// 描画先画面を裏にする(ちらつき防止)
	LoadData();//表示データの読み込み
	Init(&p1,&f1);//初期設定

	// ゲーム開始
	while (1) {
		if (ProcessMessage() == -1) { DxLib_End(); return 0; }

		
		makenext(&p1);//minotable sakusei
		usenext(&p1);//mino,now kousin
		nextcontact(&p1, &f1);
		drop(&p1, &f1);
		clear(&p1, &f1);
		operate(&p1, &f1);
		SRS(&p1, &f1);	
		fieldcontact(&p1,&f1);
		lineclear(&p1, &f1);

		DrawBack();//背景
		DrawField();//フィールド
		DrawMino();//ミノ
		//Drowsample();
		ScreenFlip();//画面を裏と交代(描画の反映)
		WaitTimer(WAIT);
	}
	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;

}
/////////////////////////////////////////////////////////////////
// プロトタイプ宣言                                            //
/////////////////////////////////////////////////////////////////

void Init(Player *p0 , Field *f0){
	//player
	p0->x=5;
	p0->y=20;
	p0->rota=0;
	p0->now=0;//無が入ってる
	p0->hold=0;
	p0->prehold = 0;
	p0->prerota = 0;
	p0->premove = 0;
	for (int i = 0; i < 5; i++) {
		p0->next[i] = 0;
	}
	p0->tablepoint=7;
	//field
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 5; k++) {
				f0->next[k][i][j] = 0;
			}
			f0->hold[i][j] = 0;
		}
	}
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 12; j++) {
			if (j == 11 || j == 0 || i == 0)f0->field_1[i][j] = 1;
			else f0->field_1[i][j] = 0;
			f0->field_2[i][j] = 0;
		}
	}
}
void LoadData() {
	//画像読み込み
	img.back = LoadGraph("image/back_0.png");
	img.field = LoadGraph("image/field_1.png");
	img.mino = LoadGraph("image/object_1.png");
	img.unit[0] = DerivationGraph(0, 0, 10, 10, img.field);
	for (int i = 0; i < 7; i++) {
		img.unit[i+1] = DerivationGraph( 0, 10*i, 10, 10, img.mino);
	}

}
void DrawBack() {
	DrawExtendGraph(0, 0, 640, 480, img.back, false);
}
void DrawField() {
	DrawExtendGraph(LEFT_1P, UP_1P, RIGHT_1P, DOWN_1P, img.field, false);
	DrawExtendGraph(LEFT_2P, UP_2P, RIGHT_2P, DOWN_2P, img.field, false);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DrawRectGraph(hLEFT_1P + 10 * i, hUP_1P + 10 * j, 0, 70, 10, 10, img.mino, false, false);
			DrawRectGraph(hLEFT_2P + 10 * i, hUP_2P + 10 * j, 0, 70, 10, 10, img.mino, false, false);
		}
	}
	for (int k = 0; k < 5; k++) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				DrawRectGraph(nLEFT_1P + 10 * i, nUP_1P(k) + 10 * j, 0, 70, 10, 10, img.mino, false, false);
				DrawRectGraph(nLEFT_2P + 10 * i, nUP_2P(k) + 10 * j, 0, 70, 10, 10, img.mino, false, false);
			}
		}
	}
}
void DrawMino() {
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 10; j++) {
			DrawExtendGraph(LEFT_1P + j*(RIGHT_1P - LEFT_1P)/10,  DOWN_1P - (i+1)*(DOWN_1P - UP_1P)/21, LEFT_1P + (j+1)*(RIGHT_1P - LEFT_1P)/10, DOWN_1P - i*(DOWN_1P - UP_1P)/21, img.unit[f1.field_2[i+1][j+1]], false);
		}
	}
	//hold view
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DrawExtendGraph(hLEFT_1P + 10 * j, (hUP_1P +40) - 10 * i, hLEFT_1P + 10 * (j+1), (hUP_1P + 40) - 10 * (i+1), img.unit[f1.hold[i][j]], false);
		}
	}
	//next view
	for (int k = 0; k < 5; k++) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				DrawExtendGraph(nLEFT_1P + 10 * j, (nUP_1P(k) +40) - 10 * i, nLEFT_1P + 10 * (j+1), (nUP_1P(k) + 40) - 10 * (i+1), img.unit[f1.next[k][i][j]], false);
			}
		}
	}
}
/*void Drowsample() {
	//for (int i = 0; i < 8; i++)DrawExtendGraph(0+10*i, 0, 10+10*i, 10, img.unit[i], false);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DrawExtendGraph(hLEFT_1P + 10 * j, (hUP_1P +40) - 10 * i, hLEFT_1P + 10 * (j+1), (hUP_1P + 40) - 10 * (i+1), img.unit[f1.hold[i][j]], false);
		}
	}
}/**/

void makenext(Player *p0) {
	int swap, rand;
	if (p0->tablepoint == 7) {
		for (int i = 0; i < 7; i++) {
			p0->nexttable[i] = i+1;
		}for (int i = 0; i < 7; i++) {
			rand = GetRand(6);
			swap = p0->nexttable[i];
			p0->nexttable[i] = p0->nexttable[rand];
			p0->nexttable[rand] = swap;
		}
		p0->tablepoint = 0;
	}
}
void usenext(Player *p0) {
	if (p0->now == 0) {
		p0->now = p0->next[0];
		p0->next[0] = p0->next[1];
		p0->next[1] = p0->next[2];
		p0->next[2] = p0->next[3];
		p0->next[3] = p0->next[4];
		p0->next[4] = p0->nexttable[p0->tablepoint];
		p0->tablepoint++;
		p0->x = 5;
		p0->y = 20;
		p0->rota = 0;
	}
}
void nextcontact(Player *p0, Field *f0) {
	for (int k = 0; k < 5; k++) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				f0->next[k][i][j] = object[(p0->next[k])][i][j];
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			f0->hold[i][j] = object[p0->hold][i][j];
		}
	}
}
void operate(Player *p0, Field *f0){
	if (flag[0] <= 1) {
		if (CheckHitKey(KEY_INPUT_UP)) {
			if (flag[1] == 0) {
				for (; able(p0, f0) == 0; p0->y--);
				p0->y++;
				p0->premove = 4;	
			}else{
			}flag[1] = 1;
		}
		else if (CheckHitKey(KEY_INPUT_LSHIFT)) {
			if (p0->prehold == 0) {
				p0->hold ^= p0->now ;
				p0->now  ^= p0->hold;
				p0->hold ^= p0->now ;
				p0->x = 5;
				p0->y = 20;
				p0->rota = 0;
				p0->premove = 0;
				p0->prehold = 1;
			}
			else {}
		}
		else {
			flag[1] = 0;
			if (CheckHitKey(KEY_INPUT_DOWN)) {
				p0->y -= 1;
				p0->premove = 1;
				if (flag[0] == 0)flag[0] = 10;
				else flag[0] = 2;
			}
			else if (CheckHitKey(KEY_INPUT_LEFT)) {
				p0->x -= 1;
				p0->premove = 2;
				if (flag[0] == 0)flag[0] = 10;
				else flag[0] = 2;
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT)) {
				p0->x += 1;
				p0->premove = 3;
				if (flag[0] == 0)flag[0] = 10;
				else flag[0] = 2;
			}
			else {
				flag[0] = 0;
				p0->premove = 0;
			}
		}
	}
	else {
		flag[0]--;
		p0->premove = 0;
	}
	//移動処理
	if (able(p0, f0) == 1) {
		if (p0->premove == 1) {
			p0->y++;
		}
		else if (p0->premove == 2) {
			p0->x++;
		}
		else if (p0->premove == 3) {
			p0->x--;
		}
	}

	if (p0->prerota == 0) {
		if (CheckHitKey(KEY_INPUT_Z)) {
			p0->rota = (p0->rota + 1) % 4;
			p0->prerota = 1;
		}
		else if (CheckHitKey(KEY_INPUT_X)) {
			p0->rota = (p0->rota + 3) % 4;
			p0->prerota = 2;
		}
	}else{
		if (!CheckHitKey(KEY_INPUT_Z) && !CheckHitKey(KEY_INPUT_X)) {
			p0->prerota = 0;
		}
		else {
			p0->prerota = 3;
		}
	}
	if (p0->prerota == 0) {
		if (CheckHitKey(KEY_INPUT_Z)) {
			p0->rota = (p0->rota + 1) % 4;
			p0->prerota = 1;
		}
		else if (CheckHitKey(KEY_INPUT_X)) {
			p0->rota = (p0->rota + 3) % 4;
			p0->prerota = 2;
		}
	}
}
void SRS(Player *p0, Field *f0){
	int a=0;//不可能動作
	int base;//回転前状態
	int bX=p0->x, bY=p0->y;
	
	/*a = able(p0,f0);
	if (a == 1) {
		if(p0->premove == 1){
			p0->y++;
		}
		else if (p0->premove == 2) {
			p0->x++;
		}
		else if (p0->premove == 3) {
			p0->x--;
		}
	}*/
	//SRS処理//
	a = able(p0, f0);
	if (a == 1) {
		if (p0->prerota == 1)base = (p0->rota+3)%4;
		else if (p0->prerota == 2)base = (p0->rota+1)%4;
		else { base = p0->rota; }

		if (p0->now == 6) {//O
			//回転なし
		}
		else if(p0->now == 5){//I
			p0->rota = base;
		}
		else{//SZLJT
			for (int S = 1;S < 6; S++) {
				if      (S == 1) {
					if(p0->prerota == 1){
						if (base==1||base==2) {
							p0->x--;
						}
						else if (base==0||base==3) {
							p0->x++;
						}
					}
					else if (p0->prerota == 2) {
						if (base == 1 || base == 0) {
							p0->x--;
						}
						else if (base == 2 || base == 3) {
							p0->x++;
						}
					}
					if (able(p0, f0) == 0)break;
				}
				else if (S == 2) {					
					if (base == 0 || base == 2) {
						p0->y++;
					}
					else if (base == 1 || base == 3) {
						p0->y--;
					}
					if (able(p0, f0) == 0)break;
				}
				else if (S == 3) {
					p0->x=bX;p0->y=bY;
					if (base == 0 || base == 2) {
						p0->y-=2;
					}
					else if (base == 1 || base == 3) {
						p0->y+=2;
					}
					if (able(p0, f0) == 0)break;
				}
				else if (S == 4) {
					if (p0->prerota == 1) {
						if (base == 1 || base == 2) {
							p0->x--;
						}
						else if (base == 0 || base == 3) {
							p0->x++;
						}
					}
					else if (p0->prerota == 2) {
						if (base == 1 || base == 0) {
							p0->x--;
						}
						else if (base == 2 || base == 3) {
							p0->x++;
						}
					}
					if (able(p0, f0) == 0)break;
				}
				else if (S == 5){
					p0->x = bX;
					p0->y = bY;
					p0->rota = base;
					break;
				}
			}
		}
	}
}
int rotation(int now,int rota,int a[4][4],int i,int j){
	int b[4][4];

	if (now == 6){//O
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				b[y][x] = a[y][x];
			}
		}
	}
	else if (now == 5) {//I
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				if (rota == 0)b[y][x] = a[y][x];
				if (rota == 1)b[x][3-y] = a[y][x];
				if (rota == 2)b[3-y][3-x] = a[y][x];
				if (rota == 3)b[3-x][y] = a[y][x];
			}
		}
	}
	else {//szljt
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				if (x == 3 || y == 3)b[3-y][x] = 0;
				else if (rota == 0)b[y+1][x] = a[y+1][x];
				else if (rota == 1)b[x+1][2-y] = a[y+1][x];
				else if (rota == 2)b[2-y+1][2-x] = a[y+1][x];
				else if (rota == 3)b[2-x+1][y] = a[y+1][x];
				
			}
		}
	}
	return b[i][j];
}
int able(Player* p0, Field* f0) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ((p0->x - 1 + j < 0) || (p0->x - 1 + j > 11) || (p0->y - 2 + i < 0)) {
			}
			else {
				if (f0->field_1[p0->y - 2 + i][p0->x - 1 + j] == 1 && rotation(p0->now, p0->rota, object_bin[p0->now], i, j) == 1) {
					return 1;
				}
			}
		}
	}
	return 0;
}
void fieldcontact(Player *p0, Field *f0){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if((p0->x -1+j <0) || (p0->x -1+j >11) || (p0->y -2+i <0) || (p0->y -2+i >50)){
			}
			else{
				if (f0->field_1[p0->y - 2 + i][p0->x - 1 + j] == 1) {

				}
				else {
					f0->field_1[p0->y - 2 + i][p0->x - 1 + j] = rotation(p0->now, p0->rota, object_bin[p0->now], i, j);//[関数の中で別の関数を使う]悪い使い方
					f0->field_2[p0->y - 2 + i][p0->x - 1 + j] = rotation(p0->now, p0->rota, object[p0->now], i, j);
				}
			}
		}
	}
}
void clear(Player* p0, Field* f0) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ((p0->x - 1 + j < 0) || (p0->x - 1 + j > 10) || (p0->y - 2 + i < 0) || (p0->y - 2 + i > 50)) {
			}
			else {
				if (rotation(p0->now, p0->rota, object_bin[p0->now], i, j)){
					f0->field_1[p0->y - 2 + i][p0->x - 1 + j] = 0;
					f0->field_2[p0->y - 2 + i][p0->x - 1 + j] = 0;
				}
			}
		}
	}
}
void drop(Player* p0, Field* f0){
	if (p0->premove == 4) {
		p0->now = 0;
		p0->prehold = 0;
		p0->premove = 0;
	}
}
void lineclear(Player* p0, Field* f0) {
	if (p0->premove == 4) {
		int count;
		for (int i = 1; i < 50; i++) {
			count = 0;
			for (int j = 1; j < 11; j++) {
				if (f0->field_1[i][j] == 1)count++;
			}
			if (count == 10) {
				for (int j = 1; j < 11; j++) {
					f0->field_1[i][j] = 0;
					f0->field_2[i][j] = 0;
				}
				for (int y = i; y < 50; y++) {
					for (int x = 1; x < 11; x++) {
						if (y == 49)f0->field_1[y][x] = 0;
						else        f0->field_1[y][x] = f0->field_1[y + 1][x];

						if (y == 49)f0->field_2[y][x] = 0;
						else        f0->field_2[y][x] = f0->field_2[y + 1][x];
					}
				}
				i--;
			}
		}
	}
}