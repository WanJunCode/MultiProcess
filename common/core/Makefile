.PHONY:all clean
#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT := libcore.a

OUTPUT_INCLUDE := ../includes/core
OUTPUT_LIBS    := ../libs

all: ${OUTPUT}
	cp ${OUTPUT} ${OUTPUT_LIBS}

#****************************************************************************
# Source files
#****************************************************************************
#
C_SRCS=$(shell find -iname "*.c")
#
CXX_SRCS=$(shell find -iname "*.cpp")

OBJS=$(subst .c,.o,$(C_SRCS))
OBJS+=$(subst .cpp,.o,$(CXX_SRCS))

#****************************************************************************
# Output
#****************************************************************************
${OBJS}:$(C_SRCS) $(CXX_SRCS)
	${CXX} ${CXXFLAGS} $(C_SRCS) $(CXX_SRCS)

${OUTPUT}: ${OBJS}
	${AR} rc $@ ${OBJS} ${LIBS} ${EXTRA_LIBS}
#	${LD} -shared -fPIC -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

clean:
	-rm -f core ${OBJS} ${OUTPUT}
	-rm -f ${OUTPUT_LIBS}/${OUTPUT}
