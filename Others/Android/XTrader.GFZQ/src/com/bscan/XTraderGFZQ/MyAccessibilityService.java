package com.bscan.XTraderGFZQ;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * ��������ǲ���Ҫ����activity��ȥ�����ģ�����ϵͳ���������� ��Ҫ��������ȥ�ֶ����� ������ƽ��app��
 * ����ʹ�õ�service ���кܴ�ͬ�� �ǳ�����
 * ������� \sdk\samples\android-23\legacy\ApiDemos ������Ŀ¼�� �ҵ�������� �������������һ��accessibility
 * �� �����й�����������demo ��Ȼ�����Ǹ�demo �ǳ����ӣ�������Ϣ���ܴ�����Ȥ�����о���ͬѧ���Զ࿴demo
 * ������ֻʵ��������Ĺ��� ��û����������쳣����ֻ�����������ܣ�������Ϊʵ��ҵ�����ߣ�
 */
public class MyAccessibilityService extends AccessibilityService {
	private UDPUtils udpUtils;
	private boolean bWaitPopup = false;
	
    public MyAccessibilityService() {
    }

    /**
     * AccessibilityService ���������Թ����ܶ����ԣ���Щ���� һ�����ͨ�����������������������ã�
     * ������͵�� ����Щ�������Ե�������xml д�� ����manifest��������Ҫʹ�õ�ʱ����Ҫ���ְ汾��
     * �����ݣ����ϵİ汾�����޷�ͨ��xml�������õ� ֻ���������������д��Щ���� һ��Ҫע��.
     * ͬʱ���ҵ������ܸ��ӱ�����Ҫ��ʼ���㲥��֮��Ĺ��� �����������������д��
     */
    @Override
    protected void onServiceConnected() {
        super.onServiceConnected();
        //Configure these here for compatibility with API 13 and below.
//        AccessibilityServiceInfo config = new AccessibilityServiceInfo();
//        config.eventTypes = AccessibilityEvent.TYPES_ALL_MASK/*.TYPE_WINDOW_STATE_CHANGED*/;
//        config.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
//        //config.packageNames=["com.gfjgj.dzh"];
//        if (Build.VERSION.SDK_INT >= 16)
//            //Just in case this helps
//            config.flags = AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
//
//        setServiceInfo(config);
        
        //udpUtils = new UDPUtils("192.168.1.255", 17077);
        udpUtils = new UDPUtils("255.255.255.255", 17077);
        //udpUtils.sendControInfo("ppppppppppabc");
        (new Thread(udpUtils)).start();
        
        String sTextLog = "";
		if (MainActivity.cbAction.isChecked()) {
			sTextLog = "���������ɹ��񣬡��ж���OK.";
		}
		else
		{
			sTextLog = "���������ɹ����븴�顾�ж�����";
		}

        MainActivity.updateData2MainUI(sTextLog);

        Toast toast=Toast.makeText(this,"onServiceConnected()..." + sTextLog, Toast.LENGTH_SHORT);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }
        

   
    /**
     * ���������������������ʱ�򣬾Ϳ��Լ����¼��ˣ���Ȼ����ʲô�¼���������ʲô�̶� �����ɸ��������������������ģ�
     * �ҵ���Щ����д��xml���ˡ�
     */
    @Override
    public void onAccessibilityEvent(AccessibilityEvent event) {
        /**
         * �¼��Ƿֺܶ��ֵģ�����������򵥵����֣�ֻ��ʾ���Ĺ��ܣ����Ҫ����ҵ������ �����Ƽ�һ���������Կ�����������type���ԡ�
         * �����type��int ֵȡ���� ��ת��16���ƣ�Ȼ��ȥAccessibilityEvent Դ����find��˳�㿴ע�� ��������Ѹ�����type���͵ķ���
         */
            String className = event.getClassName().toString();
    	
    	if(className.contains("Text") && (null != event.getSource()))
    	{
    		String sTxt = (event.getSource()).getText().toString();
        	Log.d("XXOO", className + " -> " + AccessibilityEvent.eventTypeToString(event.getEventType()) + " - " + (event.getSource()));
        	Log.d("XXOO", className + " -> " + AccessibilityEvent.eventTypeToString(event.getEventType()) + " - " + sTxt);
        	//Log.d("XXOO", className + " -> " + event.eventTypeToString(event.getEventType()) + " - " + (event.getSource()).getText().toString());
    	}
    	else
	    	Log.d("XXOO", className + " -> " + AccessibilityEvent.eventTypeToString(event.getEventType()) );//+ event.getSource().getText().toString());
    		
    	//android.widget.TextView -> TYPE_WINDOW_CONTENT_CHANGED - 
    	//android.widget.EditText -> TYPE_VIEW_TEXT_CHANGED - 
    	//android.widget.EditText -> TYPE_VIEW_TEXT_SELECTION_CHANGED - 
    	
//    	ListView xxxx = (ListView)(event.getSource().getChild(0));
//    	xxxx.perf
//    	if (className.equals("android.widget.ListView")) {
//        	Log.i("XXOO", event.getText().toString() + " -> " + event.eventTypeToString(event.getEventType()) );
//        	
//        	((event.getSource().getChild(0)).performItemClick(listView.getChildAt(click_position), click_position, listView.getItemIdAtPosition(click_position));
//        	
//            }
    	switch (event.getEventType()) {
    	
        case AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED:
        //case AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED:
            //����ط�ûʲô��˵�� ������� �ҵ���ǰ���� �����а�װ ����ؼ��ʵ� ���нڵ�Ϳ����ˡ�������Щ�ڵ��list
            //ע�������find ��ʵ��contains����˼���������������2���ڵ㣬һ���ڵ������ǰ�װ1 һ���ڵ������ǰ�װ2������2���ڵ��Ƕ��᷵�ع�����
            //�����и���Text�ҽڵ�ķ��� ���и���Id�ҽڵ�ķ��������ǵ��ڶ��ֻ�rom����һ����������Ҫ��Ҷ����һ�£��е�rom packageInstall
            //���ƵıȽ����룬���ܺ͹ٷ�rom���ĺ�Զ �����Ҫ�����ദ�����Ը��ߴ��һ��С���� ��Ͱ���Щrom�� ��װ���� Ȼ��
            //ͨ��ddms�� ��view�ṹ�İ�ť ֱ�ӽ�ȥ�������ˣ�����ֱ�ӿ����Ǹ����������ĸ�������Ҳ���Կ�����Ҫ������Ǹ���ť��id��ʲô �ܷ��㣡
        	
        	if(MainActivity.fAction == 0.0f)
        		bWaitPopup = false;
    		
            if (className.equals("com.android.dazhihui.ui.screen.InitScreen")) {
                //getPacket();
            } else if (className.equals("com.android.dazhihui.ui.screen.stock.MainScreen")) {
        		
            	List<AccessibilityNodeInfo> btn_login = findNodesById(event.getSource(), "com.gfjgj.dzh:id/btn_login");
            	List<AccessibilityNodeInfo> ll_buy = findNodesById(event.getSource(), "com.gfjgj.dzh:id/ll_buy");
            	List<AccessibilityNodeInfo> bottom_menu_button5 = findNodesById(event.getSource(), "com.gfjgj.dzh:id/bottom_menu_button5");
            	
        		if((null != btn_login) && !btn_login.isEmpty())
        		{
        			Log.d("XXOO", className + " -> ��¼����..." );
	         		Log.d("XXOO", className + " -> doLogin..." );
	            	doLogin(event); 
	            	Log.d("XXOO", className + " -> doLogin..Done" );
        		}
        		else if((null != ll_buy) && !ll_buy.isEmpty())
        		{
        			Log.d("XXOO", className + " -> ����..����..." );
        			//com.gfjgj.dzh:id/ll_sell
        			
        			//com.gfjgj.dzh:id/trade_right_string "�˳�" //������˳����ظ�һ�ε�¼����ֹ��ʱ180��������

    	        	String sAvaMoney = getTextByIDTxt(event, "com.gfjgj.dzh:id/tv44");//�����ʽ�
    	    		if(sAvaMoney.length() > 0)
    	    			MainActivity.fAvaMoney = Float.parseFloat(sAvaMoney);
    	    		else
    	    			MainActivity.fAvaMoney = 0.0f;

        	        if (MainActivity.fAction > 0){
        	        	findAndClick(event, "com.gfjgj.dzh:id/ll_buy"); //��ͨ����
        	        }
        	        else if (MainActivity.fAction < 0){
        	        	findAndClick(event, "com.gfjgj.dzh:id/ll_sell"); //��ͨ����
        	        }
        		}
        		else if((null != bottom_menu_button5) && !bottom_menu_button5.isEmpty())
        		{
        			Log.d("XXOO", className + " -> ������..." );
        			
        			findAndClick(event, "com.gfjgj.dzh:id/bottom_menu_button5");//��ҳ���½ǽ��װ�ť
        		}
                //List<AccessibilityNodeInfo> list  = event.getSource().findAccessibilityNodeInfosByText("��¼");
            } else if (className.equals("com.android.dazhihui.ui.delegate.screen.trade.TradeCommonStock")) {
        		List<AccessibilityNodeInfo> btnBS = findNodesById(event.getSource(), "com.gfjgj.dzh:id/btn_entrust");
        		if((null != btnBS) && !btnBS.isEmpty())
        			if(btnBS.get(0).getText().toString().equals("����"))
        				Log.d("XXOO", className + " -> 		�������..." );
        			else if(btnBS.get(0).getText().toString().equals("����"))
        				Log.d("XXOO", className + " -> 		��������..." );

				//Log.d("XXOO", btnBS.get(0).getText() + "->" +  btnBS.get(0).getText().toString().equals("����"));
    	        if (MainActivity.fAction > 0){
    	        	
    	        	if(MainActivity.fAvaMoney == 0.0f)
    	        	{
        	        	findAndClick(event, "com.gfjgj.dzh:id/head_left");//���˰�ť
    	        		return;
    	        	}

            		if((null != btnBS) && !btnBS.isEmpty())
            			if(!btnBS.get(0).getText().toString().equals("����"))
            			{
		            		//========================================================
            	        	findAndClick(event, "com.gfjgj.dzh:id/tv_buy"); 
		                 	return;
            			}
            		doTrade(event); 
    	        }
    	        else if (MainActivity.fAction < 0){
            		if((null != btnBS) && !btnBS.isEmpty())
            			if(!btnBS.get(0).getText().toString().equals("����"))
            			{
		            		//========================================================
            	        	findAndClick(event, "com.gfjgj.dzh:id/tv_sell");
		                 	return;
            			}
            		doTrade(event); 
    	        }
            } else if (className.equals("android.app.Dialog")) {
 	        	List<AccessibilityNodeInfo> nodeContent = findNodesById(event.getSource(), "com.gfjgj.dzh:id/content");
        		if((null != nodeContent) && (!nodeContent.isEmpty()))
        		{
        			MainActivity.updateData2MainUI(nodeContent.get(0).getText().toString());
        		}
           	
            	//com.gfjgj.dzh:id/content����ί�������ύ�ɹ�����ͬ��Ϊ��870
            	//com.gfjgj.dzh:id/confirm ȷ��
        		
            	if(getByTxt(event, "ί�������ύ�ɹ�����ͬ��Ϊ") || getByTxt(event, "���ܳ���"))
            	{
    	        	findAndClick(event, "com.gfjgj.dzh:id/confirm"); //ȷ��
    	        	
    	        	goHome();
    	        	
    	        	return;
            	}
            	
            	if(getByTxt(event, "��ȷ��������") || getByTxt(event, "��ȷ��������"))
            	{
            		bWaitPopup = false;
            		MainActivity.setAction(0.0f);
					MainActivity.fAvaMoney = 0.0f;
					
					MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ...���ί��ȷ��");
					//debug ��ί��ȷ�ϰ�ť
    	        	findAndClick(event, "com.gfjgj.dzh:id/confirm"); //ί��ȷ��
            		return;
            	}

            	if(getByTxt(event, "���ղ�����ʾ") || getByTxt(event, "��ʱ����") || getByTxt(event, "������ɷ�������"))
            	{
            		Log.d("XXOO", className + " -> ClosePpup..." );
            		findAndClick(event, "com.gfjgj.dzh:id/cancel");//�����ȷ����ť //��ʱ����
            		Log.d("XXOO", className + " -> ClosePpup..Done" );
            		return;
            	}
            }
    	}
    }

