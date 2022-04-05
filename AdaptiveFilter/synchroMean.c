#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "FFT_media_jikken2_v2.h"

// --------------------- //
//       マクロ処理       //
// --------------------- //
#define sf 48000        // 標本化周波数
#define INqb  32        // 入力ファイルの量子化ビット数
#define OUTqb 16        // 出力ファイルの量子化ビット数
#define N 262144        // フーリエ点数

// 収録したdownTSPのファイル名たち
#define dTSP_FN1  "Raw_File/dTSP01.raw"     // 収録したdTSPの 1つ目のファイル名
#define dTSP_FN2  "Raw_File/dTSP02.raw"     // 収録したdTSPの 2つ目のファイル名
#define dTSP_FN3  "Raw_File/dTSP03.raw"     // 収録したdTSPの 3つ目のファイル名
#define dTSP_FN4  "Raw_File/dTSP04.raw"     // 収録したdTSPの 4つ目のファイル名
#define dTSP_FN5  "Raw_File/dTSP05.raw"     // 収録したdTSPの 5つ目のファイル名
#define dTSP_FN6  "Raw_File/dTSP06.raw"     // 収録したdTSPの 6つ目のファイル名
#define dTSP_FN7  "Raw_File/dTSP07.raw"     // 収録したdTSPの 7つ目のファイル名
#define dTSP_FN8  "Raw_File/dTSP08.raw"     // 収録したdTSPの 8つ目のファイル名
#define dTSP_FN9  "Raw_File/dTSP09.raw"     // 収録したdTSPの 9つ目のファイル名
#define dTSP_FN10 "Raw_File/dTSP10.raw"     // 収録したdTSPの10つ目のファイル名

// 収録したupTSPのファイル名たち
#define uTSP_FN1  "Raw_File/uTSP01.raw"     // 収録したuTSPの 1つ目のファイル名
#define uTSP_FN2  "Raw_File/uTSP02.raw"     // 収録したuTSPの 2つ目のファイル名
#define uTSP_FN3  "Raw_File/uTSP03.raw"     // 収録したuTSPの 3つ目のファイル名
#define uTSP_FN4  "Raw_File/uTSP04.raw"     // 収録したuTSPの 4つ目のファイル名
#define uTSP_FN5  "Raw_File/uTSP05.raw"     // 収録したuTSPの 5つ目のファイル名
#define uTSP_FN6  "Raw_File/uTSP06.raw"     // 収録したuTSPの 6つ目のファイル名
#define uTSP_FN7  "Raw_File/uTSP07.raw"     // 収録したuTSPの 7つ目のファイル名
#define uTSP_FN8  "Raw_File/uTSP08.raw"     // 収録したuTSPの 8つ目のファイル名
#define uTSP_FN9  "Raw_File/uTSP09.raw"     // 収録したuTSPの 9つ目のファイル名
#define uTSP_FN10 "Raw_File/uTSP10.raw"     // 収録したuTSPの10つ目のファイル名

// 同期加算後の出力ファイル名
#define dFileName1 "dTSP_mean.raw"          // 出力バイナリファイル1のファイル名
#define dFileName2 "uTSP_mean.raw"          // 出力バイナリファイル2のファイル名
#define tFileName1 "dTSP_mean.dat"          // 出力テキストファイル1のファイル名
#define tFileName2 "uTSP_mean.dat"          // 出力テキストファイル2のファイル名

// --------------------- //
// 関数のプロトタイプ宣言 //
// --------------------- //
FILE   *open_inputBi_file  ( char *argv );
FILE   *open_outputBi_file ( char *argv );
FILE   *open_outputTe_file ( char *argv );
void    writeRaw ( short *destination, int qBits, int sample, FILE **fp );
void    writeTex ( short *destination, int SamplingFreqency, int sample, FILE **fp );
void    mean10dTSP ( double *destination );
void    mean10uTSP ( double *destination );
void    DoubleToShort ( double *source, short *destination );

