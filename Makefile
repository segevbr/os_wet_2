CXX = g++

CXXFLAGS = -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG -pthread

SRCS = account.cpp atm.cpp atm_commands.cpp bank.cpp bank_exc.cpp reader_writer.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = bank

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)