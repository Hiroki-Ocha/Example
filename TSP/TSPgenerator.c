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
#define samplingFrequency 48000     // 標本化周波数
#define quantizationBits 16         // 量子化ビット数
#define N 262144                    // フーリエ点数

#define dFileName1 "downTSP.raw"            // 出力バイナリファイル1のファイル名
#define dFileName2 "upTSP.raw"              // 出力バイナリファイル2のファイル名
#define dFileName3 "conv.raw"               // 出力バイナリファイル3のファイル名
#define tFileName1 "donnTSP.dat"            // 出力テキストファイル1のファイル名
#define tFileName2 "upTSP.dat"              // 出力テキストファイル2のファイル名

// --------------------- //
// 関数のプロトタイプ宣言 //
// --------------------- //
FILE  *open_inputBi_file  ( char *argv );
FILE  *open_outputBi_file ( char *argv );
FILE  *open_outputTe_file ( char *argv );
short *makeDownTSP ( void );
short *makeUpTSP ( void );
void   writeRaw ( short *destination, int qBits, int sample, FILE **fp );
void   writeTex ( short *destination, int sample, FILE **fp );

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
    FILE *sfp;
    FILE *dfp1, *dfp2;
    FILE *tfp1, *tfp2;

// --------------------- //
// 変数の宣言            //
// --------------------- //
    int     i, n, k;                // カウンタ変数
    int     max = 0;                // 正規化のための変数
    double  m, a;                   // TSP信号のパラメータのための変数
    int     cs;                     // 円状シフトのための変数
    int     Sample;                 // 音声データのサンプル数を格納
    short  *downTSP, *upTSP;        // 音声データを格納
    short  *convData;               // 畳み込んだ音声データを格納
    double  buff = 0;               // 畳み込みのための変数

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