// --------------------- //
//       main関数        //
// --------------------- //
int main( int argc, char *argv[] ) {
// ------------------------------------------ //
// ./実行ファイル名
//                        でプログラムを実行する
// ------------------------------------------ //
    // argv[0]に実行ファイル名が入る

// --------------------- //
// ファイルポインタの宣言 //
// --------------------- //
    FILE *dfp1, *dfp2;
    FILE *tfp1, *tfp2;

// --------------------- //
// 変数の宣言            //
// --------------------- //
    double *dMean, *uMean;          // 同期加算後, 平均をとった音声データを格納
    short  *downTSP, *upTSP;        // 音声データを格納

// --------------------- //
// ファイルの読み込み     //
// --------------------- //
    // バイナリファイルの出力ファイルをオープン
    dfp1 = open_outputBi_file ( dFileName1 );
    dfp2 = open_outputBi_file ( dFileName2 );

    // テキストファイルの出力ファイルをオープン
    tfp1 = open_outputTe_file ( tFileName1 );
    tfp2 = open_outputTe_file ( tFileName2 );

// --------------------- //
// メイン処理            //
// --------------------- //
// 準備フェーズ
    // 音声データの領域を確保
    dMean   = (double *) calloc ( N, sizeof( double ) );
    uMean   = (double *) calloc ( N, sizeof( double ) );
    downTSP = (short *) calloc ( N, sizeof( short ) );
    upTSP   = (short *) calloc ( N, sizeof( short ) );

// 生成フェーズ
    mean10dTSP ( dMean );
    mean10uTSP ( uMean );
    DoubleToShort ( dMean, downTSP );
    DoubleToShort ( uMean, upTSP );

// 処理フェーズ
    // 同期加算したdownTSP信号をrawファイルに書き込み(qB:16)
    writeRaw ( downTSP, OUTqb, N, &dfp1 );  // 262144点(約4.1s)分のdownTSP信号をRawファイルへ
    printf("downTSP writeRaw success!\n");

    // 同期加算したdownTSP信号をTextファイルに書き込み
    writeTex ( downTSP, sf, N, &tfp1 );
    printf("downTSP writeTex success!\n");

    // 同期加算したupTSP信号をRawファイルに書き込み(qB:16)
    writeRaw ( upTSP, OUTqb, N, &dfp2 );  // upTSP信号をRawファイルへ
    printf("upTSP writeRaw success!\n");

    // 同期加算したupTSP信号をTextファイルに書き込み
    writeTex ( upTSP, sf, N, &tfp2 );
    printf("upTSP writeTex success!\n");

// --------------------- //
// 後処理                //
// --------------------- //
    // 入力ファイルをクローズ
    // fclose ( sfp );

    // 出力ファイルをクローズ
    fclose ( dfp1 );
    fclose ( dfp2 );
    fclose ( tfp1 );
    fclose ( tfp2 );

    // 出力ファイル用に確保した領域の開放
    free ( dMean );
    free ( uMean );
    free ( downTSP );
    free ( upTSP );

    // end
    return 0;
}

