REM using Anaconda with 'tensorflow14' env

REM building
javac @sources.txt -d .

REM Update PATH
setlocal
set PATH=%PATH%;../../../binaries/windows/x86_64

REM running
java -Djava.library.path=../../../binaries/windows/x86_64 Liveness --image ../../../assets/images/disguise.jpg --assets ../../../assets

endlocal