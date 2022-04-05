#include <stdio.h>

int main ( void ) {
    FILE *gp;
/*
    // 時間波形プリセット
    gp = _popen("gnuplot -persist", "w");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set xlabel 'Time [s]'\n");
    fprintf(gp, "set ylabel 'Amplitude'\n");
    fprintf(gp, "set xrange [0:5.4613125]\n");
    fprintf(gp, "set yrange [-32767:32766]\n");
    fprintf(gp, "set xtics 1\n");
    fprintf(gp, "set ytics 10000\n");
    fprintf(gp, "set output 'Result/convTSP.png'\n");
    fprintf(gp, "plot 'Result/convTSP.dat' with lines lt rgb 'red'\n");
*/
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
    fprintf(gp, "set output 'Results/04_ADF_AMP_sn50nlmsSS0.01.png'\n");
    fprintf(gp, "plot 'Results/04_ADF_AMP_sn50nlmsSS0.01.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/04_ADF_AMP_sn50nlmsSS0.1.png'\n");
    fprintf(gp, "plot 'Results/04_ADF_AMP_sn50nlmsSS0.1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/04_ADF_AMP_sn50nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/04_ADF_AMP_sn50nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/04_ADF_AMP_sn30nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/04_ADF_AMP_sn30nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/04_ADF_AMP_sn10nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/04_ADF_AMP_sn10nlmsSS1.dat' with lines lt rgb 'red'\n");
*/

    // フィルタ係数プリセット
    gp = _popen("gnuplot -persist", "w");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set xlabel 'Number of Taps'\n");
    fprintf(gp, "set ylabel 'Amplitude'\n");
    fprintf(gp, "set xrange [0:8000]\n");
    fprintf(gp, "set yrange [-1:1]\n");
    fprintf(gp, "set xtics 1000\n");
    fprintf(gp, "set ytics 0.2\n");
    fprintf(gp, "set output 'Results/03_ADF_coef_sn50nlmsSS0.01.png'\n");
    fprintf(gp, "plot 'Results/03_ADF_coef_sn50nlmsSS0.01.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/03_ADF_coef_sn50nlmsSS0.1.png'\n");
    fprintf(gp, "plot 'Results/03_ADF_coef_sn50nlmsSS0.1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/03_ADF_coef_sn50nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/03_ADF_coef_sn50nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/03_ADF_coef_sn30nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/03_ADF_coef_sn30nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/03_ADF_coef_sn10nlmsSS1.png'\n");
    fprintf(gp, "plot 'Results/03_ADF_coef_sn10nlmsSS1.dat' with lines lt rgb 'red'\n");


    // 誤差減衰量プリセット
    gp = _popen("gnuplot -persist", "w");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set xlabel 'Iteration'\n");
    fprintf(gp, "set ylabel 'Reduction [dB]'\n");
    fprintf(gp, "set xrange [0:1680000]\n");
    fprintf(gp, "set yrange []\n");
    fprintf(gp, "set xtics 500000\n");
    fprintf(gp, "set ytics 5\n");
    fprintf(gp, "set output 'Results/01_Redution_sn50nlmsS0.01.png'\n");
    fprintf(gp, "plot 'Results/01_Reduction_sn50nlmsSS0.01.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/01_Redution_sn50nlmsS0.1.png'\n");
    fprintf(gp, "plot 'Results/01_Reduction_sn50nlmsSS0.1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/01_Redution_sn50nlmsS1.png'\n");
    fprintf(gp, "plot 'Results/01_Reduction_sn50nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/01_Redution_sn30nlmsS1.png'\n");
    fprintf(gp, "plot 'Results/01_Reduction_sn30nlmsSS1.dat' with lines lt rgb 'red'\n");
    fprintf(gp, "set output 'Results/01_Redution_sn10nlmsS1.png'\n");
    fprintf(gp, "plot 'Results/01_Reduction_sn10nlmsSS1.dat' with lines lt rgb 'red'\n");

    // end
    _pclose(gp);
    return 0;
}