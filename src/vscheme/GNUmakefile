export SHELL = /bin/ksh

TOP := ../..

BUILD = $(TOP)/build
BIN = $(TOP)/bin

.PHONY: all

all: $(BIN)/vscheme $(BIN)/vscheme.opt

$(BIN)/vscheme.opt: vscheme.sml
	mlton -output $@ -verbose 1 $<

$(BIN)/vscheme: vscheme.sml
	mosmlc -o $@ $<

.PHONY: moscow

moscow: $(BIN)/vscheme

.PHONY: mosml

mosml: $(BIN)/vscheme
