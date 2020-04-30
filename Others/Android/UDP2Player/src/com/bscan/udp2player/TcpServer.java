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
    public TcpServer(Socket clientSocket) {
        super();
        this.clientSocket = clientSocket;
        
        //建立TCP连接服务,绑定端口
        ServerSocket tcpServer;
		try {
			//tcpServer = new ServerSocket(9099);
			
			tcpServer=new ServerSocket();
			tcpServer.setReuseAddress(true); //设置 ServerSocket 的选项
			tcpServer.bind(new InetSocketAddress(9999)); //与 8080 端口绑定

	        //接受连接,传图片给连接的客户端,每个TCP连接都是一个java线程
	        while(true){
	            clientSocket = tcpServer.accept();
	            new TcpServer(clientSocket).start();
	        }
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }

    @Override
      public void run() {
        try {
            //获得客户端的ip地址和主机名
            String clientAddress = clientSocket.getInetAddress().getHostAddress();
            String clientHostName = clientSocket.getInetAddress().getHostName();
            System.out.println(clientHostName + "(" + clientAddress + ")" + " 连接成功!");
            System.out.println("Now, 传输数据...........");
            long startTime = System.currentTimeMillis();
            //获取客户端的OutputStream
            OutputStream out = clientSocket.getOutputStream();
            InputStream inn = clientSocket.getInputStream();
            //传出http数据
            //out.write(data, 0, length);

            //FileInputStream in = new FileInputStream(new File("/home/gavinzhou/test.jpg"));
            byte[] data = new byte[4096];
            int length = 0;
            while((length = inn.read(data)) != -1){
    			Log.i("TAG", new String(data).substring(0, length));
    			
    			if(length<4096) {
                //写出数据
    				//String sResp="HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent Type: video/mp2t\r\n\r\n";
    				String sResp="HTTP/1.1 200 OK\r\nServer: NWS_VCLOUD_BIGSTORAGE\r\nConnection: close\r\nDate: Thu, 30 Apr 2020 16:23:30 GMT\r\nCache-Control: max-age=600\r\nExpires: Thu, 30 Apr 2020 16:33:30 GMT\r\nLast-Modified: Thu, 03 Oct 2019 01:44:15 GMT\r\nContent-Type: video/mp2t\r\nContent-Length: 638824\r\nX-NWS-LOG-UUID: 04e9a9df-57df-4e96-aea9-9c93bd712ad5 11cc68d8ac416839db7813d322bd9066\r\nX-Cache-Lookup: Hit From Disktank3\r\nX-Cache-Lookup: Hit From Upstream\r\nX-Daa-Tunnel: hop_count=1\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
                    out.write(sResp.getBytes(), 0, sResp.length());

                  for (Map.Entry<String ,byte[]> entry : StaticBufs.vFileMap.entrySet()) {
          			//Log.i("TAG", "Key = " + entry.getKey() + ", Value = " + entry.getValue());
          			//conn.setRequestProperty(entry.getKey(), entry.getValue());
                    out.write(entry.getValue(), 0, entry.getValue().length);
                    out.flush();
        			Log.i("TAGo", "write len="+entry.getValue().length);
          		}

    			}

            }
            long endTime = System.currentTimeMillis();
            //提示信息
            System.out.println(clientHostName + "(" + clientAddress + ")" + " 图片传输成功," + "用时:" + ((endTime-startTime)) + "ms");
            //关闭资源
            out.close();
            inn.close();
            clientSocket.close();
            System.out.println("连接关闭!");
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
}
