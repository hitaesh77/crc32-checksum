CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = crc32_tests

SRCS = main.cpp \
       checksum.cpp \
       test_common.cpp \
       tick_test_utils.cpp \
       test_crc32_correctness.cpp \
       benchmark_crc32.cpp

run:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	./$(TARGET)

run-zlib:
	$(CXX) $(CXXFLAGS) -DHAS_ZLIB $(SRCS) -lz -o $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)