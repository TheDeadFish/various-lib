call egcc.bat
pushd obj
gcc %CCFLAGS2% ..\*.cc -c
gcc %CFLAGS2% ..\*.c -c
popd
copy /Y *.h %PROGRAMS%\local\include
ar -rcs  %PROGRAMS%\local\lib32\libexshit.a obj\*.o
