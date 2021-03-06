#region --- Au3Recorder generated code Start (v3.3.9.5 KeyboardLayout=00000804)  ---
#include <WindowsConstants.au3>
#include <GuiTreeView.au3>
#include <GuiScrollBars.au3>
#include <Array.au3>
#include <WinAPIShPath.au3>

#region --- Internal functions Au3Recorder Start ---
Func _Au3RecordSetup()
Opt('WinWaitDelay',100)
Opt('WinDetectHiddenText',1)
Opt('MouseCoordMode',0)
EndFunc

Func _WinWaitActivate($title,$text,$timeout=0)
	WinWait($title,$text,$timeout)
	If Not WinActive($title,$text) Then WinActivate($title,$text)
	WinWaitActive($title,$text,$timeout)
 EndFunc

Func _WaitCtrlEnable($ctrlID)
   Sleep(200)
   While (1 <> ControlCommand ($WIN_TITLE, "", $ctrlID, "IsEnabled", "" ))
	  Sleep(50)
	  OutDbgStr("Wait ctrl IsEnabled" & @CRLF)
   WEnd
EndFunc

Func _WaitCtrlVisible($ctrlID)
   While (1 <> ControlCommand ($WIN_TITLE, "", $ctrlID, "IsVisible", "" ))
	  Sleep(50)
	  OutDbgStr("Wait ctrl IsVisible" & @CRLF)
   WEnd
EndFunc

_AU3RecordSetup()
$WIN_TITLE = "华安证券徽赢PC版V7"
$ctrlTreeViewID = "SysTreeView321"
$ctrlBuyBtnID = "[CLASS:Button; INSTANCE:23]"
$ctrlSaleBtnID = "[CLASS:Button; INSTANCE:41]"
$ctrlRefreshBtnID = "[CLASS:Button; INSTANCE:57]"
$ctrlJYBtnID = "AfxWnd4212"

#CS  While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ) = 0
### ;~  While WinGetHandle("发现了新版本") = 0
###   	Sleep(50)
###  WEnd
###  ConsoleWrite(ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ))
###  ;ControlClick("[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
###  ControlClick( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
###
###  Exit;
 #CE

Local $aCmdLine = _WinAPI_CommandLineToArgv($CmdLineRaw)
;~ ConsoleWrite($aCmdLine[0]& @CRLF)
;~ ConsoleWrite($aCmdLine[1]& @CRLF)
;~ ConsoleWrite($aCmdLine[2]& @CRLF)
;~ ConsoleWrite($aCmdLine[3]& @CRLF)
;Exit

#CS If ($aCmdLine[0] ==3) Then;一个参数;1 DBG
### 	  _RunTDXHuaan($aCmdLine[3]);1 DBG
### EndIf
 #CE

If ($aCmdLine[0] ==1) Then;一个参数;1 DBG
	  _RunTDXHuaan($aCmdLine[1]);1 DBG
EndIf

Exit

Func _RunTDXHuaan($pathTDX)

While WinExists ( $WIN_TITLE ) = 1
    WinKill ( $WIN_TITLE )
	Sleep(500)
WEnd

If WinActivate ( $WIN_TITLE ) = 0 Then
    ShellExecute ( $pathTDX )
   _WinWaitActivate($WIN_TITLE,"")
EndIf


Local $hWnd = WinWaitActive($WIN_TITLE,"")
ConsoleWrite("=====" & $hWnd& @CRLF)

Sleep(500)

While StringInStr(ControlGetFocus($hWnd),"SafeEdit2")= 0
  	Sleep(50)
	ControlFocus($hWnd, "", "SafeEdit2")
WEnd
ConsoleWrite(ControlGetFocus($hWnd)& @CRLF)

;MsgBox($MB_SYSTEMMODAL, "", $hWnd&"The control that has keyboard focus is: " & ControlGetFocus($hWnd))

Sleep(500)
Send("{BACKSPACE}"); DBG
Send("{BACKSPACE}"); DBG
Send("{BACKSPACE}"); DBG

Send("9854621{ENTER}"); DBG

#CS   While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ) = 0
### ;~  While WinGetHandle("发现了新版本") = 0
### 	  	  ControlClick( "[TITLE:消息标题:; CLASS:#32770]", "", "关闭" )
###
###   	Sleep(50)
###   WEnd
###   ;Sleep(500)
### ;MsgBox($MB_SYSTEMMODAL, "Title", "This message box will timeout after 10 seconds or select the OK button.", 1)
###
###   While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
### 	 Sleep(50)
### 	  ControlClick( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
###    WEnd
 #CE
  ;ConsoleWrite(WinGetHandle("发现了新版本"))
   ;ControlEnable("发现了新版本", "关闭", "Button2")

;~    WinSetState(WinGetHandle("发现了新版本"), "", @SW_HIDE)
;~    ConsoleWrite(WinGetHandle($WIN_TITLE))
;~    WinSetState(WinGetHandle($WIN_TITLE), "", @SW_ENABLE )

;WinSetState($WIN_TITLE,"",@SW_MAXIMIZE)

   $hWnd =   WinWait("发现了新版本", "", 10)
	Sleep(1500)

;ControlClick($hWnd, "关闭", "")
ControlClick($hWnd, "", "Button2")
   ;WinClose($hWnd)


$hWnd =   WinWait("[TITLE:" & $WIN_TITLE & "; CLASS:TdxW_MainFrame_Class]", "", 10)
;WinSetState($hWnd, "", @SW_MINIMIZE )
WinSetState($hWnd, "", @SW_HIDE  )
Sleep(1000)

