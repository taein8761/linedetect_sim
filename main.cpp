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

    TickMeter tm,tm2;
    bool isFirstFrame = true;  // 첫 번째 프레임인지 확인하는 변수
    Point mainPoint, subPoint;  // mainPoint는 화면의 목표 지점
    Mat frame, gray, bin, outputImg, stats, centroids;
    int error;

    Dxl dxl;
    if(!dxl.open()) { cout << "dynamixel open error"<<endl; return -1; }

    bool mode =false; //다이나믹 셀 start/stop
    double k = 0.13; //다이나믹셀 조정값
    while (true) {
        tm.start();
        tm2.start();
        preprocess(source, frame, gray, bin);  // 전처리 함수 호출
        
        if (isFirstFrame) {
            mainPoint = Point(bin.cols / 2, bin.rows - 1);  // 화면 중간 아래쪽을 목표 지점으로 설정
            isFirstFrame = false;
        }

        detectObj(bin, mainPoint, subPoint, outputImg, stats, centroids);  // 객체 탐지
        markObj(stats, centroids, mainPoint, outputImg);  // 객체에 표시하기
        
        // Write original frame to writer
        writer1 << frame;    // 컬러 원본
        writer2 << outputImg; // 하단 1/4 이진화

        // 목표 지점과 실제 위치 차이 계산
        error = (outputImg.cols / 2) - mainPoint.x;
        
        //다이나믹셀 바퀴 계산
        if(dxl.kbhit()) // 없으면 제어 멈춤
        {
            char ch = dxl.getch();
            if(ch == 'q') break;
            else if(ch == 's') {
                mode = !mode; // 모드 토글
                }
        }

        signal(SIGINT, signalHandler); // SIGINT(Ctrl+C) 처리 핸들러 등록
        if (ctrl_c_pressed) break;
        int leftvel = 100 - k* error;
        int rightvel = -(100 + k* error);

        if(mode) {
            dxl.setVelocity(leftvel, rightvel);
        }
        tm2.stop();

        int sleep = 33 * 1000-tm2.getTimeMicro();  //0.03초를 맞추기 위한 sleep
        if(sleep < 0) sleep = 0;
        usleep(sleep);

        tm.stop();

        cout << "error:" << error << " ,lvel:" << leftvel << " ,"
           << "rvel:" << rightvel << ", time:" << fixed << setprecision(9) << tm.getTimeSec() << endl;


        tm.reset();  
        tm2.reset();
    }
    dxl.close();
    return 0;  // 프로그램 종료
}
