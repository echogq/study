package com.bscan.udp2player;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Map;
import android.app.Activity;
import android.app.ActivityManager;
import android.util.Log;
public class TcpServer extends Thread{
	Socket clientSocket;
	static ServerSocket tcpServer = null;
	public TcpServer(Socket clientSocket) {
		super();
		this.clientSocket = clientSocket;

		//建立TCP连接服务,绑定端口
		try {
			//tcpServer = new ServerSocket(9099);
			//接受连接,传图片给连接的客户端,每个TCP连接都是一个java线程
			while(true){
				if(tcpServer == null)
				{
					tcpServer=new ServerSocket();
					tcpServer.setReuseAddress(true); //设置 ServerSocket 的选项
					tcpServer.bind(new InetSocketAddress(9999)); //与 端口绑定
				}
				clientSocket = tcpServer.accept();
				new Thread() {
					Socket clientSocket0;
					public void start0(Socket clientSocket) {
						clientSocket0 = clientSocket;
						this.start();
					}
					@Override
					public void run() {
						webDataIO(clientSocket0);
					}
				}.start0(clientSocket);

				//new TcpServer(clientSocket).start();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	//    @Override
	//      public void run() {
	//    	while(true)
	//    		webDataIO(clientSocket);
	//    }

	public String memUsing() {
		ActivityManager activityManager = (ActivityManager) MainActivity.getContext().getSystemService("activity");
		//最大分配内存
		int memory = activityManager.getMemoryClass();
		//System.out.println("memory: "+memory);
		//最大分配内存获取方法2
		float maxMemory = (float) (Runtime.getRuntime().maxMemory() * 1.0/ (1024 * 1024));
		//当前分配的总内存
		float totalMemory = (float) (Runtime.getRuntime().totalMemory() * 1.0/ (1024 * 1024));
		//剩余内存
		float freeMemory = (float) (Runtime.getRuntime().freeMemory() * 1.0/ (1024 * 1024));
		return " mem: "+maxMemory
				+" / "+totalMemory
				+" / "+freeMemory;
	}


	public void webDataIO(Socket clientSocket) {
		Thread.currentThread().setName("===webDataIO.MX"); 
		try {
			//获得客户端的ip地址和主机名
			String clientAddress = clientSocket.getInetAddress().getHostAddress();
			String clientHostName = clientSocket.getInetAddress().getHostName();
			//            System.out.println(clientHostName + "(" + clientAddress + ")" + " 连接成功!");
			UDP_Push.pushLog("Now, 传输数据...........");
			long startTime = System.currentTimeMillis();
			//获取客户端的OutputStream
			OutputStream out = clientSocket.getOutputStream();
			InputStream inn = clientSocket.getInputStream();

			while(MainActivity.bytesM3u8 == null)
				try {
					Thread.sleep(100);
				} catch (InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			//传出http数据
			//out.write(data, 0, length);

			//FileInputStream in = new FileInputStream(new File("/home/gavinzhou/test.jpg"));
			byte[] data = new byte[4096];

//			while(!clientSocket.isClosed())
			{
				int length = 0;
				while((length = inn.read(data)) != -1){
					String sRequest = new String(data).substring(0, length);
					//Log.i("TAGo", sRequest);

					if(length<4096) {
						if(((sRequest.indexOf(".ts") >=0) 
								|| (sRequest.indexOf(".mp4") >=0)) && (sRequest.indexOf(".m3u8") <0))
						{
							String sHttpGetPath = "";
							for (String string : sRequest.split(" ")) {
								if(((string.indexOf(".ts") >=0) 
										|| (string.indexOf(".mp4") >=0)) && (sRequest.indexOf(".m3u8") <0))
								{
									sHttpGetPath = string;
									break;
								}
							}

							UDP_Push.pushLog("MX Player GET: "+sHttpGetPath);
							//Log.i("TAGmx", "MX Player GET: "+sHttpGetPath);

							//if(StaticBufs.conKey(sHttpGetPath) == false)
							if(StaticBufs.mapGet(sHttpGetPath) == null)
								StaticBufs.sNeedDownTS[0] = sHttpGetPath;
							StaticBufs.sMXPlayingFile[0] = sHttpGetPath;

							UDP_Push.pushLog("IsBufed? "+sHttpGetPath /*+ " Played:" +  StaticBufs.lstNames.size()*/ + " buffed:" +  StaticBufs.vFileMap.size() + memUsing());
							while(StaticBufs.buffedKey(sHttpGetPath) == false)
								try {
									Thread.sleep(100);
								} catch (InterruptedException e) {
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							UDP_Push.pushLog("Found "+sHttpGetPath /*+ " Played:" +  StaticBufs.lstNames.size()*/ + " buffed:" +  StaticBufs.vFileMap.size() + memUsing());

							{
								//写出数据
								//String sResp="HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent Type: video/mp2t\r\n\r\n";
								String sResp="HTTP/1.1 200 OK\r\nServer: NWS_VCLOUD_BIGSTORAGE\r\n"/*+"Connection: close\r\n"*/+"Date: Thu, 30 Apr 2020 16:23:30 GMT\r\nCache-Control: max-age=600\r\nExpires: Thu, 30 Apr 2020 16:33:30 GMT\r\nLast-Modified: Thu, 03 Oct 2019 01:44:15 GMT\r\nContent-Type: video/mp2t\r\n"
										+"Content-Length: " + StaticBufs.vFileMap.get(sHttpGetPath).length
										+"\r\nX-NWS-LOG-UUID: 04e9a9df-57df-4e96-aea9-9c93bd712ad5 11cc68d8ac416839db7813d322bd9066\r\nX-Cache-Lookup: Hit From Disktank3\r\nX-Cache-Lookup: Hit From Upstream\r\nX-Daa-Tunnel: hop_count=1\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
								out.write(sResp.getBytes(), 0, sResp.length());

								out.write(StaticBufs.mapGet(sHttpGetPath), 0, StaticBufs.mapGet(sHttpGetPath).length);
								out.flush();

								UDP_Push.pushLog("MX Player got: "+sHttpGetPath + " Len=" + StaticBufs.mapGet(sHttpGetPath).length);
								//        			    	Log.i("TAGmx", "MX Player got: "+sHttpGetPath + " Len=" + StaticBufs.mapGet(sHttpGetPath).length);


							}
							//    					for (Map.Entry<String ,byte[]> entry : StaticBufs.vFileMap.entrySet()) {
							//    						//Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
							//    						//conn.setRequestProperty(entry.getKey(), entry.getValue());
							//    						out.write(entry.getValue(), 0, entry.getValue().length);
							//    						out.flush();
							//    						Log.i("TAGo", "write len="+entry.getValue().length);
							//    					}
						}
						else 
						{
							String srt2=new String(MainActivity.bytesM3u8);
							while(MainActivity.bytesM3u8 == null)
								try {
									Thread.sleep(100);
								} catch (InterruptedException e) {
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							String sResp="HTTP/1.1 200 OK\r\n"/*+"Connection: close\r\n"*/+"Content-Type: application/x-mpeg\r\n"
									+"Content-Length: " + MainActivity.bytesM3u8.length
									+"\r\n\r\n";
							out.write(sResp.getBytes(), 0, sResp.length());
							out.write(MainActivity.bytesM3u8, 0, MainActivity.bytesM3u8.length);
							out.flush();

						}
					}
				}

				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			long endTime = System.currentTimeMillis();
			//提示信息
			UDP_Push.pushLog(clientHostName + "(" + clientAddress + ")" + " 传输数据成功," + "用时:" + ((endTime-startTime)) + "ms");
			//关闭资源
			//            out.close();
			//            inn.close();
			//            clientSocket.close();
			//System.out.println("连接关闭!");
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}
}
