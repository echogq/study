package com.bscan.udp2player;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.webkit.MimeTypeMap;
import android.widget.Button;
import android.widget.TextView;
 
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.text.SimpleDateFormat;
import java.util.Date;

public class UDP_Push  extends Activity {
    /*���͹㲥�˵�socket*/
    private MulticastSocket ms;
    /*���͹㲥�İ�ť*/
    private Button sendUDPBrocast;
    private String sPrefix = "";
    private String sUrl = "";
    private TextView TextView00;
    private TextView TextView01;
 
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        init();

        Intent intent = getIntent();
        String message = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);
        
        Uri data = getIntent().getData();
        if(data != null)
        {
	        Log.i("TAG", "host = " + data.getHost() + " path = " + data.getPath() + " query = " + data.getQuery());
	        sUrl = data.toString();
	        
	     // Android 4.0 ֮���������߳�����������
            new Thread(new Runnable(){
                @Override
                public void run() {
        	        setTxts();
                	sendUDP();
                }
            }).start();

	        
	        //result.setText(param+"]");
	     }
    }
 
    /*��ʼ������*/
    public void init() {
        setContentView(R.layout.activity_main);
        
        TextView00 = (TextView) findViewById(R.id.result);
        TextView01 = (TextView) findViewById(R.id.TextView01);
        
        TextView00.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               // Toast.makeText(MainActivity.this, "ʵ�ֵ��TextView�¼�", Toast.LENGTH_SHORT).show();
               
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                    	
            	        sUrl = TextView00.getText().toString();

                        sendUDP();
                    }
                });
     
                thread.start();
            }
        });

        TextView01.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               // Toast.makeText(MainActivity.this, "ʵ�ֵ��TextView�¼�", Toast.LENGTH_SHORT).show();
               
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
            	        sUrl = TextView01.getText().toString();
                        sendUDP();
                    }
                });
     
                thread.start();
            }
        });

        sendUDPBrocast = (Button) findViewById(R.id.send);
        sendUDPBrocast.setOnClickListener(new SendUDPBrocastListener());
        try {
            /*����socketʵ��*/
            ms = new MulticastSocket();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    private String intToIp(int i) {
 
        return (i & 0xFF ) + "." +
                ((i >> 8 ) & 0xFF) + "." +
                ((i >> 16 ) & 0xFF) + "." +
                ( i >> 24 & 0xFF) ;
    }
 
    public void sendUDP() {
		//���͵����ݰ��������ڵ����е�ַ�������յ������ݰ�
		DatagramPacket dataPacket = null;
		/*����socketʵ��*/
		try {
		    ms = new MulticastSocket();
		} catch (IOException e) {
		    e.printStackTrace();
		}
		try {
		    ms.setTimeToLive(4);
		    //��������IP���������д��̬��ȡ��IP����ַ�ŵ����ݰ����ʵserver�˽��յ����ݰ���Ҳ�ܻ�ȡ����������IP��
		    Date date = new Date();
		    SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd  HH:mm:ss.SSS");
		    String formatedDate = format.format(date);
 
 
//                        InetAddress localhost = InetAddress.getByName("localhost");
//                        String hostAddress = localhost.getHostAddress();
//                        String sendData = hostAddress +formatedDate;
 
 
		    //��ȡwifi����
		    WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
		    //�ж�wifi�Ƿ���
		    if (!wifiManager.isWifiEnabled()) {
		        wifiManager.setWifiEnabled(true);
		    }
		    WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		    int ipAddress = wifiInfo.getIpAddress();
		    String ip = intToIp(ipAddress);
 

		    //String sendData=ip+formatedDate;
 		    String sendData = sPrefix + sUrl;
 		    if(sPrefix.length() > 0)
 		    	sendData += "?etag=12345";
        	sPrefix = "";


//                        byte[] data = "192.168.1.109".getBytes();
		    byte[] data = sendData.getBytes();
 
		    //224.0.0.1Ϊ�㲥��ַ
		    //InetAddress address = InetAddress.getByName("224.0.0.1");
		    InetAddress address = InetAddress.getByName("255.255.255.255");
		    //InetAddress address = InetAddress.getByName("192.168.1.255");
		    //����ط���������жϸõ�ַ�ǲ��ǹ㲥���͵ĵ�ַ
		    System.out.println(address.isMulticastAddress());
		    dataPacket = new DatagramPacket(data, data.length, address,
		            8003);
		    ms.send(dataPacket);
		    ms.close();
		} catch (Exception e) {
		    e.printStackTrace();
		}
	}

	public void setTxts() {
		String ArrStr[] = sUrl.split("http");
		if(ArrStr.length>0)
		{
			TextView01.setText("http" + ArrStr[ArrStr.length-1]);
		}
		TextView00.setText(sUrl);
	}

	/**
     * ������ťʱ�����;������㲥
     * */
    class SendUDPBrocastListener implements View.OnClickListener {
 
        @Override
        public void onClick(View v) {
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                	sPrefix = "https://www.playm3u8.cn/jiexi.php?url=";
                    sendUDP();
                	sPrefix = "";
                }
            });
 
            thread.start();
 
        }
    }
    

}
