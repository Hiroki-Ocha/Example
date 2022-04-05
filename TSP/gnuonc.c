#include <stdio.h>

int main ( void ) {
    FILE *gp;

    // 時間波形プリセット
    gp = _popen("gnuplot -persist", "w");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set xlabel 'Time [ms]'\n");
    fprintf(gp, "set ylabel 'Amplitude'\n");
    fprintf(gp, "set xrange [0:5461.3125]\n");
    fprintf(gp, "set yrange [-32768:32767]\n");
    fprintf(gp, "set xtics 1000\n");
    fprintf(gp, "set ytics 10000\n");
    fprintf(gp, "set output 'downTSP.png'\n");
    fprintf(gp, "plot 'donnTSP.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'upTSP.png'\n");
    fprintf(gp, "plot 'upTSP.dat' with lines lt rgb 'red'\n");
/*
    // 周波数特性プリセット
    gp = _popen("gnuplot -persist", "w");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set xlabel 'Normalized angular frequency'\n");
    fprintf(gp, "set ylabel 'Gain [dB]'\n");
    fprintf(gp, "set xrange [0:1]\n");
    fprintf(gp, "set yrange [-60:20]\n");
    fprintf(gp, "set xtics 0.1\n");
    fprintf(gp, "set ytics 10\n");
    fprintf(gp, "set output 'Result/nConvLPS.png'\n");
    fprintf(gp, "plot 'Result/nConvLPS.dat' with lines lt rgb 'red'\n");
*/

    _pclose(gp);

    return 0;
}