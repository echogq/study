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
import android.provider.Settings;//����


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
                
        ((TextView) findViewById(R.id.tvLocalIP)).setText(Html.fromHtml("����IP: <font color='#0000FF'>" + getLocalHostIp() + "</font>"));
        
        cbAction = (CheckBox) findViewById(R.id.cbAct);

        cbAction.setOnCheckedChangeListener(this);
        
        tvLog = (TextView)findViewById(R.id.tvLog);     
        tvLog.setMovementMethod(ScrollingMovementMethod.getInstance());  
        tvLog.setText("");

		MainActivity.setAction(0.0f);
        //showConnected("");

        myHandler = new MyHandler();
        //newSecTimer();
        
        // ������һ���µ�Handlerʵ��ʱ, ����󶨵���ǰ�̺߳���Ϣ�Ķ�����,��ʼ�ַ�����
        // Handler����������, (1) : ��ʱִ��Message��Runnalbe ����
        // (2): ��һ������,�ڲ�ͬ���߳���ִ��.
 
        // ��������Ϣ,�����·���
        // post(Runnable)
        // postAtTime(Runnable,long)
        // postDelayed(Runnable,long)
        // sendEmptyMessage(int)
        // sendMessage(Message);
        // sendMessageAtTime(Message,long)
        // sendMessageDelayed(Message,long)
     
        // ���Ϸ����� post��ͷ�������㴦��Runnable����
        //sendMessage()�����㴦��Message����(Message����԰�������,)

        jumpToSettingPage(this.getApplicationContext());

        
        //infomation.append("Ŀ��IP�� "+SERVER_IP+"\n"+"Ŀ�Ķ˿ڣ� "+SERVER_PORT+"\n");
        //infomation.append("���ض˿ڣ� " +LOCAL_PORT);
    }

    protected void newSecTimer()
    {
    	// run in a second
    	// ÿһ����ִ��һ��
    	final long timeInterval = 1000;
    	Runnable runnable = new Runnable() {
    		public void run() {
    			while (true) {
    				// ------- code for task to run
    				// ------- Ҫ���е��������
    				System.out.println("Hello, Timer...");

    				// Date d = new Date();
    				// if((d.getHours() == 9) && (d.getMinutes() == 10)){
    				// return d;
    				// }else{
    				// return null;
    				// }
    				// ------- ends here
    				try {
    					// sleep()��ͬ���ӳ����ݣ����һ������߳�
    					Thread.sleep(timeInterval);
    				} catch (InterruptedException e) {
    					e.printStackTrace();
    				}
    			}
    		}
    	};
    	// ������ʱ��
    	Thread thread = new Thread(runnable);
    	// ��ʼִ��
    	thread.start();
    }
 
    public static String getLocalHostIp() {
        String ipaddress = "";
        try {
            Enumeration<NetworkInterface> en = NetworkInterface
                    .getNetworkInterfaces();
            // �������õ�����ӿ�
            while (en.hasMoreElements()) {
                NetworkInterface nif = en.nextElement();// �õ�ÿһ������ӿڰ󶨵�����ip
                Enumeration<InetAddress> inet = nif.getInetAddresses();
                // ����ÿһ���ӿڰ󶨵�����ip
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
                Log.e("feige", "��ȡ����ip��ַʧ��");
                e.printStackTrace();
        }
        return ipaddress;
    }

    /**
     * �ж��Ƿ�Ϊ���֣������������
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
            
          //����������ʽ�ķ�ʽ���ж�һ���ַ����Ƿ�Ϊ���֣����ַ�ʽ�ж���Ƚ�ȫ  
            //�����ж�����������С��  
            //?:0��1��, *:0����, +:1����  
            //Boolean strResult = str.matches("-?[0-9]+.*[0-9]*");  
            flag = tmp.matches("-?[0-9]+.*[0-9]*");
        }
        return flag;
    }

    public static void updateData2MainUI(String rcvstr) {
    	Message msg = new Message();
    	Bundle bundle = new Bundle();

    	// �����ݷŵ�buddle��
    	bundle.putString("receive", rcvstr);
    	// ��buddle���ݵ�message
    	msg.setData(bundle);
    	MainActivity.myHandler.sendMessage(msg); // ��Handler������Ϣ,����UI
    	
    }


    /**
    * ������Ϣ,������Ϣ ,��Handler���뵱ǰ���߳�һ������
    * */

    class MyHandler extends Handler {
        public MyHandler() {
        }

        public MyHandler(Looper L) {
            super(L);
        }


        // ���������д�˷���,��������
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
			super.handleMessage(msg);

			String rcvstr = msg.getData().getString("receive");
			// Log.d(TAG, "handleMessage......" + rcvstr);
			
			if (MainActivity.isNumeric(rcvstr)) {
				Log.w(TAG, "handleMessage...ָ��=" + rcvstr);
				//Log.d("XXOO", "rcvedTCP......" + data + " ��ǰָ��=" + MainActivity.fAction);

		        showLog("handleMessage...�յ�ָ��=" + rcvstr);
		        
		        Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);  
                Ringtone r = RingtoneManager.getRingtone(getApplicationContext(), notification);  
                r.play(); 
                
				if (((CheckBox) findViewById(R.id.cbAct)).isChecked()) {
					MainActivity.this.ActionByStrCMD(rcvstr);
				}
				else
				{
					MainActivity.setAction(0.0f);
			        Toast.makeText(MainActivity.this,"�յ�BSָ���������ѡ���ж���ѡ�򣡣���",Toast.LENGTH_LONG).show();

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
				


            // �˴����Ը���UI
            //Bundle b = msg.getData();
//            String color = b.getString("color");
//            MainActivity.this.button.append(color);

        }

    }

		private static void showLog(String rcvstr) {
			SimpleDateFormat   formatter   =   new   SimpleDateFormat   ("yyyy-MM-dd HH:mm:ss.SSS");
	        Date curDate =  new Date(System.currentTimeMillis());
	        //��ȡ��ǰʱ��
	        String   str   =   formatter.format(curDate);
	        
	        //((TextView) findViewById(R.id.tvLocalIP)).setText(Html.fromHtml("����IP: <font color='#0000FF'>" + getLocalHostIp() + "</font>"));

	        //tvLog.append(str + "   " + Html.fromHtml(rcvstr).toString() + "\n");//ȥ��<***/>
	        //Html.fromHtml("<font color='#0000FF'>" + str + "</font>")
	        tvLog.append(Html.fromHtml("<font color='#0000FF'>" + str + "</font>" + "   " + Html.fromHtml(rcvstr).toString() + "<br>"));//ȥ��<***/>
	        
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
    	showLog("[����ָ��Ϊ] --> " + MainActivity.fAction);
    	showLog(Thread.currentThread().getName() + " setAction---fAct=" + MainActivity.fAction);
		
		}

	/**
	 * ��⸨�������Ƿ���<br>
	 * �� �� ����isAccessibilitySettingsOn <br>
	 * �� �� �� <br>
	 * ����ʱ�䣺2016-6-22 ����2:29:24 <br>
	 * �� �� �ˣ� <br>
	 * �޸����ڣ� <br>
	 * @param mContext
	 * @return boolean
	 */
	private static boolean isAccessibilitySettingsOn(Context mContext) {
		int accessibilityEnabled = 0;
		// TestServiceΪ��Ӧ�ķ���
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
     * �ж�AccessibilityService�����Ƿ��Ѿ�����
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
     * �ж����ϰ������Ƿ���
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

     * ��ȡ��Դ�������ָ÷�������ĻϨ��ʱ��Ȼ��ȡCPUʱ����������

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

    // �ͷ��豸��Դ��

    static public void releaseWakeLock() {

//    	if (null != wifiLock) wifiLock.release();//�������
    	if (null != wakeLock && wakeLock.isHeld()) {

    		Log.i(TAG, "call releaseWakeLock");

    		wakeLock.release();

    		wakeLock = null;

    	}

    }

    //	WakeLock �����Լ�˵����
    //
    //	PARTIAL_WAKE_LOCK:����CPU ��ת����Ļ�ͼ��̵��п����ǹرյġ�
    //
    //	SCREEN_DIM_WAKE_LOCK������CPU ��ת����������Ļ��ʾ���п����ǻҵģ�����رռ��̵�
    //
    //	SCREEN_BRIGHT_WAKE_LOCK������CPU ��ת����������Ļ������ʾ������رռ��̵�
    //
    //	FULL_WAKE_LOCK������CPU ��ת��������Ļ������ʾ�����̵�Ҳ��������
    //
    //	ACQUIRE_CAUSES_WAKEUP��ǿ��ʹ��Ļ������������Ҫ���һЩ����֪ͨ�û��Ĳ���.
    //
    //	ON_AFTER_RELEASE���������ͷ�ʱ��������Ļ����һ��ʱ��
    //
    //	��� AndroidManifest.xml ����Ȩ�ޣ�
    //
    //
    //
    
    /**
     * ����ѡ���״̬�����仯ʱ�Զ����õķ���
     * @param buttonView �¼�Դ
     * @param isChecked ��ǰ��ѡ���Ƿ�ѡ��, true:ѡ��,false ��ѡ��
     */
    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(isChecked){
            //�û�����ĸ�ѡ�������
            String text=buttonView.getText().toString();
            System.out.println("text="+text+",isChecked="+isChecked);
            System.out.println("��ǰ�ؼ�����ѡ��״̬");
        }else{
            System.out.println("��ǰ�ؼ�ȡ����ѡ��״̬");
    		MainActivity.setAction(0.0f);
        }
 
    }
}

