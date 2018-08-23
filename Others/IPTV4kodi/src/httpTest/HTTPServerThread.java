package httpTest;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;

public class HTTPServerThread extends Thread  {
    static List<String> listStr = new LinkedList<String>();
	private static String sLastURL = "";
	private static String sLastm3u8URL = "";
    	static Socket svrSocket = null;
	
	@Override
	      public void run() {
			acpt();
	     }
	
    public static void acpt() {
        try {
            ServerSocket ss=new ServerSocket(7788);
            

            while(true){
                System.out.println("\r\n7788 等待连接。。。");
                svrSocket=ss.accept();
                svrSocket.setSoTimeout(10000);
                
//                svrSocket.setSendBufferSize(svrSocket.getSendBufferSize() * 16);
                System.out.println(svrSocket.getSendBufferSize());
                
                Runnable newAccept = new Runnable(){
                	   public void run(){
                	     System.out.println("Runnable running");
			                try {
								afterConnect(svrSocket);
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
                	   }
                	};
                	Thread thread = new Thread(newAccept);
                 	thread.start();

                //afterConnect(svrSocket);
//				new Thread(new Runnable() {
//					@Override
//					public void run() {
//		                try {
//							afterConnect(svrSocket);
//						} catch (IOException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						}
//					}
//				}).start();


            }
        } catch (IOException e) {
        	
    		try {
				svrSocket.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
    		System.out.println("\r\n一个连接异常了。。。。断开!!!");
    		
            e.printStackTrace();
        }
    }

	public static void afterConnect(Socket svrSocket) throws IOException {
		System.out.println("\r\n");
		System.out.println("\r\n一个客户端连上了！：：：：：：：：：");

		/**
		 * 接受HTTP请求
		 */
		BufferedReader bd=new BufferedReader(new InputStreamReader(svrSocket.getInputStream()));
		String requestHeader;
		int contentLength=0;
		String sIPTV_URL="";
		while((requestHeader=bd.readLine())!=null&&!requestHeader.isEmpty()){
		    System.out.println(requestHeader);
		    /**
		     * 获得GET参数 : PROPFIND /ppp HTTP/1.1

		     */
		    if(requestHeader.indexOf(" HTTP/1.") >= 0){
		        int begin = requestHeader.indexOf("/")+1;
		        int end = requestHeader.indexOf(" HTTP/");
		        sIPTV_URL=requestHeader.substring(begin, end);
		        System.out.println("GET参数是："+sIPTV_URL);
//                        break;
		    }
		    /**
		     * 获得POST参数
		     * 1.获取请求内容长度
		     */
		    String cl = "Content-Length:";
		    if(requestHeader.startsWith(cl)){
		        int begin=requestHeader.indexOf(cl)+cl.length();
		        String postParamterLength=requestHeader.substring(begin).trim();
		        contentLength=Integer.parseInt(postParamterLength);
		        System.out.println("POST参数长度是："+Integer.parseInt(postParamterLength));
		    }
		}
		
//                if(requestHeader.indexOf("HEAD ") < 0)
//		{

		    //Socket mSocket = new Socket("127.0.0.1", 7788); 
		    //InputStream piIns = svrSocket.getInputStream();
		    OutputStream localOS = svrSocket.getOutputStream();
//		    InputStream localIs = svrSocket.getInputStream();
		    
		    try {
		    	if(!((sLastURL.contains(".ts") && sIPTV_URL.contains(".ts"))))
		    	{
			    	HTTPSocket.newUrl(sIPTV_URL, localOS);
			    	sLastURL = sIPTV_URL;
			    	if(!sIPTV_URL.contains(".ts"))
			    	{
			    		sLastm3u8URL = sIPTV_URL;
			    	}
		    	}
		    	else
		    	{
		    		System.out.println("【访问m3u8先】：" + sLastm3u8URL);
		    		HTTPSocket.newUrl(sLastm3u8URL, null);
		    		System.out.println("【访问ts】：" + sIPTV_URL);
		    		HTTPSocket.newUrl(sIPTV_URL, localOS);
			    	sLastURL = sIPTV_URL;
			    	
//		    		HTTPSocket.mTimeMap.remove(sIPTV_URL.split("/")[sIPTV_URL.split("/").length-1]);
//		    		System.out.println("removed: " + sIPTV_URL.split("/")[sIPTV_URL.split("/").length-1]);
		    		for (String key : HTTPSocket.mTimeMap.keySet()) {

		    			System.out.println("【mTimeMap】：" + key + " = " + HTTPSocket.mTimeMap.get(key));

		    		}
		    	}
		    	
		    	//写入http video头
//		    	localOS.write("HTTP/1.1 200 OK\r\n".getBytes());
//		    	localOS.write("Connection: Keep-Alive\r\n".getBytes());
////		    	localOS.write("Date: Fri, 18 Aug 2017 07:23:29 GMT\r\n".getBytes());
////		    	localOS.write("Content-Length: 1150936\r\n".getBytes());
//		    	localOS.write("Content-Type: video/mpeg\r\n".getBytes());
////		    	localOS.write("Content-Range: bytes 0-1150935/1150936\r\n".getBytes());
////		    	localOS.write("Last-Modified: Fri, 18 Aug 2017 07:22:19 GMT\r\n".getBytes());
//		    	localOS.write("Accept-Ranges: bytes\r\n".getBytes());
//		    	localOS.write("Server: HMS Download Service\r\n".getBytes());
//		    	localOS.write("\r\n".getBytes());
		    	//读取文件写入
//		    	readFileByBytes("e:/Media/done/Erika Momodani - 120% Derived From Natural Ingredients.avi", localOS);
//		    	readFileByBytes("n:/Media/done/(S Model)(SMD-115)S Model 115 恥辱の中出し授業 大橋未久.mp4", localOS);
//		    	readFileByBytes("n:/Media/done/Young.Sister-In-Law2.Cut.mkv", localOS);
//		    	readFileByBytes("e:/Media/done/半妖乳娘.The Demon Wet Nurse（李翰祥经典作品）.mkv", localOS);
//		    	readFileByBytes("n:/Backup/1503047055-1-261341.hls.ts", localOS);
		    	
		    	
		        //System.out.println(new HttpRequestor().doPost("http://localhost:8081/TestProgect2/LoginServlet", dataMap));
//	                	while(true)
//	                	{
//	                		System.out.println(new HttpRequestor().doGet(os, listStr, sIPTV_URL));
//	                		Thread.sleep(1000);
//	                	}
		    	/*if(HttpRequestor.strRealAddress.length() > 0)
		    	{
		    		sIPTV_URL = HttpRequestor.strRealAddress + sIPTV_URL.substring(sIPTV_URL.lastIndexOf("/") + 1);
			    	HttpRequestor.timeLog("\r\nstrRealAddress + GET: " + sIPTV_URL);
		    		HttpRequestor.doGet(localOS, localIs ,sIPTV_URL.substring(sIPTV_URL.indexOf("/", 8) ));
		    	}
		    	else//First get
		    	{
		    		HttpRequestor.doGet(localOS, localIs , sIPTV_URL);
		    	}*/
//		        	if(sIPTV_URL.contains(".ts"))
//		        	{
//				    	if(HttpRequestor.strRealAddress.length() > 0)
//				    	{
//				    		sIPTV_URL = HttpRequestor.strRealAddress + sIPTV_URL.substring(sIPTV_URL.lastIndexOf("/") + 1);
//				    	}

//
//				    	if(listStr.contains(sIPTV_URL) == false)
//				    	{
//				        		DownUtil2Buf sss = new DownUtil2Buf(sIPTV_URL, 1);
//				        		sss.download(localOS);
//				        		listStr.add(sIPTV_URL);
//				    	}
//		        		
////		        		if(new HttpRequestor().doDownload(os, sIPTV_URL))
////		        			listStr.add(sIPTV_URL);
//		        	}
//		        	else
//		        	{
//		        		System.out.println(HttpRequestor.doGet(localOS, sIPTV_URL));
//		        	}

		    } catch (Exception e) {
		        // TODO Auto-generated catch block
		        e.printStackTrace();
		    }
//	}
			
		svrSocket.close();
		System.out.println("\r\n一个客户端断开了。。。。");
          
	}
	/**
     * 以字节为单位读取文件，常用于读二进制文件，如图片、声音、影像等文件。
	 * @param localOS 
     */
    public static void readFileByBytes(String fileName, OutputStream localOS) {
        InputStream in = null;
//        File file = new File(fileName);
//        try {
//            System.out.println("以字节为单位读取文件内容，一次读一个字节：");
//            // 一次读一个字节
//            in = new FileInputStream(file);
//            int tempbyte;
//            while ((tempbyte = in.read()) != -1) {
//                System.out.write(tempbyte);
//            }
//            in.close();
//        } catch (IOException e) {
//            e.printStackTrace();
//            return;
//        }
        try {
            System.out.println("以字节为单位读取文件内容，一次读多个字节：");
            // 一次读多个字节
            byte[] tempbytes = new byte[1024];
            int byteread = 0;
            int bytereadTT = 0;
            in = new FileInputStream(fileName);
            showAvailableBytes(in);
            // 读入多个字节到字节数组中，byteread为一次读入的字节数
            while ((byteread = in.read(tempbytes)) != -1) {
            	localOS.write(tempbytes, 0, byteread);
            	localOS.flush();
            	
            	bytereadTT += byteread;
                System.out.println("字节：" + bytereadTT);
            }
        } catch (Exception e1) {
            e1.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e1) {
                }
            }
        }
    }
    /**
     * 显示输入流中还剩的字节数
     */
    private static void showAvailableBytes(InputStream in) {
        try {
            System.out.println("当前字节输入流中的字节数为:" + in.available());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}