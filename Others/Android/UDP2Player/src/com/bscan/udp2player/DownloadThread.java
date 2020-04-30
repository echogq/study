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

    //��ǰ�̵߳�����λ��
    private int startPos;
    //���嵱ǰ�̸߳������ص��ļ���С
    private int currentPartSize;
    //��ǰ�߳���Ҫ���ص��ļ���,�����ʵ��֧�ֶ���������ļ��Ķ�ȡ��д�롣
    private RandomAccessFile currentPart;
    private byte[] bArray;
   //������߳������ص��ֽ���
    private int length;
    //�̼߳�����
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
    
        Log.i("TAG", "�������� "+Thread.currentThread().getName()+ " size: "  + currentPartSize);
        
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
            //�������󷽷�
            conn.setRequestMethod("GET");
            //������������
            for (Map.Entry<String ,String> entry : StaticBufs.header.entrySet()) {
    			System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
    			conn.setRequestProperty(entry.getKey(), entry.getValue());
    		}
            //Header.header.forEach((key, value) -> conn.setRequestProperty(key,value));
            inputStream = conn.getInputStream();
            //inputStream.skip(n);�����Ͷ����������������ݵ� n ���ֽ�
            inputStream.skip(this.startPos);
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            int needRead = (currentPartSize<1024)?currentPartSize:1024;
            //��ȡ��������д�뱾��
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
