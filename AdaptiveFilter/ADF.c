// =========================================================
// 適応フィルタによるシステム同定プログラムのサンプル
//
// (注) 入力ファイルの量子化ビット数は16 bitとする.
//
// -実行方法-
// gcc ADF.c -o ADF -lm
// ./ADF
// =========================================================
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
#define FILE_IN  "Raw_File/wn_48000_35s.raw"	// 入力信号のrawファイル (フォルダ名/ファイル名)
#define FILE_DES "Raw_File/wn_sn50_35s.raw"		// 所望信号のrawファイル (フォルダ名/ファイル名)
#define FILE_OPT "Raw_File/convTSP.raw"			// 最適フィルタ(インパルス応答)のrawファイル

// 出力ファイル名の定義
#define FILE_RED	  "Results/01_Reduction_sn50lmsSS0.01.dat"		// 誤差減衰量
#define FILE_NWEA	  "Results/02_NWEA_sn50lmsSS0.01.dat"			// 正規化推定誤差
#define FILE_ADF_COEF "Results/03_ADF_coef_sn50lmsSS0.01.dat"		// 適応フィルタの係数
#define FILE_ADF_AMP  "Results/04_ADF_AMP_sn50lmsSS0.01.dat"		// 適応フィルタの振幅特性
#define FILE_ADF_PHS  "Results/04_ADF_PHS_sn50lmsSS0.01.dat"		// 適応フィルタの位相特性

// パラメータの定義
#define FS		 48000.0 	// サンプリング周波数 [Hz]
#define SECOND	 35			// ホワイトノイズの秒数 [s]
#define QB		 16			// 量子化ビット数 [bit]
#define TAP		 8000		// 適応フィルタのタップ長
#define FFT_SIZE 8192		// 適応フィルタの周波数特性算出時のFFT点数

#define STP		 0.01		// ステップサイズ
#define REG		 1.0e-6		// 正則化係数 (NLMS用)


//------------------
// 関数宣言
//------------------
FILE   *open_inputBi_file  ( char *argv );
FILE   *open_outputBi_file ( char *argv );
FILE   *open_outputTe_file ( char *argv );
void    writeRaw  ( short  *destination, int qBits, int sample, FILE **fp );
void    writeTex  ( short  *destination, int SamplingFreqency, int sample, FILE **fp );
void    writeCoef ( double *destination, int tap, FILE **fp );
void    calLPS    ( double *Yr, double *Yi, double *Lps , int Fourier );
void    printAmp  ( double *Lps, int Fourier, FILE **fp );
void    printPhs  ( double *Yr, double *Yi, int Fourier, FILE **fp );

double  conv  ( double *input, double *filter, int current);	// 畳み込み関数
void    shift ( double *input, int current); 					// 時間シフト関数


