@echo off

REM 
echo ��ѡ������ʽ:
echo.
echo     0 - Debug    1 - Release
echo.

:start
set /p mode="��������(q�˳�):"
if %mode% LEQ 1 (
  if %mode% == 1 (
      set modestr=release
  ) else (
      set modestr=debug
  )
) else (
  if "%mode%" == "q" (
    exit
  ) else (
    echo ��Чֵ��
    goto start
  )
)

REM 
cls
echo ��ѡ����ڻ��ߺ���SDK:
echo.
echo     0 - China(��½)    1 - OverSeas(����)
echo.

:Sdkstart
set /p sdk="��������(q�˳�):"
if %sdk% LEQ 1 (
  if %sdk% == 1 (
      set sdk_mode=overSeas
  ) else (
      set sdk_mode=china
  )
) else (
  if "%sdk%" == "q" (
    exit
  ) else (
    echo ��Чֵ��
    goto Sdkstart
  )
)

REM ��Ϣ���ȷ��
cls
echo ***************************** check *****************************
echo ���ģʽ: %modestr%
echo ���SDK�汾: %sdk_mode%
echo ��ǰ·��: %~dp0
echo *****************************************************************
set /p choice="��˲�������Ϣ��ȷ�Ͽ�ʼ�����(y/n)"
if /i "%choice%"=="n" (
  exit
)
echo.

REM �������ݱ�
echo 1.���ڼ������ݱ�...
set AES_TOOLS_PATH=%~dp0..\Tools\TEATools.jar
if "%sdk_mode%" == "china" (
     set INPUT_DIR=%~dp0..\DataSource
  ) else (
     set INPUT_DIR=%~dp0..\DataSource_yn
  )
set OUTPUT_DIR=%~dp0..\Resources\Data
set TYPE=encrypt
java -jar %AES_TOOLS_PATH% %INPUT_DIR% %OUTPUT_DIR% %TYPE%
echo.

REM ����Img��Դ
echo 2.���ڿ���Img��Դ...
rmdir /s /q %~dp0..\Resources\Image
if "%sdk_mode%" == "china" (
     xcopy %~dp0..\ImageTiny\* %~dp0..\Resources /s /e
  ) else (
     xcopy %~dp0..\ImageTiny_yn\* %~dp0..\Resources /s /e
  )
echo.

REM ����Sound��Դ
echo 3.���ڿ���Sound��Դ...
set SOUND_EFFECT_PATH=%~dp0..\SoundSource\effect_Android
del /s /q %~dp0..\Resources\Sound\music
copy %~dp0..\SoundSource\music %~dp0..\Resources\Sound\music
del /s /q %~dp0..\Resources\Sound\effect
copy %SOUND_EFFECT_PATH% %~dp0..\Resources\Sound\effect
echo.
cls

REM ������Ŀ����lib��Ŀ¼ �޸�Manifest����
if "%sdk_mode%" == "china" (
     copy %~dp0\propertiesChina\project.properties %~dp0\project.properties
     xcopy /y %~dp0..\LB_UpdateSDK\sdk\res\raw\* %~dp0\res\raw /s /e
     python ./sdk_script.py -s 61280
  ) else (
     copy %~dp0\propertiesOverseas\project.properties %~dp0\project.properties
     xcopy /y %~dp0..\LB_UpdateSDK_OverSeas\sdk\res\raw\* %~dp0\res\raw /s /e
     python ./sdk_script.py -s 61281
  )

echo 4.��ʼ����...

REM ������
cmd /c "ant clean"

REM ndk����
python ./build_native.py -s %sdk_mode%

REM ���̱��벢���apk
cmd /c "ant %modestr%"
echo.

REM ĸ����������
set datetime=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
set final-pkg-name=prince_ori_%datetime%_%modestr%.apk

REM ǩ��
@echo off
if %mode% == 0 (
  ren %~dp0bin\Prince-debug.apk %final-pkg-name%
  del %~dp0bin\Prince-debug-unaligned.apk
  del %~dp0bin\Prince-debug-unaligned.apk.d
) else (
  REM ǩ��apk(��releaseģʽ��Ҫ��ʽǩ����debugģʽ���Զ�����debugǩ��)
  echo 5.����ǩ��...
  jarsigner -verbose -keystore prince.keystore -storepass zcjoyprince -signedjar %~dp0bin\%final-pkg-name% %~dp0bin\Prince-release-unsigned.apk android ^
  -digestalg SHA1 -sigalg MD5withRSA
  REM -tsa https://timestamp.geotrust.com/tsa
  del %~dp0bin\Prince-release-unsigned.apk
  del %~dp0bin\Prince-release-unsigned.apk.d
)
echo.

echo *************************** ������! ***************************
set pkgdir=%~dp0bin\
echo Ĭ�ϴ��·��: %pkgdir%
echo �ļ���: %final-pkg-name%
echo.

REM ����apk
set /p iscopy="�Ƿ񿽱���U8SDK��ӦĿ¼��(y/n)"
if /i "%iscopy%"=="y" (
  copy /y %pkgdir%%final-pkg-name% %~dp0..\SDK\U8SDKTool-Win-P34\games\prince\u8.apk
)

PAUSE