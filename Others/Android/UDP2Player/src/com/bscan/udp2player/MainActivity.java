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
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.webkit.MimeTypeMap;
import android.widget.TextView;
 
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
 
public class MainActivity extends Activity implements Runnable{
 
    private MulticastSocket ds;
    String multicastHost="224.0.0.1";
    InetAddress receiveAddress;
    TextView result;
    String beforeResult="";
    Handler mHandler;
	TcpServer m3u8Server;
    public final static String EXTRA_MESSAGE = "com.bscan.udp2player.MESSAGE";
    static byte[] b = null;
    
  //���ַ���modelStr���ַ���str�е�count�γ���ʱ���±�
    private int getFromIndex(String str, String modelStr, Integer count) {
    	//�����ַ�������ƥ��
            Matcher slashMatcher = Pattern.compile(modelStr).matcher(str);
    	int index = 0;
            //matcher.find();���Բ������ģʽƥ����������е���һ��������
    		while(slashMatcher.find()) {
    	    index++;
    	    //��modelStr�ַ���count�γ��ֵ�λ��
    	    if(index == count){
    	       break;
    	    }
    	}
            //matcher.start();������ǰƥ��ĳ�ʼ������
    	return slashMatcher.start();
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        new Thread() {
        	@Override
        	public void run() {
                m3u8Server = new TcpServer(null);
        	}
        }.start();
        
        new Thread() {
        	@Override
        	public void run() {
        	     //����д�����߳���Ҫ���Ĺ���
                try {
                	String filepath = Environment.getExternalStorageDirectory() + "/xxx";
                	
                	File file2 = new File(filepath);
                	if (!file2.exists())
                	file2.mkdirs();

                	String sUrl = "https://cn4.5311444.com/hls/20190426/97ed0bf400fc7efb547d3f91ea31d7b1/1556253639/index.m3u8";
                    DownUtil downUtil = new DownUtil(
                    		sUrl,
                    		//filepath + "/filename2",
                    		"",
                    		2);
                    
                    b = null;

                    b = downUtil.downLoad();
                          
                    //����M3U8
                    ByteArrayInputStream byteArray = new ByteArrayInputStream(b);
                    BufferedReader bInput = new BufferedReader(new InputStreamReader(byteArray));
                    //String[] subUrl = sUrl.split("/");
                    String input;  
                    while ((input= bInput.readLine()) != null) {  
                    	Log.i("TAG", input);  
                    	
                        String sPrefix = "";
                    	if((input.indexOf("/") ==0))
                    		sPrefix = sUrl.substring(0, getFromIndex(sUrl, ("/"), 3));
                    	else
                    		sPrefix = sUrl.substring(0, sUrl.lastIndexOf("/"))+"/";
                            
                    	if((input.indexOf(".ts") >=0) 
                    			|| (input.indexOf(".mp4") >=0))
                    	{
							DownUtil downOne = new DownUtil(
                            		sPrefix + input,
                            		//filepath + "/filename2",
                            		"",
                            		5);
                            
                    		StaticBufs.vFileMap.put(input, downOne.downLoad());
                    		
                    		Log.i("TAG", "len=" + StaticBufs.vFileMap.get(input).length);
                    	}
//                        for (Map.Entry<String ,byte[]> entry : StaticBufs.vFileMap.entrySet()) {
//                			Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
//                			conn.setRequestProperty(entry.getKey(), entry.getValue());
//                		}

                    }  
                    //5�߳����أ���������
                    //����������5*2ʱ��ͣ
                    //tcp������������ɵģ�ɾ����-5λ��ǰ��
                } catch (Exception e) {
                    e.printStackTrace();
                }

        	        }
        	   }.start();

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
        	     //����д�����߳���Ҫ���Ĺ���
        		
//                String packageName = "com.mitv.mivideoplayer";
//                //private void killApp(String packageName) {
//                   // ActivityManager am = (ActivityManager)getApplicationContext().getSystemService(ACTIVITY_SERVICE);
//                    Log.d("aaa", "Trying to kill app " + packageName);
//                    
//                    ActivityManager am=(ActivityManager) getSystemService(ACTIVITY_SERVICE);  
//                    am.killBackgroundProcesses(packageName);  
                    /*
            		List<RunningTaskInfo> runningServices = am.getRunningTasks(maxNum)(maxNum, flags)(100);// ��ȡϵͳ�����������еķ���,��෵��100��
            		 
            		for (RunningServiceInfo runningServiceInfo : runningServices) {
            			String className = runningServiceInfo.service.getClassName();// ��ȡ���������
            			// System.out.println(className);
            			if (className.equals(serviceName)) {// �������
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
        	   
//ע��ϵͳ���а�ļ������¼��������а����ݷ����仯��ʱ�򣬾��ܻ�ȡ�����а������
        	   final ClipboardManager clipboardManager = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        	   clipboardManager.addPrimaryClipChangedListener(new ClipboardManager.OnPrimaryClipChangedListener()
        	   {
        	       @Override
        	   	public void onPrimaryClipChanged()
        	       {
        	           ClipData.Item itemAt = clipboardManager.getPrimaryClip().getItemAt(0);
        	           Log.e("���������а��е�����:",itemAt.getText().toString());
        	           
       				Intent intent = new Intent();
    				intent.setClass(MainActivity.this, UDP_Push.class);  //��IntentActivity��ת��SubActivity
    				intent.putExtra("name", "xiazdong");  //��������


//        	           UDP_Push sss;
//        	           Intent intent = new Intent(getApplicationContext(), UDP_Push.class);
        	           intent.setAction(Intent.ACTION_SEND);
        	           intent.setType("http");
        	           String message = itemAt.getText().toString();
        	           intent.setData(Uri.parse(message));
        	           //intent.putExtra(EXTRA_MESSAGE, message);
       				startActivity(intent);  //��ʼ��ת
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
                        
                        //String url = "https://www.baidu.com/1.m3u8?plplp=899";//ʾ����ʵ�������������Ƶ����
                        String url = beforeResult;//"http://www.baidu.com/1.m3u8";//ʾ����ʵ�������������Ƶ����
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
	            return pgkProcessAppMap; //�ҵ�ָ���İ������ذ�����
	    	}
	        pgkProcessAppMap += pro + ",";
	    }
	    if (sPackageName.length() > 0 ) {
	        return ""; //û�ҵ�ָ���İ�
	    }
	    //�г�ȫ���İ�
	    if (pgkProcessAppMap.contains(",") && pgkProcessAppMap.length() > 0 ) {
	        pgkProcessAppMap = pgkProcessAppMap.substring(0, pgkProcessAppMap.length()-1);
	    }
	    long endTime = System.currentTimeMillis();
	    Log.d("aaa", "do_exec pgkProcessAppMap = " + pgkProcessAppMap + "\t time = " + (endTime - startTime));
	    return pgkProcessAppMap;
	}
}
