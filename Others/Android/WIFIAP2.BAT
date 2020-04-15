echo off

mode con cols=52 lines=22

title win7无线路由 By踏雪无痕

color 5f

cls

:wifi

echo.

echo     ╭--------------------------------------╮

echo    ┊         win7虚拟WiFi自动设置           ┃

echo    ┝----------------------------------------┫

echo    ┊     自动WiFi设置                       ║

echo    ┊   手机，电脑共享上网                   ║

echo    ┊ 请关闭防护墙及部分杀毒软件可正常使用   ║

echo    ┊                            By HL       ║

echo    ┊                                        ║

echo    ┊                         2013.5         ║

echo    ╰=======================================-╯

echo.



:AUTO  

echo 开启必要服务

sc config   MpsSvc start= AUTO

sc start MpsSvc

sc config   LanmanServer  start= AUTO

sc start LanmanServer

sc config   LanmanWorkstation start= AUTO

sc start LanmanWorkstation

sc config   Browser start= AUTO

sc start Browser

sc config   Dhcp start= AUTO

sc start Dhcp

sc config   RpcSs start= AUTO

sc start RpcSs

sc config   RpcLocator start= AUTO

sc start RpcLocator

sc config   Dnscache start= AUTO

sc start Dnscache

sc config   FDResPub start= AUTO

sc start FDResPub

sc config   upnphost start= AUTO

sc start upnphost

sc config   SSDPSRV start= AUTO

sc start   SSDPSRV

sc config   lmhosts start= AUTO

sc start lmhosts

echo.设置wifi账号

netsh wlan set hostednetwork mode=allow

netsh wlan set hostednetwork ssid=WIN7WIFI2 

echo. 设置密码

netsh wlan set hostednetwork key=1234567800

netsh wlan start hostednetwork

echo.

cls

:d

color 2f

cls

title 查看WIFI运行状态

echo.

netsh wlan show hostednetwork
netsh wlan show drivers

echo.

pause

echo 如果失败，启用虚拟网卡：Microsoft Virtual Wifi Miniport...
pause