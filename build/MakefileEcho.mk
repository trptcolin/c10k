#Set this to @ to keep the makefile quiet
SILENCE = @

PROJECT_NAME=EchoServer
COMPONENT_NAME = $(PROJECT_NAME)
TARGET_LIB = \
	lib/lib$(COMPONENT_NAME).a

TEST_TARGET = \
	out/$(COMPONENT_NAME)

PROJECT_HOME_DIR = .
CPP_PLATFORM = Gcc

SRC_DIRS= \
				 $(PROJECT_HOME_DIR)/src/$(PROJECT_NAME)

TEST_SRC_DIRS= \
							 $(PROJECT_HOME_DIR)/test \
							 $(PROJECT_HOME_DIR)/test/$(PROJECT_NAME) \

INCLUDE_DIRS = \
	. \
  $(CPPUTEST_HOME)/include \
  $(PROJECT_HOME_DIR)/include

CPPUTEST_HOME=./cpputest
CPPUTEST_WARNINGFLAGS=-Wall

include $(CPPUTEST_HOME)/build/MakefileWorker.mk

acceptance-test: clean $(TEST_TARGET)
	test/echo_test.sh

