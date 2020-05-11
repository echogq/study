package com.bscan.udp2player;

//import android.support.v7.app.ActionBarActivity;
import android.text.TextUtils;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.webkit.MimeTypeMap;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
 
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.math.BigDecimal;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
 
public class MainActivity extends Activity implements Runnable{
 
    private MulticastSocket ds;
    String multicastHost="224.0.0.1";
    InetAddress receiveAddress;
    TextView result;
    public static String sRcvUDPData="";
    Handler mHandler;
	TcpServer m3u8Server;
	private static Button sendUDPBrocast1;
    public final static String EXTRA_MESSAGE = "com.bscan.udp2player.MESSAGE";
	protected static final int MAX_THREADS = 1;
    public static final int MAX_BLOCKs = 80;

    static byte[] bytesM3u8 = null;
	private static OkHttpClient okHttpClientG = null;
    private static Context mContext = null;

    //����һ����̬�ķ������Ա��ȡcontext����
    public static Context getContext(){
        return mContext;
    }
    
  //���ַ���modelStr���ַ���str�е�count�γ���ʱ���±�
    public static int getFromIndex(String str, String modelStr, Integer count) {
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
    
	//��Byte����ת�����ļ�
    public static void getFileByBytes(byte[] bytes, String filePath, String fileName) {
        BufferedOutputStream bos = null;
        FileOutputStream fos = null;
        File file = null;
        try {
//            File dir = new File(filePath);
//            if (!dir.exists() && dir.isDirectory()) {// �ж��ļ�Ŀ¼�Ƿ����
//                dir.mkdirs();
//            }
            file = new File(filePath + "/" + fileName);
            fos = new FileOutputStream(file);
            bos = new BufferedOutputStream(fos);
            bos.write(bytes);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
    
	/**
     * ������ťʱ��
     * */
    class SendUDPBrocastListener1 implements View.OnClickListener {
 
        @Override
        public void onClick(View v) {
            new Thread(new Runnable() {
                @Override
                public void run() {
//                	openIntent("https://tv1.youkutv.cc/2020/03/28/h0fA8TSZSijKdCi4/playlist.m3u8");
//                	openIntent("https://cn4.5311444.com/hls/20190426/97ed0bf400fc7efb547d3f91ea31d7b1/1556253639/index.m3u8");
//                	openIntent("https://leshi.cdn-zuyida.com/20180421/23526_27748718/index.m3u8");
//                	openIntent("http://videohy.tc.qq.com/vwecam.tc.qq.com/1006_7f00e3e804394aefbccd579689afacc8.f0.mp4?vkey=50A1AED2B5A08D6F7AB1C53C71207F5CD95376F186A3E02319626CF3D722451D4E914DE0502D5EF6E7F7B87F9FA206ED1B27ACE3A07CE18E&rf=mobile.qzone.qq.com");
                	openIntent("");
                	//openIntent("http://127.0.0.1:9999/?go=https://56.com-t-56.com/20190222/6275_993e32bb/index.m3u8");
               }
            }).start();
         }
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mContext = getApplicationContext();
      
        new Thread() {
        	@Override
        	public void run() {
                m3u8Server = new TcpServer(null);
        	}
        }.start();
        
        sendUDPBrocast1 = (Button) findViewById(R.id.sendd);
        sendUDPBrocast1.setOnClickListener(new SendUDPBrocastListener1());
 
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
    protected void onPause() {
        super.onPause();
// ����ǰ�ᡪ�����app����������һ��service�������ʾ�����̲���ǰ̨ʱ�����Ͽ���һ��service
        Intent intent = new Intent(this, MyService.class);
        startService(intent);
    }

	public static Response okGetUrl2(String url2) {
		if(okHttpClientG == null)
			okHttpClientG = new OkHttpClient();
		final Request request = new Request.Builder()
		        .url(url2)
		        .build();
		final Call call = okHttpClientG.newCall(request);
		Response response = null;
		try {
	    	setBtnText2("���أ�" + url2);
			response = call.execute();
			 
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			call.cancel();
			okHttpClientG = null;
			Log.d("TAG", url2+" : ʧ����" );
	    	setBtnText2("����ʧ�ܣ�" + url2);
		} finally{
		}
		return response; 		
	}
	
	public static void okGetUrl(final String url2) {
		OkHttpClient okHttpClient = new OkHttpClient();
		final Request request = new Request.Builder()
		        .url(url2)
		        .build();
		final Call call = okHttpClient.newCall(request);
		new Thread(new Runnable() {
		    @Override
		    public void run() {
		    	
		    	setBtnText2("���أ�" + url2);
		    	call.enqueue(new Callback() {//���
		    		@Override
				    public void onFailure(Call call, IOException e) {
				    //��ʧ��ʱ
				    	setBtnText2("����ʧ�ܣ�" + url2);
				        Log.d("TAG", request.url().toString()+" : ʧ��" );
				    }

				    @Override
				    public void onResponse(Call call, final Response response) throws IOException {
				    	
				       // Log.d("TAG", request.url().toString()+" : " + response.body().contentLength());
				    	if(request.url().toString().indexOf(".m3u8") >0){
				    		String sBody = response.body().string();
					    	setBtnText2("������ɣ�" + request.url());

	        				if(sBody.indexOf(".m3u8") <0)
	        				{//�޸�M3U8, �ܿ�MXPLAYER PRO��bug
	        					bytesM3u8 = sBody.getBytes();
	        					modM3u8ForMXPlayer(request.url().toString());
	        					sBody = new String (bytesM3u8);
	        				}


//	    					if(sBody.indexOf(".m3u8") >0) {
						        Log.d("TAG", request.url().toString()+" : " + response.body().contentLength() + sBody);
						        
						        String lines[] = sBody.split("(\r\n|\r|\n)", -1);
						        for(int i=0;i<lines.length;i++)
						        {
						        	//System.out.print(lines[i]+" ");//���a b c d e����ȡ�ַ�������
						        	if((lines[i].indexOf(".m3u8") >0) || (lines[i].indexOf(".ts") >0)){
	
						        		String sPrefix = "";
						        		String sUrl = request.url().toString();
						        		if((lines[i].indexOf("/") ==0))
						        			sPrefix = sUrl.substring(0, getFromIndex(sUrl, ("/"), 3));
						        		else
						        			sPrefix = sUrl.substring(0, sUrl.lastIndexOf("/"))+"/";

						        		if(lines[i].indexOf(".ts") >0){

						        			/////////////////////////////////////
						        			if((StaticBufs.sNeedDownFN[0].length() > 0) && !StaticBufs.haveKey(StaticBufs.sNeedDownFN[0])){
						        				for(i=0;i<lines.length;i++)
						        				{
						        					if(lines[i].equals(StaticBufs.sNeedDownFN[0]))
						        					{
						        						break;
						        					}
						        				}
						        			}

						        			if(StaticBufs.lstNames.indexOf(lines[i]) == -1)
						        				StaticBufs.lstNames.add(lines[i]);

						        			while((StaticBufs.vFileMap.size() >= MainActivity.MAX_BLOCKs))
												try {
													Thread.sleep(50);
								        			for (int a = 0; a < StaticBufs.lstNames.size()-(MainActivity.MAX_BLOCKs/3); a++) {
							        					if(StaticBufs.lstNames.get(a).equals(StaticBufs.sMXPlayingFN[0]))
							        					{
							        						break;
							        					}
		
								        				StaticBufs.vecIngAndDone.remove(StaticBufs.lstNames.get(a));
								        				StaticBufs.mapRemove(StaticBufs.lstNames.get(a));
								        				//StaticBufs.lstNames.RemoveRange(1,3);
								        			}
												} catch (InterruptedException e) {
													// TODO Auto-generated catch block
													e.printStackTrace();
												}

						        			//if(!StaticBufs.conKey(sOneLine) && !DownUtil.isDowning(sOneLine))
						        			//if(!StaticBufs.conKey(sOneLine) && !DownUtil.isDowning(sOneLine))
						        			if(!StaticBufs.vecIngAndDone.contains(lines[i]))
						        			{
						        				StaticBufs.vecIngAndDone.addElement(lines[i]);
						        				//UDP_Push.pushLog(sPrefix +sOneLine + " " +(!StaticBufs.conKey(sOneLine)) + "_" +!DownUtil.isDowning(sOneLine));

						        				if(lines[i].equals(StaticBufs.sNeedDownFN[0]))
						        				{
						        					StaticBufs.sNeedDownFN[0] = "";
						        				}
						        				//					    								else
						        				//					    	        						while((StaticBufs.iCntThreads[0] >= MAX_THREADS)||(StaticBufs.vFileMap.size() >= MainActivity.MAX_BLOCKs))
						        				//					    	        							Thread.sleep(50);

						        				////////////////////////////

										    	setBtnText2("���أ�" + sPrefix + lines[i]);
										    	Log.d("TAG", "���أ�" + sPrefix + lines[i]);
						        				Response response2 = null;
						        				while(response2 == null)
						        					response2 = okGetUrl2(sPrefix + lines[i]);

						        				Log.d("TAG", sPrefix + lines[i]+" : " + response2.body().contentLength());
										    	setBtnText2("������ɣ�" + sPrefix + lines[i]+" : " + response2.body().contentLength());
										    	
						        				String path = sPrefix + lines[i];
						        				if(StaticBufs.mapGet(path .substring(MainActivity.getFromIndex(path, ("/"), 3))) == null)
						        					StaticBufs.mapPut(path.substring(MainActivity.getFromIndex(path, ("/"), 3)), response2.body().bytes());
						        			}
						        		}
						        		else
						        		{
						        			sRcvUDPData = sPrefix + lines[i];

						        			okGetUrl(sPrefix + lines[i]);
						        		}
						        	}
						        }
		        				Log.d("TAG", "Done!!!!!!!!!!!");

	
//	    					}
//	    					

				    	}
//    					else if(request.url().toString().indexOf(".ts") >0){
//					        Log.d("TAG", request.url().toString()+" : " + response.body().contentLength());
//   						
//    					}


//		                    ����Ӧʱ
//		                    runOnUiThread(new Thread(){//�����̸߳���UI
//		                        @Override
//		                        public void run() {
//		                            try {
//		                                mTextView.setText(response.body().string());//�������ϻ�ȡ�ľ���JSON
//		                            } catch (IOException e) {
//		                                e.printStackTrace();
//		                            }
//		                        }
//		                    });

				    }
				});
		    }
		}).start();
	}

	public void openIntent(String url) {
		
		String sUrl2Player = url;
		if(url.length() == 0) {
			url = sRcvUDPData;
			//url = "https://56.com-t-56.com/20190222/6275_993e32bb/index.m3u8";
			//url = "https://leshi.cdn-zuyida.com/20180421/23526_27748718/index.m3u8";
			if(url.length() == 0)
				url = "https://leshi.cdn-zuyida.com/20180421/23526_27748718/index.m3u8";
			setBtnText(url);
			sUrl2Player = "http://127.0.0.1:9999/?go="+url;
			//buffM3U8(url);
			
			StaticBufs.vecIngAndDone.clear();
			StaticBufs.vFileMap.clear();
			StaticBufs.lstNames.clear();
			StaticBufs.iCntThreads[0] = 0;
			StaticBufs.sNeedDownFN[0] = "";
			

			okGetUrl(url);
			
		}
		else
			setBtnText(url);

//		String sUrl2Player = "http://127.0.0.1:9999/?go="+url;
//		if(url.indexOf(".ts.m3u8") >=0)
//			sUrl2Player = url;
//		else if(url.indexOf(".m3u") >=0)
//			buffM3U8(url);
//		else
//			sUrl2Player = url;
		
		String extension = MimeTypeMap.getFileExtensionFromUrl(url);
        String mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
        if(mimeType == null)
        	mimeType = "video/*";
        
        Intent mediaIntent = new Intent(Intent.ACTION_VIEW);
//        mediaIntent.setDataAndType(Uri.parse(url), mimeType);
//        startActivity(mediaIntent);
        ///*****************************************************************************************
        
        //Ҫ������һ��APP��activity���ڵİ���
        //String packageName = "com.mxtech.videoplayer.pro";
        //Ҫ������һ��APP��activity����
        //String activity = "com.mxtech.videoplayer.pro/.ActivityMediaList";
        //ComponentName component = new ComponentName(packageName, activity);
        //Intent mediaIntent = new Intent();
        //mediaIntent.setComponent(component);
        mediaIntent.setDataAndType(Uri.parse(sUrl2Player), mimeType);
        //mediaIntent.setFlags(0x10200000);
        //intent.putExtra("data", setData());
        mediaIntent.setPackage("com.mxtech.videoplayer.pro");
        
        if(!sUrl2Player.equals(url))
	        while(StaticBufs.vFileMap.size() < 1)
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
        startActivity(mediaIntent);
        
//        String[] headers = {
//        		"User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.99 Safari/537.36",
//        		"Referer", "https://vidcloud.icu"};
//        		Intent intent = new Intent(Intent.ACTION_VIEW);
//        		intent.setDataAndType(Uri.parse(finalUrl), "video/*");
//        		intent.putExtra("headers", headers);
//        		intent.setPackage("com.mxtech.videoplayer.pro");
//        		startActivity(intent);
	}

	public void setBtnText(final String url) {
		this.runOnUiThread(new Runnable()
		{
		@Override
		public void run()
		{
			((Button) findViewById(R.id.sendd)).setText(url);
			((Button) findViewById(R.id.sendd)).setWidth(((Button) findViewById(R.id.sendd)).getMeasuredWidth());
		}
		});
	}


	public void buffM3U8(String url) {
		new Thread() {
        	String sUrl;
        	public void start0(String url) {
        		this.sUrl = url;
        		this.start();
        	}

        	@Override
        	public void run() {
        		//����д�����߳���Ҫ���Ĺ���
        		try {
        			//                	String filepath = Environment.getExternalStorageDirectory() + "/xxx";
        			//                	
        			//                	File file2 = new File(filepath);
        			//                	if (!file2.exists())
        			//                	file2.mkdirs();

        			while(true){
        				setBtnText("��ʼ���� " + this.sUrl);
        				Log.i("TAGo", "��ʼ���� " + this.sUrl);
        				String sUrl = this.sUrl;
        				DownUtil downUtil = new DownUtil(
        						sUrl,
        						//filepath + "/filename2",
        						"",
        						1);

        				bytesM3u8 = null;

        				StaticBufs.vFileMap.clear();
        				StaticBufs.lstNames.clear();
        				StaticBufs.iCntThreads[0] = 0;
        				StaticBufs.sNeedDownFN[0] = "";
        				bytesM3u8 = downUtil.downLoad();

        				Log.i("TAGo",  "M3u8 ������ɣ����� :" + bytesM3u8.length);
        				setBtnText("M3u8 ������ɣ����� :" + bytesM3u8.length);
        				
        				String strTmp = new String(bytesM3u8);
        				if(strTmp.indexOf(".m3u8") <0)
        				{//�޸�M3U8, �ܿ�MXPLAYER PRO��bug
        					modM3u8ForMXPlayer(sUrl);
        				}
        					Log.i("TAp", new String(bytesM3u8));  
        				//����M3U8
        				ByteArrayInputStream byteArray = new ByteArrayInputStream(bytesM3u8);
        				BufferedReader bInput = new BufferedReader(new InputStreamReader(byteArray));
        				//String[] subUrl = sUrl.split("/");
        				String sOneLine;  
        				while ((sOneLine= bInput.readLine()) != null) {  
        					Log.i("TAG", sOneLine);  

        					String sPrefix = "";
        					if((sOneLine.indexOf("/") ==0))
        						sPrefix = sUrl.substring(0, getFromIndex(sUrl, ("/"), 3));
        					else
        						sPrefix = sUrl.substring(0, sUrl.lastIndexOf("/"))+"/";
        					if(sOneLine.indexOf(".m3u8") >0) {
        						bytesM3u8 = null;
        						this.sUrl = sPrefix + sOneLine;
        						break;
        					}
        					if(((sOneLine.indexOf(".ts") >=0) 
        							|| (sOneLine.indexOf(".mp4") >=0)) && (sOneLine.indexOf(".m3u8") <0))
        					{
        						if((StaticBufs.sNeedDownFN[0].length() > 0) && !StaticBufs.haveKey(StaticBufs.sNeedDownFN[0])){
        							bInput = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(bytesM3u8)));
        							while ((sOneLine= bInput.readLine()) != null) {
        								if(sOneLine.equals(StaticBufs.sNeedDownFN[0]))
        								{
        									//StaticBufs.sCurPlayingPart[0] = "";

        									break;
        								}
        							}
        						}

        						if(StaticBufs.lstNames.indexOf(sOneLine) == -1)
        							StaticBufs.lstNames.add(sOneLine);
        						
    							for (int i = 0; i < StaticBufs.lstNames.size()-(MainActivity.MAX_BLOCKs/3); i++) {
    								StaticBufs.vecIngAndDone.remove(StaticBufs.lstNames.get(i));
    								StaticBufs.mapRemove(StaticBufs.lstNames.get(i));
        						//StaticBufs.lstNames.RemoveRange(1,3);
    							}

        						//if(!StaticBufs.conKey(sOneLine) && !DownUtil.isDowning(sOneLine))
        						//if(!StaticBufs.conKey(sOneLine) && !DownUtil.isDowning(sOneLine))
        						if(!StaticBufs.vecIngAndDone.contains(sOneLine))
        						{
        							StaticBufs.vecIngAndDone.addElement(sOneLine);
	        			            //UDP_Push.pushLog(sPrefix +sOneLine + " " +(!StaticBufs.conKey(sOneLine)) + "_" +!DownUtil.isDowning(sOneLine));
	
        							int i=1;
    								if(sOneLine.equals(StaticBufs.sNeedDownFN[0]))
    								{
    									StaticBufs.sNeedDownFN[0] = "";

    									i=5;
    								}
    								else
    	        						while((StaticBufs.iCntThreads[0] >= MAX_THREADS)||(StaticBufs.vFileMap.size() >= MainActivity.MAX_BLOCKs))
    	        							Thread.sleep(50);
    									
    								for(;i>0;i--){

		        						new Thread() {
		        							String sPrefix;
		        							String input;
		        							public void start0(String sPrefix, String input) {
		        								this.sPrefix = sPrefix;
		        								this.input = input;
		        								StaticBufs.iCntThreads[0]++;
		        								this.start();
		        							}
		        							@Override
		        							public void run() {
		        								//����д�����߳���Ҫ���Ĺ���
		        								DownUtil downOne = new DownUtil(
		        										sPrefix + input,
		        										//filepath + "/filename2",
		        										"",
		        										1);//��ʱֻ�ܵ��߳����أ�ֱ�������������skip
		        								byte[] pppm;
		        								try {
		        									setBtnText("��ʼ����: " + input);
		        						            UDP_Push.pushLog("Down: "+sPrefix + input + "->");
		        			        				pppm = downOne.downLoad(); //С�ģ����ڶ��߳�����ͬһ�ļ���һ����ɺ������̻߳���;�˳�������������ص�������ʵ������
		        									setBtnText(input + "������ɣ����� :" + pppm.length);
		        									//getFileByBytes(pppm, filepath, "file00000000.ts");
		        									Log.i("TAG", "len=" + StaticBufs.vFileMap.get(input).length);
		        								} catch (Exception e) {
		        									// TODO Auto-generated catch block
		        									e.printStackTrace();
		        								} finally {
		        									StaticBufs.iCntThreads[0]--;
		        								}
		        							}
		        						}.start0(sPrefix , sOneLine);							
    								}
        						}

        						//break;
        					}
        					//                        for (Map.Entry<String ,byte[]> entry : StaticBufs.vFileMap.entrySet()) {
        					//                			Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
        					//                			conn.setRequestProperty(entry.getKey(), entry.getValue());
        					//                		}

        				}  

			            if(bytesM3u8 != null) UDP_Push.pushLog("M3u8: "+ new String(bytesM3u8).substring(0, 500));
        				if(!sUrl.equals(this.sUrl))
        					continue;
        				else
        					break;
        			}	
        			//5�߳����أ���������
        			//����������5*2ʱ��ͣ
        			//tcp������������ɵģ�ɾ����-5λ��ǰ��
        		} catch (Exception e) {
        			e.printStackTrace();
        		}

        	}
		}.start0(url);
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
                sRcvUDPData = x;
                System.out.println(x);
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        result.setText(sRcvUDPData+"<>");
                        
                        //String url = "https://www.baidu.com/1.m3u8?plplp=899";//ʾ����ʵ�������������Ƶ����
                        String url = sRcvUDPData;//"http://www.baidu.com/1.m3u8";//ʾ����ʵ�������������Ƶ����
                        
                		new Thread() {
                        	String sUrl;
                        	public void start0(String url) {
                        		this.sUrl = url;
                        		this.start();
                        	}

                        	@Override
                        	public void run() {
                                openIntent(this.sUrl);
                        	}
                		}.start0(url);

                    }

                });
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    


