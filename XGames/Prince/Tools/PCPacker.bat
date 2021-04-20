
rmdir /s /q ..\..\build\Debug.win32\Data
rmdir /s /q ..\..\build\Debug.win32\fonts
rmdir /s /q ..\..\build\Debug.win32\Image
rmdir /s /q ..\..\build\Debug.win32\Shader
rmdir /s /q ..\..\build\Debug.win32\Sound
rmdir /s /q ..\..\build\Debug.win32\Video

REM 加密数据表
set AES_TOOLS_PATH=TEATools.jar
set INPUT_DIR=..\DataSource
set OUTPUT_DIR=..\Resources\Data
set TYPE=encrypt
java -jar %AES_TOOLS_PATH% %INPUT_DIR% %OUTPUT_DIR% %TYPE%

REM 拷贝Resources目录下的所有文件到Debug.win32目录下
xcopy ..\Resources\* ..\..\build\Debug.win32 /s /e /y
xcopy mvc_dll\* ..\..\build\Debug.win32 /y

7z a -t7z Prince.7z ..\..\build\Debug.win32

pause