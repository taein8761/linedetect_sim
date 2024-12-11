# linedetect_sim

# 1. 라인 검출

```
volatile bool ctrl_c_pressed = false;
```

설명: 프로그램이 실행 중에 Ctrl+C 입력을 감지하기 위해 사용하는 변수입니다.

목적: 프로그램 종료를 위해 신호 처리 함수(signalHandler)에서 이 값을 변경합니다

```
void preprocess(VideoCapture& source, Mat& frame, Mat& gray, Mat& bin)
```

기능: 비디오 프레임을 전처리하여 관심 영역(ROI)을 설정하고 이진화된 이미지를 생성합니다.

작업 흐름: 비디오 프레임 읽기: source >> frame으로 한 프레임을 가져옵니다.

그레이스케일 변환: cvtColor로 이미지를 흑백으로 변환.

밝기 조정: 이미지의 평균 밝기를 기준으로 조정.

이진화: 밝기가 임계값(128) 이상이면 흰색, 아니면 검은색으로 변환.

ROI 설정: 화면의 아래쪽 90px만 잘라 관심 영역으로 지정.

결과: bin에 관심 영역의 이진화된 이미지가 저장됩니다.
```
void detectObj(const Mat& bin, Point& mainPoint, Point& subPoint, Mat& outputImg, Mat& stats, Mat& centroids)
```
기능: 이진화된 이미지에서 객체를 탐지하고, 중심 좌표를 계산하여 가장 가까운 객체를 찾습니다.

작업 흐름: 연결된 구성요소 분석: connectedComponentsWithStats로 객체들을 탐지하고, 각 객체의 면적, 중심점, 경계 정보를 얻음.

가장 가까운 객체 탐지: mainPoint와 가장 가까운 객체를 탐지(거리 계산), 면적이 일정 크기(120) 이상이고, 거리 조건(140 이하)을 만족하는 객체를 선택.

객체 위치 업데이트: 가장 가까운 객체가 있으면 mainPoint를 해당 객체의 중심으로 업데이트, 조건에 맞는 객체가 없으면 객체 이탈 처리.

결과: 탐지된 객체 중심 좌표가 mainPoint에 저장됩니다.
```
void markObj(const Mat& stats, const Mat& centroids, const Point& mainPoint, Mat& outputImg)
```
기능: 탐지된 객체들을 이미지 위에 표시하고, 추적 중인 주요 객체를 강조합니다.
작업 흐름:

객체 정보 가져오기: stats로 객체의 경계 상자, 면적 등 정보를 가져옴, centroids로 각 객체의 중심 좌표 계산.

조건에 따른 객체 표시: 면적이 120 이상인 객체만 경계 상자와 중심점을 그림, mainPoint와 동일한 중심점은 빨간색(MAIN_COLOR), 나머지는 파란색(OBJECT_COLOR)으로 표시.

추적 대상 표시:mainPoint에 원(circle) 표시로 강조.

결과: 탐지된 객체가 시각적으로 강조된 이미지를 생성.
```
void signalHandler(int)
```
기능: Ctrl+C 입력을 감지하여 프로그램 종료를 처리.

작업 흐름: Ctrl+C 입력 시 ctrl_c_pressed 플래그를 true로 설정.

#
5번 영상 (out 라인)
https://github.com/user-attachments/assets/2c7b4cca-d7ea-4661-aa09-77735ee17bd1

7번 영상 (in 라인)
https://github.com/user-attachments/assets/532c0771-29b6-48f9-ac69-ae82a88e48b7

# 2. 모션 제어

모터속도비교
https://github.com/user-attachments/assets/8ea1fd0f-e510-4484-964e-03f432a571b9

# 3. 라인 트레이서
   
