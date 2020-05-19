package com.bscan.udp2player;

//import entity.Header;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;
import java.util.concurrent.CountDownLatch;

import okhttp3.Call;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

import android.util.Log;

/**
 * @author xbl
 * @date 2018-09-17
 */
public class DownloadThread implements Runnable {

    //当前线程的下载位置
    private int startPos;
    //定义当前线程负责下载的文件大小
    private int currentPartSize;
    //当前线程需要下载的文件块,此类的实例支持对随机访问文件的读取和写入。
    private RandomAccessFile currentPart;
    private byte[] bArray;
   //定义该线程已下载的字节数
    private int length;
    //线程计数器
    private CountDownLatch latch;

    private String path="";

    public DownloadThread(int startPos,int currentPartSize,RandomAccessFile currentPart, String path ,CountDownLatch latch){
        this.startPos = startPos;
        this.currentPartSize = currentPartSize;
        this.currentPart = currentPart;
        this.path = path;
        this.latch =latch;
        this.length = 0;
    }
    public DownloadThread(int startPos,int currentPartSize,byte[]  bArray, String path ,CountDownLatch latch){
        this.startPos = startPos;
        this.currentPartSize = currentPartSize;
        this.currentPart = null;
        this.bArray = bArray;
        this.path = path;
        this.latch =latch;
        this.length = 0;
    }
    @Override
    public void run(){
    
        Log.i("TAG", "即将下载 "+Thread.currentThread().getName()+ " size: "  + currentPartSize);
        
    	while(!downHTTP())
    	{
            this.currentPart = null;
            this.length = 0;
    	}	
        Log.i("TAG", "Done下载 "+Thread.currentThread().getName()+ " size: "  + currentPartSize);
    }
    
	public boolean downHTTP() {
		InputStream inputStream = null;
        try{
            URL url = new URL(path);
        	if(DownUtil.conn == null) {
	            DownUtil.conn = (HttpURLConnection)url.openConnection();
	            DownUtil.conn.setConnectTimeout(2 * 1000);
	            DownUtil.conn.setReadTimeout(9 * 1000);
	            //设置请求方法
	        	DownUtil.conn.setRequestMethod("GET");
	            //设置请求属性
	            for (Map.Entry<String ,String> entry : StaticBufs.header.entrySet()) {
	    			System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
	    			DownUtil.conn.setRequestProperty(entry.getKey(), entry.getValue());
	    		}
	            DownUtil.conn.setRequestProperty("Accept-Encoding", "identity");
        	}
        	else
        	{
//        		DownUtil.conn.getOutputStream().write(("GET " + "/123.ts"+ " HTTP/1.1\r\n").getBytes());
        	}
	        
        	String url2 = "https://leshi.cdn-zuyida.com/20180421/23526_27748718/index.m3u8";
        	okGetUrl(url2);
        	okGetUrl("https://leshi.cdn-zuyida.com/20180421/23526_27748718/800k/hls/index.m3u8");
            //Header.header.forEach((key, value) -> conn.setRequestProperty(key,value));
            inputStream = DownUtil.conn.getInputStream();
            //inputStream.skip(n);跳过和丢弃此输入流中数据的 n 个字节
            inputStream.skip(this.startPos);
            //byte[] buffer = new byte[StaticBufs.iBufBlockSize];
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            int needRead = (currentPartSize<StaticBufs.iBufBlockSize)?currentPartSize:StaticBufs.iBufBlockSize;
            //读取网络数据写入本地
            Log.i("TAG", Thread.currentThread().getName()+ " :::::: " + currentPartSize + " == " +  startPos );
            while(length < currentPartSize && (hasRead = inputStream.read(buffer, 0, 1024)) != -1){
            	if(StaticBufs.mapGet(path.substring(MainActivity.getFromIndex(path, ("/"), 3))) != null)
            	{
            		length = currentPartSize;
            		break;
            	}
            	if(currentPart != null)
            		currentPart.write(buffer, 0, hasRead);
            	else
//            		for(int index  = 0 ; index  < hasRead ; index ++){
//            			bArray[this.startPos+length+index] = buffer[index];
//                    }
            		System.arraycopy(buffer,0, bArray, this.startPos+length, hasRead);
            		//bArray.write(buffer, 0, hasRead);
    			 
                length += hasRead;
                needRead = (needRead<(currentPartSize-length))?needRead:(currentPartSize-length);
               // Log.i("TAG", Thread.currentThread().getName()+ " :::::: " + length + " -- " +  hasRead );
            }
            if(path.indexOf(".m3u8") <0)
            	if(StaticBufs.mapGet(path.substring(MainActivity.getFromIndex(path, ("/"), 3))) == null)
            		StaticBufs.mapPut(path.substring(MainActivity.getFromIndex(path, ("/"), 3)), bArray);

            UDP_Push.pushLog("Down: "+path+" [OK]");
             
            Log.i("TAG", Thread.currentThread().getName()+ " ...... " + length + " -- " +  hasRead );
        }
        catch(Exception e){
            e.printStackTrace();
    		return false;
        }finally {
            try {
            	if(currentPart != null)
            		currentPart.close();
            	if(inputStream != null)
            		inputStream.close();
            }catch (Exception e){
                e.printStackTrace();
            }
            latch.countDown();
        }
        
        if(length < currentPartSize)
    		return false;
        else
        	return true;
	}
	public void okGetUrl(String url2) {
		OkHttpClient okHttpClient = new OkHttpClient();
		final Request request = new Request.Builder()
		        .url(url2)
		        .build();
		final Call call = okHttpClient.newCall(request);
		new Thread(new Runnable() {
		    @Override
		    public void run() {
            	Thread.currentThread().setName("===okGetUrl"); 
		        try {
		            Response response = call.execute();
		            Log.d("TAG", "run: " + response.body().string());
		            
		        } catch (IOException e) {
		            e.printStackTrace();
		        }
		    }
		}).start();
	}

    public String getPath() {
        return path;
    }
    public int getLength() {
        return length;
    }
    public int getInitLen() {
        return currentPartSize;
    }
    public int getRemainLen() {
        return currentPartSize - length;
    }
}
