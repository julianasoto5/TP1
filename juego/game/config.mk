# Compile options
VERBOSE=n
OPT=g
USE_NANO=y
SEMIHOST=n
USE_FPU=y

# Libraries
USE_LPCOPEN=y
USE_SAPI=y
USE_FREERTOS=y
FREERTOS_HEAP_TYPE=4

DEFINES+=SAPI_USE_INTERRUPTS

SRC += $(wildcard $(PROGRAM_PATH_AND_NAME)/src/drivers/*.c)
SRC += $(wildcard $(PROGRAM_PATH_AND_NAME)/src/tareas/*.c)


