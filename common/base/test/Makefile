ALL_OBJS = AsyncLogging_test.out Atomic_unittest.out Timestamp_unittest.out

all: ${ALL_OBJS}

GCC = clang++
CFLAGES = -std=c++11 -ggdb -Wall -ffunction-sections -O0 -Wno-format \
 -DMYSQLPP_MYSQL_HEADERS_BURIED -DHAVE_SCHED_GET_PRIORITY_MAX -DLOG4CPP_FIX_ERROR_COLLISION -DLOG4CPP
LIBS = -L.. -lbase -pthread
RM = rm -rf

AsyncLogging_test.out: AsyncLogging_test.cc
	${GCC} ${CFLAGES} $^ -o $@ ${LIBS}

Atomic_unittest.out: Atomic_unittest.cc
	${GCC} ${CFLAGES} $^ -o $@ ${LIBS}

Timestamp_unittest.out: Timestamp_unittest.cc
	${GCC} ${CFLAGES} $^ -o $@ ${LIBS}

.PHONY:clean

clean:
	${RM} ${ALL_OBJS} *.log