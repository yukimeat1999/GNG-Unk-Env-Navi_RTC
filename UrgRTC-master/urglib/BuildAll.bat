@echo ���̃X�N���v�g�����s����ɂ́Acmake�����msbuild�̑o����PATH�ɓ����Ă���K�v������܂��Bcmake�̃p�X�́AC:\Program Files (x86)\CMake 2.8\bin�Bmsbuild�̃p�X�́AC:\Windows\Microsoft.NET\Framework\v4.0.30319�Ƃ��āA���������܂����A���̈Ⴂ����r���h�ł��Ȃ��ꍇ�́A�蓮�ŃX�N���v�g�̃p�X��ύX���Ă�������

@timeout 5

@call :BUILDW urg


@timeout 10
@GOTO :EOF

:BUILDW
@if NOT EXIST build-win32 mkdir build-win32
@cd build-win32
@cmake ../
@msbuild %1.sln /p:Configuration=Release /p:Platform=WIN32
@cd ..
@exit /b