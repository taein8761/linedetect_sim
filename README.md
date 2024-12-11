# linedetect_sim

# 1. 라인 검출

설명: 프로그램이 실행 중에 Ctrl+C 입력을 감지하기 위해 사용하는 변수입니다.

목적: 프로그램 종료를 위해 신호 처리 함수(signalHandler)에서 이 값을 변경합니다
```
volatile bool ctrl_c_pressed = false;
```
##
기능: 비디오 프레임을 전처리하여 관심 영역(ROI)을 설정하고 이진화된 이미지를 생성합니다.

작업 흐름: 비디오 프레임 읽기: source >> frame으로 한 프레임을 가져옵니다.

그레이스케일 변환: cvtColor로 이미지를 흑백으로 변환.

밝기 조정: 이미지의 평균 밝기를 기준으로 조정.

이진화: 밝기가 임계값(128) 이상이면 흰색, 아니면 검은색으로 변환.

ROI 설정: 화면의 아래쪽 90px만 잘라 관심 영역으로 지정.

결과: bin에 관심 영역의 이진화된 이미지가 저장됩니다.

```
// 전처리 및 ROI(관심영역) 설정
void preprocess(VideoCapture& source, Mat& frame, Mat& gray, Mat& bin)
```


5번 영상 (out 라인)
https://github.com/user-attachments/assets/2c7b4cca-d7ea-4661-aa09-77735ee17bd1

7번 영상 (in 라인)
https://github.com/user-attachments/assets/532c0771-29b6-48f9-ac69-ae82a88e48b7

# 2. 모션 제어

모터속도비교
https://github.com/user-attachments/assets/8ea1fd0f-e510-4484-964e-03f432a571b9

# 3. 라인 트레이서
   
