package com.bscan.XTraderGFZQ;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import org.apache.http.conn.util.InetAddressUtils;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.text.Html;
import android.util.Log;
import android.widget.TextView;

/**
 * UDP������
 * 
 * @author Sunward
 * 
 */
public class UDPUtils implements Runnable {

	public boolean keepRunning = true;// �߳̿�ʼ��־
	public static final String TAG = "XXOO";
	// ����Ŀ������IP�Ͷ˿�
	private static String SERVER_IP;
	private static int SERVER_PORT;

	// ���������Ķ˿�
	private static int LOCAL_PORT = 17077;

	// ���͵���Ϣ
	private String message = "test";

	// ���������յ���Ϣ
	private String receive;

	// Handler���ݵ�����
	private Message msg;
	// Message���ݵ�Buddle����
	private Bundle bundle;

	// wifi��������
	private String SSID, password;

	public UDPUtils(int Local_PORT) {
		LOCAL_PORT = Local_PORT;
	}

	public UDPUtils(String Server_IP, int Server_Port) {
		SERVER_IP = Server_IP;
		SERVER_PORT = Server_Port;
	}

	public void setMessage(String message) {
		this.message = message;
	}

	public String getMessage() {
		return message;
	}

	/**
	 * �߳�ֹͣ��־
	 * 
	 * @param keepRunning
	 */
	public void setKeepRunning(boolean keepRunning) {
		this.keepRunning = keepRunning;
	}

	public boolean getKeepRunning() {
		return this.keepRunning;
	}

