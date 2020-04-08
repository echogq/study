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
 * UDP工具类
 * 
 * @author Sunward
 * 
 */
public class UDPUtils implements Runnable {

	public boolean keepRunning = true;// 线程开始标志
	public static final String TAG = "XXOO";
	// 发送目的主机IP和端口
	private static String SERVER_IP;
	private static int SERVER_PORT;

	// 本机监听的端口
	private static int LOCAL_PORT = 17077;

	// 发送的消息
	private String message = "test";

	// 服务器接收的消息
	private String receive;

	// Handler传递的数据
	private Message msg;
	// Message传递的Buddle参数
	private Bundle bundle;

	// wifi名和密码
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
	 * 线程停止标志
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
	                         && !ip.isLoopbackAddress() //loopback地址即本机地址，IPv4的loopback范围是127.0.0.0 ~ 127.255.255.255
	                          && ip.getHostAddress().indexOf(":")==-1){
	                     System.out.println("本机的IP = " + ip.getHostAddress());
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
	 * 服务端监听程序
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
				// 等待客户机连接
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
//				// 把数据放到buddle中
//				bundle.putString("receive", receive);
//				// 把buddle传递到message
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
	// 利用Handler将接收的数据实时打印出来
//	Handler myHandler = new Handler() {
//		@Override
//		public void handleMessage(Message msg) {
//			super.handleMessage(msg);
//			Bundle bundle = new Bundle();
//			// 从传过来的message数据中取出传过来的绑定数据的bundle对象
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

			InetAddress ip = InetAddress.getByName(SERVER_IP); // 即目的IP
			DatagramPacket sendPacket = new DatagramPacket(configInfo,
					configInfo.length, ip, SERVER_PORT);// 创建发送类型的数据报：

			Log.i(TAG, MainActivity.getLocalHostIp() + " ->sendUDPPing: " + message);
			sendSocket.send(sendPacket); // 通过套接字发送数据：
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

				//创建tcp连接creatConnect("服务端IP地址","服务端端口号");
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
				MainActivity.updateData2MainUI("UDP广播17077查找中...");

				sendUDPPing("Java...UDP");
			}
			else
			{
				MainActivity.updateData2MainUI("Connected");
				

//				Log.d("XXOO", "TCPpp......连接断没？？.." +  TcpClientConnector.isServerClose());
//				Log.d("XXOO", "TCPpp......连接断没  ？？.." +  TcpClientConnector.mClient.isClosed());
//				Log.d("XXOO", "TCPpp......连接断没   ？？.." +  TcpClientConnector.mClient.isConnected());
				
//				if(TcpClientConnector.isServerClose())
//					Log.d("XXOO", "TCPpp......对方已经断开..。。。。。。" );

//				try {
//					TcpClientConnector.rcvData();
//				} catch (Exception e) {
//					// TODO Auto-generated catch block
//					Log.d("XXOO", "TCP....对方已经断开..。。。。。。" );
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
