package httpTest;

import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;

/**
 * 线程类
 * @author Administrator
 *
 */
public class DownThread2File extends Thread{
    
    private String sURL;
    private int startPos;
    private int currentPartSize;
    private RandomAccessFile currentPart;
    
    public int length;
    
    public DownThread2File(String httpURL, int startPos, int currentPartSize,
            RandomAccessFile currentPart) {
        
        this.sURL = httpURL;
        this.startPos = startPos;
        this.currentPartSize = currentPartSize;
        this.currentPart = currentPart;
    }

    @Override
    public void run() {
        try {
            URL url = new URL(sURL);
            HttpURLConnection conn;
            conn = (HttpURLConnection)url.openConnection();
            /* 设置超时时长 */
            conn.setConnectTimeout(5000);
            /* 设置发送请求的方法 */
            conn.setRequestMethod("GET");
            
            conn.setRequestProperty("Accept", "image/gif, image/x-xbitmap, " +
                    "image/jpeg, image/pjpeg, application/x-shockwave-flash, " +
                    "application/vnd.ms-excel, application/vnd.ms-powerpoint, " +
                    "application/msword, application/x-silverlight, */*");
            
            conn.setRequestProperty("Accept-Language", "zh-CN");
            conn.setRequestProperty("Charset", "UTF-8");
            /* 返回该Socket对象对应的输出流，让程序通过该输入流从Socket中取出数据 */
            InputStream inStream = conn.getInputStream();
            inStream.skip(startPos);//寻到开始下载的位置
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            while(length < currentPartSize && (hasRead = inStream.read(buffer)) > 0){
                currentPart.write(buffer, 0, hasRead);
                length += hasRead;
            }
            currentPart.close();
            inStream.close();
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (ProtocolException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    
}