// ----------------- //
// 宣言した関数の実装 //
// ----------------- //
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
    }

    // 出力Textファイルの書き出し
    void writeTex ( short *destination, int SamplingFreqency, int sample, FILE **fp ) {
        int i;
        for ( i = 0; i < sample; i++ ) {
            fprintf(*fp, "%.7lf %d\n", (double)i / SamplingFreqency, destination[i]);
        }
    }

    // 収録した10個のdTSPを加算
    void mean10dTSP ( double *destination ) {
    // --------------------- //
    // ファイルポインタの宣言 //
    // --------------------- //
        FILE *sfp1, *sfp2, *sfp3, *sfp4, *sfp5, *sfp6, *sfp7, *sfp8, *sfp9, *sfp10;

    // --------------------- //
    // 変数の宣言            //
    // --------------------- //
        int   n;                      // カウンタ変数
        int  *data1, *data2, *data3;  // 音声データを格納
        int  *data4, *data5, *data6;  // 音声データを格納
        int  *data7, *data8, *data9;  // 音声データを格納
        int  *data10;

        // --------------------- //
        // ファイルの読み込み     //
        // --------------------- //
        // バイナリファイルの入力ファイルをオープン
        sfp1  = open_inputBi_file ( dTSP_FN1  );
        sfp2  = open_inputBi_file ( dTSP_FN2  );
        sfp3  = open_inputBi_file ( dTSP_FN3  );
        sfp4  = open_inputBi_file ( dTSP_FN4  );
        sfp5  = open_inputBi_file ( dTSP_FN5  );
        sfp6  = open_inputBi_file ( dTSP_FN6  );
        sfp7  = open_inputBi_file ( dTSP_FN7  );
        sfp8  = open_inputBi_file ( dTSP_FN8  );
        sfp9  = open_inputBi_file ( dTSP_FN9  );
        sfp10 = open_inputBi_file ( dTSP_FN10 );

    // --------------------- //
    // メイン処理            //
    // --------------------- //
    // 準備フェーズ
        // 音声データの領域を確保
        data1  = (int *) calloc ( N, sizeof( int ) );
        data2  = (int *) calloc ( N, sizeof( int ) );
        data3  = (int *) calloc ( N, sizeof( int ) );
        data4  = (int *) calloc ( N, sizeof( int ) );
        data5  = (int *) calloc ( N, sizeof( int ) );
        data6  = (int *) calloc ( N, sizeof( int ) );
        data7  = (int *) calloc ( N, sizeof( int ) );
        data8  = (int *) calloc ( N, sizeof( int ) );
        data9  = (int *) calloc ( N, sizeof( int ) );
        data10 = (int *) calloc ( N, sizeof( int ) );

        // 音声データの読み込み
        fread ( data1 , INqb / 8 , N, sfp1  );
        fread ( data2 , INqb / 8 , N, sfp2  );
        fread ( data3 , INqb / 8 , N, sfp3  );
        fread ( data4 , INqb / 8 , N, sfp4  );
        fread ( data5 , INqb / 8 , N, sfp5  );
        fread ( data6 , INqb / 8 , N, sfp6  );
        fread ( data7 , INqb / 8 , N, sfp7  );
        fread ( data8 , INqb / 8 , N, sfp8  );
        fread ( data9 , INqb / 8 , N, sfp9  );
        fread ( data10, INqb / 8 , N, sfp10 );

    // 処理フェーズ
        for ( n = 0; n < N; n++ ) {
            destination[n] = (double)(  data1[n] + data2[n] + data3[n] + data4[n] +
                                        data5[n] + data6[n] + data7[n] + data8[n] +
                                        data9[n] + data10[n] );
            destination[n] = destination[n] / (double)10;
        }

    // --------------------- //
    // 後処理                //
    // --------------------- //
        // 入力ファイルをクローズ
        free ( sfp1  );
        free ( sfp2  );
        free ( sfp3  );
        free ( sfp4  );
        free ( sfp5  );
        free ( sfp6  );
        free ( sfp7  );
        free ( sfp8  );
        free ( sfp9  );
        free ( sfp10 );

        // 確保した領域の開放
        free ( data1  );
        free ( data2  );
        free ( data3  );
        free ( data4  );
        free ( data5  );
        free ( data6  );
        free ( data7  );
        free ( data8  );
        free ( data9  );
        free ( data10 );
    }

    // 収録した10個のuTSPを加算
    void mean10uTSP ( double *destination ) {
    // --------------------- //
    // ファイルポインタの宣言 //
    // --------------------- //
        FILE *sfp1, *sfp2, *sfp3, *sfp4, *sfp5, *sfp6, *sfp7, *sfp8, *sfp9, *sfp10;

    // --------------------- //
    // 変数の宣言            //
    // --------------------- //
        int   n;                      // カウンタ変数
        int  *data1, *data2, *data3;  // 音声データを格納
        int  *data4, *data5, *data6;  // 音声データを格納
        int  *data7, *data8, *data9;  // 音声データを格納
        int  *data10;

        // --------------------- //
        // ファイルの読み込み     //
        // --------------------- //
        // バイナリファイルの入力ファイルをオープン
        sfp1  = open_inputBi_file ( uTSP_FN1  );
        sfp2  = open_inputBi_file ( uTSP_FN2  );
        sfp3  = open_inputBi_file ( uTSP_FN3  );
        sfp4  = open_inputBi_file ( uTSP_FN4  );
        sfp5  = open_inputBi_file ( uTSP_FN5  );
        sfp6  = open_inputBi_file ( uTSP_FN6  );
        sfp7  = open_inputBi_file ( uTSP_FN7  );
        sfp8  = open_inputBi_file ( uTSP_FN8  );
        sfp9  = open_inputBi_file ( uTSP_FN9  );
        sfp10 = open_inputBi_file ( uTSP_FN10 );

    // --------------------- //
    // メイン処理            //
    // --------------------- //
    // 準備フェーズ
        // 音声データの領域を確保
        data1  = (int *) calloc ( N, sizeof( int ) );
        data2  = (int *) calloc ( N, sizeof( int ) );
        data3  = (int *) calloc ( N, sizeof( int ) );
        data4  = (int *) calloc ( N, sizeof( int ) );
        data5  = (int *) calloc ( N, sizeof( int ) );
        data6  = (int *) calloc ( N, sizeof( int ) );
        data7  = (int *) calloc ( N, sizeof( int ) );
        data8  = (int *) calloc ( N, sizeof( int ) );
        data9  = (int *) calloc ( N, sizeof( int ) );
        data10 = (int *) calloc ( N, sizeof( int ) );

        // 音声データの読み込み
        fread ( data1 , INqb / 8 , N, sfp1  );
        fread ( data2 , INqb / 8 , N, sfp2  );
        fread ( data3 , INqb / 8 , N, sfp3  );
        fread ( data4 , INqb / 8 , N, sfp4  );
        fread ( data5 , INqb / 8 , N, sfp5  );
        fread ( data6 , INqb / 8 , N, sfp6  );
        fread ( data7 , INqb / 8 , N, sfp7  );
        fread ( data8 , INqb / 8 , N, sfp8  );
        fread ( data9 , INqb / 8 , N, sfp9  );
        fread ( data10, INqb / 8 , N, sfp10 );

    // 処理フェーズ
        for ( n = 0; n < N; n++ ) {
            destination[n] = (double)(  data1[n] + data2[n] + data3[n] + data4[n] +
                                        data5[n] + data6[n] + data7[n] + data8[n] +
                                        data9[n] + data10[n] );
            destination[n] = destination[n] / 10;
        }

    // --------------------- //
    // 後処理                //
    // --------------------- //
        // 入力ファイルをクローズ
        free ( sfp1  );
        free ( sfp2  );
        free ( sfp3  );
        free ( sfp4  );
        free ( sfp5  );
        free ( sfp6  );
        free ( sfp7  );
        free ( sfp8  );
        free ( sfp9  );
        free ( sfp10 );

        // 確保した領域の開放
        free ( data1  );
        free ( data2  );
        free ( data3  );
        free ( data4  );
        free ( data5  );
        free ( data6  );
        free ( data7  );
        free ( data8  );
        free ( data9  );
        free ( data10 );
    }

    // double型からshort型へ変換し, 正規化
    void DoubleToShort ( double *source, short *destination ) {
    // 変数宣言
        int n;          // カウンタ変数
        double max = 0; // 正規化のための変数
        double buf = 0; // 一時変数

    // 処理
        // 最大値を検出
        for ( n = 0; n < N; n++ ) {
            if ( fabs( source[n] ) > max ){
                max = fabs( source[n] );
            }
        }
        // 正規化
        for ( n = 0; n < N; n++ ) {
            buf = source[n] / max;
            destination[n] = (short)( buf * 32767 );
        }
    }