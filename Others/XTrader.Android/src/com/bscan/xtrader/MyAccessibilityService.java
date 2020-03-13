package com.bscan.xtrader;

import java.util.List;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;
import android.widget.Toast;

/**
 * 这个服务是不需要你在activity里去开启的，属于系统级别辅助服务 需要在设置里去手动开启 和我们平常app里
 * 经常使用的service 是有很大不同的 非常特殊
 * 你可以在 \sdk\samples\android-23\legacy\ApiDemos 这样的目录下 找到这个工程 这个工程下面有一个accessibility
 * 包 里面有关于这个服务的demo 当然他们那个demo 非常复杂，但是信息量很大，有兴趣深入研究的同学可以多看demo
 * 我这里只实现最基本的功能 且没有做冗余和异常处理，只包含基础功能，不能作为实际业务上线！
 */
public class MyAccessibilityService extends AccessibilityService {
    public MyAccessibilityService() {
    }

    /**
     * AccessibilityService 这个服务可以关联很多属性，这些属性 一般可以通过代码在这个方法里进行设置，
     * 我这里偷懒 把这些设置属性的流程用xml 写好 放在manifest里，如果你们要使用的时候需要区分版本号
     * 做兼容，在老的版本里是无法通过xml进行引用的 只能在这个方法里手写那些属性 一定要注意.
     * 同时你的业务如果很复杂比如需要初始化广播啊之类的工作 都可以在这个方法里写。
     */
    @Override
    protected void onServiceConnected() {
        super.onServiceConnected();
        //Configure these here for compatibility with API 13 and below.
        AccessibilityServiceInfo config = new AccessibilityServiceInfo();
        config.eventTypes = AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED;
        config.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
        //config.packageNames=["com.huaanzq.dzh"];
        if (Build.VERSION.SDK_INT >= 16)
            //Just in case this helps
            config.flags = AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;

        setServiceInfo(config);
        
        Toast toast=Toast.makeText(this,"onServiceConnected()...辅助开启成功鸟",Toast.LENGTH_SHORT    );
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    /**
     * 当你这个服务正常开启的时候，就可以监听事件了，当然监听什么事件，监听到什么程度 都是由给这个服务的属性来决定的，
     * 我的那些属性写在xml里了。
     */
    @Override
    public void onAccessibilityEvent(AccessibilityEvent event) {
        /**
         * 事件是分很多种的，我这里是最简单的那种，只演示核心功能，如果要做成业务上线 这里推荐一个方法可以快速理解这里的type属性。
         * 把这个type的int 值取出来 并转成16进制，然后去AccessibilityEvent 源码里find。顺便看注释 ，这样是迅速理解type类型的方法
         */
            String className = event.getClassName().toString();
    	Log.i("XXOO", className + " -> " + event.eventTypeToString(event.getEventType()) );
    	switch (event.getEventType()) {
    	
        case AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED:
        case AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED:
            //这个地方没什么好说的 你就理解成 找到当前界面 包含有安装 这个关键词的 所有节点就可以了。返回这些节点的list
            //注意这里的find 其实是contains的意思，比如你界面上有2个节点，一个节点内容是安装1 一个节点内容是安装2，那这2个节点是都会返回过来的
            //除了有根据Text找节点的方法 还有根据Id找节点的方法。考虑到众多手机rom都不一样，这里需要大家多测试一下，有的rom packageInstall
            //定制的比较深入，可能和官方rom里差的很远 这里就要做冗余处理，可以告诉大家一个小技巧 你就把这些rom的 安装器打开 然后
            //通过ddms里 看view结构的按钮 直接进去看就行了，可以直接看到那个界面属于哪个包名，也可以看到你要捕获的那个按钮的id是什么 很方便！
            if (className.equals("com.android.dazhihui.ui.screen.InitScreen")) {
                //getPacket();
            } else if (className.equals("com.android.dazhihui.ui.screen.stock.MainScreen")) {
                //List<AccessibilityNodeInfo> list  = event.getSource().findAccessibilityNodeInfosByText("登录");
            	Log.i("XXOO", className + " -> doLogin..." );
            	doLogin(event); 
            	Log.i("XXOO", className + " -> doLogin..Done" );
            } else if (className.equals("com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen")) {
               //close();
	        	//doBuy(event); 
            } else if (className.equals("android.app.Dialog")) {
            	Log.i("XXOO", className + " -> ClosePpup..." );
        		List<AccessibilityNodeInfo> okBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/cancel");
        		if ((null != okBtn) && (!okBtn.isEmpty())){
        		    clickIt(okBtn); //公告的确定按钮
        		}
            	Log.i("XXOO", className + " -> ClosePpup..Done" );
        		
            }

            //
          //
          //com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen
            //
            //
            

    	}
    }

	private void doBuy(AccessibilityEvent event) {
		//========================================================
		List<AccessibilityNodeInfo> listBuy  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/ll_dbpmr");
		
//
     	clickIt(listBuy); 
		//========================================================
		List<AccessibilityNodeInfo> listCode  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/et_code");
		List<AccessibilityNodeInfo> listNum  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/et_num");
		pasteInto(listCode, "159915"); 
		
		List<AccessibilityNodeInfo> listAvaNum = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/tv_ava_num");//可买179800张
		
		List<AccessibilityNodeInfo> listEnTrust  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/btn_entrust");
		List<AccessibilityNodeInfo> listConfirm  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/confirm");
		
		if ((null != listAvaNum) && (!listAvaNum.isEmpty())){
		    for (AccessibilityNodeInfo info : listAvaNum) {
		        if (info.getText().toString().length()>0) {
		        	
		        	pasteInto(listNum, "10800"); 
		        	//pasteInto(listNum, info.getText().toString()); 
		        	clickIt(listEnTrust); 
		        	//clickIt(listConfirm); 
		        }
		    }
		}
	}

	private void doLogin(AccessibilityEvent event) {
		
		List<AccessibilityNodeInfo> jyBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/bottom_menu_button5");
		if ((null != jyBtn) && (!jyBtn.isEmpty())){
		    clickIt(jyBtn); //首页右下角交易按钮
		}
		
		List<AccessibilityNodeInfo> list  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/tv_yzm");
		List<AccessibilityNodeInfo> listYZM  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_yzm");
		List<AccessibilityNodeInfo> listPass  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_passwords");
		List<AccessibilityNodeInfo> listLogin  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/btn_login");
		if(listLogin.isEmpty())
			return;
		
		pasteInto(listPass, "288603"); 
		
		if (null != list) {
		    for (AccessibilityNodeInfo info : list) {
		        if (info.getText().toString().length()>0) {
		        	
		        	pasteInto(listYZM, info.getText().toString()); 
		        }
		    }
		} 
		
		clickIt(listLogin);
	}

	private List<AccessibilityNodeInfo> waitAndFindNodes(
			AccessibilityNodeInfo nodeInfo, String sText) {
		List<AccessibilityNodeInfo>  listNodes  = null;
		while((null == listNodes) || (listNodes.isEmpty())){
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			listNodes  = findNodesById(nodeInfo, sText);
		}
		return listNodes;
	}

	private void clickIt(List<AccessibilityNodeInfo> listLogin) {
		if (null != listLogin) {
		    for (AccessibilityNodeInfo info : listLogin) {
		    	
		        //if (info2.getText().toString().equals("登录")) {
		            //找到你的节点以后 就直接点击他就行了
		            info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		        //}
		    }
		}
	}

	private void pasteInto(List<AccessibilityNodeInfo> listPass, String sText) {
		if (null != listPass) {
		    for (AccessibilityNodeInfo info : listPass) {
		    	
		    	//android>21 = 5.0时可以用ACTION_SET_TEXT
		    	//android>18 3.0.1可以通过复制的手段,先确定焦点，再粘贴ACTION_PASTE
		    	//使用剪切板
		    	ClipboardManager clipboard = (ClipboardManager)this.getSystemService(Context.CLIPBOARD_SERVICE);  
		    	ClipData clip = ClipData.newPlainText("text", sText);  
		    	//焦点（n是AccessibilityNodeInfo对象）  
		    	info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);  
		    	////粘贴进入内容  
		    	info.performAction(AccessibilityNodeInfo.ACTION_CUT);  
		    	clipboard.setPrimaryClip(clip);  
		    	info.performAction(AccessibilityNodeInfo.ACTION_PASTE);  
		    	
		    	//info2.setText(info.getText());
		        //if (info2.getText().toString().equals("登录")) {
		            //找到你的节点以后 就直接点击他就行了
		            //info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            //info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		        //}
		    }
		}
	}
    /**
     * 根据View的ID搜索符合条件的节点,精确搜索方式;
     * 这个只适用于自己写的界面，因为ID可能重复
     * api要求18及以上
     * @param viewId
     */    
    public List<AccessibilityNodeInfo> findNodesById(AccessibilityNodeInfo nodeInfo, String viewId) {
        if (nodeInfo != null) {
            if (Build.VERSION.SDK_INT >= 18) {
                return nodeInfo.findAccessibilityNodeInfosByViewId(viewId);
            }
        }
        return null;
    }
    @Override
    public void onInterrupt() {

    }

}