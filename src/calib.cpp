#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
int main() {
    // Konfigurasi Papan Catur
    int boardWidth = 9;  
    int boardHeight = 6; 
    cv::Size boardSize(boardWidth, boardHeight);
    float squareSize = 2.5f; 

    // OpenCam, ini pakai IP saya karena ga bisa pakai 2, 3 dst
    cv::VideoCapture cap("http://10.78.159.48:4747/video", cv::CAP_FFMPEG); 
    if (!cap.isOpened()) {
        cerr << "Error: Kamera tidak dapat dibuka!" << endl;
        return -1;
    }

    cv::Mat frame, gray;
    int imgCount = 0;
    int targetPhotos = 25; 

    vector<vector<cv::Point3f>> objectPoints;
    vector<vector<cv::Point2f>> imagePoints;

    vector<cv::Point3f> obj;
    for (int i = 0; i < boardHeight; i++) {
        for (int j = 0; j < boardWidth; j++) {
            obj.push_back(cv::Point3f(j * squareSize, i * squareSize, 0.0f));
        }
    }

    cout << "=== PROGRAM KALIBRASI & SCREENSHOT ===" << endl;
    cout << "Arahkan kamera ke papan catur. Tunggu garis warna-warni muncul." << endl;
    cout << "Tekan 'C' untuk memotret dan menyimpan (Target: 25)." << endl;

    // Pengambilan 25 Screenshot Calib CHesss Fisik
    while (imgCount < targetPhotos) {
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat display = frame.clone(); // Mat terpisah agar screenshot yang disimpan tetap bersih tanpa coretan
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        vector<cv::Point2f> corners;
        // Proses deteksi pola catur secara real time
        bool found = cv::findChessboardCorners(gray, boardSize, corners, 
            cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK);

        // Jika ketemu, gambar garis warna warni sebagai penanda seperti yang dicontohkan Mas Brenanda dan Mas Abrar
        if (found) {
            cv::drawChessboardCorners(display, boardSize, corners, found);
        }

        // Teks bahwa tersimpan
        cv::putText(display, cv::format("Screenshot tersimpan: %d / %d", imgCount, targetPhotos), 
                    cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        
        if(found) {
            cv::putText(display, "Pola Terkunci! Tekan 'C' sekarang", cv::Point(20, 70), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
        } else {
            cv::putText(display, "Mencari pola... Geser/Arahkan kamera", cv::Point(20, 70), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("Kamera Uji Coba", display);

        char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 27) {
            break;
        } 
        else if (key == 'c' || key == 'C') {
            if (found) {
                // Perhalus titik koordinat
                cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), 
                    cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
                
                imagePoints.push_back(corners);
                objectPoints.push_back(obj);

                imgCount++;
                
                // Simpan gambar bersih (tanpa coretan garis) ke dalam folder untuk bukti tugas
                string filename = "screenshot_uji_coba_" + to_string(imgCount) + ".jpg";
                cv::imwrite(filename, frame); 

                cout << "Berhasil menyimpan: " << filename << endl;
                
                // Efek kedip layar
                cv::bitwise_not(display, display);
                cv::imshow("Kamera Uji Coba", display);
                cv::waitKey(200);
            } else {
                cout << "Peringatan: Pola belum terdeteksi! Jangan tekan C dulu." << endl;
            }
        }
    }
    
    cv::destroyAllWindows();
    cap.release();

    //  Proses Kalibrasi (Otomatis setelah 25 screenshot valid terkumpul)
    if (imgCount == targetPhotos) {
        cout << "\n=== 25 FOTO TERKUMPUL! MEMULAI PERHITUNGAN MATRIKS ===" << endl;
        
        cv::Mat cameraMatrix = cv::Mat::eye(3, 3, cv::CV_64F);
        cv::Mat distCoeffs = cv::Mat::zeros(8, 1, cv::CV_64F);
        vector<cv::Mat> rvecs, tvecs;

        double rms = cv::calibrateCamera(objectPoints, imagePoints, frame.size(), 
                                     cameraMatrix, distCoeffs, rvecs, tvecs);
                                     
        cout << "Kalibrasi Selesai! Nilai RMS Error: " << rms << endl;

        cv::FileStorage fs("yaml/kalibrasi.yaml", cv::FileStorage::WRITE);
        if (fs.isOpened()) {
            fs << "camera_matrix" << cameraMatrix;
            fs << "distortion_coefficients" << distCoeffs;
            fs.release();
            cout << "SUCCESS: File berhasil dibuat di 'yaml/kalibrasi.yaml'" << endl;
        } else {
            cerr << "ERROR: Gagal menyimpan! Pastikan folder 'yaml/' sudah ada di foldermu." << endl;
        }
    } else {
        cout << "Dibatalkan. Foto kurang dari 25." << endl;
    }

    return 0;
}