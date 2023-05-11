TARGET    = ./bin/$(shell basename `readlink -f .`)
INCLUDES  = -I./include
LDFLAGS   =
NOMAKEDIR =
OBJDIR    = obj
GCC = g++
CFLAGS = -g -O0 -MMD -MP -Wall -ljpeg -lm -std=c++14 -Wextra -Wpedantic -Werror

SRCS = $(shell find ./src/* -name *.cpp)
DIRS = $(dir $(SRCS))
BINDIRS = $(addprefix $(OBJDIR)/, $(DIRS))

OBJS = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS = $(OBJS:.o=.d)
TILS = $(patsubst %.cpp, %.cpp~, $(SRCS))

ifeq "$(strip $(OBJDIR))" ""
    OBJDIR = .
endif

ifeq "$(strip $(DIRS))" ""
    OBJDIR = .
endif

default:
	@[ -d  $(OBJDIR)   ] || mkdir -p $(OBJDIR)
	@[ -d "$(BINDIRS)" ] || mkdir -p $(BINDIRS)
	@make all --no-print-directory
#	./$(TARGET)

all : $(OBJS) $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(GCC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(TARGET) $(TILS) $(OBJDIR)

-include $(DEPS)