#include "serial_reader.h"
#include "synthetic_mag_data.h"
#include "linear_fit.h"
#include "plot_data_realtime.h"
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "plot_matrix.h"

// ---- Clean shutdown on Ctrl+C -------------------------------------------
static volatile std::sig_atomic_t g_stop = 0;
static void onSigint(int) { g_stop = 1; }

int main() {

// TEST LINEAR FIT FOR MAGNETOMETER
    // int numSamples=500;
    // auto synth = generateFakeMagData(numSamples);

    // auto result=ellipsoid_fit_linear(synth.rawSamples);
    //  // your function
    
    // std::cout << "Ground truth A:\n" << synth.A_true << "\n";
    // std::cout << "Ground truth b:\n" << synth.b_true.transpose() << "\n";
    // std::cout << "Fit A:\n" << result.A_scale << "\n";
    // std::cout << "Fit B:\n" << result.B_offset << "\n";
    
    // Eigen::MatrixXd correctedSamples(numSamples,3);
    // correctedSamples = (synth.rawSamples.rowwise() - result.B_offset.transpose())* result.A_scale.inverse().transpose();
    // std::cout << "Actual:\n"
    //       << synth.actual.topRows(5) << "\n";

    // std::cout << "Corrected:\n"
    //       << correctedSamples.topRows(5) << "\n";
          
    // plot_matrices(
    // {synth.actual, correctedSamples},
    // {"Actual", "Corrected"}
    // );
    // return 0;

/*
IMU SENSOR FUSION
*/

    std::string prompt;
    std::string reply;
    std::string display_msg;
    
    const std::string REPLY_MARKER = "[REPLY]", DATA_MAG_CAL_MARKER = "[DATA_MAG_CAL]", 
    IMU_DATA_MARKER = "[DATA_IMU]";

    int numSamples=500;
    int sample = 0;

    Eigen::MatrixXd magdata(numSamples,3);

    // IMU CSV logging setup (for Allan variance analysis)
    constexpr size_t IMU_FIELDS = 9;   // ax,ay,az,gx,gy,gz,mx,my,mz
    std::signal(SIGINT, onSigint);
    std::ofstream imu_csv("data/imu_log.csv");
    imu_csv << "idx,t_host_s,ax,ay,az,gx,gy,gz\n";
    const auto t0 = std::chrono::steady_clock::now();
    unsigned long long imu_idx = 0, bad_lines = 0;

   // _________________________________________________________________________
    // Open Serial Port to communicate with Arduino
    int fd = openSerialPort("/dev/ttyACM0", B9600);
    if (fd < 0) {
        printf("Failed to open serial port\n");
        return 1;                       
    } else {
        printf("Port Open!\n");
    }
    tcflush(fd, TCIFLUSH);
    sleep(2);

    // Recieve, Sort, and Actionize Serial Data
     while (!g_stop) {
        if (serialDataAvailable(fd)) {
             prompt = readLine(fd);

            bool needsReply = prompt.rfind(REPLY_MARKER, 0) == 0; 
            bool mag_cal_data_incoming=prompt.rfind(DATA_MAG_CAL_MARKER, 0) == 0;
            bool imu_data_incoming=prompt.rfind(IMU_DATA_MARKER, 0) == 0;
 
            if (needsReply) {
                display_msg = prompt.substr(REPLY_MARKER.length());
                std::cout << display_msg +'\n';
                std::getline(std::cin, reply);   
                std::string line = reply + '\n'; 
                ssize_t written = 0;
                while (written < (ssize_t)line.length()) {
                    ssize_t n = write(fd, line.c_str() + written, line.length() - written);
                    if (n < 0) {                    
                        perror("Write Failed!");
                        break;
                    }
                    written += n;
                }
            } else if(mag_cal_data_incoming) {
                display_msg = prompt.substr(DATA_MAG_CAL_MARKER.length());
                std::cout << display_msg +'\n';
                std::stringstream ss(display_msg);
                std::string value;
                int16_t x, y, z;
                std::getline(ss, value, ',');
                x = std::stoi(value);
                std::getline(ss, value, ',');
                y = std::stoi(value);
                std::getline(ss, value);
                z = std::stoi(value);
            
                if (sample < numSamples) {
                    magdata(sample,0)=x;
                    magdata(sample,1)=y;
                    magdata(sample,2)=z;
                    sample++;

                    if(sample>0){
                    plot_data_realtime(x, y, z, numSamples);
                }
                }
                if (sample==numSamples){
                    sleep(1);
                    std::cout << "Data received!\n";
                    sleep(2);
                    auto ellipsoid_params = ellipsoid_fit_linear(magdata);
                    std::cout << "Fit A:\n" << ellipsoid_params.A_scale << "\n";
                    std::cout << "Fit B:\n" << ellipsoid_params.B_offset << "\n";
                    std::stringstream calibration_msg;

                    calibration_msg << "A,"
                                    << ellipsoid_params.A_inverse(0,0) << ","
                                    << ellipsoid_params.A_inverse(0,1) << ","
                                    << ellipsoid_params.A_inverse(0,2) << ","
                                    << ellipsoid_params.A_inverse(1,0) << ","
                                    << ellipsoid_params.A_inverse(1,1) << ","
                                    << ellipsoid_params.A_inverse(1,2) << ","
                                    << ellipsoid_params.A_inverse(2,0) << ","
                                    << ellipsoid_params.A_inverse(2,1) << ","
                                    << ellipsoid_params.A_inverse(2,2) << "\n";

                    calibration_msg << "B,"
                                    << ellipsoid_params.B_offset(0) << ","
                                    << ellipsoid_params.B_offset(1) << ","
                                    << ellipsoid_params.B_offset(2) << "\n";

                    std::string message = calibration_msg.str();

                    ssize_t written = 0;

                    while (written < static_cast<ssize_t>(message.length())) {

                        ssize_t n = write(fd,message.c_str() + written,message.length() - written);

                        if (n < 0) {
                            perror("Write failed");
                            break;
                        }

                        written += n;
                    }

            }
            } else if (imu_data_incoming) {
                std::string payload = prompt.substr(IMU_DATA_MARKER.length());
                size_t a = payload.find_first_not_of(" ,\t");
                size_t b = payload.find_last_not_of(" ,\t\r\n");
                payload = (a == std::string::npos) ? "" : payload.substr(a, b - a + 1);

                std::vector<std::string> f;
                std::string why;
                if (!parseFields(payload, f, IMU_FIELDS, why)) {
                    if (bad_lines++ < 5)
                        std::cerr << "BAD [" << why << "]: '" << prompt << "'\n";
                    continue;
                }
                double t = std::chrono::duration<double>(
                            std::chrono::steady_clock::now() - t0).count();
                imu_csv << imu_idx++ << ',' << t;
                for (int i = 0; i < 6; ++i) imu_csv << ',' << f[i];   // MPU only
                imu_csv << '\n';
                if (imu_idx % 200 == 0) imu_csv.flush();
            }
            else {
                std::cout << prompt +'\n';
            }
        }
        else {
             usleep(1000);                      
        }

     }

    imu_csv.close();
    std::cout << "Stopped. IMU samples logged: " << imu_idx
              << ", bad lines skipped: " << bad_lines << "\n";
    close(fd);
    return 0;
}