	private void goHome() {
		Intent intent= new Intent(Intent.ACTION_MAIN);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);//����Ƿ�������ã��������new task��ʶ
		intent.addCategory(Intent.CATEGORY_HOME);
		startActivity(intent);
		}

	public boolean getByTxt(AccessibilityEvent event, String sTxttt) {
		if(null == event)
			return false;
		
		AccessibilityNodeInfo xxx = event.getSource();
		if(null == xxx)
			return false;
		
		List<AccessibilityNodeInfo> nodeInfByTxt = xxx.findAccessibilityNodeInfosByText(sTxttt);
		if((null != nodeInfByTxt) && (!nodeInfByTxt.isEmpty()))
			return true;
		else
			return false;
	}

	public void findAndClick(AccessibilityEvent event, String sViewId) {
		List<AccessibilityNodeInfo> listBuy  = findNodesById(event.getSource(), sViewId);
		clickIt(listBuy);
	}

	private String getTextByIDTxt(AccessibilityEvent event, String sInOutStr) {
		List<AccessibilityNodeInfo> listUsableM = findNodesById(event.getSource(), sInOutStr);
		sInOutStr = "";
		if ((null != listUsableM) && (!listUsableM.isEmpty()))
		{
		    for (AccessibilityNodeInfo info : listUsableM) 
		    {
		        if ((null != info.getText()) && (getNumeric(info.getText().toString()).length()>0) )
		        {
		        	sInOutStr = getNumeric(info.getText().toString()) ;
		        	Log.i("XXOO", "getTextByIDTxt -> " + sInOutStr);
		        	break;
		        }
		    }
		}
		return sInOutStr;
	}

	private void doTrade(AccessibilityEvent event) {
		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " doTrade...fAct=" + MainActivity.fAction);
		
		if(bWaitPopup)
			return;
		
		//com.gfjgj.dzh:id/tv_cc
		//com.gfjgj.dzh:id/avaliable_text
		//========================================================
