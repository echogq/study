package com.bscan.udp2player;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Vector;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class StaticBufs {
    //public static final ArrayList<String> lstNames = new ArrayList();
    public static final String[] sCurM3U8Url = new String[1];
    public static final String[] sNeedDownTS = new String[1];
    public static final String[] sMXPlayingFile = new String[1];
    public static final int[] iCntThreads = new int[1];
    public static final int iBufBlockSize = 32*1024;
	//public static Vector<String> vecIngAndDone=new Vector<String>();

    public static final LinkedHashMap<String ,byte[]> vFileMap = new LinkedHashMap<>(); //map，hashmap不是线程安全的
    public static final LinkedHashMap<String ,String> vUrlMap = new LinkedHashMap<>(); //map，hashmap不是线程安全的
  //Hashtable
//    public static final Map<String, byte[]> vFileMap = new Hashtable<>();
    public static final HashMap<String ,String> header = new HashMap<>();
    public static Lock lock = new ReentrantLock();;
    public static boolean haveKey(String sKey) {
//		UDP_Push.pushLog("Buffed== "+sKey + " Played:" +  StaticBufs.lstNames.size() + " buffed:" +  StaticBufs.vFileMap.size());
    	boolean ret = false;
		lock.lock(); 
		ret = (vFileMap.get(sKey)!=null);
		lock.unlock();  
		return ret;
    }
    private StaticBufs() {
    }
    static {
    	StaticBufs.sCurM3U8Url[0] = "";
		StaticBufs.iCntThreads[0] = 0;
		StaticBufs.sNeedDownTS[0] = "";
//        header.put("Accept-Encoding", "gzip, deflate");
//        header.put("Accept-Language", "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
//        header.put("Cache-Control", "max-age=0");
        header.put("Connection", "keep-alive");
        header.put("Accept-Encoding", "identity");
//        header.put("User-Agent","Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0");
//        header.put("Accept",
//                "image/gif,image/jpeg,image/pjpeg,image/pjpeg, "
//                        + "application/x-shockwave-flash, application/xaml+xml, "
//                        + "application/vnd.ms-xpsdocument, application/x-ms-xbap"
//                        + "application/x-ms-application,application/vnd.ms-excel"
//                        + "application/vnd.ms-powerpoint, application/msword,*/*");
    }
	public static void mapPut(String sKey, byte[] pppm) {
		UDP_Push.pushLog("mapPut++ "+sKey /*+ " Played:" +  StaticBufs.lstNames.size()*/ + " buffed:" +  StaticBufs.vFileMap.size());
		lock.lock();  
		vFileMap.put(sKey, pppm);
		lock.unlock();  
		UDP_Push.pushLog("vFileMap.put("+sKey);
	}
	public static byte[] mapGet(String sKey) {
		lock.lock();  
		byte[] tmp = vFileMap.get(sKey);
		lock.unlock();  
		return tmp;
	}
	public static void mapRemove(String sKey) {
		lock.lock(); 
		vFileMap.put(sKey, null); 
//		byte[] ppp = vFileMap.remove(sKey);
//		if(ppp != null)
//			UDP_Push.pushLog("Removed-- "+sKey /*+ " Played:" +  StaticBufs.lstNames.size()*/ + " buffed:" +  StaticBufs.vFileMap.size());
		lock.unlock();  
	}
	public static void mapRemoveTailOne(){
		lock.lock();  
		Iterator<Entry<String, byte[]>> iterator = vFileMap.entrySet().iterator();
		Entry<String, byte[]> tail = null;
		while (iterator.hasNext()) {
			tail = iterator.next();
		}
		if(tail != null)
			vFileMap.remove(tail.getKey());
		lock.unlock();  
	}
	public static void mapRemoveFirstOne(String sNotThis){
		lock.lock();  
		Object obj = null;
        for (Entry<String, byte[]> entry : vFileMap.entrySet()) {
            obj = entry.getKey();
            if (obj != null) {
            	if(!obj.equals(sNotThis))
            		break;
            }
        }

		if(obj != null)
		{
			UDP_Push.pushLog("Removed---- "+obj /*+ " Played:" +  StaticBufs.lstNames.size()*/ + " buffed:" +  StaticBufs.vFileMap.size());
			
			vFileMap.remove(obj);
		}
		lock.unlock();  
	}
	public static int mapGetBufedSize() {
		lock.lock();  
		int iSize = 0;
		Object obj = null;
        for (Entry<String, byte[]> entry : vFileMap.entrySet()) {
            obj = entry.getValue();
            if (obj != null) {
            	iSize++;
            }
        }

		lock.unlock();  
		return iSize;
	}
	public static void mapKeepSize(int maxBlocks) {
		while((StaticBufs.mapGetBufedSize() >= maxBlocks)) {
			lock.lock();  
			Object obj = null;
	        for (Entry<String, byte[]> entry : vFileMap.entrySet()) {
	            obj = entry.getKey();
	            if (obj != null) {
	            	if(!obj.equals(StaticBufs.sMXPlayingFile[0]))
	            		entry.setValue(null);
	            	else
	            		break;
	            }
	        }

			lock.unlock();  
		}
		
		if((StaticBufs.mapGetBufedSize() >= maxBlocks)) {
			lock.lock();  
			Object obj = null;
	        for (Entry<String, byte[]> entry : vFileMap.entrySet()) {
	            obj = entry.getKey();
	            if (obj != null) {
	            	if(!obj.equals(StaticBufs.sMXPlayingFile[0]))
	            		entry.setValue(null);
	            }
	        }
			lock.unlock();  			
		}			
	}
}