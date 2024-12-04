#include "vision.hpp"

volatile bool ctrl_c_pressed = false;  // main.cpp에서만 정의

// 전처리 및 ROI(관심영역) 설정
void preprocess(VideoCapture& source, Mat& frame, Mat& gray, Mat& bin) {
    source >> frame;  // 비디오에서 한 프레임을 읽어옴
    if (frame.empty()) {  // 프레임이 비어 있으면 오류 메시지 출력
        cerr << "Empty frame encountered!" << endl;
        return;
    }

    cvtColor(frame, gray, COLOR_BGR2GRAY);  // 컬러 이미지를 그레이스케일로 변환
    Scalar avgBright = mean(gray); // 이미지의 평균 밝기를 구함
    // 이미지 밝기를 조정: 평균 밝기와 원하는 밝기(100)의 차이를 더함
    gray = gray + (100 - avgBright[0]);

    // 이진화: 밝기가 128 이상이면 흰색, 그렇지 않으면 검은색으로 변환
    threshold(gray, bin, 128, 255, THRESH_BINARY);

    // 화면의 원하는 크기 (640x90)
    int outputWidth = 640;
    int outputHeight = 90;

    // 화면의 아래쪽 90px을 잘라서 관심영역(ROI) 설정
    int RoiHeight = bin.rows - outputHeight;
    Rect Roi(0, RoiHeight, outputWidth, outputHeight);

    bin = bin(Roi);  // 관심영역만 잘라서 bin에 저장
}

// 객체 탐지
void detectObj(const Mat& bin, Point& mainPoint, Point& subPoint, Mat& outputImg, Mat& stats, Mat& centroids) {
    Mat label;
    // bin: 이진화된 이미지, label: 각 객체를 구분한 라벨 이미지
    // stats: 각 객체의 통계(면적, 위치 등), centroids: 각 객체의 중심 좌표
    int numObjects = connectedComponentsWithStats(bin, label, stats, centroids); // 객체 개수
    outputImg = bin.clone();  // 이진화된 이미지를 복사하여 출력 이미지로 사용
    cvtColor(outputImg, outputImg, COLOR_GRAY2BGR);  // 이미지를 컬러로 변환

    int closestObj = -1;  // 가장 가까운 객체의 인덱스
    int mindist = outputImg.cols;  // 최소 거리, 초기값을 화면의 너비로 설정

    // 객체들을 반복하면서 가장 가까운 객체를 찾기
    for (int i = 1; i < numObjects; i++) {
        int area = stats.at<int>(i, CC_STAT_AREA);  // 객체의 면적(4)
        if (area > 120) {  // 면적이 120보다 큰 객체만 탐지
            Point center(cvRound(centroids.at<double>(i, 0)), cvRound(centroids.at<double>(i, 1)));  // 객체 중심 좌표

            // 객체 중심과 mainPoint 간의 거리 계산
            int dist = norm(center - mainPoint);
            if (dist <= 140 && dist < mindist) {  // mainPoint와 가까운 객체를 찾음
                mindist = dist;  // 최소 거리 갱신
                closestObj = i;  // 가장 가까운 객체의 인덱스를 저장
            }
        }
    }

    // 가장 가까운 객체가 있으면 mainPoint를 해당 객체의 중심으로 업데이트
    if (closestObj > 0) {
        mainPoint = Point(cvRound(centroids.at<double>(closestObj, 0)),
            cvRound(centroids.at<double>(closestObj, 1)));
    }
    else {
        //객체 이탈 
    }
}

// 객체 표시
void markObj(const Mat& stats, const Mat& centroids, const Point& mainPoint, Mat& outputImg) {
    const Scalar MAIN_COLOR = Scalar(0, 0, 255);  // 목표 지점 색상 (빨간색)
    const Scalar OBJECT_COLOR = Scalar(255, 0, 0);  // 일반 객체 색상 (파란색)
    const int MIN_OBJECT_AREA = 120;  // 객체 면적의 최소 크기 

    // 객체들을 반복하면서 그리기
    for (int i = 1; i < stats.rows; i++) {
        // 면적이 설정된 값보다 큰 객체만 표시
        int area = stats.at<int>(i, CC_STAT_AREA);
        if (area > MIN_OBJECT_AREA) {
            // 객체 중심 좌표 계산
            Point center(cvRound(centroids.at<double>(i, 0)), cvRound(centroids.at<double>(i, 1)));
            // 객체가 mainPoint와 같으면 빨간색, 아니면 파란색
            Scalar color = (center.x == mainPoint.x) ? MAIN_COLOR : OBJECT_COLOR;

            // 객체의 경계 상자와 객체 중심에 원 그리기
            rectangle(outputImg,
                Rect(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP),
                    stats.at<int>(i, CC_STAT_WIDTH), stats.at<int>(i, CC_STAT_HEIGHT)),
                color, 2);  // 경계선 두께

            circle(outputImg, center, 5, color, -1);  // 객체 중심에 원 그리기
        }
    }
    //따라가는 포인트
    circle(outputImg, mainPoint, 5, MAIN_COLOR, -1);
}

void signalHandler(int) {
    ctrl_c_pressed = true; // Ctrl+C 눌림 플래그 설정
}
