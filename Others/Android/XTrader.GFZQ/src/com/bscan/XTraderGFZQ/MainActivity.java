package com.bscan.XTraderGFZQ;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.conn.util.InetAddressUtils;

import android.accessibilityservice.AccessibilityServiceInfo;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.text.Html;
import android.text.TextUtils;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityManager;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;
import android.widget.Toast;
import android.provider.Settings;//导包


public class MainActivity extends Activity  implements CompoundButton.OnCheckedChangeListener {
	public static Context gMainContext = null;
	public static WifiManager.MulticastLock wifiLock  =  null;
	
	public static float fAction = 0.0f;
	public static float fAvaMoney = 0.0f;
	//public static float iAction000 = 0;
	public static MyHandler myHandler;
	
	public static TextView tvLog;
	public static CheckBox cbAction;
	
	private static final String ACTION = "action";
	private static final String ACTION_START_ACCESSIBILITY_SETTING = "action_start_accessibility_setting";
	public static final String TAG = "XXOO";
	public static final String TAG_SVR = "SVR: ";

	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
                
        ((TextView) findViewById(R.id.tvLocalIP)).setText(Html.fromHtml("本机IP: <font color='#0000FF'>" + getLocalHostIp() + "</font>"));
        
        cbAction = (CheckBox) findViewById(R.id.cbAct);

        cbAction.setOnCheckedChangeListener(this);
        
        tvLog = (TextView)findViewById(R.id.tvLog);     
        tvLog.setMovementMethod(ScrollingMovementMethod.getInstance());  
        tvLog.setText("");

		MainActivity.setAction(0.0f);
        //showConnected("");

        myHandler = new MyHandler();
        //newSecTimer();
        
        // 当创建一个新的Handler实例时, 它会绑定到当前线程和消息的队列中,开始分发数据
        // Handler有两个作用, (1) : 定时执行Message和Runnalbe 对象
        // (2): 让一个动作,在不同的线程中执行.
 
        // 它安排消息,用以下方法
        // post(Runnable)
        // postAtTime(Runnable,long)
        // postDelayed(Runnable,long)
        // sendEmptyMessage(int)
        // sendMessage(Message);
        // sendMessageAtTime(Message,long)
        // sendMessageDelayed(Message,long)
     
        // 以上方法以 post开头的允许你处理Runnable对象
        //sendMessage()允许你处理Message对象(Message里可以包含数据,)

