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

import android.util.Log;
public class TcpServer extends Thread{
	Socket clientSocket;
    static ServerSocket tcpServer = null;
    public TcpServer(Socket clientSocket) {
        super();
        this.clientSocket = clientSocket;
        
        //����TCP���ӷ���,�󶨶˿�
		try {
			//tcpServer = new ServerSocket(9099);
	        //��������,��ͼƬ�����ӵĿͻ���,ÿ��TCP���Ӷ���һ��java�߳�
	        while(true){
				if(tcpServer == null)
				{
					tcpServer=new ServerSocket();
					tcpServer.setReuseAddress(true); //���� ServerSocket ��ѡ��
					tcpServer.bind(new InetSocketAddress(9999)); //�� 8080 �˿ڰ�
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

    @Override
      public void run() {
        webDataIO(clientSocket);
    }

	public void webDataIO(Socket clientSocket) {
		try {
            //��ÿͻ��˵�ip��ַ��������
            String clientAddress = clientSocket.getInetAddress().getHostAddress();
            String clientHostName = clientSocket.getInetAddress().getHostName();
            System.out.println(clientHostName + "(" + clientAddress + ")" + " ���ӳɹ�!");
            System.out.println("Now, ��������...........");
            long startTime = System.currentTimeMillis();
            //��ȡ�ͻ��˵�OutputStream
            OutputStream out = clientSocket.getOutputStream();
            InputStream inn = clientSocket.getInputStream();
            
            while(MainActivity.bytesM3u8 == null)
				try {
					Thread.sleep(100);
				} catch (InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
            //����http����
            //out.write(data, 0, length);

            //FileInputStream in = new FileInputStream(new File("/home/gavinzhou/test.jpg"));
            byte[] data = new byte[4096];
            int length = 0;
            while((length = inn.read(data)) != -1){
            	String sRequest = new String(data).substring(0, length);
    			Log.i("TAGo", sRequest);
    			
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
    					
    					UDP_Push.pushLog("MX Player ask: "+sHttpGetPath);
    			    	Log.i("TAGmx", "MX Player ask: "+sHttpGetPath);

    					//if(StaticBufs.conKey(sHttpGetPath) == false)
    					if(!StaticBufs.vecIngAndDone.contains(sHttpGetPath))
								StaticBufs.sCurPlayingPart[0] = sHttpGetPath;

    					while(StaticBufs.conKey(sHttpGetPath) == false)
							try {
								Thread.sleep(100);
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
    					UDP_Push.pushLog("Buffed "+sHttpGetPath + " Played:" +  StaticBufs.lstNames.size() + " buffed:" +  StaticBufs.vFileMap.size());
    					
    					{
    						//д������
    						//String sResp="HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent Type: video/mp2t\r\n\r\n";
    						String sResp="HTTP/1.1 200 OK\r\nServer: NWS_VCLOUD_BIGSTORAGE\r\nConnection: close\r\nDate: Thu, 30 Apr 2020 16:23:30 GMT\r\nCache-Control: max-age=600\r\nExpires: Thu, 30 Apr 2020 16:33:30 GMT\r\nLast-Modified: Thu, 03 Oct 2019 01:44:15 GMT\r\nContent-Type: video/mp2t\r\n"
    								+"Content-Length: " + StaticBufs.vFileMap.get(sHttpGetPath).length
    								+"\r\nX-NWS-LOG-UUID: 04e9a9df-57df-4e96-aea9-9c93bd712ad5 11cc68d8ac416839db7813d322bd9066\r\nX-Cache-Lookup: Hit From Disktank3\r\nX-Cache-Lookup: Hit From Upstream\r\nX-Daa-Tunnel: hop_count=1\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    						out.write(sResp.getBytes(), 0, sResp.length());

    						out.write(StaticBufs.mapGet(sHttpGetPath), 0, StaticBufs.mapGet(sHttpGetPath).length);
    						out.flush();
 
        					UDP_Push.pushLog("MX Player got: "+sHttpGetPath + " Len=" + StaticBufs.mapGet(sHttpGetPath).length);
        			    	Log.i("TAGmx", "MX Player got: "+sHttpGetPath + " Len=" + StaticBufs.mapGet(sHttpGetPath).length);

    						
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
    					String sResp="HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/x-mpeg\r\n"
    							+"Content-Length: " + MainActivity.bytesM3u8.length
    							+"\r\n\r\n";
    					out.write(sResp.getBytes(), 0, sResp.length());
    					out.write(MainActivity.bytesM3u8, 0, MainActivity.bytesM3u8.length);

    				}
    			}

            }
            long endTime = System.currentTimeMillis();
            //��ʾ��Ϣ
            System.out.println(clientHostName + "(" + clientAddress + ")" + " ͼƬ����ɹ�," + "��ʱ:" + ((endTime-startTime)) + "ms");
            //�ر���Դ
            out.close();
            inn.close();
            clientSocket.close();
            System.out.println("���ӹر�!");
        }
        catch (IOException e) {
            e.printStackTrace();
        }
	}
}
