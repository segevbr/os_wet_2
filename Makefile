CXX = g++

CXXFLAGS = -std=c++17 -g -Wall -Werror -pedantic-errors -DNDEBUG -pthread

SRCS = account.cpp atm.cpp bank.cpp bank_exc.cpp reader_writer.cpp log.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = bank

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)