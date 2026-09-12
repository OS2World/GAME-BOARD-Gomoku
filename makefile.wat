# makefile.wat - OpenWatcom wmake build for Gomoku
# Usage: wmake -f makefile.wat [all | clean]

CC      = wcc386
RC      = wrc
LINK    = wlink
CFLAGS  = -bt=os2 -d0 -ox -w4 -ze -zq -mf -i=$(SRC)
SRC     = src
BIN     = bin

.BEFORE
	@if not exist $(BIN) mkdir $(BIN)

all: $(BIN)\gomoku.exe

$(BIN)\gomoku.obj: $(SRC)\gomoku.c $(SRC)\gomoku.h
	$(CC) $(CFLAGS) -fo=$(BIN)\gomoku.obj $(SRC)\gomoku.c

$(BIN)\gomoku.res: $(SRC)\gomoku.rc $(SRC)\gomoku.h $(SRC)\about.dlg $(SRC)\box.bmp
	$(RC) -r -i=$(SRC) -fo=$(BIN)\gomoku.res $(SRC)\gomoku.rc

$(BIN)\gomoku.exe: $(BIN)\gomoku.obj $(BIN)\gomoku.res
	$(LINK) system os2v2 pm &
	    name $(BIN)\gomoku.exe &
	    file $(BIN)\gomoku.obj &
	    @$(SRC)\gomoku.def
	$(RC) $(BIN)\gomoku.res $(BIN)\gomoku.exe

clean: .SYMBOLIC
	@if exist $(BIN)\gomoku.obj del $(BIN)\gomoku.obj
	@if exist $(BIN)\gomoku.res del $(BIN)\gomoku.res
	@if exist $(BIN)\gomoku.exe del $(BIN)\gomoku.exe
	@if exist $(BIN)\gomoku.map del $(BIN)\gomoku.map
