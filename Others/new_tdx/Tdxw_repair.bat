@echo off
echo 注册WMI服务...
regsvr32 /s wmidcprv.dll
regsvr32 /s wmiprvsd.dll
echo 注册完成...
pause

