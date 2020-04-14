package com.bscan.udp2player;

//import android.support.v7.app.ActionBarActivity;
import android.text.TextUtils;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.webkit.MimeTypeMap;
import android.widget.TextView;
 
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;
 
public class MainActivity extends Activity implements Runnable{
 
    private MulticastSocket ds;
    String multicastHost="224.0.0.1";
    InetAddress receiveAddress;
    TextView result;
    String beforeResult="";
    Handler mHandler;
    public final static String EXTRA_MESSAGE = "com.bscan.udp2player.MESSAGE";
    
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        mHandler = new Handler(new Handler.Callback() {
            @Override
            public boolean handleMessage(Message msg) {
                return false;
            }
        });
 
        result  =(TextView)findViewById(R.id.result);
        
        try {
            ds = new MulticastSocket(8003);
            receiveAddress=InetAddress.getByName(multicastHost);
            ds.joinGroup(receiveAddress);
            new Thread(this).start();
        } catch (Exception e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        
        new Thread() {
        	@Override
        	public void run() {
        	     //这里写入子线程需要做的工作
        		
//                String packageName = "com.mitv.mivideoplayer";
//                //private void killApp(String packageName) {
//                   // ActivityManager am = (ActivityManager)getApplicationContext().getSystemService(ACTIVITY_SERVICE);
//                    Log.d("aaa", "Trying to kill app " + packageName);
//                    
//                    ActivityManager am=(ActivityManager) getSystemService(ACTIVITY_SERVICE);  
//                    am.killBackgroundProcesses(packageName);  
                    /*
            		List<RunningTaskInfo> runningServices = am.getRunningTasks(maxNum)(maxNum, flags)(100);// 获取系统所有正在运行的服务,最多返回100个
            		 
            		for (RunningServiceInfo runningServiceInfo : runningServices) {
            			String className = runningServiceInfo.service.getClassName();// 获取服务的名称
            			// System.out.println(className);
            			if (className.equals(serviceName)) {// 服务存在
            				return true;
            			}
            		}*/
                    //Toast.makeText(getApplicationContext(), "1. FoundPackage:" + getRunningApk(packageName), Toast.LENGTH_SHORT).show();  
                    
                    //while(getRunningApk(packageName).length() > 0)
                    while(true)
                    {
                        //Toast.makeText(getApplicationContext(), "killBackgroundProcesses" + packageName, Toast.LENGTH_SHORT).show();  
        	            //Toast.makeText(getApplicationContext(), "3. FoundPackage:" + getRunningApk(packageName), Toast.LENGTH_SHORT).show();  
                   	try{
                    	    Thread.sleep(300);
                    	}catch (Exception e ){

                    	}
                   	
                    String packageName = "com.mitv.mivideoplayer";
                    //private void killApp(String packageName) {
                       // ActivityManager am = (ActivityManager)getApplicationContext().getSystemService(ACTIVITY_SERVICE);
                        //Log.d("aaa", "Trying to kill app " + packageName);
                        
                        ActivityManager am=(ActivityManager) getSystemService(ACTIVITY_SERVICE);  
                        am.killBackgroundProcesses(packageName);  
     
                    }
        	        }
        	   }.start();
        	   
//注册系统剪切板的监听器事件，当剪切板数据发生变化的时候，就能获取到剪切板的数据
        	   final ClipboardManager clipboardManager = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        	   clipboardManager.addPrimaryClipChangedListener(new ClipboardManager.OnPrimaryClipChangedListener()
        	   {
        	       @Override
        	   	public void onPrimaryClipChanged()
        	       {
        	           ClipData.Item itemAt = clipboardManager.getPrimaryClip().getItemAt(0);
        	           Log.e("监听到剪切板中的内容:",itemAt.getText().toString());
        	           
       				Intent intent = new Intent();
    				intent.setClass(MainActivity.this, UDP_Push.class);  //从IntentActivity跳转到SubActivity
    				intent.putExtra("name", "xiazdong");  //放入数据


//        	           UDP_Push sss;
//        	           Intent intent = new Intent(getApplicationContext(), UDP_Push.class);
        	           intent.setAction(Intent.ACTION_SEND);
        	           intent.setType("http");
        	           String message = itemAt.getText().toString();
        	           intent.setData(Uri.parse(message));
        	           //intent.putExtra(EXTRA_MESSAGE, message);
       				startActivity(intent);  //开始跳转
       				moveToFront();
       				
        	       }
        	   });
    }
 
