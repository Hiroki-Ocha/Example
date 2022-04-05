// ---------------------------------------------------
// FFTのサブルーチン                
// 作成者：Kenta Iwai, 2017. 02. 27 
// ---------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef	___FFT_LIB
#define ___FFT_LIB

#define	FFT			1							// フーリエ変換を行う
#define IFFT		0							// 逆フーリエ変換を行う

int		FFT_2N( int, double *, double *, int );			// 2N点の1次元FFT
int		log2i( int x );
void	Bit_reverse( int, int * );

// FFT基本関数 (基数2のFFT)
int FFT_2N( int fft, double *x_re, double *x_im, int FFT_size )
{
	// 定数
#ifndef pi
	double pi = acos(-1.0);
#endif

	// 変数
	int		n = 0, k = 0, i, j;
	int		imax, jmax, jstep;
	int		stage;
	int		*Bit_rev;
	double	omega_unit = 0.0;
	double	angle;
	double	temp_r = 0.0, temp_i = 0.0;

	double	*Wn_re, *Wn_im;
	double	temp_re = 0.0, temp_im = 0.0;

	Bit_rev = ( int* )calloc( FFT_size, sizeof( int ) );
	Wn_re = ( double* )calloc( FFT_size/2, sizeof( double ) );
	Wn_im = ( double* )calloc( FFT_size/2, sizeof( double ) );

	// ステージ数
	stage = (int)( log2i( (double)FFT_size ) + 0.5 ); // ビット数の計算(ビット反転後のインデックスの計算用)

	// ビット反転後のインデックス
	Bit_reverse( FFT_size, Bit_rev );

	// 回転子の計算
	omega_unit = 2.0 * pi / (double)FFT_size;
	for( n=0; n<FFT_size/2; n++ )
	{
		angle = omega_unit * (double)n;
		temp_r = cos( angle  );
		temp_i = -sin( angle );

		// 0のとき, sin(0) は演算誤差により非ゼロとなる. このとき, 直接0を格納する
		if( temp_r == 1.0 /*n==0*/ )
		{
			temp_i = 0.0;
		}

		// pi/2.0のとき, cos(pi/2.0) は演算誤差により非ゼロとなる. このとき, 直接0を格納する
		if( temp_i == -1.0 /*n==FFT_size/4*/ )
		{
			temp_r = 0.0;
		}

		Wn_re[n] = temp_r;
		Wn_im[n] = temp_i;
	}

	// FFT処理
	for( k=0; k<stage; k++ )
	{
		jmax = 1 << k; // kを1ビットだけ左シフト
		imax = FFT_size >> ( k+1 ); // Nを(k+1)ビットだけ右シフト
		jstep = FFT_size >> k; // Nをkビットだけ右シフト

		for( j=0; j<jmax; j++ )
		{
			for( i=0; i<imax; i++ )
			{
				// 実部、虚部をそれぞれ一時確保
				temp_re = x_re[i + j*jstep];
				temp_im = x_im[i + j*jstep];

				// 加算処理
				x_re[i + j*jstep] += x_re[i + imax + j*jstep];
				x_im[i + j*jstep] += x_im[i + imax + j*jstep];
				
				// 一時確保していた値から減算
				temp_re -= x_re[i + imax + j*jstep];
				temp_im -= x_im[i + imax + j*jstep];

				// FFTの結果 (複素数の掛け算であることに注意)
				x_re[i + imax + j * jstep] = temp_re * Wn_re[i * jmax] - temp_im * Wn_im[i * jmax];
				x_im[i + imax + j * jstep] = temp_re * Wn_im[i * jmax] + temp_im * Wn_re[i * jmax];
			}
		}
	}

	// 順序の整理
	for( k=0; k<FFT_size; k++ )
	{
		if( Bit_rev[k] > k )
		{
			temp_re = x_re[ Bit_rev[k] ];
			temp_im = x_im[ Bit_rev[k] ];

			x_re[ Bit_rev[k] ] = x_re[k];
			x_im[ Bit_rev[k] ] = x_im[k];

			x_re[k] = temp_re;
			x_im[k] = temp_im;
		}
	}


	// IFFTの場合の処理
	if( fft==IFFT )
	{
		double	inverse = 1.0 / (double)FFT_size;
		
		x_re[0] *= inverse;
		x_im[0] *= inverse;

		x_re[FFT_size/2] *= inverse; 
		x_im[FFT_size/2] *= inverse; 

		for( i=1; i<FFT_size/2; i++ )
		{
			temp_re = x_re[i] * inverse;
			temp_im = x_im[i] * inverse;

			x_re[i] = x_re[FFT_size-i] * inverse;
			x_im[i] = x_im[FFT_size-i] * inverse;

			x_re[FFT_size-i] = temp_re;
			x_im[FFT_size-i] = temp_im;
		}
	}


	free( Bit_rev );
	free( Wn_re );
	free( Wn_im );

	return 0;
}

void Bit_reverse( int FFT_size, int *Index_reverse )
{
	int n = FFT_size, m = 0, i = 0, j = 0;
	
	for ( i=0; i<n; i++ )
	{
		//fprintf( stdout, " i = %d, j = %d\n", i, j ); // 反転前のインデックスiに対する反転後のインデックスj

		Index_reverse[i] = j;
		
		m = n >> 1; //n (=N) を1bitだけ右シフト ( N=8なら, 1000 -> 0100 )

		while ( m>=1 && j>=m ) // (i-1)ビットを反転した値j から N/2, N/4
		{
			j -= m;		// (i-1)ビットの反転からN/2, N/4, ...を引いていく
			m >>= 1;	// mを1bitだけ右シフト (m = N/2 -> N/4 -> ... としていく操作)
		}

		j += m; // 反転前のインデックス(i+1)に対する反転後のインデックス
	}
}

int log2i( int x )
{
	int		i = 0;

	do
	{
		// xが2のべき乗でない場合の処理
		if( x%2 )
		{
		  //			return NULL;
			return 0;
		}
		
		i++;

		x >>= 1;

	}while( x != 1 );

	return i;
}

#endif