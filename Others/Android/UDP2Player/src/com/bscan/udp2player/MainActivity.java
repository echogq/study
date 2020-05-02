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
	protected static final int MAX_THREADS = 5;
    public static final int MAX_BLOCKs = 50;

    static byte[] bytesM3u8 = null;
    
  //子字符串modelStr在字符串str中第count次出现时的下标
    private int getFromIndex(String str, String modelStr, Integer count) {
    	//对子字符串进行匹配
            Matcher slashMatcher = Pattern.compile(modelStr).matcher(str);
    	int index = 0;
            //matcher.find();尝试查找与该模式匹配的输入序列的下一个子序列
    		while(slashMatcher.find()) {
    	    index++;
    	    //当modelStr字符第count次出现的位置
    	    if(index == count){
    	       break;
    	    }
    	}
            //matcher.start();返回以前匹配的初始索引。
    	return slashMatcher.start();
    }
    
	//将Byte数组转换成文件
    public static void getFileByBytes(byte[] bytes, String filePath, String fileName) {
        BufferedOutputStream bos = null;
        FileOutputStream fos = null;
        File file = null;
        try {
//            File dir = new File(filePath);
//            if (!dir.exists() && dir.isDirectory()) {// 判断文件目录是否存在
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
     * 单击按钮时，
     * */
    class SendUDPBrocastListener1 implements View.OnClickListener {
 
        @Override
        public void onClick(View v) {
            new Thread(new Runnable() {
                @Override
                public void run() {
//                	openIntent("https://tv1.youkutv.cc/2020/03/28/h0fA8TSZSijKdCi4/playlist.m3u8");
//                	openIntent("https://cn4.5311444.com/hls/20190426/97ed0bf400fc7efb547d3f91ea31d7b1/1556253639/index.m3u8");
                	openIntent("https://leshi.cdn-zuyida.com/20180421/23526_27748718/index.m3u8");
               }
            }).start();
         }
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
        
        Button sendUDPBrocast1 = (Button) findViewById(R.id.sendd);
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

	public void openIntent(String url) {
		String sUrl2Player = "http://127.0.0.1:9999";
		if(url.indexOf(".ts.m3u8") >=0)
			sUrl2Player = url;
		else if(url.indexOf(".m3u") >=0)
			buffM3U8(url);
		else
			sUrl2Player = url;


		
		String extension = MimeTypeMap.getFileExtensionFromUrl(url);
        String mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
        if(mimeType == null)
        	mimeType = "video/*";
        
        Intent mediaIntent = new Intent(Intent.ACTION_VIEW);
//        mediaIntent.setDataAndType(Uri.parse(url), mimeType);
//        startActivity(mediaIntent);
        ///*****************************************************************************************
        
        //要调用另一个APP的activity所在的包名
        //String packageName = "com.mxtech.videoplayer.pro";
        //要调用另一个APP的activity名字
        //String activity = "com.mxtech.videoplayer.pro/.ActivityMediaList";
        //ComponentName component = new ComponentName(packageName, activity);
        //Intent mediaIntent = new Intent();
        //mediaIntent.setComponent(component);
        mediaIntent.setDataAndType(Uri.parse(sUrl2Player), mimeType);
        //mediaIntent.setFlags(0x10200000);
        //intent.putExtra("data", setData());
        mediaIntent.setPackage("com.mxtech.videoplayer.pro");
        
        while(StaticBufs.vFileMap.size() == 0)
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

	public void buffM3U8(String url) {
		new Thread() {
        	String sUrl;
        	public void start0(String url) {
        		this.sUrl = url;
        		this.start();
        	}

        	@Override
        	public void run() {
        		//这里写入子线程需要做的工作
        		try {
        			//                	String filepath = Environment.getExternalStorageDirectory() + "/xxx";
        			//                	
        			//                	File file2 = new File(filepath);
        			//                	if (!file2.exists())
        			//                	file2.mkdirs();

        			while(true){
        				Log.i("TAGo", "开始下载 " + this.sUrl);
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
        				StaticBufs.sCurPlayingPart[0] = "";
        				bytesM3u8 = downUtil.downLoad();

        				Log.i("TAGo",  "M3u8 下载完成，长度 :" + bytesM3u8.length);
        				
        				String strTmp = new String(bytesM3u8);
        				if(strTmp.indexOf(".m3u8") <0)
        				{//修改M3U8, 避开MXPLAYER PRO的bug
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
        					Log.i("TAp", new String(bytesM3u8));  
        				//解析M3U8
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
        						if((StaticBufs.sCurPlayingPart[0].length() > 0) && !StaticBufs.vFileMap.containsKey(StaticBufs.sCurPlayingPart[0])){
        							bInput = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(bytesM3u8)));
        							while ((sOneLine= bInput.readLine()) != null) {
        								if(sOneLine.equals(StaticBufs.sCurPlayingPart[0]))
        									break;
        							}
        						}

        						while((StaticBufs.iCntThreads[0] >= MAX_THREADS)||(StaticBufs.vFileMap.size() >= MainActivity.MAX_BLOCKs))
        							Thread.sleep(50);

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
        								//这里写入子线程需要做的工作
        								DownUtil downOne = new DownUtil(
        										sPrefix + input,
        										//filepath + "/filename2",
        										"",
        										1);//暂时只能单线程下载，直到改正了里面的skip
        								byte[] pppm;
        								try {
        									pppm = downOne.downLoad();
        									StaticBufs.vFileMap.put(input, pppm);
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


        						//break;
        					}
        					//                        for (Map.Entry<String ,byte[]> entry : StaticBufs.vFileMap.entrySet()) {
        					//                			Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
        					//                			conn.setRequestProperty(entry.getKey(), entry.getValue());
        					//                		}

        				}  

        				if(!sUrl.equals(this.sUrl))
        					continue;
        				else
        					break;
        			}	
        			//5线程下载，存入数组
        			//数组总数》5*2时暂停
        			//tcp监听，请求完成的，删除其-5位置前的
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
                beforeResult = x;
                System.out.println(x);
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        result.setText(beforeResult+"<>");
                        
                        //String url = "https://www.baidu.com/1.m3u8?plplp=899";//示例，实际填你的网络视频链接
                        String url = beforeResult;//"http://www.baidu.com/1.m3u8";//示例，实际填你的网络视频链接
                        
                        openIntent(url);

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
