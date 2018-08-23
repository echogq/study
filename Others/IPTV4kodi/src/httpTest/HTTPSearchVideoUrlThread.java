package httpTest;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.LinkedList;
import java.util.List;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonIOException;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.JsonSyntaxException;

public class HTTPSearchVideoUrlThread extends Thread  {
    static List<String> listStr = new LinkedList<String>();
	private static String sLastURL = "";
	private static String sLastm3u8URL = "";
	private static OutputStream localOS;
	private static int iPort = 7791;
	private static String [] videoSites = {"芒果TV", "腾讯", "爱奇艺", "电影网", "乐视", "在线观看", "PPS", "优酷"};
	
	@Override
	      public void run() {
			acpt();
	     }
	
    public static void acpt() {
    	Socket svrSocket = null;
        try {
            ServerSocket ss=new ServerSocket(iPort);
            

            while(true){
                System.out.println("\r\n"+iPort+" 等待连接。。。");
                svrSocket=ss.accept();
                svrSocket.setSoTimeout(10000);
                
//                svrSocket.setSendBufferSize(svrSocket.getSendBufferSize() * 16);
                System.out.println(svrSocket.getSendBufferSize());
                
                afterConnect(svrSocket);
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
    		System.out.println("\r\n一个连接异常了。。。。断开!!! " + iPort);
    		
            e.printStackTrace();
        }
    }

	public static void afterConnect(Socket svrSocket) throws IOException {
		System.out.println("\r\n");
		System.out.println("\r\n一个客户端连上了！：：：：：：：：：" + iPort);

		/**
		 * 接受HTTP请求
		 */
		BufferedReader bd=new BufferedReader(new InputStreamReader(svrSocket.getInputStream()));
		String requestHeader;
		int contentLength=0;
		String sIPTV_URL="";
		while ((requestHeader = bd.readLine()) != null && !requestHeader.isEmpty()) {
			System.out.println(requestHeader);
			/**
			 * 获得GET参数 : PROPFIND /ppp HTTP/1.1
			 * 
			 */
			if (requestHeader.indexOf(" HTTP/1.") >= 0) {
				int begin = requestHeader.indexOf("/") + 1;
				int end = requestHeader.indexOf(" HTTP/");
				sIPTV_URL = requestHeader.substring(begin, end);
				System.out.println("GET参数是：" + sIPTV_URL);
				// break;
			}
			/**
			 * 获得POST参数 1.获取请求内容长度
			 */
			String cl = "Content-Length:";
			if (requestHeader.startsWith(cl)) {
				int begin = requestHeader.indexOf(cl) + cl.length();
				String postParamterLength = requestHeader.substring(begin).trim();
				contentLength = Integer.parseInt(postParamterLength);
				System.out.println("POST参数长度是：" + Integer.parseInt(postParamterLength));
			}
		}

		// if(requestHeader.indexOf("HEAD ") < 0)
		// {

		localOS = svrSocket.getOutputStream();
		// InputStream localIs = svrSocket.getInputStream();
		// System.out.println(URLEncoder.encode("http://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&tn=baidu&wd="
		// + "放逐","utf-8"));
		System.out.println(URLEncoder.encode("http://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&tn=baidu&wd="
				+ URLDecoder.decode(sIPTV_URL, "UTF-8"), "utf-8"));
		System.out.println("播放电影：" + URLDecoder.decode(sIPTV_URL, "UTF-8"));

		parsePlayUrl(URLDecoder.decode(sIPTV_URL, "UTF-8"));
		
		svrSocket.close();
		System.out.println("\r\n一个客户端断开了。。。。");
	}

	public static void parsePlayUrl(String sSiteUrl) throws UnsupportedEncodingException, IOException {
		sSiteUrl = getJsonByInternet(
				"http://api.v2.flvurl.cn/parse/?appid=6170b6db0a881c18389f47d6d994340e&type=vod&url="
						+ URLEncoder.encode(sSiteUrl, "utf-8"));

		if (sSiteUrl.contains("\"code\":0") || sSiteUrl.contains("\"code\": 0")) {
			parseAndWriteVideoUrl2Player(sSiteUrl);
//					break;//可以播放，跳出循环
		} else {
			System.out.println("json出错了， sBody is：" + sSiteUrl);
			try {
				Thread.sleep(10000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public static void parseAndWriteVideoUrl2Player(String sBody) throws IOException {
		JsonParser parse = new JsonParser(); // 创建json解析器
		try {
			JsonObject json = (JsonObject) parse.parse(sBody); // 创建jsonObject对象
			// System.out.println("resultcode:"+json.get("data").getAsInt());
			// //将json数据转为为int型的数据
			// System.out.println("reason:"+json.get("reason").getAsString());
			// //将json数据转为为String型的数据
			String sQuality = "";
			for (JsonElement jsonElement : json.get("data").getAsJsonObject().get("streams").getAsJsonArray()) {
				sQuality += jsonElement.getAsJsonObject().get("quality").getAsString() + " ";
			}
			// 写入http video头
			writeHeader();

			// 流的清晰度（SPEED SD HD SUPERHD ORIGINAL 720P 1080P 2K 4K）
			if (sQuality.contains("ORIGINAL")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "ORIGINAL");
			} else if (sQuality.contains("4K")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "4K");
			} else if (sQuality.contains("2K")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "2K");
			} else if (sQuality.contains("1080P")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "1080P");
			} else if (sQuality.contains("720P")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "720P");
			} else if (sQuality.contains("SUPERHD")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "SUPERHD");
			} else if (sQuality.contains("HD")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "HD");
			} else if (sQuality.contains("SD")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "SD");
			} else if (sQuality.contains("SPEED")) {
				loopList(json.get("data").getAsJsonObject().get("streams").getAsJsonArray(), "SPEED");
			}
		} catch (JsonIOException e) {
			e.printStackTrace();
		} catch (JsonSyntaxException e) {
			e.printStackTrace();
		}
	}

	public static void writeHeader() throws IOException {
		localOS.write("HTTP/1.1 200 OK\r\n".getBytes());
		localOS.write("Connection: Keep-Alive\r\n".getBytes());
		// localOS.write("Date: Fri, 18 Aug 2017 07:23:29
		// GMT\r\n".getBytes());
		// localOS.write("Content-Length: 1150936\r\n".getBytes());
		// localOS.write("Content-Type: video/mpeg\r\n".getBytes());
		// localOS.write("Content-Range: bytes
		// 0-1150935/1150936\r\n".getBytes());
		// localOS.write("Last-Modified: Fri, 18 Aug 2017 07:22:19
		// GMT\r\n".getBytes());
		// localOS.write("Accept-Ranges: bytes\r\n".getBytes());
		// localOS.write("Server: HMS Download
		// Service\r\n".getBytes());
		localOS.write("\r\n".getBytes());
	}

	public static void loopList(JsonArray jsonElement, String sQuality) {
		for (JsonElement jj:jsonElement){
			System.out.println(jj.getAsJsonObject().get("quality").getAsString());
			if(jj.getAsJsonObject().get("quality").getAsString().equals(sQuality))
			{
				System.out.println(sQuality+" List:");
				for (JsonElement jsonElmt : jj.getAsJsonObject().get("segs").getAsJsonArray()) {
					System.out.println(jsonElmt.getAsJsonObject().get("url").getAsString());
					try {
//						localOS.write('"');
						localOS.write(jsonElmt.getAsJsonObject().get("url").getAsString().getBytes());
//						localOS.write('"');
						localOS.write(' ');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
				try {
					localOS.flush();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				break;
			}
		}
		
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
    public static String getJsonByInternet(String path){
        try {
        	System.out.println(" <getJsonByInternet> path=" + path);
            URL url = new URL(path.trim());
            //打开连接
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

            if(200 == urlConnection.getResponseCode()){
                //得到输入流
                InputStream is =urlConnection.getInputStream();
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                byte[] buffer = new byte[1024];
                int len = 0;
                while(-1 != (len = is.read(buffer))){
                    baos.write(buffer,0,len);
                    baos.flush();
                }
				System.out.println(baos.toString("utf-8").length() + " <=Len URL=> " + path);
                return baos.toString("utf-8");
            }
        }  catch (IOException e) {
            e.printStackTrace();
        }

        return "";
    }
    
    public static String get302ByInternet(String path){
    	String xxx="";
    	try {
            URL url = new URL(path.trim());
            //打开连接
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

            urlConnection.setInstanceFollowRedirects(false);
            
            xxx=urlConnection.getHeaderField("Location");
            if(xxx == null)
            {
            	xxx = path;
            }
        }  catch (IOException e) {
            e.printStackTrace();
        }

        return xxx;
    }
}