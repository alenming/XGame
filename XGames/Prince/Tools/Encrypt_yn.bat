
set AES_TOOLS_PATH=TEATools.jar
set INPUT_DIR=..\DataSource_yn
set OUTPUT_DIR=..\Resources\Data
set TYPE=encrypt

java -jar %AES_TOOLS_PATH% %INPUT_DIR% %OUTPUT_DIR% %TYPE%

pause