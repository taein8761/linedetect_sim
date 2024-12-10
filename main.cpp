#include "vision.hpp"
#include <unistd.h>
int main() {
    
    VideoCapture source("5.mp4");
	if (!source.isOpened()) { cout << "Camera error" << endl; return -1; }
    string src = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.156 port=8001 sync=false";
    VideoWriter writer1(src,0, (double)30,Size(640,360),true);
    if(!writer1.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    string dst = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
	nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
	h264parse ! rtph264pay pt=96 ! \
	udpsink host=203.234.58.156 port=8002 sync=false";
    VideoWriter writer2(dst,0, (double)30,Size(640,90),true);
    if(!writer2.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}
    

    TickMeter tm, tm2;
    bool isFirstFrame = true;  // Check if it's the first frame
    Point mainPoint, subPoint;  // mainPoint is the target point on the screen
    Mat frame, gray, bin, outputImg, stats, centroids;
    int error;

    Dxl dxl;
    if (!dxl.open()) {
        cout << "Dynamixel open error" << endl;
        return -1;
    }

    bool mode = false;  // Dynamixel start/stop mode
    double k = 0.13;    // Dynamixel adjustment value
    while (true) {

        tm.start();
        tm2.start();
        preprocess(source, frame, gray, bin);  // Preprocessing function call
        
        if (isFirstFrame) {
            mainPoint = Point(bin.cols / 2, bin.rows - 1);  // Set target point at the bottom center of the screen
            isFirstFrame = false;
        }

        detectObj(bin, mainPoint, subPoint, outputImg, stats, centroids);  // Object detection
        markObj(stats, centroids, mainPoint, outputImg);  // Mark the detected object
        
        // Write original frame to writer1
        writer1 << frame;    // Color original frame
        writer2 << outputImg; // Bottom 1/4 binary image

        // Calculate error based on target point and actual position
        error = (outputImg.cols / 2) - mainPoint.x;
        
        // Dynamixel wheel calculation
        if (dxl.kbhit()) {  // Stop control if no key press
            char ch = dxl.getch();
            if (ch == 'q') break;
            else if (ch == 's') {
                mode = !mode; // Toggle mode
            }
        }

        signal(SIGINT, signalHandler);  // Handle SIGINT (Ctrl+C)
        if (ctrl_c_pressed) break;

        int leftvel = 100 - k * error;
        int rightvel = -(100 + k * error);

        if (mode) {
            dxl.setVelocity(leftvel, rightvel);
        }

        tm2.stop();

        int sleep = 33 * 1000 - tm2.getTimeMicro();  // Sleep for 0.03 seconds
        if (sleep < 0) sleep = 0;
        usleep(sleep);

        tm.stop();

        cout << "error:" << error << " ,lvel:" << leftvel << " ,"
             << "rvel:" << rightvel << ", time:" << fixed << setprecision(9) << tm.getTimeSec() << endl;

        tm.reset();  
        tm2.reset();
    }
    
    dxl.close();
    return 0;  // Program exit
}