	public static void modM3u8ForMXPlayer(String sUrl) throws IOException {
		String strTmp2 = "";
		ByteArrayInputStream byteArray = new ByteArrayInputStream(bytesM3u8);
		BufferedReader bInput = new BufferedReader(new InputStreamReader(byteArray));
		//String[] subUrl = sUrl.split("/");
		String sOneLine;  
		while ((sOneLine= bInput.readLine()) != null) {
			if(sOneLine.indexOf(".ts") >0) {
				if((sOneLine.indexOf("/") ==0))
					break;
				
				String sPrefix = "";
				sPrefix = sUrl.substring(0, sUrl.lastIndexOf("/"))+"/";
				sOneLine = sPrefix + sOneLine;

				sOneLine = sOneLine.substring(getFromIndex(sUrl, ("/"), 3));
			}
			strTmp2 += sOneLine;
			strTmp2 += "\n";
		}
		if(sOneLine== null)
			bytesM3u8 = strTmp2.getBytes();
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

	public static void setBtnText2(final String url2) {
		// ����һ�����̵߳� handler
		new Handler(MainActivity.getContext().getMainLooper()).post(
				new Runnable() {
					@Override
					public void run() {
						MainActivity.sendUDPBrocast1.setText(url2);
					}
				});
	}
}
