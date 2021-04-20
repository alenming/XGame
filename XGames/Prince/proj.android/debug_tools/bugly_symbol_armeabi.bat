set datetime=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
java -jar bugly/buglySymbolAndroid.jar -i ../obj -o symbol_%datetime%.zip

pause