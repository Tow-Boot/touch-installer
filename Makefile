#
# Makefile
#
CC ?= gcc
AR ?= ar
LVGL_DIR ?= ${shell pwd}/lvgui
PREFIX ?= /usr

BINARY = tbgui
VERSION = 0.0.1

LVGL_ENV_SIMULATOR ?= 1

# External components
include $(LVGL_DIR)/lvgui.mk

# Additional source files
CSRCS += ./main.c

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS)
OBJS = $(AOBJS) $(COBJS)

all: default

%.o: %.c $(CONFFILES)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

default: $(BINARY)
    
tbgui: $(AOBJS) $(COBJS) $(CONFFILES)
	$(CC) -o $@ -fPIC $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BINARY) $(AOBJS) $(COBJS)

install:
	install -vD $(BINARY) $(PREFIX)/bin
