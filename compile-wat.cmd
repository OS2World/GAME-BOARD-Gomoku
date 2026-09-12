@echo off
rem compile-wat.cmd - Build Gomoku with OpenWatcom on ArcaOS/OS2
rem Redirects output to compile-wat.log

set LOGFILE=compile-wat.log

echo. > %LOGFILE%
echo ===== Gomoku OpenWatcom Build ===== >> %LOGFILE%
echo Date: %DATE% %TIME% >> %LOGFILE%
echo. >> %LOGFILE%

echo Building Gomoku (OpenWatcom)...

if not exist bin mkdir bin 2>>%LOGFILE%

echo --- Compiling gomoku.c --- >> %LOGFILE%
wcc386 -bt=os2 -d0 -ox -w4 -ze -zq -mf -i=src -fo=bin\gomoku.obj src\gomoku.c >> %LOGFILE% 2>&1
if errorlevel 1 goto :err

echo --- Compiling resources --- >> %LOGFILE%
wrc -r -i=src -fo=bin\gomoku.res src\gomoku.rc >> %LOGFILE% 2>&1
if errorlevel 1 goto :err

echo --- Linking --- >> %LOGFILE%
wlink system os2v2 pm name bin\gomoku.exe file bin\gomoku.obj @src\gomoku.def >> %LOGFILE% 2>&1
if errorlevel 1 goto :err

echo --- Binding resources --- >> %LOGFILE%
wrc bin\gomoku.res bin\gomoku.exe >> %LOGFILE% 2>&1
if errorlevel 1 goto :err

echo. >> %LOGFILE%
echo Build SUCCESSFUL. >> %LOGFILE%
echo Build SUCCESSFUL. See %LOGFILE% for details.
goto :eof

:err
echo. >> %LOGFILE%
echo Build FAILED. >> %LOGFILE%
echo Build FAILED. See %LOGFILE% for errors.
exit 1
