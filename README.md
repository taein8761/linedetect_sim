# linedetect_sim
# 전체 블록도 
![image](https://github.com/user-attachments/assets/e4d0dcae-160f-4d23-ab07-221a750481fd)


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
```
Dxl dxl;
if (!dxl.open()) {
    cout << "Dynamixel open error" << endl;
    return -1;
}
```
Dxl 객체를 생성하고 dxl.open()을 호출하여 Dynamixel 모터를 초기화합니다.

초기화가 실패하면 프로그램을 종료합니다.
```
error = (outputImg.cols / 2) - mainPoint.x;
```
에러 계산: 화면의 중심과 목표점(mainPoint) 간의 가로 좌표 차이를 계산합니다, outputImg.cols / 2: 화면의 중앙 x좌표.

mainPoint.x: 탐지된 객체의 중심 x좌표, 결과적으로, 목표점이 화면 중심에서 얼마나 떨어져 있는지를 나타냅니다.
```
int leftvel = 100 - k * error;
int rightvel = -(100 + k * error);
```
k: 보정 계수로, 에러에 따라 속도를 조정합니다.

왼쪽 모터 속도: 화면 중심보다 객체가 왼쪽에 있으면 속도가 증가하여 오른쪽으로 회전합니다.

오른쪽 모터 속도: 화면 중심보다 객체가 오른쪽에 있으면 속도가 증가하여 왼쪽으로 회전합니다.

결과: 에러 값에 따라 두 모터의 속도가 비례적으로 조정됩니다.
```
if (mode) {
    dxl.setVelocity(leftvel, rightvel);
}
```
mode 확인: mode가 true일 때만 모터 속도가 설정됩니다.

mode는 키 입력 's'로 토글됩니다.

속도 설정: dxl.setVelocity(leftvel, rightvel)를 호출하여 왼쪽 및 오른쪽 모터의 속도를 설정합니다.
```
if (dxl.kbhit()) {  // 키 입력 확인
    char ch = dxl.getch();
    if (ch == 'q') break;   // 'q'를 누르면 종료
    else if (ch == 's') {
        mode = !mode;       // 's'를 누르면 모터 작동 상태 전환
    }
}
```
모터 속도와 에러 정보를 출력:

error: 화면 중심과 목표점 간 거리.

lvel/rvel: 왼쪽/오른쪽 모터 속도.

time: 루프 실행 시간.
```
cout << "error:" << error << " ,lvel:" << leftvel << " ,"
     << "rvel:" << rightvel << ", time:" << fixed << setprecision(9) << tm.getTimeSec() << endl;
```
모터 정보 출력

#
모터속도비교
https://github.com/user-attachments/assets/8ea1fd0f-e510-4484-964e-03f432a571b9

# 3. 라인 트레이서
**라인트레이서(Line Tracer) 설명**
++ 주행시에는 0.14로 조정, time 과 관련된 함수 모두 삭제 ++
라인트레이서는 로봇이 주어진 경로(라인)를 따라 움직이는 시스템입니다. 이 프로그램에서는 **비디오 입력**으로 라인을 감지하고, **Dynamixel 모터**를 제어하여 경로를 따라 움직이는 라인트레이서를 구현합니다.

---

### **작업 흐름**

1. **비디오 입력 처리 및 라인 감지**  
   - 카메라에서 입력받은 프레임을 처리하여 라인의 위치를 감지합니다.
   - 화면의 중심과 라인의 중심 간의 에러를 계산하여 로봇이 라인에서 얼마나 벗어났는지 확인합니다.

2. **에러 계산**  
   - 화면 중심(x좌표)과 라인의 중심(x좌표) 간의 차이를 계산하여 **오차(error)**를 구합니다.  
     ```cpp
     error = (outputImg.cols / 2) - mainPoint.x;
     ```
   - 이 값이 음수면 로봇이 왼쪽으로 벗어난 것이고, 양수면 오른쪽으로 벗어난 것입니다.

3. **속도 계산 및 방향 제어**  
   - 에러를 기반으로 양쪽 모터 속도를 비례적으로 조정하여 방향을 제어합니다.
     ```cpp
     int leftvel = 100 - k * error;
     int rightvel = -(100 + k * error);
     ```
     - 왼쪽 모터(`leftvel`): 에러가 크면 속도를 낮춰 로봇이 오른쪽으로 회전.
     - 오른쪽 모터(`rightvel`): 에러가 크면 속도를 높여 로봇이 왼쪽으로 회전.
   - `k`는 보정 계수로, 에러에 따른 회전 강도를 조정합니다.

4. **Dynamixel 모터 제어**  
   - 계산된 속도를 Dynamixel 모터에 전달하여 로봇을 움직입니다.
     ```cpp
     dxl.setVelocity(leftvel, rightvel);
     ```

5. **키 입력 처리 및 종료 조건**  
   - `'q'`: 프로그램 종료.
   - `'s'`: 모터 작동 상태를 켜거나 끕니다.
   - `Ctrl+C`: 비상 종료를 처리합니다.

---

### **작동 원리 요약**
1. **센싱**: 비디오 입력에서 라인의 중심 위치를 감지.
2. **판단**: 라인과 화면 중심 간의 에러 계산.
3. **제어**: 모터 속도를 조정하여 에러를 보정.
4. **반복**: 이 과정을 반복하며 라인을 따라 이동.

---

### **라인트레이서의 동작 결과**
- 로봇은 감지된 라인의 중심을 기준으로 이동하며 경로를 벗어나지 않도록 제어됩니다.
- 장애물이 감지되거나 경로가 사라질 경우, 목표 지점(`mainPoint`)의 업데이트를 통해 다음 행동을 결정합니다.
- **실시간 키 입력**으로 작동 상태를 제어하거나 프로그램을 종료할 수 있습니다.
5번 실제 동작영상 (out 라인)


https://github.com/user-attachments/assets/f12ced1b-3158-4691-a7fc-aa249f986755



7번 실제 동작영상 (in 라인)


https://github.com/user-attachments/assets/b21756fb-2a4a-4f68-99b6-7f0034d88a46


---

### **응용**
이 라인트레이서는 도로의 차선을 따라가는 자율주행 차량, 공장의 이송 로봇, 또는 단순 교육용 로봇에 응용될 수 있습니다.
   