        jumpToSettingPage(this.getApplicationContext());

        
        //infomation.append("目的IP： "+SERVER_IP+"\n"+"目的端口： "+SERVER_PORT+"\n");
        //infomation.append("本地端口： " +LOCAL_PORT);
    }

    protected void newSecTimer()
    {
    	// run in a second
    	// 每一秒钟执行一次
    	final long timeInterval = 1000;
    	Runnable runnable = new Runnable() {
    		public void run() {
    			while (true) {
    				// ------- code for task to run
    				// ------- 要运行的任务代码
    				System.out.println("Hello, Timer...");

    				// Date d = new Date();
    				// if((d.getHours() == 9) && (d.getMinutes() == 10)){
    				// return d;
    				// }else{
    				// return null;
    				// }
    				// ------- ends here
    				try {
    					// sleep()：同步延迟数据，并且会阻塞线程
    					Thread.sleep(timeInterval);
    				} catch (InterruptedException e) {
    					e.printStackTrace();
    				}
    			}
    		}
    	};
    	// 创建定时器
    	Thread thread = new Thread(runnable);
    	// 开始执行
    	thread.start();
    }
 
    public static String getLocalHostIp() {
        String ipaddress = "";
        try {
            Enumeration<NetworkInterface> en = NetworkInterface
                    .getNetworkInterfaces();
            // 遍历所用的网络接口
            while (en.hasMoreElements()) {
                NetworkInterface nif = en.nextElement();// 得到每一个网络接口绑定的所有ip
                Enumeration<InetAddress> inet = nif.getInetAddresses();
                // 遍历每一个接口绑定的所有ip
                while (inet.hasMoreElements()) {
                    InetAddress ip = inet.nextElement();
                    if (!ip.isLoopbackAddress()
                            && InetAddressUtils.isIPv4Address(ip
                            .getHostAddress())) {
                        return ip.getHostAddress();
                    }
                }
            }
        }
        catch(SocketException e)
        {
                Log.e("feige", "获取本地ip地址失败");
                e.printStackTrace();
        }
        return ipaddress;
    }

    /**
     * 判断是否为数字，包含负数情况
     * @param str
     * @return
     */
    public static boolean isNumeric(String str){
        Boolean flag = false;
        String tmp;
        if((str.length() > 0)){
            if(str.startsWith("-")){
                tmp = str.substring(1);
            }else{
                tmp = str;
            }
            
          //采用正则表达式的方式来判断一个字符串是否为数字，这种方式判断面比较全  
            //可以判断正负、整数小数  
            //?:0或1个, *:0或多个, +:1或多个  
            //Boolean strResult = str.matches("-?[0-9]+.*[0-9]*");  
            flag = tmp.matches("-?[0-9]+.*[0-9]*");
        }
        return flag;
    }

    public static void updateData2MainUI(String rcvstr) {
    	Message msg = new Message();
    	Bundle bundle = new Bundle();

    	// 把数据放到buddle中
    	bundle.putString("receive", rcvstr);
    	// 把buddle传递到message
    	msg.setData(bundle);
    	MainActivity.myHandler.sendMessage(msg); // 向Handler发送消息,更新UI
    	
    }


    /**
    * 接受消息,处理消息 ,此Handler会与当前主线程一块运行
    * */

    class MyHandler extends Handler {
        public MyHandler() {
        }

        public MyHandler(Looper L) {
            super(L);
        }


        // 子类必须重写此方法,接受数据
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
			super.handleMessage(msg);

			String rcvstr = msg.getData().getString("receive");
			// Log.d(TAG, "handleMessage......" + rcvstr);
			
			if (MainActivity.isNumeric(rcvstr)) {
				Log.w(TAG, "handleMessage...指令=" + rcvstr);
				//Log.d("XXOO", "rcvedTCP......" + data + " 当前指令=" + MainActivity.fAction);

		        showLog("handleMessage...收到指令=" + rcvstr);
		        
		        Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);  
                Ringtone r = RingtoneManager.getRingtone(getApplicationContext(), notification);  
                r.play(); 
                
				if (((CheckBox) findViewById(R.id.cbAct)).isChecked()) {
					MainActivity.this.ActionByStrCMD(rcvstr);
				}
				else
				{
					MainActivity.setAction(0.0f);
			        Toast.makeText(MainActivity.this,"收到BS指令，不过请先选中行动复选框！！！",Toast.LENGTH_LONG).show();

				}

			}
			else if(rcvstr.equalsIgnoreCase("Connected"))
			{
				//public static void showConnected(String sIP) {
					((RadioButton) findViewById(R.id.svrIP)).setChecked(true);
			        //((RadioButton) findViewById(R.id.svrIP)).setText(rcvstr);
				//}
			        //showLog(rcvstr);
				
			}
			else
			{
				if(rcvstr.contains(TAG_SVR)) {
					((RadioButton) findViewById(R.id.svrIP)).setChecked(false);
			        ((RadioButton) findViewById(R.id.svrIP)).setText(Html.fromHtml(rcvstr));
				}
			    
		        Toast.makeText(MainActivity.this, rcvstr, Toast.LENGTH_LONG).show();
//		        Toast toast=Toast.makeText(MainActivity.this,rcvstr, Toast.LENGTH_SHORT);
//		        toast.setGravity(Gravity.CENTER, 0, 0);
//		        toast.show();


				showLog(rcvstr);
			}
				


            // 此处可以更新UI
            //Bundle b = msg.getData();
