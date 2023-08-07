pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Protocol.pb.h "../../../Server"
XCOPY /Y Protocol.pb.cc "../../../Server"
XCOPY /Y Protocol.pb.h "../../../Source/MMORPG2/Network"
XCOPY /Y Protocol.pb.cc "../../../Source/MMORPG2/Network"


DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE