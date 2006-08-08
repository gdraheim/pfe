@echo off

rem Batch file to compile pfe with EMX and Watcom C on OS/2
rem Builds directory bin.

set emxdir=E:\EMX
set watdir=D:\WATCOM

rm -rf bin
md bin

rem EMX binaries
rem ------------

cd src
make clean
bash config.sh EMX
make all
cd ..
md bin\EMX
copy src\config.h bin\EMX
copy src\Makefile bin\EMX
copy src\*.exe bin\EMX
copy %emxdir%\bin\emx.exe bin\EMX
copy %emxdir%\dll\emx.dll bin\EMX

rem Help files assuming prefix=c:\
rem ------------------------------

c:
rm -rf /pfe
md \lib
md \lib\pfe
md \lib\pfe\help
cd \lib\pfe\help
copy e:help\*.hlp
bash -c "e:bin/EMX/helpidx -o index *.hlp"
copy index e:bin\EMX
e:


rem Watcom binaries
rem ---------------

cd src
make clean
set TARGET=OS2V2
bash config.sh WATCOM
make TARGET=OS2V2 pfe.exe
cd ..
md bin\WC_OS2V2
copy src\config.h bin\WC_OS2V2
copy src\config.h src\config\WATCOM
copy src\Makefile bin\WC_OS2V2
copy src\pfe.exe bin\WC_OS2V2

cd src
make clean
set TARGET=DOS4G
bash config.sh WATCOM
make TARGET=DOS4G pfe.exe
cd ..
md bin\WC_DOS4G
copy src\config.h bin\WC_DOS4G
del src\config\WATCOM\config.h
copy src\Makefile bin\WC_DOS4G
copy src\pfe.exe bin\WC_DOS4G
copy %watdir%\bin\dos4gw.exe bin\WC_DOS4G