//            String color = b.getString("color");
//            MainActivity.this.button.append(color);

        }

    }

		private static void showLog(String rcvstr) {
			SimpleDateFormat   formatter   =   new   SimpleDateFormat   ("yyyy-MM-dd HH:mm:ss.SSS");
	        Date curDate =  new Date(System.currentTimeMillis());
	        //获取当前时间
	        String   str   =   formatter.format(curDate);
	        
	        //((TextView) findViewById(R.id.tvLocalIP)).setText(Html.fromHtml("本机IP: <font color='#0000FF'>" + getLocalHostIp() + "</font>"));

	        //tvLog.append(str + "   " + Html.fromHtml(rcvstr).toString() + "\n");//去除<***/>
	        //Html.fromHtml("<font color='#0000FF'>" + str + "</font>")
	        tvLog.append(Html.fromHtml("<font color='#0000FF'>" + str + "</font>" + "   " + Html.fromHtml(rcvstr).toString() + "<br>"));//去除<***/>
	        
	        tvLog.post(new Runnable() {
	        	 
	            @Override
	            public void run() {
	                //int lineCount = tvLog.getLineCount();
	                //bug: getLineCount() wrong value; need be fixed later...
	    	        Log.w(TAG, " getLineCount=" + tvLog.getLineCount()
	    	        		+ " getLineHeight=" + tvLog.getLineHeight()
	    	        		+ " getHeight=" + tvLog.getHeight());
	    	        
	    	        int offset=tvLog.getLineCount()*tvLog.getLineHeight();
	    	        if(offset>tvLog.getHeight()){
	    	        	tvLog.scrollTo(0,offset-tvLog.getHeight());
	    	        }

	            }
	        });
		}
		
    public static void setAction(float fff) {
			// TODO Auto-generated method stub
    	MainActivity.fAction = fff;
    	cbAction.setText("" + MainActivity.fAction);
    	showLog("[更新指令为] --> " + MainActivity.fAction);
    	showLog(Thread.currentThread().getName() + " setAction---fAct=" + MainActivity.fAction);
		
		}

	/**
	 * 检测辅助功能是否开启<br>
	 * 方 法 名：isAccessibilitySettingsOn <br>
	 * 创 建 人 <br>
	 * 创建时间：2016-6-22 下午2:29:24 <br>
	 * 修 改 人： <br>
	 * 修改日期： <br>
	 * @param mContext
	 * @return boolean
	 */
	private static boolean isAccessibilitySettingsOn(Context mContext) {
		int accessibilityEnabled = 0;
		// TestService为对应的服务
		final String service = mContext.getPackageName() + "/" + ".MyAccessibilityService"/*MyAccessibilityService.class.getCanonicalName()*/;
		Log.i(TAG, "service:" + service);
		// com.z.buildingaccessibilityservices/android.accessibilityservice.AccessibilityService
		try {
			accessibilityEnabled = Settings.Secure.getInt(mContext.getApplicationContext().getContentResolver(),
					android.provider.Settings.Secure.ACCESSIBILITY_ENABLED);
			Log.v(TAG, "accessibilityEnabled = " + accessibilityEnabled);
		} catch (Settings.SettingNotFoundException e) {
			Log.e(TAG, "Error finding setting, default accessibility to not found: " + e.getMessage());
		}
		TextUtils.SimpleStringSplitter mStringColonSplitter = new TextUtils.SimpleStringSplitter(':');
 
		if (accessibilityEnabled == 1) {
			Log.v(TAG, "***ACCESSIBILITY IS ENABLED*** -----------------");
			String settingValue = Settings.Secure.getString(mContext.getApplicationContext().getContentResolver(),
					Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES);
			// com.z.buildingaccessibilityservices/com.z.buildingaccessibilityservices.TestService
			if (settingValue != null) {
				mStringColonSplitter.setString(settingValue);
				while (mStringColonSplitter.hasNext()) {
					String accessibilityService = mStringColonSplitter.next();
 
					Log.v(TAG, "-------------- > accessibilityService :: " + accessibilityService + " " + service);
					if (accessibilityService.equalsIgnoreCase(service)) {
						Log.v(TAG, "We've found the correct setting - accessibility is switched on!");
						return true;
					}
				}
			}
		} else {
			Log.v(TAG, "***ACCESSIBILITY IS DISABLED***");
		}
		return false;
	}
	
	/**
     * 判断AccessibilityService服务是否已经启动
     * @param context
     * @param name
     * @return
     */
    public static boolean isStartAccessibilityService(Context context, String name){
        AccessibilityManager am = (AccessibilityManager) context.getSystemService(Context.ACCESSIBILITY_SERVICE);
        List<AccessibilityServiceInfo> serviceInfos = am.getEnabledAccessibilityServiceList(AccessibilityServiceInfo.FEEDBACK_GENERIC);
        for (AccessibilityServiceInfo info : serviceInfos) {
            String id = info.getId();
            Log.v(TAG, "all -->" + id);
            if (id.contains(name)) {
                return true;
            }
        }
        return false;
    }
    /**
     * 判断无障碍服务是否开启
     *
     * @param context
     * @return
     */
    private static boolean isStartAccessibilityServiceEnable(Context context) {
        AccessibilityManager accessibilityManager =
                (AccessibilityManager)         
                 context.getSystemService(Context.ACCESSIBILITY_SERVICE);
        assert accessibilityManager != null;
        List<AccessibilityServiceInfo> accessibilityServices =
                accessibilityManager.getEnabledAccessibilityServiceList(
                        AccessibilityServiceInfo.FEEDBACK_ALL_MASK);
        for (AccessibilityServiceInfo info : accessibilityServices) {
            if (info.getId().contains(context.getPackageName())) {
                return true;
            }
        }
        return false;
    }

	public static boolean isAccessibilityEnabled(Context context, String id) {

		AccessibilityManager am = (AccessibilityManager) context
				.getSystemService(Context.ACCESSIBILITY_SERVICE);

		List<AccessibilityServiceInfo> runningServices = am
				.getEnabledAccessibilityServiceList(AccessibilityEvent.TYPES_ALL_MASK);
		for (AccessibilityServiceInfo service : runningServices) {
			if (id.equals(service.getId())) {
				return true;
			}
		}

		return false;
	}
	
    public static void jumpToSettingPage(Context context) {
            try {
            	gMainContext = context;
//        		WifiManager manager = (WifiManager) gMainContext.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
//        		wifiLock  = manager.createMulticastLock("localWifi");
        		
//                Intent intent = new Intent(context,  MainActivity.class);
//                intent.putExtra(ACTION, ACTION_START_ACCESSIBILITY_SETTING);
//                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//                context.startActivity(intent);

//            	isAccessibilityEnabled(gMainContext, "mmm");
            	if (!isAccessibilitySettingsOn(gMainContext)) {
//        			Intent intent = new Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS);
//        			startActivity(intent);
        			
	            	Intent intent = new Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS);
	            	intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
	            	intent.putExtra(ACTION, ACTION_START_ACCESSIBILITY_SETTING);
	            	context.startActivity(intent);
        		}
            	
            	} catch (Exception ignore) {
            		int xxx=999;
            	}
        }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    public void click(View v){
        Toast.makeText(MainActivity.this,((Button)findViewById(v.getId())).getText().toString(),Toast.LENGTH_LONG).show();
        String sAction = ((Button)v).getText().toString();
        
        ActionByStrCMD(sAction);
        
    }

	public void ActionByStrCMD(String sAction) {
		MainActivity.setAction(Float.parseFloat(sAction));
        Log.i("XXOO", fAction + " -> 		click..." );

        runHAZQapp();
	}

	public void runHAZQapp() {
//		PackageManager packageManager = this.getPackageManager();   
//        Intent intent = packageManager.getLaunchIntentForPackage("com.gfjgj.dzh");
//        startActivity(intent);
   
		Intent intent = getPackageManager().getLaunchIntentForPackage("com.gfjgj.dzh");
		startActivity(intent);
		
//        Intent i = new Intent("com.gfjgj.dzh");
//        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//        this.startActivity(i);
		
//		Intent i = new Intent(); 
//        ComponentName cn = new ComponentName("com.gfjgj.dzh", 
//                  "com.android.dazhihui.ui.screen.stock.MainScreen"); 
//        i.setComponent(cn); 
//        i.setAction("android.intent.action.MAIN"); 
//        startActivity(i);
		
//        startActivityForResult(i, RESULT_OK);
	}
	
    @Override
    protected void onDestroy() {  
    super.onDestroy();
    }
    
    /**

     * 获取电源锁，保持该服务在屏幕熄灭时仍然获取CPU时，保持运行

     */

    static public WakeLock wakeLock;
    static public void acquireWakeLock(){

//    	if (null != wifiLock) wifiLock.acquire();	
    	if (null == wakeLock) {

    		PowerManager pm = (PowerManager) gMainContext.getSystemService(Context.POWER_SERVICE);

    		wakeLock = pm.newWakeLock(
    				PowerManager.PARTIAL_WAKE_LOCK 
    				| PowerManager.ON_AFTER_RELEASE
    				//| PowerManager.FULL_WAKE_LOCK
    				, gMainContext.getClass()

    				.getCanonicalName());
			Log.i("XXOO", "call acquireWakeLock = " + wakeLock);

    		if (null != wakeLock) {


    			wakeLock.acquire();

    		}

    	}

    }

    // 释放设备电源锁

    static public void releaseWakeLock() {

//    	if (null != wifiLock) wifiLock.release();//必须调用
    	if (null != wakeLock && wakeLock.isHeld()) {

    		Log.i(TAG, "call releaseWakeLock");

    		wakeLock.release();

    		wakeLock = null;

    	}

    }

    //	WakeLock 类型以及说明：
    //
    //	PARTIAL_WAKE_LOCK:保持CPU 运转，屏幕和键盘灯有可能是关闭的。
    //
    //	SCREEN_DIM_WAKE_LOCK：保持CPU 运转，允许保持屏幕显示但有可能是灰的，允许关闭键盘灯
    //
    //	SCREEN_BRIGHT_WAKE_LOCK：保持CPU 运转，允许保持屏幕高亮显示，允许关闭键盘灯
    //
    //	FULL_WAKE_LOCK：保持CPU 运转，保持屏幕高亮显示，键盘灯也保持亮度
    //
    //	ACQUIRE_CAUSES_WAKEUP：强制使屏幕亮起，这种锁主要针对一些必须通知用户的操作.
    //
    //	ON_AFTER_RELEASE：当锁被释放时，保持屏幕亮起一段时间
    //
    //	最后 AndroidManifest.xml 声明权限：
    //
    //
    //
    
    /**
     * 当复选框的状态发生变化时自动调用的方法
     * @param buttonView 事件源
     * @param isChecked 当前复选框是否选中, true:选中,false 不选中
     */
    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(isChecked){
            //用户点击的复选框的内容
            String text=buttonView.getText().toString();
            System.out.println("text="+text+",isChecked="+isChecked);
            System.out.println("当前控件处于选中状态");
        }else{
            System.out.println("当前控件取消了选中状态");
    		MainActivity.setAction(0.0f);
        }
 
    }
}