    //@TargetApi(11)
    protected void moveToFront() {
        if (Build.VERSION.SDK_INT >= 11) { // honeycomb
            final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
            final List<RunningTaskInfo> recentTasks = activityManager.getRunningTasks(Integer.MAX_VALUE);

            for (int i = 0; i < recentTasks.size(); i++) 
            {
                   Log.d("Executed app", "Application executed : " 
                           +recentTasks.get(i).baseActivity.toShortString()
                           + "\t\t ID: "+recentTasks.get(i).id+"");  
                   // bring to front                
                   if (recentTasks.get(i).baseActivity.toShortString().indexOf("udp2player") > -1) {                     
                      activityManager.moveTaskToFront(recentTasks.get(i).id, ActivityManager.MOVE_TASK_WITH_HOME);
                   }
            }
        }
    }
    
    @Override
    public void run() {
        // TODO Auto-generated method stub
        byte buf[] = new byte[1024];
        DatagramPacket dp = new DatagramPacket(buf, 1024);
        while (true) {
            try {
                ds.receive(dp);
                //Toast.makeText(this, new String(buf, 0, dp.getLength()), Toast.LENGTH_LONG);
                String x = "" + new String(buf, 0, dp.getLength());
                beforeResult = x;
                System.out.println(x);
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        result.setText(beforeResult+"<>");
                        
                        //String url = "https://www.baidu.com/1.m3u8?plplp=899";//示例，实际填你的网络视频链接
                        String url = beforeResult;//"http://www.baidu.com/1.m3u8";//示例，实际填你的网络视频链接
                        String extension = MimeTypeMap.getFileExtensionFromUrl(url);
                        String mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
                        if(mimeType == null)
                        	mimeType = "video/*";
                        Intent mediaIntent = new Intent(Intent.ACTION_VIEW);
                        mediaIntent.setDataAndType(Uri.parse(url), mimeType);
                        startActivity(mediaIntent);
                    }
                });
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    


	private static  String getRunningApk(String sPackageName) {
	    long startTime = System.currentTimeMillis();
	    String pgkProcessAppMap = "";
	    Set<String> rProcess = new HashSet<String>();
	    String cmd = "ps";
	    try {
	        java.lang.Process p = Runtime.getRuntime().exec(cmd);
	        BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
	        String line = null;
	        int index = 0;
	        while ((line = in.readLine()) != null) {
	            if (index > 0) {
	                StringTokenizer st = new StringTokenizer(line);
	                int lenght = st.countTokens();
	                if (lenght > 0) {
	                    String uid = st.nextToken();//0 index USER
	                    if (uid.startsWith("u0_") ) {
	                        String processInfo = "";
	                        for (int i = 0; i < (lenght - 1); i++) {
	                            if (!(i == (lenght - 2))) {
	                                st.nextToken();
	                            } else {
	                                processInfo = st.nextToken();
	                            }
	                        }
	                        
	                        if (!TextUtils.isEmpty(processInfo)) {
	                            if (processInfo.contains(":")) {
	                                String a[] = processInfo.split(":");
	                                rProcess.add(a[0]);
	                            } else {
	                                rProcess.add(processInfo);
	                            }
	                        }
	                        
	                    }
	                }
	            }
	            index++;
	        }
	    } catch (IOException e) {
	        Log.d("aaa", "getRunningApk err="+e.toString());
	    }
	    for(String pro : rProcess){
	    	
	    	if(pro.equalsIgnoreCase(sPackageName))
	    	{
	    		pgkProcessAppMap = pro;
	            return pgkProcessAppMap; //找到指定的包，返回包名字
	    	}
	        pgkProcessAppMap += pro + ",";
	    }
	    if (sPackageName.length() > 0 ) {
	        return ""; //没找到指定的包
	    }
	    //列出全部的包
	    if (pgkProcessAppMap.contains(",") && pgkProcessAppMap.length() > 0 ) {
	        pgkProcessAppMap = pgkProcessAppMap.substring(0, pgkProcessAppMap.length()-1);
	    }
	    long endTime = System.currentTimeMillis();
	    Log.d("aaa", "do_exec pgkProcessAppMap = " + pgkProcessAppMap + "\t time = " + (endTime - startTime));
	    return pgkProcessAppMap;
	}
}
