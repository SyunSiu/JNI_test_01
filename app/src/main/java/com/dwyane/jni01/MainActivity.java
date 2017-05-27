package com.dwyane.jni01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;


public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private TextView msgTV;
    private EditText inputET, sendNameET;
    private Button sendBtn, getUserBtn, exitBtn, loginBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        // Example of a call to a native method
        msgTV = (TextView) findViewById(R.id.message_tv);
        msgTV.append(NativeManager.stringFromJNI());
        msgTV.append("\n\n");
        msgTV.append(NativeManager.getStrFromNative());


        inputET = (EditText) findViewById(R.id.input_et);
        sendNameET = (EditText) findViewById(R.id.duifang_user_name_et);


        sendBtn = (Button) findViewById(R.id.btn_send_btn);
        getUserBtn = (Button) findViewById(R.id.btn_get_u_btn);
        exitBtn = (Button) findViewById(R.id.btn_exit_btn);
        loginBtn = (Button) findViewById(R.id.btn_login_btn);

        sendBtn.setOnClickListener(this);
        getUserBtn.setOnClickListener(this);
        exitBtn.setOnClickListener(this);
        loginBtn.setOnClickListener(this);
    }


    private void llog(String msg) {
        Log.e("Tony", msg + "\n");
    }


    private void toast(String msg) {
        Toast.makeText(MainActivity.this, msg, Toast.LENGTH_LONG).show();
    }


    private String serverIP = "119.23.229.22";
    private String mName = "name1";


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_login_btn:
                llog("登录按钮，serverIP:" + serverIP + "， userName:" + mName);
                NativeManager.init(serverIP, mName);
                break;

            case R.id.btn_send_btn:
                String msg = inputET.getText().toString();
                String name = sendNameET.getText().toString();
                if (TextUtils.isEmpty(msg) || TextUtils.isEmpty(name)) {
                    llog("消息或用户名不能为空");
                    toast("消息或用户名不能为空");
                } else {
                    llog("发送按钮， 内容：" + msg + " to user: " + name);
                    NativeManager.sendMsg(name, msg);
                }
                break;

            case R.id.btn_exit_btn:
                llog("退出按钮");
                NativeManager.exit();
                break;

            case R.id.btn_get_u_btn:
                llog("获取用户按钮");
                NativeManager.getAllUsers();
                break;
        }
    }
}
