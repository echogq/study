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
        
        //����TCP���ӷ���,�󶨶˿�
        ServerSocket tcpServer;
		try {
			//tcpServer = new ServerSocket(9099);
			
			tcpServer=new ServerSocket();
			tcpServer.setReuseAddress(true); //���� ServerSocket ��ѡ��
			tcpServer.bind(new InetSocketAddress(9999)); //�� 8080 �˿ڰ�

	        //��������,��ͼƬ�����ӵĿͻ���,ÿ��TCP���Ӷ���һ��java�߳�
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
            //��ÿͻ��˵�ip��ַ��������
            String clientAddress = clientSocket.getInetAddress().getHostAddress();
            String clientHostName = clientSocket.getInetAddress().getHostName();
            System.out.println(clientHostName + "(" + clientAddress + ")" + " ���ӳɹ�!");
            System.out.println("Now, ��������...........");
            long startTime = System.currentTimeMillis();
            //��ȡ�ͻ��˵�OutputStream
            OutputStream out = clientSocket.getOutputStream();
            InputStream inn = clientSocket.getInputStream();
            //����http����
            //out.write(data, 0, length);

            //FileInputStream in = new FileInputStream(new File("/home/gavinzhou/test.jpg"));
            byte[] data = new byte[4096];
            int length = 0;
            while((length = inn.read(data)) != -1){
    			Log.i("TAG", new String(data).substring(0, length));
    			
    			if(length<4096) {
                //д������
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
