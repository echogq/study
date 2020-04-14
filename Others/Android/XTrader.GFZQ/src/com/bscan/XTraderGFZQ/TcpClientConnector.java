package com.bscan.XTraderGFZQ;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

/**
 * Created by hadisi on 2016/6/28.
 */

public class TcpClientConnector {
    private static TcpClientConnector mTcpClientConnector;
    public static Socket mClient = null;
    private ConnectLinstener mListener;
    public static Thread mConnectThread = null;
    private String mmmSerIP;
    private int mmmSerPort;

    public interface ConnectLinstener {
        void onReceiveData(String data);
    }

    public void setOnConnectLinstener(ConnectLinstener linstener) {
        this.mListener = linstener;
    }

    public void setSvrIPPort(String mSerIP, int mSerPort) {
    	mmmSerIP = mSerIP;
    	mmmSerPort = mSerPort;
    }

    public static TcpClientConnector getInstance() {
        if (mTcpClientConnector == null)
            mTcpClientConnector = new TcpClientConnector();
        return mTcpClientConnector;
    }

//    Handler mHandler = new Handler() {
//        @Override
//        public void handleMessage(Message msg) {
//            super.handleMessage(msg);
//            switch (msg.what) {
//                case 100:
//                    if (mListener != null) {
//                        mListener.onReceiveData(msg.getData().getString("data"));
//                    }
//                    break;
//            }
//        }
//    };

    public void creatConnect(final String mSerIP, final int mSerPort) {
    	setSvrIPPort( mSerIP,  mSerPort);
    	
        if (mConnectThread == null) {
            mConnectThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        connect(mmmSerIP, mmmSerPort);
                    } catch (IOException e) {
                    	mConnectThread = null;
                    	try {
							disconnect();
						} catch (IOException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
                        e.printStackTrace();
                    }
                    
                	mConnectThread = null;
                	try {
						disconnect();
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}

                }
            });
            mConnectThread.start();
        }
    }

    /**
     * 与服务端进行连接
     *
     * @throws IOException
     */
    private void connect(String mSerIP, int mSerPort) throws IOException {
        /*if (mClient == null)*/ {
            mClient = new Socket(mmmSerIP, mmmSerPort);
        }
        mClient.setSoTimeout(7000); //对端是3s一次发送心跳："AutoTrade...TCPPing"
        InputStream inputStream = mClient.getInputStream();
        byte[] buffer = new byte[1024];
        int len = -1;
        
        String lastRcvData = "";
        while ((len = inputStream.read(buffer)) != -1) {
            String data = new String(buffer, 0, len);
            
            //if(!lastRcvData.equals(data))
            {
				Log.v("XXOO", "rcvedTCP......" + data + " 前指令=" + MainActivity.fAction);
            	lastRcvData = data;
            }

			String a[] = data.split("\\.\\.\\.");
			if(a.length == 1)
			{
				//Rcved B/S CMD. Need Action right now.
				
				Message msg = new Message();
				Bundle bundle = new Bundle();

				// 把数据放到buddle中
				bundle.putString("receive", data);
				// 把buddle传递到message
				msg.setData(bundle);
				MainActivity.myHandler.sendMessage(msg); // 向Handler发送消息,更新UI

			}
//            Message message = new Message();
//            message.what = 100;
//            Bundle bundle = new Bundle();
//            bundle.putString("data", data);
//            message.setData(bundle);
            //mHandler.sendMessage(message);
        }
        disconnect();
    }

    /**
     * 发送数据
     *
     * @param data 需要发送的内容
     */
    public void send(String data) throws IOException {
        OutputStream outputStream = mClient.getOutputStream();
        outputStream.write(data.getBytes());
    }

    /**
     * 断开连接
     *
     * @throws IOException
     */
    public static void disconnect() throws IOException {
        if (mClient != null) {
            mClient.close();
            mClient = null;
        }
    }

    /**
    * 判断是否断开连接，断开返回true,没有返回false
    * @param socket
    * @return
    */ 
//    public static Boolean isServerClose(){ 
//       try{ 
//    	   mClient.sendUrgentData(0xFF);//发送1个字节的紧急数据，默认情况下，服务器端没有开启紧急数据处理，不影响正常通信 
//    	   return false; 
//       }catch(Exception se){ 
//    	   return true; 
//       } 
//    } 

//	public static void DetectAlive()  throws IOException{
//		try{
//	        mClient.sendUrgentData(0xff);
//	        
//			}catch(Exception ex){
//				disconnect();
//				mConnectThread = null;
//				Log.d("XXOO", "TCP....DetectAlive对方已经断开..。。。。。。" );
//			}
//	}

	public static String rcvData() throws Exception {
        InputStream inStream = mClient.getInputStream();
        byte[] rebyte = readStream(inStream);  
        if(null == rebyte)
        	return "";
        String remess = new String(rebyte);  
        Log.d("XXOO", "收到主机消息：" + remess); 
        
		return remess;
	}
	/** 
	 * @功能 读取流 
	 * @param inStream 
	 * @return 字节数组 
	 * @throws Exception 
	 */  
	public static byte[] readStream(InputStream inStream) throws Exception {  
	    int count = 0;  
	    //while (count == 0) {  
	        count = inStream.available();  
	    //}  
	    if(count >0)
	    {
		    byte[] b = new byte[count];  
		    inStream.read(b);  
		    return b;  
	    }
	    else
	    {
		    byte[] b = new byte[1024];  
		    Log.d("XXOO", "...inStream.read..主机：" + inStream.read(b)); 
	    	return null;
	    }
	}  
}
