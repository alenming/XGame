
rmdir /s /q ..\..\build\Debug.win32\Data\*
rmdir /s /q ..\..\build\Debug.win32\fonts\*
rmdir /s /q ..\..\build\Debug.win32\Image\*
rmdir /s /q ..\..\build\Debug.win32\Shader\*
rmdir /s /q ..\..\build\Debug.win32\Sound\*
rmdir /s /q ..\..\build\Debug.win32\Video\*

REM �������ݱ�
set AES_TOOLS_PATH=TEATools.jar
set INPUT_DIR=..\DataSource_yn
set OUTPUT_DIR=..\Resources\Data
set TYPE=encrypt
java -jar %AES_TOOLS_PATH% %INPUT_DIR% %OUTPUT_DIR% %TYPE%

REM ����ResourcesĿ¼�³�Image�ļ�����������ļ���Debug.win32Ŀ¼��
xcopy ..\Resources\Data\* ..\..\build\Debug.win32\Data /s /e /y
xcopy ..\Resources\fonts\* ..\..\build\Debug.win32\fonts /s /e /y
xcopy ..\Resources\Shader\* ..\..\build\Debug.win32\Shader /s /e /y
xcopy ..\Resources\Sound\* ..\..\build\Debug.win32\Sound  /s /e /y
xcopy ..\Resources\Video\* ..\..\build\Debug.win32\Video /s /e /y

REM ����Խ����ԴResources_ynĿ¼�µ������ļ���ResourcesĿ¼��
xcopy ..\Resources_yn\* ..\..\build\Debug.win32 /s /e /y

REM xcopy ..\Resources\* ..\..\build\Debug.win32\ /s /e /y

xcopy mvc_dll\* ..\..\build\Debug.win32 /y

7z a -t7z Prince.7z ..\..\build\Debug.win32

pause