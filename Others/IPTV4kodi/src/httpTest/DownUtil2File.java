package httpTest;

import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;


public class DownUtil2File {
    /* 下载资源的路径--url */
    private String sURL;
    private String targetFile;
    private int threadNum;
    /* 多线程下载 */
    private DownThread2File[] threads;
    private int fileSize;
    public DownUtil2File(String httpURL, String targetFile, int threadNum) {
        
        this.sURL = httpURL;
        this.targetFile = targetFile;
        threads = new DownThread2File[threadNum];
        this.threadNum = threadNum;
    }
    
    public void download() throws Exception{
        URL url = new URL(sURL);
        HttpURLConnection conn;
        conn = (HttpURLConnection)url.openConnection();
    
        conn.setConnectTimeout(5000);
        
        conn.setRequestMethod("GET");
        
        conn.setRequestProperty("Accept", "image/gif, image/x-xbitmap, " +
                "image/jpeg, image/pjpeg, application/x-shockwave-flash, " +
                "application/vnd.ms-excel, application/vnd.ms-powerpoint, " +
                "application/msword, application/x-silverlight, */*");
        
        conn.setRequestProperty("Accept-Language", "zh-CN");
        conn.setRequestProperty("Charset", "UTF-8");
        conn.setRequestProperty("Connection", "Keep-Alive");
        /* 得到文件的大小 */
        fileSize = conn.getContentLength();
        conn.disconnect();
        int currentPartSize = fileSize / threadNum + 1;
        RandomAccessFile file = new RandomAccessFile(targetFile, "rw");
        /* 设置本地文件的大小 */
        file.setLength(fileSize);
        file.close();
        
        for (int i = 0; i < threadNum; i++) {
            int startPos = i * currentPartSize;
            /* 每个线程使用一个RandomAccessFile进行下载 */
            RandomAccessFile currentPart = new RandomAccessFile(targetFile, "rw");
            /* 定位文件下载的位置*/
            currentPart.seek(startPos);
            threads[i] = new DownThread2File(sURL, startPos, currentPartSize, currentPart);
            threads[i].start();
        }
    }
    
    public double getCompleteRate(){
        int sumSize = 0;
        for (int i = 0; i < threadNum; i++) {
            sumSize += threads[i].length;
        }
        return sumSize * 1.0 / fileSize;
    }
}