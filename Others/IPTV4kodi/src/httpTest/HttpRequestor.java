package httpTest;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.NetworkInterface;
import java.net.Proxy;
import java.net.SocketException;
import java.net.URL;
import java.net.URLConnection;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HttpRequestor {

	private String charset = "utf-8";
	private Integer connectTimeout = null;
	private Integer socketTimeout = null;
	private static String proxyHost = null;
	private static Integer proxyPort = null;
	private OutputStream SvrOS;
	private byte[] tsBytes;
	static String strRealAddress = "";
	private static int tsTotalSize = 0;
	private static int tsCurSize = 0;
	private static String tsName = "";
	private static String tsHeader = "";
	private static ByteBuffer tsBuf = null;
	static List<String> listFileStr = new LinkedList<String>();
	private static String sFirst = "";

	static HttpURLConnection httpURLConnection = null;
	static InputStream inputStream = null;
	static InputStreamReader inputStreamReader = null;
	static BufferedReader reader = null;
	static StringBuffer resultBuffer = null;

	/**
	 * Do GET request
	 * 
	 * @param os
	 * @param url
	 * @return
	 * @throws Exception
	 * @throws IOException
	 */
	public boolean doDownload(OutputStream os, String url) throws Exception {
		boolean bResult = true;

		url = strRealAddress + url.substring(url.lastIndexOf("/") + 1);
		if ((tsName == url) && (tsCurSize > 0)) {
			while (tsCurSize < tsTotalSize) {// 等待上个缓冲线程结束
				Thread.sleep(100);
			}
			// 写入返回头，写入实际size
			os.write(tsHeader.getBytes());

			// 写入数据
			os.write(tsBytes);
			timeLog("缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: 缓冲: ");

			initBuf();
		} else {
			URL localURL = new URL(url);

			URLConnection connection = this.openConnection(localURL);
			HttpURLConnection httpURLConnection = (HttpURLConnection) connection;

			// NetworkInterface nif = NetworkInterface.getByName("eth0");
			// Enumeration<InetAddress> nifAddresses = nif.getInetAddresses();
			//
			// Socket soc = new java.net.Socket();
			// soc.bind (new InetSocketAddress(nifAddresses.nextElement(), 0));
			// soc.connect(new InetSocketAddress(address, port));
			//
			// Proxy proxy = new Proxy(Proxy.Type.HTTP, new InetSocketAddress(
			// "proxy", 5555));
			// URLConnection urlConnection = url.openConnection(proxy);

			// httpURLConnection.setRequestProperty("Accept-Charset", charset);
			// httpURLConnection.setRequestProperty("Content-Type",
			// "application/x-www-form-urlencoded");

			// timeLog(outStr);

			InputStream inputStream = null;
			InputStreamReader inputStreamReader = null;
			BufferedReader reader = null;
			StringBuffer resultBuffer = new StringBuffer();
			int tempLine = 0;
			// 响应失败
			if (httpURLConnection.getResponseCode() >= 300) {
				throw new Exception(
						"HTTP Request is not success, Response code is " + httpURLConnection.getResponseCode());
			}
			// 获取所有响应头字段
			timeLog("\r\ndoDownload 所有响应头字段: ");

			for (int n = 0;; n++) {
				String sTmp = httpURLConnection.getHeaderFieldKey(n);
				String sTmp2 = httpURLConnection.getHeaderField(n);
				if (sTmp2 == null) {
					break;
				}
				if (n == 0) {
					timeLog(sTmp2);
					os.write(sTmp2.getBytes());
				} else {
					timeLog(sTmp + ": " + sTmp2);
					os.write((sTmp + ": " + sTmp2).getBytes());
				}
				os.write(13);
				os.write(10);
			}
			os.write(13);
			os.write(10);
			// os.write(13);
			// os.write(10);

			try {
				inputStream = httpURLConnection.getInputStream();
				/*
				 * inputStreamReader = new InputStreamReader(inputStream);
				 * reader = new BufferedReader(inputStreamReader);
				 * 
				 * CharBuffer arg0 = CharBuffer.allocate(10240); tempLine =
				 * reader.read(arg0); while (tempLine >= 0) {
				 * timeLog("本次接收长度："+tempLine); //resultBuffer.append(tempLine);
				 * tempLine = reader.read(arg0); }
				 */

				Date d = new Date();
				int sec = d.getSeconds();

				byte[] buffer = new byte[1024];
				int len = 0;

				long tlen = 0;
				long lastsecTlen = 0;
				// ByteArrayOutputStream bos = new ByteArrayOutputStream();
				while ((len = inputStream.read(buffer)) != -1) {
					tlen += len;
					// timeLog("本次接收长度/总长度："+len + "/" + tlen);
					// bos.write(buffer, 0, len);

					d = new Date();
					if ((sec) != (d.getSeconds())) {
						double fSpeed = (tlen - lastsecTlen) / 1024.0 / 1024.0;
						timeLog("*************** 速度： " + String.format("%.2f", fSpeed) + "M/s");
						if ((fSpeed < 0.10) && (lastsecTlen != 0)) {
							break;
						}
						lastsecTlen = tlen;
						sec = d.getSeconds();
					}
					try {
						os.write(buffer, 0, len);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						timeLog("*******IOException******** 本次接收长度/总长度：" + len + "/" + tlen);
						bResult = false;
						throw e;
						// break;
					} /*
						 * catch (SocketException e) { // TODO Auto-generated
						 * catch block e.printStackTrace(); }
						 */
					os.flush();
				}
				// bos.close();
				// return bos.toByteArray();
				timeLog(localURL + " 下载总长度：" + tlen);

			} finally {

				if (reader != null) {
					reader.close();
				}

				if (inputStreamReader != null) {
					inputStreamReader.close();
				}

				if (inputStream != null) {
					inputStream.close();
				}

			}
		}

		int i = listFileStr.lastIndexOf(url.substring(url.lastIndexOf("/") + 1));

		while (i >= 0) {
			listFileStr.remove(i);
			i--;
		}
		// 下载它的下一个
		// if(listFileStr.size() > 0)
		// DownloadNextFileToBuf(strRealAddress + listFileStr.get(0));

		return bResult;

		// return resultBuffer.toString();
	}

	public static void timeLog(String outStr) {
		Date d = new Date();
		String s = null;
		/** 输出格式: 2014-5-05 00:00:00 大写H为24小时制 */
		DateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS ==>");
		s = sdf.format(d);
		System.out.println(s + "ThreadID=" + Thread.currentThread().getId() + " " + outStr);
	}

	private void DownloadNextFileToBuf(String url) throws Exception {
		if (tsName == url) {
			return;
		}
		tsName = url;
		// http://111.13.42.8/PLTV/88888888/224/3221225885/1502699833-1-1499223698.hls.ts
		// 取得当前文件名 + 1
		// String tmp =
		// "http://111.13.42.8/PLTV/88888888/224/3221225885/1502699833-1-1499223698.hls.ts";
		String tmp = url;

		// Pattern p = Pattern.compile("\\d+(,\\d{3})*");
		// Matcher m = p.matcher(tmp);
		// String xxx = null;
		// while (m.find()){
		// xxx = (m.group());
		// }
		// tsName = tmp.replace(xxx, "" + (Integer.parseInt(xxx) + 1));
		// timeLog(tmp);//原串
		// timeLog(tsName);//新串

		URL localURL = new URL(url);
		// tsName = "" + (Integer.parseInt(xxx) + 1);

		URLConnection connection = this.openConnection(localURL);
		HttpURLConnection httpURLConnection = (HttpURLConnection) connection;

		httpURLConnection.setRequestProperty("Pragma", "no-cache");

		InputStream inputStream = null;
		InputStreamReader inputStreamReader = null;
		BufferedReader reader = null;
		// 响应失败
		if (httpURLConnection.getResponseCode() >= 300) {
			throw new Exception("DownloadNextFileToBuf: HTTP Request is not success, Response code is "
					+ httpURLConnection.getResponseCode());
		}
		// 获取所有响应头字段
		timeLog("\r\ndoDownload 所有响应头字段: ");

		tsTotalSize = Integer.parseInt(httpURLConnection.getHeaderField("Content-Length"));

		tsBytes = new byte[tsTotalSize];
		tsBuf = ByteBuffer.wrap(tsBytes);

		for (int n = 0;; n++) {
			String sTmp = httpURLConnection.getHeaderFieldKey(n);
			String sTmp2 = httpURLConnection.getHeaderField(n);
			if (sTmp2 == null) {
				break;
			}
			if (n == 0) {
				// timeLog(sTmp2);
				// tsBuf.put(sTmp2.getBytes());
				tsHeader += sTmp2;
				// os.write(sTmp2.getBytes());
			} else {
				// timeLog(sTmp + ": " + sTmp2);
				// os.write((sTmp + ": " + sTmp2).getBytes());
				// tsBuf.put((sTmp + ": " + sTmp2).getBytes());
				tsHeader += (sTmp + ": " + sTmp2);
			}
			// os.write(13);
			// os.write(10);
			// tsBuf.put(("\r\n").getBytes());
			tsHeader += "\r\n";
		}
		// os.write(13);
		// os.write(10);
		tsHeader += "\r\n";

		try {
			inputStream = httpURLConnection.getInputStream();

			byte[] buffer = new byte[1024];
			int len = 0;
			tsCurSize = 0;
			// ByteArrayOutputStream bos = new ByteArrayOutputStream();
			while ((len = inputStream.read(buffer)) != -1) {
				// os.write(buffer, 0, len);
				tsBuf.put(buffer, 0, len);
			}
			// bos.close();
			// return bos.toByteArray();

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			tsBuf.flip();
			tsCurSize = tsBuf.limit();
			timeLog(localURL + " 下载总长度：" + tsCurSize);

			if (reader != null) {
				reader.close();
			}

			if (inputStreamReader != null) {
				inputStreamReader.close();
			}

			if (inputStream != null) {
				inputStream.close();
			}
		}
	}

	public void initBuf() {
		tsTotalSize = 0;
		tsCurSize = 0;
		tsName = "";
		tsHeader = "";
		tsBuf = null;
		tsBytes = null;
		// tsBuf.clear();
	}

	public static String doGet(OutputStream localOs, InputStream localIs, String url) throws Exception {

		// SvrOS = os;
		
		if(sFirst.length() == 0)
		{
			sFirst = url;

			URL localURL = new URL(url);
	
			URLConnection connection = openConnection(localURL);
			httpURLConnection = (HttpURLConnection) connection;
			httpURLConnection.setDoOutput(true);
			httpURLConnection.setRequestProperty("connection", "Keep-Alive");
	
			// httpURLConnection.setRequestProperty("Accept-Charset", charset);
			// httpURLConnection.setRequestProperty("Content-Type",
			// "application/x-www-form-urlencoded");
			// httpURLConnection.setInstanceFollowRedirects(false);
	
			resultBuffer = new StringBuffer();
			String tempLine = null;
			// 响应失败
			if (httpURLConnection.getResponseCode() >= 300) {
				if (httpURLConnection.getResponseCode() != 302) {
					throw new Exception(
							"HTTP Request is not success, Response code is " + httpURLConnection.getResponseCode());
				} else {
	
				}
			}
			// timeLog(httpURLConnection.getHeaderFields());
			timeLog("\r\nURL: " + httpURLConnection.getURL());
			
			strRealAddress = httpURLConnection.getURL().toString().substring(0, httpURLConnection.getURL().toString().lastIndexOf("/") + 1);
			timeLog(strRealAddress);
	
			// 获取所有响应头字段
			timeLog("所有响应头字段: \r\n");
	
			for (int n = 0;; n++) {
				String sTmp = httpURLConnection.getHeaderFieldKey(n);
				String sTmp2 = httpURLConnection.getHeaderField(n);
				if (sTmp2 == null) {
					break;
				}
				if (n == 0) {
					timeLog(sTmp2);
					localOs.write(sTmp2.getBytes());
				} else {
					String lct = "";
	
					if (sTmp.contains("Location")) {
						String addr = "";
						try {
							addr = InetAddress.getLocalHost().getHostAddress();
	
						} catch (UnknownHostException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						} // 获得本机IP
	
						lct = (sTmp + ": " + "http://" + addr + ":7788/" + sTmp2);
					} else {
						lct = (sTmp + ": " + sTmp2);
					}
					timeLog(lct);
					localOs.write(lct.getBytes());
				}
				localOs.write(13);
				localOs.write(10);
			}
			localOs.write(13);
			localOs.write(10);
			// os.write(13);
			// os.write(10);
			timeLog("\r\n响应头字段结束。 " + httpURLConnection.getHeaderField("Content-Length"));
		}
		else
		{
			//写入外部长连接 GET 。。。 HTTP 1.1...
//			httpURLConnection.get
//			httpURLConnection.getOutputStream().write(("GET " + url + " HTTP/1.1").getBytes());
//			 out = new PrintWriter(httpURLConnection.getOutputStream());
//	            // 发送请求参数
//	            out.print(params);  //写入缓存
//	      
//	            out.flush();//将缓存中的刷入流中 ，必须写
		}
		
		
		try {
			inputStream = httpURLConnection.getInputStream();
			inputStreamReader = new InputStreamReader(inputStream);
			reader = new BufferedReader(inputStreamReader);
			
			int iLen = Integer.parseInt(httpURLConnection.getHeaderField("Content-Length"));
			byte[] buffer = new byte[iLen];
			int len = 0;
			while ((len = inputStream.read(buffer, len, iLen)) != -1) {
			}

			localOs.write(buffer);
			//resultBuffer.append(new String(buffer));
			HttpRequestor.timeLog("\r\n\r\n已经写入本地 httpBody：" + new String(buffer) + "[[["+ buffer.length);
			/**
			 * 接受HTTP请求
			 */
//			BufferedReader bd=new BufferedReader(new InputStreamReader(localIs));
//			String requestHeader;
//			while((requestHeader=bd.readLine())!=null&&!requestHeader.isEmpty()){
//			    System.out.println(requestHeader);
//			}
			
//			while(localIs.read(buffer) >= 0)
//				{
//				
//				};
//				HttpRequestor.timeLog("\r" + new String(buffer));
//			listFileStr.clear();
//			String[] tmps = (new String(buffer)).split("\r\n");
//			for(int i=0; i<tmps.length; i++)
//			{
//				if(tmps[i].contains(".ts"))
//				{
//					listFileStr.add(tmps[i]);
//				}
//			}


		} finally {

//			if (reader != null) {
//				reader.close();
//			}
//
//			if (inputStreamReader != null) {
//				inputStreamReader.close();
//			}
//
//			if (inputStream != null) {
//				inputStream.close();
//			}

		}

		return resultBuffer.toString();
	}

	/**
	 * Do POST request
	 * 
	 * @param url
	 * @param parameterMap
	 * @return
	 * @throws Exception
	 */
	public String doPost(String url, Map parameterMap) throws Exception {

		/* Translate parameter map to parameter date string */
		StringBuffer parameterBuffer = new StringBuffer();
		if (parameterMap != null) {
			Iterator iterator = parameterMap.keySet().iterator();
			String key = null;
			String value = null;
			while (iterator.hasNext()) {
				key = (String) iterator.next();
				if (parameterMap.get(key) != null) {
					value = (String) parameterMap.get(key);
				} else {
					value = "";
				}

				parameterBuffer.append(key).append("=").append(value);
				if (iterator.hasNext()) {
					parameterBuffer.append("&");
				}
			}
		}

		timeLog("POST parameter : " + parameterBuffer.toString());

		URL localURL = new URL(url);

		URLConnection connection = this.openConnection(localURL);
		HttpURLConnection httpURLConnection = (HttpURLConnection) connection;

		httpURLConnection.setDoOutput(true);
		httpURLConnection.setRequestMethod("POST");
		httpURLConnection.setRequestProperty("Accept-Charset", charset);
		httpURLConnection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
		httpURLConnection.setRequestProperty("Content-Length", String.valueOf(parameterBuffer.length()));

		OutputStream outputStream = null;
		OutputStreamWriter outputStreamWriter = null;
		InputStream inputStream = null;
		InputStreamReader inputStreamReader = null;
		BufferedReader reader = null;
		StringBuffer resultBuffer = new StringBuffer();
		String tempLine = null;

		try {
			outputStream = httpURLConnection.getOutputStream();
			outputStreamWriter = new OutputStreamWriter(outputStream);

			outputStreamWriter.write(parameterBuffer.toString());
			outputStreamWriter.flush();
			// 响应失败
			if (httpURLConnection.getResponseCode() >= 300) {
				throw new Exception(
						"HTTP Request is not success, Response code is " + httpURLConnection.getResponseCode());
			}
			// 接收响应流
			inputStream = httpURLConnection.getInputStream();
			inputStreamReader = new InputStreamReader(inputStream);
			reader = new BufferedReader(inputStreamReader);

			while ((tempLine = reader.readLine()) != null) {
				resultBuffer.append(tempLine);
			}

		} finally {

			if (outputStreamWriter != null) {
				outputStreamWriter.close();
			}

			if (outputStream != null) {
				outputStream.close();
			}

			if (reader != null) {
				reader.close();
			}

			if (inputStreamReader != null) {
				inputStreamReader.close();
			}

			if (inputStream != null) {
				inputStream.close();
			}

		}

		return resultBuffer.toString();
	}

	private static URLConnection openConnection(URL localURL) throws IOException {
		URLConnection connection;
		if (proxyHost != null && proxyPort != null) {
			Proxy proxy = new Proxy(Proxy.Type.HTTP, new InetSocketAddress(proxyHost, proxyPort));
			connection = localURL.openConnection(proxy);
		} else {
			connection = localURL.openConnection();
		}
		return connection;
	}

	/**
	 * Render request according setting
	 * 
	 * @param request
	 */
	private void renderRequest(URLConnection connection) {

		if (connectTimeout != null) {
			connection.setConnectTimeout(connectTimeout);
		}

		if (socketTimeout != null) {
			connection.setReadTimeout(socketTimeout);
		}

	}

	/*
	 * Getter & Setter
	 */
	public Integer getConnectTimeout() {
		return connectTimeout;
	}

	public void setConnectTimeout(Integer connectTimeout) {
		this.connectTimeout = connectTimeout;
	}

	public Integer getSocketTimeout() {
		return socketTimeout;
	}

	public void setSocketTimeout(Integer socketTimeout) {
		this.socketTimeout = socketTimeout;
	}

	public String getProxyHost() {
		return proxyHost;
	}

	public void setProxyHost(String proxyHost) {
		this.proxyHost = proxyHost;
	}

	public Integer getProxyPort() {
		return proxyPort;
	}

	public void setProxyPort(Integer proxyPort) {
		this.proxyPort = proxyPort;
	}

	public String getCharset() {
		return charset;
	}

	public void setCharset(String charset) {
		this.charset = charset;
	}

}