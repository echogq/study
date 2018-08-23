package httpTest;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.Proxy;
import java.net.Socket;
import java.net.SocketException;
import java.net.URLConnection;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainGo {
	//static HTTPServerThread svr = new HTTPServerThread();
	static String sIP = "";
public static void main(String[] args) {
      /* Post Request */
//    Map dataMap = new HashMap();
//    dataMap.put("username", "Nick Huang");
//    dataMap.put("blog", "IT");
//    List<String> list = new LinkedList<String>();
//    list.add("fsfs");
//    list.add("abc");
//    list.add("ffdes");
//    System.out.println(list.contains("abc"));
//	DownUtil aaa = null;
//	try {
//		aaa = new DownUtil("http://adminah.xbsafe.cn/download/ct10000helper_ah.exe", "./testDown.exe", 10);
//		aaa.download();
//	} catch (Exception e) {
//		// TODO Auto-generated catch block
//		e.printStackTrace();
//	}
//	System.out.println(aaa.getCompleteRate());
//	if((15/2) != (16/2))
//	{
//		System.out.println("IP:");  
//	}
	try {  
        for (Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces(); interfaces.hasMoreElements();) {  
            NetworkInterface networkInterface = interfaces.nextElement();  
            if (networkInterface.isLoopback() || networkInterface.isVirtual() || !networkInterface.isUp()) {  
                continue;  
            }  
            List<InterfaceAddress> addresses = networkInterface.getInterfaceAddresses();  
            for (InterfaceAddress interfaceAddress : addresses) {  
                String ip = interfaceAddress.getAddress().getHostAddress();  
                if (ip.length() < 20) {  
                    System.out.println("IP:" + ip);  
                    System.out.println("networkInterface.getName(): " + networkInterface.getName());  
                    System.out.println("networkInterface.getDisplayName(): " + networkInterface.getDisplayName());  
                    if(networkInterface.getName().contains("eth") && (sIP.length() == 0))
                    {
                    	sIP = ip;
                    	break;
                    }
                }  
            }  
        }  
    } catch (SocketException e) {  
        e.printStackTrace();  
    }  
//	Enumeration<NetworkInterface> nif;
//	try {
//		nif = NetworkInterface.getNetworkInterfaces();//.getByName("电信光纤10M");
//		Enumeration<InetAddress> nifAddresses = nif.;
//		Proxy proxy = new Proxy(Proxy.Type.HTTP, new InetSocketAddress(
//		        "proxy", 5555));
//		URLConnection urlConnection = null;
////	url.openConnection(proxy);
//	} catch (SocketException e) {
//		// TODO Auto-generated catch block
//		e.printStackTrace();
//	}
//	Socket soc = new java.net.Socket();
//	soc.bind   (new InetSocketAddress(nifAddresses.nextElement(), 0));
//	soc.connect(new InetSocketAddress(address, port));
	
	// 注释指定系统属性值
	System.setProperty("java.net.preferIPv4Stack", "true");
//	System.setProperty("java.net.preferIPv6Addresses", "true");
    System.out.println("sIP:============== " + sIP);  
	
	String tmp = "http://221.179.217.75:80/wh7f454c46tw480315831_149727393/PLTV/88888888/224/3221225684/index.m3u8?icpid=88888888&RTS=1502729738&from=0&ocs=2_221.179.217.70_80&hms_devid=103,63";
//	Pattern p = Pattern.compile("\\d+(,\\d{3})*");
//	Matcher m = p.matcher(tmp);
//	String xxx = null;
//	while (m.find()){
//		xxx = (m.group());
//	}
	System.out.println(tmp.substring(0, tmp.lastIndexOf("/") + 1));
	
//	Map map = new HashMap();
//    String key1 = "java1";
//    String key2 = "java2";
//    map.put(key1, 10);
//    map.put(key2, 22);
//    System.out.println(map.get(key1));
//    System.out.println(map.get(key2));	
//	for (int i = 0; i < 100; i++)
//	{
//		final String tmp2 = "*** " + i;
////		tmp2 = "==";
//		System.out.println(tmp2);
//	}
	
//	System.out.println(tmp.replace(xxx, "" + (Integer.parseInt(xxx) + 1)));
    Thread myThread2 = new HTTPServerThread();     // 创建一个新的线程 myThread2 此线程进入新建状态
    myThread2.start(); 
    
    new HTTPSearchThread().start();
    new HTTPSearchSiteThread().start();
    new HTTPSearchVideoUrlThread().start();
//    try {
//        //System.out.println(new HttpRequestor().doPost("http://localhost:8081/TestProgect2/LoginServlet", dataMap));
//    	while(true)
//    	{
//    		System.out.println(new HttpRequestor().doGet(listStr, "http://111.13.42.8/PLTV/88888888/224/3221225681/index.m3u8"));
//    		Thread.sleep(1000);
//    	}
//    } catch (Exception e) {
//        // TODO Auto-generated catch block
//        e.printStackTrace();
//    }
    
    /* Get Request */
  
}
}