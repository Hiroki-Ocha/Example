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
#define sf 48000            // 標本化周波数
#define qb 16               // 量子化ビット数
#define N  262144           // フーリエ点数

#define sFileName1 "Raw_File/dTSP_mean.raw"  // 入力1のバイナリファイル名
#define sFileName2 "Raw_File/upTSP.raw"      // 入力2のバイナリファイル名
#define dFileName1 "convTSP.raw"             // 出力1のバイナリファイル名
#define tFileName1 "nConvTSP.dat"            // 出力1のテキストファイル名
#define tFileName2 "nConvLPS.dat"            // 出力2のテキストファイル名

// --------------------- //
// 関数のプロトタイプ宣言 //
// --------------------- //
FILE  *open_inputBi_file  ( char *argv );
FILE  *open_outputBi_file ( char *argv );
FILE  *open_outputTe_file ( char *argv );
void   writeRaw ( short *destination, int qBits, int sample, FILE **fp );
void   writeTex ( short *destination, int SamplingFreqency, int sample, FILE **fp );
void   writeTexDouble ( double *destination, int SamplingFreqency, int sample, FILE **fp );
short *convolution ( short *source1, short *source2, int Fourier );
void   calLPS ( double *Yr, double *Yi, double *Lps , int Fourier );
void   printLps ( double *Lps, int Fourier, FILE **fp );


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
    FILE *sfp1, *sfp2;
    FILE *dfp1;
    FILE *tfp1, *tfp2;

// --------------------- //
// 変数の宣言            //
// --------------------- //
    int     n;                      // カウンタ変数
    struct  stat statBuf;           // stat関数の利用のための変数
    int     fileSize;               // 音声データのファイルサイズを格納
    int     Sample;                 // 音声データのサンプル数を格納
    short  *rdTSP, *uTSP;           // 音声データを格納
    short  *convData;               // 畳み込んだ音声データを格納
    double *nConv;                  // 畳み込んだ音声データを最大値1で正規化したものを格納
    double *yr, *yi;                // 複素数を格納
    double *lps;                    // 対数パワースペクトルを格納

// --------------------- //
// ファイルの読み込み     //
// --------------------- //
    // バイナリファイルの入力ファイルをオープン
    sfp1 = open_inputBi_file ( sFileName1 );
    sfp2 = open_inputBi_file ( sFileName2 );

    // バイナリファイルの出力ファイルをオープン
    dfp1 = open_outputBi_file ( dFileName1 );

    // テキストファイルの出力ファイルをオープン
    tfp1 = open_outputTe_file ( tFileName1 );
    tfp2 = open_outputTe_file ( tFileName2 );

// --------------------- //
// メイン処理            //
// --------------------- //
// 準備フェーズ
    // 音声データの領域を確保
    rdTSP    = (short *)  calloc ( N, sizeof( short ) );
    uTSP     = (short *)  calloc ( N, sizeof( short ) );
    nConv    = (double *) calloc ( N, sizeof( double ) );


    // 複素数, 対数パワースペクトルの領域を確保
    yr  = (double *) calloc ( N, sizeof( double ) );
    yi  = (double *) calloc ( N, sizeof( double ) );
    lps = (double *) calloc ( N, sizeof( double ) );

// 生成フェーズ
    fread ( rdTSP, qb / 8, N, sfp1 );
    fread ( uTSP,  qb / 8, N, sfp2 );
    convData = convolution ( rdTSP, uTSP, N );
    for ( n = 0; n < N; n++ ) {
        nConv[n] = (double)( convData[n] ) / 32768.0;
    }
/*
    // 畳み込み実行(時間領域)(周波数領域での掛け算ではうまく行かなかったため)
    convData = filtering ( N, upTSP, &conv );
*/

// 処理フェーズ
    // 畳み込んだ信号の対数パワースペクトルを算出
    for (n = 0; n < N; n++ ) {
        yr[n] = nConv[n];
    }

    FFT_2N ( FFT, yr, yi, N );
    calLPS ( yr, yi, lps, N );

    // 畳み込んだ信号をrawファイルに書き込み
    writeRaw ( convData, qb, N, &dfp1 );
    printf("convSignal writeRaw success!\n");

    // 畳み込んだ信号をTextファイルに書き込み
    writeTexDouble ( nConv, sf, N, &tfp1 );
    printf("convSignal writeTexDouble success!\n");

    // 畳み込んだ信号の対数パワースペクトルをTextファイルに書き込み
    printLps ( lps, N, &tfp2 );
    printf("convLPS writeTex success!\n");

