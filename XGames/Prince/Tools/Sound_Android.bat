
set SOUND_EFFECT_PATH=..\SoundSource\effect_Android

del /s /q ..\Resources\Sound\music
copy ..\SoundSource\music ..\Resources\Sound\music

del /s /q ..\Resources\Sound\effect
copy %SOUND_EFFECT_PATH% ..\Resources\Sound\effect

pause