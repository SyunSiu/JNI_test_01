package com.dwyane.jni01;


import android.util.Log;

/**
 * 本地库统一入口
 */
public class NativeManager {


    // Used to load the 'native-lib' library on application startup.
    static {
        // TODO: 2017/5/26 对应
        System.loadLibrary("libName");
    }


    public static void sendMsg(String userName, String msg) {
        String cmd = "send " + userName + " " + msg;
        Log.e("Tony", "发送消息的命令：\n" + cmd);
        sendCmd(cmd);
    }

    public static void exit() {
        String cmd = "exit";
        Log.e("Tony", "退出的命令：\n" + cmd);
        sendCmd(cmd);
    }

    public static void getAllUsers() {
        String cmd = "getu";
        Log.e("Tony", "获取用户信息的命令：\n" + cmd);
        sendCmd(cmd);
    }


    /**
     * 初始化方法
     *
     * @param serverIP 服务器ip地址
     * @param userName 登录的用户名
     */
    public static native void init(String serverIP, String userName);


    /**
     * 发送命令的方法
     *
     * @param cmdStr
     */
    public static native void sendCmd(String cmdStr);


    /**
     * 模板方法。
     * <p>
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

    public static native String getStrFromNative();


}
