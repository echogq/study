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
public class DownThread2Buf extends Thread{
    
    private String sURL;
    public int startPos;
    public int currentPartSize;
    private byte[] currentPart = null;
    
    public int length;
    
    public DownThread2Buf(String httpURL, int startPos, int currentPartSize,
            byte[] xx) {
    	
        HttpRequestor.timeLog(xx.length +"<bufLen*****DownThread2Buf*****startPos---endPos size=" + startPos + "-" + (currentPartSize+startPos-1) + " " + (currentPartSize) );
        
        this.sURL = httpURL;
        this.startPos = startPos;
        this.currentPartSize = currentPartSize;
        this.currentPart = xx;
    }

    @Override
    public void run() {
        try {
            URL url = new URL(sURL);
            HttpRequestor.timeLog("\r\n" + sURL);
            
            HttpURLConnection conn;
            conn = (HttpURLConnection)url.openConnection();
            /* 设置超时时长 */
            conn.setConnectTimeout(5000);
            /* 设置发送请求的方法 */
            conn.setRequestMethod("GET");
            
//            conn.setRequestProperty("Accept", "*/*");
            conn.setRequestProperty("User-Agent", "Daum PotPlayer");
//            conn.setRequestProperty("Connection", "Keep-Alive");
//            conn.setRequestProperty("Range", "bytes=" + startPos + "-" + (startPos+currentPartSize-1));
            conn.setRequestProperty("Pragma", "no-cache");
            conn.setRequestProperty("Host", "221.179.217.75");
            
//            GET http://221.179.217.75/wh7f454c46tw95055278_241730986/PLTV/88888888/224/3221225684/1502866861-1-247481.hls.ts HTTP/1.1
//            	User-Agent: Daum PotPlayer
//            	Host: 221.179.217.75
//            	Pragma: no-cache            
            
            HttpRequestor.timeLog("**********bytes=" + startPos + "-" + (startPos+currentPartSize-1));
            HttpRequestor.timeLog("\r\n");
            
    		for (int n = 0;; n++) {
    			String sTmp = conn.getHeaderFieldKey(n);
    			String sTmp2 = conn.getHeaderField(n);
    			if (sTmp2 == null) {
    				break;
    			}
    			if (n == 0) {
    				HttpRequestor.timeLog(sTmp2);
    			} else {
    				HttpRequestor.timeLog(sTmp + ": " + sTmp2);
    			}
    		}
            
            /* 返回该Socket对象对应的输出流，让程序通过该输入流从Socket中取出数据 */
            InputStream inStream = conn.getInputStream();
            HttpRequestor.timeLog("开始下载");
//            inStream.skip(startPos);//寻到开始下载的位置
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            while(true)
            	if(inStream.read(buffer)==-1) break;
            HttpRequestor.timeLog("end下载");
            
            while(length < currentPartSize && (hasRead = inStream.read(currentPart, this.startPos + length, Math.min(1024, currentPartSize - length))) >= 0){
//                currentPart..write(buffer, 0, hasRead);
                length += hasRead;
            }
            HttpRequestor.timeLog("length=" + length + " =======/====== " + "currentPartSize=" + currentPartSize + " startPos+length= " + (this.startPos + length));
//            currentPart.close();
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