package com.bscan.udp2player;

//import entity.Header;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;
import java.util.concurrent.CountDownLatch;

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

    private String path;

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
    		;
    }
    
	public boolean downHTTP() {
		InputStream inputStream = null;
        try{
            URL url = new URL(path);
            HttpURLConnection conn = (HttpURLConnection)url.openConnection();
            conn.setConnectTimeout(5 * 1000);
            conn.setReadTimeout(2 * 1000);
            //设置请求方法
            conn.setRequestMethod("GET");
            //设置请求属性
            for (Map.Entry<String ,String> entry : StaticBufs.header.entrySet()) {
    			System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
    			conn.setRequestProperty(entry.getKey(), entry.getValue());
    		}
            //Header.header.forEach((key, value) -> conn.setRequestProperty(key,value));
            inputStream = conn.getInputStream();
            //inputStream.skip(n);跳过和丢弃此输入流中数据的 n 个字节
            inputStream.skip(this.startPos);
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            int needRead = (currentPartSize<1024)?currentPartSize:1024;
            //读取网络数据写入本地
            Log.i("TAG", Thread.currentThread().getName()+ " :::::: " + currentPartSize + " == " +  startPos );
            while(length < currentPartSize && (hasRead = inputStream.read(buffer, 0, needRead)) != -1){
            	if(currentPart != null)
            		currentPart.write(buffer, 0, hasRead);
            	else
            		System.arraycopy(buffer,0, bArray, this.startPos+length, hasRead);
            		//bArray.write(buffer, 0, hasRead);
                length += hasRead;
                needRead = (needRead<(currentPartSize-length))?needRead:(currentPartSize-length);
               // Log.i("TAG", Thread.currentThread().getName()+ " :::::: " + length + " -- " +  hasRead );
            }
            Log.i("TAG", Thread.currentThread().getName()+ " ...... " + length + " -- " +  hasRead );
        }
        catch(Exception e){
            e.printStackTrace();
    		return false;
        }finally {
            try {
            	if(currentPart != null)
            		currentPart.close();
                inputStream.close();
            }catch (Exception e){
                e.printStackTrace();
            }
            latch.countDown();
        }
		return true;
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
