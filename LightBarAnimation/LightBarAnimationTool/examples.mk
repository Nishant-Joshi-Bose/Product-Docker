CURRENT_DIR  = ${PWD}
MONGOOSE_DIR = ${CURRENT_DIR}
#SOURCES      = $(PROG).c ${MONGOOSE_DIR}/mongoose.c
SOURCES      = mainLB.cpp LightBarAnimation.cpp ${MONGOOSE_DIR}/mongoose.c
CFLAGS       = -fpermissive -I${MONGOOSE_DIR} -Wno-unused-function $(CFLAGS_EXTRA) $(MODULE_CFLAGS)

$(info MONGOOSE_DIR: ${MONGOOSE_DIR})

all: $(PROG)

ifeq ($(OS), Windows_NT)
# TODO(alashkin): enable SSL in Windows
CFLAGS += -lws2_32 
CC = g++
else
CFLAGS += -pthread -std=c++11
endif

ifeq ($(SSL_LIB),openssl)
CFLAGS += -DMG_ENABLE_SSL -lssl -lcrypto
endif
ifeq ($(SSL_LIB), krypton)
CFLAGS += -DMG_ENABLE_SSL ../../../krypton/krypton.c -I../../../krypton
endif
ifeq ($(SSL_LIB),mbedtls)
CFLAGS += -DMG_ENABLE_SSL -DMG_SSL_IF=MG_SSL_IF_MBEDTLS -DMG_SSL_MBED_DUMMY_RANDOM -lmbedcrypto -lmbedtls -lmbedx509
endif

ifeq ($(JS), yes)
	V7_PATH = ../../deps/v7
	CFLAGS_EXTRA += -DMG_ENABLE_JAVASCRIPT -I $(V7_PATH) $(V7_PATH)/v7.c
endif

$(PROG): $(SOURCES)
	$(CXX) $(SOURCES) -o $@ $(CFLAGS)
	#$(CC) $(SOURCES) -o $@ $(CFLAGS)

$(PROG).exe: $(SOURCES)
	cl $(SOURCES) /I../.. /MD /Fe$@

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)