	private static String getHostIp(){
	       try{
	           Enumeration<NetworkInterface> allNetInterfaces = NetworkInterface.getNetworkInterfaces();
	          while (allNetInterfaces.hasMoreElements()){
	                NetworkInterface netInterface = (NetworkInterface) allNetInterfaces.nextElement();
	               Enumeration<InetAddress> addresses = netInterface.getInetAddresses();
	              while (addresses.hasMoreElements()){
	                   InetAddress ip = (InetAddress) addresses.nextElement();
	                  if (ip != null 
	                         //&& ip instanceof Inet4Address
	                         && !ip.isLoopbackAddress() //loopback��ַ��������ַ��IPv4��loopback��Χ��127.0.0.0 ~ 127.255.255.255
	                          && ip.getHostAddress().indexOf(":")==-1){
	                     System.out.println("������IP = " + ip.getHostAddress());
	                      return ip.getHostAddress();
	                   } 
	              }
	           }
	       }catch(Exception e){
	          e.printStackTrace();
	        }
	       return null;
	    }
	/**
	 * ����˼�������
	 */
	private static TcpClientConnector connector = null;
	public void StartListen() {
		keepRunning = getKeepRunning();
		DatagramSocket socket = null;
		byte[] data = new byte[1024];
		DatagramPacket packet = new DatagramPacket(data, data.length);

		try {
			socket = new DatagramSocket(LOCAL_PORT);
			socket.setBroadcast(true);
			Log.i(TAG, "socket->" +  getHostIp() );
			// socket.setSoTimeout(200);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}

		while (keepRunning) {
			try {
				// �ȴ��ͻ�������
				packet.setData(data);
				Log.e(TAG, "receive0");
				socket.setSoTimeout(200);  
				Log.e(TAG, "socket.setSoTimeout(200)");
					MainActivity.acquireWakeLock();
			socket.receive(packet);
				MainActivity.releaseWakeLock();
				
				Log.i(TAG, "rcv: " + packet.getAddress().getHostAddress());
				Log.i(TAG, "rcv: " + packet.getSocketAddress());
				
				Log.e(TAG, "receive111");
				receive = new String(packet.getData(), 0, packet.getLength());
				Log.i(TAG, "rcv: " + receive);
//				msg = new Message();
//				bundle = new Bundle();
//
//				// �����ݷŵ�buddle��
//				bundle.putString("receive", receive);
//				// ��buddle���ݵ�message
//				msg.setData(bundle);
//				myHandler.sendMessage(msg);
//
			} catch (Exception e) {
				e.printStackTrace();
				continue;
			}
		}

		if (socket != null) {
			socket.close();
			socket = null;
		}
	}

//    public boolean setSuspendOptimizations(boolean enabled) {
//        if (mSuspendOptEnabled == enabled) return true;
//        mSuspendOptEnabled = enabled;
//        if (enabled) {
//            return doBooleanCommand("DRIVER SETSUSPENDMODE 1");
//        } else {
//            return doBooleanCommand("DRIVER SETSUSPENDMODE 0");
//        }
//    }	
	// ����Handler�����յ�����ʵʱ��ӡ����
//	Handler myHandler = new Handler() {
//		@Override
//		public void handleMessage(Message msg) {
//			super.handleMessage(msg);
//			Bundle bundle = new Bundle();
//			// �Ӵ�������message������ȡ���������İ����ݵ�bundle����
//			bundle = msg.getData();
//			receive = bundle.getString("receive");
//			setMessage(receive);
//		}
//	};

	public void sendUDPPing(String message) {

		try {
			DatagramSocket sendSocket = new DatagramSocket();

			byte[] data = new byte[1024];
			DatagramPacket rcvPacket = new DatagramPacket(data, data.length);
			rcvPacket.setData(data);

			byte[] configInfo = message.getBytes();

			InetAddress ip = InetAddress.getByName(SERVER_IP); // ��Ŀ��IP
			DatagramPacket sendPacket = new DatagramPacket(configInfo,
					configInfo.length, ip, SERVER_PORT);// �����������͵����ݱ���

			Log.i(TAG, MainActivity.getLocalHostIp() + " ->sendUDPPing: " + message);
			sendSocket.send(sendPacket); // ͨ���׽��ַ������ݣ�
			sendSocket.setSoTimeout(200);  
			//Log.i(TAG, "socket.setSoTimeout(200)");
			sendSocket.receive(rcvPacket);
			
			Log.i(TAG, "rcv: " + rcvPacket.getAddress().getHostAddress());
			Log.i(TAG, "rcv: " + rcvPacket.getSocketAddress());
			
			String rcvstr = new String(rcvPacket.getData(), 0, rcvPacket.getLength());
			Log.i(TAG, "rcv: " + rcvstr + " len=" + rcvPacket.getLength());
			
			if(rcvstr.length() > 6) //1.1.1.1
			{
				if(connector == null)
				{
					connector = TcpClientConnector.getInstance();
				}

				//����tcp����creatConnect("�����IP��ַ","����˶˿ں�");
				connector.creatConnect(rcvstr,17078);
			}
			
	        //((TextView) findViewById(R.id.tvLocalIP)).setText(Html.fromHtml(MainActivity.TAG_SVR + "<font color='#00FF33'>" + rcvstr + "</font>"));

			//updateData2MainUI(TAG_SVR + rcvstr);
			MainActivity.updateData2MainUI(MainActivity.TAG_SVR + "<font color='#00BB33'>" + rcvstr + "</font>");
			
			sendSocket.close();
		} catch (Exception e) {
			e.printStackTrace();
			MainActivity.updateData2MainUI(MainActivity.TAG_SVR);
		}
	}

	@Override
	public void run() {
		//StartListen();
		
		while(true){
			if(null == TcpClientConnector.mConnectThread)
			{
				MainActivity.updateData2MainUI("UDP�㲥17077������...");

				sendUDPPing("Java...UDP");
			}
			else
			{
				MainActivity.updateData2MainUI("Connected");
				

//				Log.d("XXOO", "TCPpp......���Ӷ�û����.." +  TcpClientConnector.isServerClose());
//				Log.d("XXOO", "TCPpp......���Ӷ�û  ����.." +  TcpClientConnector.mClient.isClosed());
//				Log.d("XXOO", "TCPpp......���Ӷ�û   ����.." +  TcpClientConnector.mClient.isConnected());
				
//				if(TcpClientConnector.isServerClose())
//					Log.d("XXOO", "TCPpp......�Է��Ѿ��Ͽ�..������������" );

//				try {
//					TcpClientConnector.rcvData();
//				} catch (Exception e) {
//					// TODO Auto-generated catch block
//					Log.d("XXOO", "TCP....�Է��Ѿ��Ͽ�..������������" );
//
//					e.printStackTrace();
//				}
			}
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		}
	}
	
}
