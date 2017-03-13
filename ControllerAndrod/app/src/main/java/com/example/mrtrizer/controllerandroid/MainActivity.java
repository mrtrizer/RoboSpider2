package com.example.mrtrizer.controllerandroid;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "bluetooth1";

    // SPP UUID сервиса
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // MAC-адрес Bluetooth модуля
    private static String address = "20:16:09:26:72:18";

    Button buttonConnect;

    Button buttonForward;
    Button buttonStop;
    Button buttonBack;
    Button buttonLeft;
    Button buttonRight;
    Button buttonForwardLeft;
    Button buttonForwardRight;

    private static final int REQUEST_ENABLE_BT = 1;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonConnect = (Button) findViewById(R.id.button_connect);

        buttonForward = (Button) findViewById(R.id.button_forward);
        buttonStop = (Button) findViewById(R.id.button_stop);
        buttonBack = (Button) findViewById(R.id.button_back);
        buttonLeft = (Button) findViewById(R.id.button_left);
        buttonRight = (Button) findViewById(R.id.button_right);
        buttonForwardLeft = (Button) findViewById(R.id.button_forward_left);
        buttonForwardRight = (Button) findViewById(R.id.button_forward_right);

        buttonConnect.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                // Set up a pointer to the remote node using it's address.
                connect();
            }
        });

        buttonForward.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                sendData("f");
            }
        });

        buttonStop.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                sendData("s");
            }
        });

        buttonBack.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                sendData("b");
            }
        });

        buttonLeft.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                sendData("l");
            }
        });

        buttonRight.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                sendData("r");
            }
        });

        buttonForwardLeft.setOnClickListener(new OnClickListener() {
            public void onClick(View v) { sendData("1"); }
        });

        buttonForwardRight.setOnClickListener(new OnClickListener() {
            public void onClick(View v) { sendData("2"); }
        });
    }

    public void connect() {

        buttonConnect.setEnabled(false);

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        checkBTState(btAdapter);

        // Two things are needed to make a connection:
        //   A MAC address, which we got above.
        //   A Service ID or UUID.  In this case we are using the
        //     UUID for SPP.
        try {
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            buttonConnect.setEnabled(true);
        }

        // Discovery is resource intensive.  Make sure it isn't going on
        // when you attempt to connect and pass your message.
        //btAdapter.cancelDiscovery();

        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "Connecting");
        try {
            btSocket.connect();
            Log.d(TAG, "Connected");
        } catch (IOException e) {
            buttonConnect.setEnabled(true);
            try {
                btSocket.close();
            } catch (IOException e2) {
                buttonConnect.setEnabled(true);
            }
        }

        // Create a data stream so we can talk to server.
        Log.d(TAG, "Socket initialization");
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            buttonConnect.setEnabled(true);
        }
        
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.d(TAG, "Try to resume");

        connect();

    }

    @Override
    public void onPause() {
        super.onPause();

        Log.d(TAG, "Paused");

        if (outStream != null) {
            try {
                if (outStream != null)
                    outStream.flush();
            } catch (IOException e) {
                errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
            }
        }

        try {
            if (btSocket != null)
                btSocket.close();
        } catch (IOException e2) {
            errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
        }
    }

    private void checkBTState(BluetoothAdapter btAdapter) {
        // Check for Bluetooth support and then check to make sure it is turned on
        // Emulator doesn't support Bluetooth and will return null
        if(btAdapter==null) {
            errorExit("Fatal Error", "Bluetooth is no supported");
        } else {
            if (btAdapter.isEnabled()) {
                Log.d(TAG, "Bluetooth on.");
            } else {
                //Prompt user to turn on Bluetooth
                Intent enableBtIntent = new Intent(btAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }
    }

    private void errorExit(String title, String message){
        Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
        finish();
    }

    private void sendData(String message) {
        byte[] msgBuffer = message.getBytes();

        try {
            outStream.write(msgBuffer);
        } catch (IOException e) {
            Log.d(TAG, "Send error");
            buttonConnect.setEnabled(true);
        }
    }

}