// --------------------- //
// 後処理                //
// --------------------- //
    //入力ファイルをクローズ
    fclose ( sfp1 );
    fclose ( sfp2 );

    // 出力ファイルをクローズ
    fclose ( dfp1 );
    fclose ( tfp1 );
    fclose ( tfp2 );

    // 出力ファイル用に確保した領域の開放
    free ( rdTSP );
    free ( uTSP );
    free ( convData );
    free ( nConv );
    free ( yr );
    free ( yi );
    free ( lps );

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
            fprintf(*fp, "%lf %d\n", (double)i / SamplingFreqency, destination[i]);
        }
    }
    // 出力Textファイルの書き出し
    void   writeTexDouble ( double *destination, int SamplingFreqency, int sample, FILE **fp ) {
        int i;
        for ( i = 0; i < sample; i++ ) {
            fprintf(*fp, "%d %lf\n", i, destination[i]);
        }
    }

    // 2つのraw形式の音データを畳み込んで出力信号を返す
    short *convolution ( short *source1, short *source2, int Fourier ) {
    // 変数宣言
        int     n, k;
        short   buf = 0;
        short  *destination;
        double *Yr1, *Yr2;
        double *Yi1, *Yi2;
        double *cYr, *cYi;
        double  max = 0;

    // 領域確保
        destination = (short *) calloc ( Fourier, sizeof( short ) );
        Yr1 = (double *) calloc ( Fourier, sizeof( double ) );
        Yr2 = (double *) calloc ( Fourier, sizeof( double ) );
        Yi1 = (double *) calloc ( Fourier, sizeof( double ) );
        Yi2 = (double *) calloc ( Fourier, sizeof( double ) );
        cYr = (double *) calloc ( Fourier, sizeof( double ) );
        cYi = (double *) calloc ( Fourier, sizeof( double ) );

    // 2つの音源をそれぞれFFT
        for ( n = 0; n < Fourier; n++ ) {
            Yr1[n] = (double)source1[n];
            Yr2[n] = (double)source2[n];
        }

        FFT_2N ( FFT, Yr1, Yi1, Fourier );
        FFT_2N ( FFT, Yr2, Yi2, Fourier );

    // 2つの音源を周波数領域で掛け算
        for ( k = 0; k < Fourier; k++ ) {
            cYr[k] = Yr1[k] * Yr2[k] - Yi1[k] * Yi2[k];
            cYi[k] = Yr1[k] * Yi2[k] + Yr2[k] * Yi1[k];
        }

    // 周波数領域で掛け算したものをIFFT
        FFT_2N ( IFFT, cYr, cYi, Fourier );

    // double型からshort型へ変換し, 正規化
        for ( n = 0; n < Fourier; n++ ) {
            if ( cYr[n] > max ) {
                max = cYr[n];
            }
        }
        for ( n = 0; n < Fourier; n++ ) {
            destination[n] = (short)( 32766 * ( cYr[n] / max ) );
        }

    // 領域開放
        free ( Yr1 );
        free ( Yr2 );
        free ( Yi1 );
        free ( Yi2 );
        free ( cYr );
        free ( cYi );

    // 終了
        return destination;
    }

    // 対数パワースペクトルの計算
    void calLPS ( double *Yr, double *Yi, double *Lps, int Fourier ) {
        int i;
        for ( i = 0; i < Fourier; i++ ) {
            Lps[i] = 20 * log10 ( sqrt ( pow ( Yr[i], 2.0 ) + pow ( Yi[i], 2.0 ) ) );
        }
    }

    // 振幅特性の書き出し
    void printLps ( double *Lps, int Fourier, FILE **fp ) {
        int i;
        for (i = 0; i < Fourier / 2; i++ ) {
            fprintf(*fp, "%lf %lf\n", (double)i / (Fourier / 2), Lps[i]);
        }
        fprintf(*fp, "%lf %lf\n", (double)i / (Fourier / 2), Lps[i]);
    }

    //