void main()
{
	//struct stat statbuf;

	//---------------------
	// ファイルポインタの設定
	//---------------------
	FILE *FP_IN,  *FP_DES,  *FP_OPT;
	FILE *FP_RED, *FP_NWEA, *FP_ADF_COEF, *FP_ADF_AMP, *FP_ADF_PHS;

    // バイナリファイルの入力ファイルをオープン
    FP_IN  = open_inputBi_file ( FILE_IN );
    FP_DES = open_inputBi_file ( FILE_DES );
	FP_OPT = open_inputBi_file ( FILE_OPT );

    // テキストファイルの出力ファイルのをオープン
    FP_RED      = open_outputTe_file ( FILE_RED );
    FP_NWEA     = open_outputTe_file ( FILE_NWEA );
    FP_ADF_COEF = open_outputTe_file ( FILE_ADF_COEF );
    FP_ADF_AMP  = open_outputTe_file ( FILE_ADF_AMP );
    FP_ADF_PHS  = open_outputTe_file ( FILE_ADF_PHS );

	// struct stat buf;

	//---------------------
	// 変数宣言 (テンプレート用, 編集厳禁)
	//---------------------
	int		n, i, j, k;
	int		l = 0;// フレームインデックス

	short   input_s = 0, desired_s = 0;
	double	input = 0.0, desired = 0.0;


	//---------------------
	// 変数宣言 (こちらに必要な変数を追加, 定義すること)
	//---------------------
	double *data_x, *data_d, *data_y, *data_e;
	double *AdaptFilter;
	short  *Opt;
	double *OptFilter;
	double *yr, *yi, *lps;
	int     repertNum = FS * SECOND / TAP;
	double *Reduction;
	double  Nwea = 0.0;
	double  SumOfSquare_d = 0.0, SumOfSquare_e = 0.0;
	double  SumOfSquare_o = 0.0, SumOfSquare_diff = 0.0;
	double  Px;

	// 動的メモリ確保
	data_x      = (double *) calloc ( FS * SECOND, sizeof( double ) );
    data_d      = (double *) calloc ( FS * SECOND, sizeof( double ) );
    data_y      = (double *) calloc ( FS * SECOND, sizeof( double ) );
    data_e      = (double *) calloc ( FS * SECOND, sizeof( double ) );
	AdaptFilter = (double *) calloc ( TAP, sizeof( double ) );
	Opt         = (short *)  calloc ( TAP, sizeof( short ) );
	OptFilter   = (double *) calloc ( TAP, sizeof( double ) );
	Reduction   = (double *) calloc ( repertNum, sizeof( double ) );
	yr          = (double *) calloc ( FFT_SIZE, sizeof( double ) );
	yi          = (double *) calloc ( FFT_SIZE, sizeof( double ) );
	lps         = (double *) calloc ( FFT_SIZE, sizeof( double ) );

	//-------------------------
	// 最適フィルタの読み込み
	//-------------------------
	/*
	fread ( Opt, QB / 8, TAP, FP_OPT );
	for ( i = 0; i < TAP; i++ ) {
		OptFilter[i] = (double)( Opt[i] ) / 32768.0;
		SumOfSquare_o += OptFilter[i] * OptFilter[i];
	}
	*/

	//-------------------------
	// 適応フィルタによるシステム同定
	// 音源ファイルから1サンプルずつ読み込む. EOFになった瞬間にwhileから抜ける.
	//-------------------------

	n = 0;
	j = 0;

	while( fread( &input_s,   sizeof( short ), 1, FP_IN  ) != 0 &&
		   fread( &desired_s, sizeof( short ), 1, FP_DES ) != 0 )
	{
		// 信号を実数値に変換
		input   = ( double )( input_s )   / 32768.0;
		desired = ( double )( desired_s ) / 32768.0;

		// 適応フィルタ用の信号シフト
		shift ( data_x, n );
		data_x[0] = input;
		data_d[n] = desired;

		// 適応フィルタと信号の畳み込み
		data_y[n] = conv ( data_x, AdaptFilter, n );

		// 誤差信号の算出
		data_e[n] = data_d[n] - data_y[n];

/*
		// 適応フィルタの更新 (LMS法)
		for ( i = 0; i < TAP; i++ ) {
				AdaptFilter[i] = AdaptFilter[i] + ( STP * data_e[n] * data_x[i] );
			
			if ( n < TAP ) {
				AdaptFilter[i] = AdaptFilter[i] + ( ( STP / ( n + 1 ) ) * data_e[n] * data_x[i] );
			} else {
				AdaptFilter[i] = AdaptFilter[i] + ( ( STP / TAP ) * data_e[n] * data_x[i] );
			}
			
		}
*/

		// 適応フィルタの更新 (NLMS法)
		Px = conv ( data_x, data_x, n );
		for ( i = 0; i < TAP; i++ ) {
				AdaptFilter[i] = AdaptFilter[i] + ( ( STP / ( Px + REG ) ) * data_e[n] * data_x[i] );
		}


		// 適応フィルタの収束特性の計算
		SumOfSquare_e += ( data_e[n] * data_e[n] );
		SumOfSquare_d += ( data_d[n] * data_d[n] );

		if ( ( n % TAP ) == ( TAP - 1 ) ) {
			/*
			for ( i = 0; i < TAP; i++ ) {
				SumOfSquare_diff += pow ( OptFilter[i] - AdaptFilter[i], 2.0 );
			}
			*/
			Reduction[j] = ( -10.0 ) * log10 ( SumOfSquare_e / SumOfSquare_d );
			// Nwea = -10 * log10 ( SumOfSquare_diff / SumOfSquare_o );

			// 変数の初期化
			SumOfSquare_d = 0.0; SumOfSquare_e = 0.0;
			SumOfSquare_diff = 0.0;

			// 回数インデックスの更新
			j++;
		}

		// 時刻インデックスの更新
		n++;
	}

	//------------------------
	// 適応フィルタの係数書き出し
	//------------------------
	writeCoef ( AdaptFilter, TAP, &FP_ADF_COEF );

	//------------------------
	// 適応フィルタの周波数特性の書き出し
	// (注) タップ長が2のべき乗でない場合, ゼロ詰めを行うこと.
	//------------------------
	// 適応フィルタの係数をyrに移動
	for ( i = 0; i < TAP; i++ ) {
		yr[i] = AdaptFilter[i];
	}

	// FFT
	FFT_2N ( FFT, yr, yi, FFT_SIZE );

	// 周波数振幅特性の算出
	calLPS ( yr, yi, lps, FFT_SIZE );

	// 周波数特性の書き出し
	printAmp ( lps, FFT_SIZE, &FP_ADF_AMP);
	printPhs ( yr, yi, FFT_SIZE, &FP_ADF_PHS );

	//------------------------
	// 誤差減衰量の書き出し
	//------------------------
	for ( i = 0; i < repertNum; i++ ) {
		fprintf ( FP_RED,  "%d %lf\n", ( ( i + 1 ) * TAP ) - 1 , Reduction[i] );
		// fprintf ( FP_NWEA, "%d %lf\n", ( ( i + 1 ) * TAP ) - 1, Nwea );
	}

	//------------------------
	// 後処理 (メモリ開放, ファイルのクローズ)
	//------------------------
	fclose( FP_IN ); fclose( FP_DES ); fclose ( FP_OPT );
	fclose( FP_RED ); fclose( FP_NWEA );
	fclose( FP_ADF_COEF );
	fclose( FP_ADF_AMP ); fclose( FP_ADF_PHS );


	//------------------------
	// 後処理 (メモリ開放, ファイルのクローズ)
	// 追加した変数, ファイルポインタに関して, 処理を記述
	//------------------------
	free ( data_x ); free ( data_d ); free ( data_y ); free ( data_e );
	free ( AdaptFilter );
	free ( Opt ); free ( OptFilter ); free ( Reduction );
	free ( yr ); free ( yi ); free ( lps );

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

// フィルタ係数の書き出し
void writeCoef ( double *destination, int tap, FILE **fp ) {
	int i;
	for ( i = 0; i < tap; i++ ) {
		fprintf(*fp, "%d %.7lf\n", i, destination[i] );
	}
	printf("writeCoef success!\n");
}

// 対数パワースペクトルの計算
void calLPS ( double *Yr, double *Yi, double *Lps, int Fourier ) {
	int i;
	for ( i = 0; i < Fourier; i++ ) {
		Lps[i] = 20 * log10 ( sqrt ( pow ( Yr[i], 2.0 ) + pow ( Yi[i], 2.0 ) ) );
	}
}

// 振幅特性の書き出し
void printAmp ( double *Lps, int Fourier, FILE **fp ) {
	int i;
	for (i = 0; i <= Fourier / 2; i++ ) {
		fprintf ( *fp, "%lf %lf\n", (double)i / (Fourier / 2), Lps[i] );
	}
}

// 位相特性の書き出し
void printPhs ( double *Yr, double *Yi, int Fourier, FILE **fp ) {
	int i;
	double buf;
	for ( i = 0; i <= Fourier / 2; i++ ) {
		buf = atan2 ( Yi[i], Yr[i] );
		fprintf( *fp, "%lf %lf\n", (double)i / (Fourier / 2), buf );
	}
}

// 畳み込み関数
double conv ( double *input, double *filter, int current ) {
	int i;
	double sum = 0;

	for ( i = 0; i < TAP; i++) {
		if ( i <= current ) {
			sum += input[i] * filter[i];
		}
	}

	return sum;
}

// 時間シフト関数
void shift ( double *input, int current ) {
	int i;

	for ( i = current; i > 0; i-- ) {
		input[i]  = input[i - 1];
	}
}

// 