//    	Log.i("XXOO", event.getClassName().toString() + " -> doTrade.......��A 0602572118." ); //Bug...
//		List<AccessibilityNodeInfo> tmpCode  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/edittext");
//		pasteInto(tmpCode, "��A 0602572118"); 
		
		//listView.performItemClick(listView.getChildAt(click_position), click_position, listView.getItemIdAtPosition(click_position));

    	Log.i("XXOO", event.getClassName().toString() + " -> doTrade......159915" );
		List<AccessibilityNodeInfo> listCode  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/et_code");
		pasteInto(listCode, "159915"); 
		
		//com.gfjgj.dzh:id/tv_sell5_price
		//com.gfjgj.dzh:id/tv_buy5_price
		String sPrice = "";

		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ...��ȡ�۸�");
		for (int i = 0; i < 40; i++)//�40�Σ�����۸�û���¾ͷ���
		{
//			try {//�˴�sleep��ȫ��Ч
//				Thread.sleep(50);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				MainActivity.updateData2MainUI(Thread.currentThread().getName() + " Thread.sleep(50)...�쳣�ˣ�����");
//				e.printStackTrace();
//			}

			if (MainActivity.fAction > 0)
			{
				sPrice = getTextByIDTxt(event, "com.gfjgj.dzh:id/tv_sell5_price");
				if(sPrice.length() == 0)
					sPrice = getTextByIDTxt(event, "com.gfjgj.dzh:id/tv_sell1_price");
			}
			else if (MainActivity.fAction < 0)
			{
				sPrice = getTextByIDTxt(event, "com.gfjgj.dzh:id/tv_buy5_price");
				if(sPrice.length() == 0)
					sPrice = getTextByIDTxt(event, "com.gfjgj.dzh:id/tv_buy1_price");
			}
			if((sPrice.length() > 0)|| (i==39))
			{
				MainActivity.updateData2MainUI(Thread.currentThread().getName() + " i = " + i);
				break;
			}
		}
		
		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..sPrice = " + sPrice + " ..Len = " + sPrice.length());
		if(sPrice.length() == 0)
		{
			reEvent();
			return;
		}

    	Log.i("XXOO", event.getClassName().toString() + " -> doTrade......" + sPrice );
		List<AccessibilityNodeInfo> listPrice  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/et_price");
		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..pasteInto(listPrice, sPrice)" );
		pasteInto(listPrice, sPrice); 
		
		List<AccessibilityNodeInfo> listNum  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/et_num");
		int iNum = 0;
		if (MainActivity.fAction > 0)
		{
			if(sPrice.length() > 0)
			{
				//Calc buy numbers
				iNum = (int) (MainActivity.fAvaMoney/Float.parseFloat(sPrice)/1.00025);
				//fill it
			}
		}
		else if (MainActivity.fAction < 0)
		{
//			MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..sleep(150)" );
//			try {//�˴�sleep��ȫ��Ч
//				Thread.sleep(150); //���Ż����������Ż����������Ż����������Ż�������
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
			
			List<AccessibilityNodeInfo> listAvaNum = findNodesById(event.getSource(), "com.gfjgj.dzh:id/tv_ava_num");//��*179800��
			
			if ((null != listAvaNum) && (!listAvaNum.isEmpty()))
			{
			    for (AccessibilityNodeInfo info : listAvaNum) 
			    {
			        if ((null != info.getText()) && (getNumeric(info.getText().toString()).length()>0) )
			        {
			        	Log.i("XXOO", event.getClassName().toString() + " -> doTrade......" + info.getText().toString() );
						//pasteInto(listNum, "10800"); 
						
						//����38100.00��  mText	"����190100��"[��ʱ����] (id=73)	
						String sb = info.getText().toString();
						if(sb.indexOf(".") >= 0)
							sb = sb.substring(0, sb.indexOf("."));
						
						iNum = Integer.parseInt(getNumeric(sb.toString()));
						Log.i("XXOO", info.getText() + ",,," + iNum );
						
						break;
			        }
			    }
			}
		}
		
        if (Math.abs(MainActivity.fAction) >= 100){//number
        	iNum = (int)Math.abs(MainActivity.fAction); 
        }
        else if (Math.abs(MainActivity.fAction) > 0){//rate
			Log.d("XXOO", "---" + iNum );
        	iNum = (int) Math.floor(iNum * Math.abs(MainActivity.fAction)/100);
			Log.d("XXOO", "===" + iNum );
        	iNum *= 100;
			Log.d("XXOO", "..." + iNum );
        }
		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..����������" + iNum);
		if(iNum == 0)
		{
			reEvent();
			
			return;
		}

        pasteInto(listNum, "" + iNum); 

		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..btn_entrust" );
		//debug �����밴ť
		findAndClick(event, "com.gfjgj.dzh:id/btn_entrust");
		bWaitPopup = true;

		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " doTrade...[DONE]");

	}

	public void reEvent() {
		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " ..�����¼��ֶ�����������" );
		
		PackageManager packageManager = this.getPackageManager();   
        Intent intent = packageManager.getLaunchIntentForPackage("com.gfjgj.dzh");
        startActivity(intent);

