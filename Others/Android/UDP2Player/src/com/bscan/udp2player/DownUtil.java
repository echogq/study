package com.bscan.udp2player;

//import entity.Header;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.RandomAccessFile;
import java.math.BigDecimal;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;
import java.util.concurrent.CountDownLatch;

import android.os.Environment;
import android.util.Log;

/**
 * @author xbl
 * @date 2018-09-17
 */
public class DownUtil {
    //定义下载路径
    private String path;
    //指定所下载的文件的保存位置
    private String sFileName;
    //定义下载线程的数量
    private int threadNum;
    //定义下载线程的对象
    public static DownloadThread[] threads;
    //下载文件的总大小
    private long fileSize;
    //线程计数器
    private CountDownLatch latch;

    public DownUtil(String path,String targetFile,int threadNum){
        this.path = path;
        this.sFileName = targetFile;
        this.threadNum = threadNum;
        threads = new DownloadThread[threadNum];
        latch = new CountDownLatch(threadNum);
    }
    public byte[] downLoad() throws Exception{
        long t1 = System.currentTimeMillis();
        URL url = new URL(path);
        
        fileSize = -1;
        while(fileSize == -1)
        {
	        HttpURLConnection conn = (HttpURLConnection)url.openConnection();
	        conn.setConnectTimeout(2 * 1000);
	        conn.setReadTimeout(9 * 1000);
	        //设置请求方法
	        conn.setRequestMethod("GET");
	        //设置请求属性
	        for (Map.Entry<String ,String> entry : StaticBufs.header.entrySet()) {
				Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
				conn.setRequestProperty(entry.getKey(), entry.getValue());
			}
	        //Header.header.forEach((key, value) -> conn.setRequestProperty(key,value));
	
	        conn.setRequestProperty("Accept-Encoding", "identity");
	        //得到文件大小
	        fileSize = conn.getContentLength();
	        
	        //fileSize = 15000;
	        conn.disconnect();
        }
        long currentPartSize = fileSize / threadNum + 1;
        byte[]b = null;
        
        for(int i = 0;i < threadNum;i++){
            //计算每个线程的下载位置
            long startPos = i * currentPartSize;
            
            if(fileSize - startPos < currentPartSize)
            	currentPartSize = fileSize - startPos;
            if(sFileName.length() > 0){
	            //每个线程使用一个RandomAccessFile进行下载
	            RandomAccessFile currentPart = new RandomAccessFile(sFileName,"rw");
	            //定位该线程的下载位置
	            currentPart.seek(startPos);
	            //创建下载线程
	            threads[i] = new DownloadThread((int)startPos, (int)currentPartSize, currentPart , path , latch);
            }
            else{
            	if(b == null)
            		b = new byte[(int)fileSize];
            	//ByteArrayOutputStream bArray = new ByteArrayOutputStream(currentPartSize);
	            threads[i] = new DownloadThread((int)startPos, (int)currentPartSize, b , path , latch);
            }
            Thread t = new Thread(threads[i]);
            t.start();
        }
        BigDecimal n = BigDecimal.ZERO;
        while (true){
            BigDecimal length = BigDecimal.ZERO;
            if (latch.getCount()==0 && BigDecimal.ONE.compareTo(n)<=0){break;}
            for (DownloadThread downloadThread:threads) {
                length = length.add(new BigDecimal(downloadThread.getLength()));
                Log.i("TAG", "已下载 "+downloadThread+"：" + downloadThread.getLength()+".剩." + downloadThread.getRemainLen());
            }
            n = length.divide(new BigDecimal(fileSize), 4, BigDecimal.ROUND_HALF_UP);
            Log.i("TAG", "已下载:" + length + "/" + fileSize + "/" + n.multiply(new BigDecimal("100"))+"%");
            Thread.sleep(1000);
        }
        long t2 = System.currentTimeMillis();
        Log.i("TAG", "下载完成，文件大小为"
        +String.format("%.3f", fileSize*1.0/(1024*1024))+"M["+ fileSize+ "]，共用时"+(t2-t1)*1.0/1000+"秒,平均"
        +String.format("%.3f", fileSize*1.0/(1024*1024)/((t2-t1)/1000))+"MB/s");
		return b;
        
        
        //线程等待,一个文件下载成功后继续下一个
//        latch.await();
    }
	public static boolean isDowning(String sOneLine) {

        for (DownloadThread downloadThread:DownUtil.threads) {
        	if(downloadThread != null && downloadThread.getPath().equals(sOneLine))
        		return true;
        }
		return false;
	}
}