// 生成フェーズ
    downTSP = makeDownTSP ( );
    upTSP   = makeUpTSP   ( );

    // downTSP信号をrawファイルに書き込み
    writeRaw ( downTSP, quantizationBits, N, &dfp1 );  // 262144点(約4.1s)分のdownTSP信号をRawファイルへ
    printf("downTSP writeRaw success!\n");

    // downTSP信号をTextファイルに書き込み
    writeTex ( downTSP, N, &tfp1 );
    printf("downTSP writeTex success!\n");

    // upTSP信号をRawファイルに書き込み
    writeRaw ( upTSP, quantizationBits, N, &dfp2 );  // upTSP信号をRawファイルへ
    printf("upTSP writeRaw success!\n");

    // upTSP信号をTextファイルに書き込み
    writeTex ( upTSP, N, &tfp2 );
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

    // downTSPを生成し, Rawファイル, Textファイルに書き込み
    short *makeDownTSP ( void ) {
        // 変数宣言
        int     i, n, k;                // カウンタ変数
        int     max = 0;                // 正規化のための変数
        double  m, a;                   // TSP信号のパラメータのための変数
        int     cs;                     // 円状シフトのための変数
        int     Sample;                 // 音声データのサンプル数を格納
        short  *data;                   // 音声データを格納
        short  *buf;                    // 円状シフトのための退避用データを格納
        double *yr;                     // 時間波形, スペクトルの実部を格納
        double *yi;                     // 時間波形, スペクトルの虚部を格納

        // メイン処理
    // 準備フェーズ
        // 音声データの領域を確保
        data = (short *) calloc (N, sizeof( short ) );

        // TSP用のパラメータを設定
        m  = N / 4;
        a  = 4 * m * M_PI;

        // 円状シフト用のパラメータと退避用の配列の領域を確保
        cs  = (int)m;
        buf = (short *) calloc ( cs, sizeof( short ) );

        // 周波数領域のための領域確保
        yr = (double *) calloc ( N, sizeof( double ) );
        yi = (double *) calloc ( N, sizeof( double ) );

    // 生成フェーズ
        // 周波数領域でのdownTSP信号を生成(実部yr, 虚部yiで複素指数関数を表現)
        for ( k = 0; k <= N / 2; k++ ) {
            yr[k] = 1000000 * cos ( a * pow ( (double)k / N, 2.0 ) );
            yi[k] = 1000000 * sin ( a * pow ( (double)k / N, 2.0 ) );
        }
        for ( k = ( N / 2 ) + 1; k < N; k++ ) {
            yr[k] = 1000000 * cos ( -a * pow ( ( N - (double)k ) / N, 2.0 ) );
            yi[k] = 1000000 * sin ( -a * pow ( ( N - (double)k ) / N, 2.0 ) );
        }

        // フーリエ逆変換
        FFT_2N ( IFFT, yr, yi, N );             // 周波数領域から時間領域へ変換

        // 実音声信号であるyrをdataにコピー
        for ( n = 0; n < N; n++ ) {
            data[n] = (short)yr[n];
            if ( data[n] > max ) {
                max = data[n];
            }
        }

        // data(downTSP信号)を正規化し, 振幅を32767倍
        for ( n = 0; n < N; n++ ) {
            data[n] = (short)( 32767 * (double)data[n] / max );
        }

        // 円状シフト
        // downTSPの先頭からcs個のデータを退避
        for ( n = 0; n < cs; n++ ) {
            buf[n] = data[n];
        }

        // downTSPを前方に移動
        for ( n = 0; n < ( N - cs ); n++ ) {
            data[n] = data[n + cs];
        }

        // 退避した信号を後方に挿入
        for ( n = ( N - cs ); n < N; n++ ) {
            data[n] = buf[n - ( N - cs )];
        }

        // 後処理
        free ( buf );
        free ( yr );
        free ( yi );
        return data;
    }

    // upTSPを生成し, Rawファイル, Textファイルに書き込み
    short *makeUpTSP ( void ) {
        int     i, n, k;                // カウンタ変数
        int     max = 0;                // 正規化のための変数
        double  m, a;                   // TSP信号のパラメータのための変数
        int     cs;                     // 円状シフトのための変数
        int     Sample;                 // 音声データのサンプル数を格納
        short  *data;                   // 音声データを格納
        short  *buf;                    // 円状シフトのための退避用データを格納
        double *yr;                     // 時間波形, スペクトルの実部を格納
        double *yi;                     // 時間波形, スペクトルの虚部を格納

        // メイン処理
    // 準備フェーズ
        // 音声データの領域を確保
        data = (short *) calloc (N, sizeof( short ) );

        // TSP用のパラメータを設定
        m  = N / 4;
        a  = 4 * m * M_PI;

        // 円状シフト用のパラメータと退避用の配列の領域を確保
        cs  = (int)m;
        buf = (short *) calloc ( cs, sizeof( short ) );

        // 周波数領域のための領域確保
        yr = (double *) calloc ( N, sizeof( double ) );
        yi = (double *) calloc ( N, sizeof( double ) );

    // 生成フェーズ
        // upTSP信号(downTSPの時間軸を反転させた信号)を生成
        for ( k = 0; k <= N / 2; k++ ) {
            yr[k] = 1000000 * cos ( -a * pow ( (double)k / N, 2.0 ) );
            yi[k] = 1000000 * sin ( -a * pow ( (double)k / N, 2.0 ) );
        }
        for ( k = ( N / 2 ) + 1; k < N; k++ ) {
            yr[k] = 1000000 * cos ( a * pow ( ( N - (double)k ) / N, 2.0 ) );
            yi[k] = 1000000 * sin ( a * pow ( ( N - (double)k ) / N, 2.0 ) );
        }

        // フーリエ逆変換
        FFT_2N ( IFFT, yr, yi, N );             // 周波数領域から時間領域へ変換

        // 実音声信号であるyrをdataにコピー
        for ( n = 0; n < N; n++ ) {
            data[n] = (short)yr[n];
            if ( data[n] > max ) {
                max = data[n];
            }
        }

        // data(upTSP信号)を正規化
        for ( n = 0; n < N; n++ ) {
            data[n] = (short)( 32767 * (double)data[n] / max );
        }

        // 円状シフト
        // upTSPの後方からcs個のデータを退避
        for ( n = 0; n < cs; n++ ) {
            buf[n] = data[( N - cs ) + n];
        }

        // upTSPを後方に移動
        for ( n = cs; n < N; n++ ) {
            data[( N + cs - 1 ) - n] = data[( N - 1 ) - n];
        }

        // 退避した信号を前方に挿入
        for ( n = 0; n < cs; n++ ) {
            data[n] = buf[n];
        }

        // 後処理
        free ( buf );
        free ( yr );
        free ( yi );
        return data;
    }

    // 出力Rawデータの書き出し
    void writeRaw ( short *destination, int qBits, int sample, FILE **fp ) {
        fwrite ( destination, qBits / 8, sample, *fp);
    }

    // 出力Textファイルの書き出し
    void writeTex ( short *destination, int sample, FILE **fp ) {
        int i;
        for ( i = 0; i < sample; i++ ) {
            fprintf(*fp, "%.7lf %d\n", (double)i * 1000 / samplingFrequency, destination[i]);
        }
    }
