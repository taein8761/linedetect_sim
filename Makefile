CX = g++
CXFLAGS = -g -Wall -I/usr/local/include/dynamixel_sdk `pkg-config opencv4 --cflags`
LDFLAGS = -ldxl_x64_cpp `pkg-config opencv4 --libs`
TARGET = vision
OBJS = main.o dxl.o vision.o  # vision.o 파일을 추가

$(TARGET) : $(OBJS)
	$(CX) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o : main.cpp
	$(CX) $(CXFLAGS) -c main.cpp

dxl.o : dxl.hpp dxl.cpp
	$(CX) $(CXFLAGS) -c dxl.cpp

vision.o : vision.hpp vision.cpp  # vision.cpp, vision.hpp에 대한 규칙 추가
	$(CX) $(CXFLAGS) -c vision.cpp

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
