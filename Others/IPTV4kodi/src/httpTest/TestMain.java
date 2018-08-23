package httpTest;

import java.net.URL;
import java.net.URLConnection;
import java.util.List;
import java.util.Map;

public class TestMain {

    /**
     * @author lcq
     * @date 2013-4-8
     * @param args
     */
//    public static void main(String[] args) {
//        String a = "a";
//        String b = "b";
//        operator(a, b);
//        StringBuilder x = new StringBuilder("x"); 
//        StringBuilder y = new StringBuilder("y");
//        operator(x, y);
//        System.out.println(a + "," + b);
//        System.out.println(x + "," + y);
//        
//        byte[] xx=null;
////        xx[0]=0x11;
////        xx[1]=0x12;
//        xx=writeX();
//        System.out.println(xx);
//        
//    }
	public static void main(String[] args) {
		while(true)
		{
        try {
                URL obj = new URL("http://111.13.42.8/PLTV/88888888/224/3221225684/index.m3u8");
                URLConnection conn = obj.openConnection();
                conn.setDoOutput(true);
                
//				new Thread(new Runnable() {
//				@Override
//				public void run() {
//	                try {
//						afterConnect(svrSocket);
//					} catch (IOException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
//				}
//			}).start();
                Map<String, List<String>> map = conn.getHeaderFields();
      
                System.out.println(conn + "显示响应Header信息...\n");
      
                for (Map.Entry<String, List<String>> entry : map.entrySet()) {
                        System.out.println("Key : " + entry.getKey() +
                                           " ,Value : " + entry.getValue());
                }
 
//                      Key : Connection ,Value : [keep-alive]
      
                System.out.println("\n使用key获得响应Header信息 \n");
                List<String> server = map.get("Server");
      
                if (server == null) {
                        System.out.println("Key 'Server' is not found!");
                } else {
                        for (String values : server) {
                                System.out.println(values);
                        }
                }
                conn.getOutputStream().write("GET ".getBytes());;
      
        } catch (Exception e) {
                e.printStackTrace();
        }
		}
    }
    private static byte[] writeX() {
    	
    	byte[] currentPart = new byte[2];
    	currentPart[0]=(byte) 0x88;
    	currentPart[1]=(byte) 0x99;
    	return currentPart;
	}

	public static void operator(String a, String b) {
        a += b;
        b = a;
    }

    public static void operator(StringBuilder a, StringBuilder b) {
        a.append(b);
        b = a;
    }
}