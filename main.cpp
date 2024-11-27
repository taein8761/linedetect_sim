#include "vision.hpp"
int main() {
    VideoCapture source("5.mp4");  // 비디오 파일 열기
    if (!source.isOpened()) {
        cerr << "Failed to open video file!" << endl;  // 비디오 열기 실패 시 오류 메시지 출력
        return -1;
    }

    bool isFirstFrame = true;  // 첫 번째 프레임인지 확인하는 변수
    Point mainPoint, subPoint;  // mainPoint는 화면의 목표 지점
    Mat frame, gray, bin, outputImg, stats, centroids;
    int Error;

    while (true) {
        preprocess(source, frame, gray, bin);  // 전처리 함수 호출
        if (bin.empty()) break;  // 영상이 끝났으면 종료

        if (isFirstFrame) {
            mainPoint = Point(bin.cols / 2, bin.rows - 1);  // 화면 중간 아래쪽을 목표 지점으로 설정--------------------수정
            isFirstFrame = false;
        }

        detectObj(bin, mainPoint, subPoint, outputImg, stats, centroids);  // 객체 탐지
        markObj(stats, centroids, mainPoint, outputImg);  // 객체에 표시하기

        imshow("color image", frame);  // 원본 이미지 표시
        imshow("Object Detection", outputImg);  // 객체 탐지 결과 표시

        Error = (outputImg.cols / 2) - mainPoint.x;  // 목표 지점과 실제 위치 차이 계산
        cout << "error:" << Error << ", time: " << endl;

        if (waitKey(30) == 'q') break;  // 'q'를 누르면 종료
    }

    return 0;  // 프로그램 종료
}
