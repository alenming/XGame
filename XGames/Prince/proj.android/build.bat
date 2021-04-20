@echo off

REM 
echo 请选择打包方式:
echo.
echo     0 - Debug    1 - Release
echo.

:start
set /p mode="输入数字(q退出):"
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
    echo 无效值！
    goto start
  )
)

REM 
cls
echo 请选择国内或者海外SDK:
echo.
echo     0 - China(大陆)    1 - OverSeas(海外)
echo.

:Sdkstart
set /p sdk="输入数字(q退出):"
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
    echo 无效值！
    goto Sdkstart
  )
)

REM 信息输出确认
cls
echo ***************************** check *****************************
echo 打包模式: %modestr%
echo 打包SDK版本: %sdk_mode%
echo 当前路径: %~dp0
echo *****************************************************************
set /p choice="请核查以上信息，确认开始打包？(y/n)"
if /i "%choice%"=="n" (
  exit
)
echo.

REM 加密数据表
echo 1.正在加密数据表...
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

REM 拷贝Img资源
echo 2.正在拷贝Img资源...
rmdir /s /q %~dp0..\Resources\Image
if "%sdk_mode%" == "china" (
     xcopy %~dp0..\ImageTiny\* %~dp0..\Resources /s /e
  ) else (
     xcopy %~dp0..\ImageTiny_yn\* %~dp0..\Resources /s /e
  )
echo.

REM 拷贝Sound资源
echo 3.正在拷贝Sound资源...
set SOUND_EFFECT_PATH=%~dp0..\SoundSource\effect_Android
del /s /q %~dp0..\Resources\Sound\music
copy %~dp0..\SoundSource\music %~dp0..\Resources\Sound\music
del /s /q %~dp0..\Resources\Sound\effect
copy %SOUND_EFFECT_PATH% %~dp0..\Resources\Sound\effect
echo.
cls

REM 设置项目依赖lib库目录 修改Manifest配置
if "%sdk_mode%" == "china" (
     copy %~dp0\propertiesChina\project.properties %~dp0\project.properties
     xcopy /y %~dp0..\LB_UpdateSDK\sdk\res\raw\* %~dp0\res\raw /s /e
     python ./sdk_script.py -s 61280
  ) else (
     copy %~dp0\propertiesOverseas\project.properties %~dp0\project.properties
     xcopy /y %~dp0..\LB_UpdateSDK_OverSeas\sdk\res\raw\* %~dp0\res\raw /s /e
     python ./sdk_script.py -s 61281
  )

echo 4.开始编译...

REM 清理工程
cmd /c "ant clean"

REM ndk编译
python ./build_native.py -s %sdk_mode%

REM 工程编译并打包apk
cmd /c "ant %modestr%"
echo.

REM 母包命名规则
set datetime=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
set final-pkg-name=prince_ori_%datetime%_%modestr%.apk

REM 签名
@echo off
if %mode% == 0 (
  ren %~dp0bin\Prince-debug.apk %final-pkg-name%
  del %~dp0bin\Prince-debug-unaligned.apk
  del %~dp0bin\Prince-debug-unaligned.apk.d
) else (
  REM 签名apk(仅release模式需要显式签名，debug模式下自动加入debug签名)
  echo 5.正在签名...
  jarsigner -verbose -keystore prince.keystore -storepass zcjoyprince -signedjar %~dp0bin\%final-pkg-name% %~dp0bin\Prince-release-unsigned.apk android ^
  -digestalg SHA1 -sigalg MD5withRSA
  REM -tsa https://timestamp.geotrust.com/tsa
  del %~dp0bin\Prince-release-unsigned.apk
  del %~dp0bin\Prince-release-unsigned.apk.d
)
echo.

echo *************************** 打包完成! ***************************
set pkgdir=%~dp0bin\
echo 默认存放路径: %pkgdir%
echo 文件名: %final-pkg-name%
echo.

REM 拷贝apk
set /p iscopy="是否拷贝至U8SDK对应目录？(y/n)"
if /i "%iscopy%"=="y" (
  copy /y %pkgdir%%final-pkg-name% %~dp0..\SDK\U8SDKTool-Win-P34\games\prince\u8.apk
)

PAUSE