//		Constructor<AccessibilityEvent> constructor;
//		try {
//			constructor = AccessibilityEvent.class.getDeclaredConstructor();
//			constructor.setAccessible(true);
//			AccessibilityEvent accessibilityEvent;
//			try {
//				accessibilityEvent = constructor.newInstance();
//				accessibilityEvent.setEventType(AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED);
//				accessibilityEvent.setClassName("com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen");
//				
//			} catch (InstantiationException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			} catch (IllegalAccessException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			} catch (IllegalArgumentException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			} catch (InvocationTargetException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		} catch (NoSuchMethodException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
	}

	private void doLogin(AccessibilityEvent event) {
		MainActivity.updateData2MainUI("doLogin...");
		
		List<AccessibilityNodeInfo> list  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/tv_yzm");
		List<AccessibilityNodeInfo> listYZM  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/et_yzm");
		List<AccessibilityNodeInfo> listPass  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/et_passwords");
		List<AccessibilityNodeInfo> listLogin  = findNodesById(event.getSource(), "com.gfjgj.dzh:id/btn_login");
		if(listLogin.isEmpty())
			return;
		
		pasteInto(listPass, "288604"); 
		
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

	private void clickIt(List<AccessibilityNodeInfo> listTmp) {
		if ((null != listTmp) && (!listTmp.isEmpty())){
		    for (AccessibilityNodeInfo info : listTmp) {
		            //�ҵ���Ľڵ��Ժ� ��ֱ�ӵ����������
		            info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		            
			        if ((null != info.getText()) && ((info.getText().toString()).length()>0) )
			        {
			    		MainActivity.updateData2MainUI(Thread.currentThread().getName() + " �ҵ��ѵ� [" +  info.getText().toString() + "]");
			        }
			        else
			        	MainActivity.updateData2MainUI(Thread.currentThread().getName() + " �ҵ�����.. "  +  info.getViewIdResourceName());

		            break;
		    }
		}
	}

	private void pasteInto(List<AccessibilityNodeInfo> listPass, String sText) {
		if (null != listPass) {
		    for (AccessibilityNodeInfo info : listPass) {
		    	
		    	CharSequence txt = info.getText();
		    	if(txt == null) txt = "";
		    	if(txt.toString().equals(sText) )
		    	{
		    		txt = "";
		    		int xxx=88;
		    			break;
		    	}
		    	  
		    	//android>21 = 5.0ʱ������ACTION_SET_TEXT
		    	//android>18 3.0.1����ͨ�����Ƶ��ֶ�,��ȷ�����㣬��ճ��ACTION_PASTE
		    	//ʹ�ü��а�
		    	ClipboardManager clipboard = (ClipboardManager)this.getSystemService(Context.CLIPBOARD_SERVICE);  
		    	ClipData clip = ClipData.newPlainText("text", sText);  
		    	clipboard.setPrimaryClip(clip);  
		    	
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                    Bundle arguments = new Bundle();
                    arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT, 0);
                    arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT, txt.length());
                    info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
                    info.performAction(AccessibilityNodeInfo.ACTION_SET_SELECTION, arguments);
                    info.performAction(AccessibilityNodeInfo.ACTION_PASTE);
                }
                break;

//		    	//���㣨n��AccessibilityNodeInfo����  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);  
//		    	////ճ����������  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_CUT);  
//		    	clipboard.setPrimaryClip(clip);  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_PASTE);  
		    	
		    	//info2.setText(info.getText());
		        //if (info2.getText().toString().equals("��¼")) {
		            //�ҵ���Ľڵ��Ժ� ��ֱ�ӵ����������
		            //info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            //info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		        //}
		    }
		}
	}
    /**
     * ����View��ID�������������Ľڵ�,��ȷ������ʽ;
     * ���ֻ�������Լ�д�Ľ��棬��ΪID�����ظ�
     * apiҪ��18������
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
        //�ر��߳�
        //udpUtils.setKeepRunning(false); 

    }
    /**
     * ���˷�����
     * @param str
     * @return
     */
    public static String getNumeric(String str) {
    	
//    	Pattern compile = Pattern.compile("\\d+\\.\\d+");
//        Matcher matcher = compile.matcher(str);
//        matcher.find();
//        return matcher.group();//��ȡƥ�䵽�Ľ��
        
        String regEx="[^0-9//.]";  
        Pattern p = Pattern.compile(regEx);  
        Matcher m = p.matcher(str);  
        return m.replaceAll("").trim();
    }
}