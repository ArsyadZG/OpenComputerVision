#ifndef TRACKER_HPP //Agar CPP tidak bingung kompilasi berulang kali
#define TRACKER_HPP

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

// Struktur data diperbarui untuk mengakomodasi warna merah yang terbelah di 2 rentang Hue
struct ObjekFisik {
    string nama;
    Scalar lowerHLS;
    Scalar upperHLS;
    Scalar warnaBox;
    float W_real; 
    bool isRed;           // Penanda apakah objek ini butuh 2 rentang warna (khusus merah)
    Scalar lowerHLS2;     // Batas bawah HLS kedua
    Scalar upperHLS2;     // Batas atas HLS kedua
};

#endif 