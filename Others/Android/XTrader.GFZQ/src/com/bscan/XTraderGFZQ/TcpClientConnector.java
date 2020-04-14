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
     * �����˽�������
     *
     * @throws IOException
     */
    private void connect(String mSerIP, int mSerPort) throws IOException {
        /*if (mClient == null)*/ {
            mClient = new Socket(mmmSerIP, mmmSerPort);
        }
        mClient.setSoTimeout(7000); //�Զ���3sһ�η���������"AutoTrade...TCPPing"
        InputStream inputStream = mClient.getInputStream();
        byte[] buffer = new byte[1024];
        int len = -1;
        
        String lastRcvData = "";
        while ((len = inputStream.read(buffer)) != -1) {
            String data = new String(buffer, 0, len);
            
            //if(!lastRcvData.equals(data))
            {
				Log.v("XXOO", "rcvedTCP......" + data + " ǰָ��=" + MainActivity.fAction);
            	lastRcvData = data;
            }

			String a[] = data.split("\\.\\.\\.");
			if(a.length == 1)
			{
				//Rcved B/S CMD. Need Action right now.
				
				Message msg = new Message();
				Bundle bundle = new Bundle();

				// �����ݷŵ�buddle��
				bundle.putString("receive", data);
				// ��buddle���ݵ�message
				msg.setData(bundle);
				MainActivity.myHandler.sendMessage(msg); // ��Handler������Ϣ,����UI

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
     * ��������
     *
     * @param data ��Ҫ���͵�����
     */
    public void send(String data) throws IOException {
        OutputStream outputStream = mClient.getOutputStream();
        outputStream.write(data.getBytes());
    }

    /**
     * �Ͽ�����
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
    * �ж��Ƿ�Ͽ����ӣ��Ͽ�����true,û�з���false
    * @param socket
    * @return
    */ 
//    public static Boolean isServerClose(){ 
//       try{ 
//    	   mClient.sendUrgentData(0xFF);//����1���ֽڵĽ������ݣ�Ĭ������£���������û�п����������ݴ�����Ӱ������ͨ�� 
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
//				Log.d("XXOO", "TCP....DetectAlive�Է��Ѿ��Ͽ�..������������" );
//			}
//	}

	public static String rcvData() throws Exception {
        InputStream inStream = mClient.getInputStream();
        byte[] rebyte = readStream(inStream);  
        if(null == rebyte)
        	return "";
        String remess = new String(rebyte);  
        Log.d("XXOO", "�յ�������Ϣ��" + remess); 
        
		return remess;
	}
	/** 
	 * @���� ��ȡ�� 
	 * @param inStream 
	 * @return �ֽ����� 
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
		    Log.d("XXOO", "...inStream.read..������" + inStream.read(b)); 
	    	return null;
	    }
	}  
}