ConsoleWrite("打开自选股"& @CRLF & $hWnd)
;打开自选股
   Local Const $WM_COMMAND = 273
   Local Const $WM_SYSCOMMAND = 274

   ;Local Const $CMD_ZXG = 570
   _SendMessage($hWnd, $WM_COMMAND, 570, 0) ;自选股

   Opt("WinTitleMatchMode", 2);1=start, 2=subStr, 3=exact, 4=advanced, -1 to -4=Nocase

   $hWnd =   WinWait("自选股", "", 10)
   _SendMessage($hWnd, $WM_COMMAND, 33235, 0) ;多股同列

   $hWnd =   WinWait("多股同列", "", 10)

   _SendMessage($hWnd, $WM_COMMAND, 4200, 0) ;	"5分钟图\n5分钟图"
WinSetState($hWnd, "", @SW_MINIMIZE )
WinSetState($hWnd, "", @SW_SHOW  )


Exit
  ;_SendMessage($hWnd, WM_COMMAND, MAKEWPARAM(5333, 0), NULL);
  ;点击【】
  MouseClick("left",521,174,1)

  Sleep(1000)
  ;点击【信用】
  MouseClick("left",152,754,1)

  ;点击买入、卖出、资金股份1

  _ClickTreeViewItem("买入", $ctrlBuyBtnID, 198, 9)
  _ClickTreeViewItem("卖出", $ctrlSaleBtnID, 198, 29)
  _ClickTreeViewItem("资金股份", $ctrlRefreshBtnID, 198, 290)
  _ClickTreeViewItem("卖出", $ctrlSaleBtnID, 198, 29)
  ;_hideJY()


  Sleep(1000)
  MouseClick("left",355,299,1); 激活行情主窗口
  Sleep(1000)
  Send("1")
  _WinWaitActivate("Tdx","")
  Send("59915{ENTER}")
  Sleep(3000)
  WinActivate ($WIN_TITLE,"")
  MouseClick("left",521,174,1)
  Send("m")
  _WinWaitActivate("Tdx","")
  Send("5{ENTER}")
EndFunc



Func _ClickTreeViewItem($tvText, $ctrlBtnID, $x, $y)
	  ;滚动条移动到top
	  $hTreeView = ControlGetHandle($WIN_TITLE, "", $ctrlTreeViewID )
	  ConsoleWrite($hTreeView & @CRLF)
	  $hItemBuy = _GUICtrlTreeView_FindItem($hTreeView, $tvText)
	  ConsoleWrite($hItemBuy)

	  ControlFocus  ($WIN_TITLE, "", $ctrlTreeViewID)
	  Opt("MouseCoordMode", 2) ;设置鼠标函数的坐标参照,相对当前激活窗口客户区坐标
	  $a=ControlGetPos($WIN_TITLE, "", $ctrlTreeViewID) ;获取指定控件相对其窗口的坐标位置和大小
	  While _GUIScrollBars_GetScrollPos ( $hTreeView, $SB_VERT )>0
		 ConsoleWrite(_GUIScrollBars_GetScrollPos ( $hTreeView, $SB_VERT )& @CRLF)
		 MouseClick("left",$a[0]+$x,$a[1]+$y)
		 ConsoleWrite("点击滚动条"& @CRLF)
		 ;_GUIScrollBars_ScrollWindow( $hTreeView, 0, 10 )
	  WEnd

;点击“TreeViewItem”
;~ 	  ControlFocus  ($WIN_TITLE, "", $ctrlTreeViewID)
;~ 	  Opt("MouseCoordMode", 2) ;设置鼠标函数的坐标参照,相对当前激活窗口客户区坐标
;~ 	  $a=ControlGetPos($WIN_TITLE, "", $ctrlTreeViewID) ;获取指定控件相对其窗口的坐标位置和大小
;~ 	  MouseClick("left",$a[0]+34,$a[1]+9)
	  _GUICtrlTreeView_ClickItem($hTreeView, $hItemBuy)
	  ConsoleWrite("点击"&$tvText& @CRLF)

	_WaitCtrlEnable($ctrlBtnID)
;~  	While(ControlGetFocus($WIN_TITLE, "") <> "Edit1")
;~ 	   Sleep(50)
;~     WEnd
EndFunc

Func _showJY()
   $hWnd = ControlGetHandle ($WIN_TITLE, "", "AfxMDIFrame421" )

   Local $iState = WinGetState($hWnd)
   ConsoleWrite($hWnd& @CRLF)
   ; Check if the Notepad window is minimized and display the appropriate message box.
   While (BitAND($iState, 2) ==0)
	  ControlClick($WIN_TITLE, "", $ctrlJYBtnID)
	  Sleep(500)
	  $iState = WinGetState($hWnd)
   WEnd
EndFunc

Func _hideJY()
   $hWnd = ControlGetHandle ($WIN_TITLE, "", "AfxMDIFrame421" )

   Local $iState = WinGetState($hWnd)
   ConsoleWrite($hWnd& @CRLF)
   ; Check if the Notepad window is minimized and display the appropriate message box.
   While (BitAND($iState, 2) <> 0)
	  ControlClick($WIN_TITLE, "", $ctrlJYBtnID)
	  Sleep(500)
	  $iState = WinGetState($hWnd)
   WEnd
EndFunc


