@ECHO OFF

ECHO Generating to Visual Studio Project...

IF NOT EXIST Win32 MKDIR Win32

CD Win32
CMAKE -DCMAKE_GENERATOR_PLATFORM=x64 ../
CMAKE --build . --config Release
CD..

PAUSE