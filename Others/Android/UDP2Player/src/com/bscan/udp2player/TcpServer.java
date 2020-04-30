package com.bscan.udp2player;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

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
			tcpServer.bind(new InetSocketAddress(59099)); //�� 8080 �˿ڰ�

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
    			
    			if()

                //out.write(data, 0, length);
                //д������
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
