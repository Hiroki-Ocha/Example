#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "FFT_media_jikken2_v2.h"

//------------------
// マクロ定義
//------------------

// 入力ファイル名の定義
#define FILE_WN_s   "Raw_File/wn_48000.raw"       // 入力wnのrawファイル (フォルダ名/ファイル名)
#define FILE_WN10_s "Raw_File/wn_sn10_rec.raw"    // 収録wnのrawファイル (フォルダ名/ファイル名)
#define FILE_WN30_s "Raw_File/wn_sn30_rec.raw"    // 収録wnのrawファイル (フォルダ名/ファイル名)
#define FILE_WN50_s "Raw_File/wn_sn50_rec.raw"    // 収録wnのrawファイル (フォルダ名/ファイル名)

// 出力ファイル名の定義
#define FILE_WN_d   "Raw_File/wn_48000_35s.raw"	    // 35sに切ったwn
#define FILE_WN10_d	"Raw_File/wn_sn10_35s.raw"	    // 35sに切ったwn_sn10
#define FILE_WN30_d "Raw_File/wn_sn30_35s.raw"      // 35sに切ったwn_sn30
#define FILE_WN50_d "Raw_File/wn_sn50_35s.raw"	    // 35sに切ったwn_sn50

#define FILE_WN_t   "Text_File/wn_48000_35s.dat"	// 35sに切ったwn
#define FILE_WN10_t	"Text_File/wn_sn10_35s.dat"	    // 35sに切ったwn_sn10
#define FILE_WN30_t "Text_File/wn_sn30_35s.dat"     // 35sに切ったwn_sn30
#define FILE_WN50_t "Text_File/wn_sn50_35s.dat"	    // 35sに切ったwn_sn50

// パラメータの定義
#define sf		48000   // サンプリング周波数 [Hz]
#define INqb    32      // 収録音源は32bit
#define OUTqb	16      // 出力は16bit
#define slength 80      // 入力のwnの時間長は80s
#define dlength 35      // 出力のwnの時間長は80s

//------------------
// 関数宣言
//------------------
FILE   *open_inputBi_file  ( char *argv );
FILE   *open_outputBi_file ( char *argv );
FILE   *open_outputTe_file ( char *argv );
void    writeRaw ( short *destination, int qBits, int sample, FILE **fp );
void    writeTex ( short *destination, int SamplingFreqency, int sample, FILE **fp );
void    IntToShort ( int *source, short *destination, int slen, int dlen );

void main()
{
	//struct stat statbuf;

	//---------------------
	// ファイルポインタの設定
	//---------------------
	FILE *sfp, *dfp, *tfp;

    // ---------------------
    // ファイルの読み込み
    // ---------------------

    // バイナリファイルの入力ファイルをオープン
    sfp = open_inputBi_file  ( FILE_WN50_s );

    // バイナリファイルの出力ファイルをオープン
    dfp = open_outputBi_file ( FILE_WN50_d );

    // テキストファイルの出力ファイルのをオープン
    tfp = open_outputTe_file ( FILE_WN50_t );

	// struct stat buf;

	//---------------------
	// 変数宣言 (テンプレート用, 編集厳禁)
	//---------------------
	int		n, i, j, k;
	int		l = 0;// フレームインデックス

	short	input_s = 0, desired_s = 0;
	double	input = 0.0, desired = 0.0;


	//---------------------
	// 変数宣言 (こちらに必要な変数を追加, 定義すること)
	//---------------------
    int    *s_data;
    short  *d_data;

    //-------------------------
	// メイン処理
	//-------------------------

    // メモリ動的確保
    s_data = (int *)   calloc ( sf * slength, sizeof( int ) );
    d_data = (short *) calloc ( sf * slength, sizeof( short ) );

    // Rawファイル読み込み
    // fread ( d_data , OUTqb / 8 , sf * slength, sfp );
    fread ( s_data , INqb  / 8 , sf * slength, sfp );


    // int から short へ
    IntToShort ( s_data, d_data, sf * slength, sf * dlength );

    //------------------------
	// ファイルの書き出し
	//------------------------
	writeRaw ( d_data , OUTqb, sf * dlength, &dfp );
    writeTex ( d_data, sf, sf * dlength, &tfp );

	//------------------------
	// 後処理 (メモリ開放, ファイルのクローズ)
	//------------------------
	fclose ( sfp ); fclose ( dfp ); fclose ( tfp );

	//------------------------
	// 後処理 (メモリ開放, ファイルのクローズ)
	// 追加した変数, ファイルポインタに関して, 処理を記述
	//------------------------
    free ( s_data ); free ( d_data );

	//------------------------
	// EOF
	//------------------------
}

// -----------------
// 宣言した関数の実装
// -----------------
    // バイナリファイルの入力ファイルをオープン
    FILE* open_inputBi_file ( char *argv ) {
        FILE *fp;
        if ( ( fp = fopen ( argv, "rb" ) ) == NULL ) {
        printf( "bFILE OPEN ERROR\n" );
        exit( -1 );
        }
        return fp;
    }

    // バイナリファイルの出力ファイルをオープン
    FILE* open_outputBi_file ( char *argv ) {
        FILE *fp;
        if ( ( fp = fopen ( argv, "wb" ) ) == NULL ) {
        printf( "bFILE OPEN ERROR\n" );
        exit( -1 );
        }
        return fp;
    }

    // テキストファイルの出力ファイルをオープン
    FILE* open_outputTe_file ( char *argv ) {
        FILE *fp;
        if ( ( fp = fopen ( argv, "w" ) ) == NULL ) {
        printf( "tFILE OPEN ERROR\n" );
        exit( -1 );
        }
        return fp;
    }

    // 出力Rawデータの書き出し
    void writeRaw ( short *destination, int qBits, int sample, FILE **fp ) {
        fwrite ( destination, qBits / 8, sample, *fp);
        printf("writeRaw success!\n");
    }

    // 出力Textファイルの書き出し
    void writeTex ( short *destination, int SamplingFreqency, int sample, FILE **fp ) {
        int i;
        for ( i = 0; i < sample; i++ ) {
            fprintf(*fp, "%.7lf %d\n", (double)i / SamplingFreqency, destination[i]);
        }
        printf("writeTex success!\n");
    }

    // int型からshort型へ変換し, 正規化
    void IntToShort ( int *source, short *destination, int slen, int dlen ) {
    // 変数宣言
        int n;          // カウンタ変数
        int max = 0;    // 正規化のための変数
        double buf = 0;    // 一時変数

    // 処理
        // 最大値を検出
        for ( n = 0; n < slen; n++ ) {
            if ( abs( source[n] ) > max ){
                max = abs( source[n] );
            }
        }
        // 正規化
        for ( n = 0; n < dlen; n++ ) {
            buf = (double)source[n] / (double)max;
            destination[n] = (short)( buf * 32767 );
        }
    }