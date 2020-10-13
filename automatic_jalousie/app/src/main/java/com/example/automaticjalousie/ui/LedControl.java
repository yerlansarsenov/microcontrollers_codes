package com.example.automaticjalousie.ui;

import android.annotation.SuppressLint;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.automaticjalousie.R;

import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;

import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

/**
 * Created by Sarsenov Yerlan on 30.09.2020.
 */
public class LedControl extends AppCompatActivity {

    // useful links
    // todo: https://www.instructables.com/id/Android-Bluetooth-Control-LED-Part-2/
    // todo: https://programmingdigest.com/arduino-bluetooth-controlling-system-for-home-automation-using-arduino-bluetooth/#Arduino_Bluetooth_controlling_Application_Designing_and_Development

    Button btnAuto, btnMan, btnOn, btnOff, btnDisc;
    TextView statusView;
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    private boolean mIsUserInitiatedDisconnect = false;
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private ReadInput mReadThread = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_ledcontrol);
        initViews();
        new ConnectBT().execute();
        btnDisc.setOnClickListener(view -> disconnect());
        btnOn.setOnClickListener(view -> turnToCommand("21"));
        btnOff.setOnClickListener(view -> turnToCommand("22"));
        btnAuto.setOnClickListener(view -> turnAuto());
        btnMan.setOnClickListener(view -> turnManual());
        statusView.setText("NaN");
    }

    private void initViews() {
        Intent intent = getIntent();
        address = intent.getStringExtra(DevicesActivity.EXTRA_ADDRESS);
        btnAuto = findViewById(R.id.btn_auto);
        btnMan = findViewById(R.id.btn_man);
        btnOn = findViewById(R.id.btn_on);
        btnOff = findViewById(R.id.btn_off);
        btnDisc = findViewById(R.id.btn_disc);
        statusView = findViewById(R.id.status_view);
    }

    private void disconnect() {
        mIsUserInitiatedDisconnect = true;
        new DisConnectBT().execute();
    }

    private void turnAuto() {
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("10".getBytes());
                disableManuals(true);
            } catch (IOException e) {
                msg("Error");
            }
        }
    }

    private void turnToCommand(String s) {
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write(s.getBytes());
            } catch (IOException e) {
                msg("Error");
            }
        }
    }

    private void disableManuals(boolean disable) {
        btnOff.setEnabled(!disable);
        btnOn.setEnabled(!disable);
    }

    private void turnManual() {
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("20".getBytes());
                disableManuals(false);
            } catch (IOException e) {
                msg("Error");
            }
        }
    }

    @Override
    protected void onDestroy() {
        turnManual();
        super.onDestroy();
    }

    private void changeTextView(String strInput) {
        Log.e("changeTV: ", "here we go: ".concat(strInput));
        statusView.setText(strInput);
    }

    private void msg(String s) {
        Toast.makeText(this, s, Toast.LENGTH_SHORT).show();
    }

    public class ConnectBT extends AsyncTask<Void, Void, Void> {

        private boolean ConnectSuccess = true;

        @Override
        protected void onPreExecute() {
            progress = ProgressDialog.show(LedControl.this, "Connecting...", "Please wait!!!");
        }

        @Override
        protected Void doInBackground(Void... devices) {
            try {
                if (btSocket == null || !isBtConnected) {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();
                    BluetoothDevice device = myBluetooth.getRemoteDevice(address);
                    btSocket = device.createInsecureRfcommSocketToServiceRecord(myUUID);
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();
                }
            } catch (IOException e) {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            if (!ConnectSuccess) {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            } else {
                msg("Connected.");
                isBtConnected = true;
                // initialize readInput (Runnable)
                mReadThread = new ReadInput();
            }
            progress.dismiss();
        }
    }

    private class DisConnectBT extends AsyncTask<Void, Void, Void> {

        @Override
        protected void onPreExecute() {
            progress = ProgressDialog.show(LedControl.this, "Disconnecting...", "Please wait!!!");
        }

        @Override
        protected Void doInBackground(Void... params) {
            if (mReadThread != null) {
                mReadThread.stop();
                while (mReadThread.isRunning()); // Wait until it stops
                mReadThread = null;
            }
            try {
                btSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            isBtConnected = false;
            if (mIsUserInitiatedDisconnect) {
                finish();
            }
            progress.dismiss();
        }

    }

    private class ReadInput implements Runnable {

        private boolean bStop = false;
        private Thread t;

        public ReadInput() {
            t = new Thread(this, "Input Thread");
            t.start();
        }

        public boolean isRunning() {
            return t.isAlive();
        }

        @Override
        public void run() {
            InputStream inputStream;
            try {
                inputStream = btSocket.getInputStream();
                while (!bStop) {
                    byte[] buffer = new byte[256];
                    if (inputStream.available() > 0) {
                        /*inputStream.read(buffer);
                        int i = 0;
                        /*
                         * This is needed because new String(buffer) is taking the entire buffer i.e. 256 chars on Android 2.3.4 http://stackoverflow.com/a/8843462/1287554
                         *
                        while(i < buffer.length && buffer[i] != 0) i++;
//                        for (; i < buffer.length && buffer[i] != 0; i++) {
//                        }
                        final String strInput = new String(buffer, 0, i);*/
                        int bytes = inputStream.read(buffer);
                        String strInput = new String(buffer, 0, bytes);
                        Log.e("Runnable: ", "input is ".concat(strInput));
                        Handler mainHandler = new Handler(LedControl.this.getMainLooper());
                        mainHandler.post(() -> changeTextView(strInput));
                        /*
                         * If checked then receive text, better design would probably be to stop thread if unchecked and free resources, but this is a quick fix
                         */
                    }
                    Thread.sleep(500);
                }
            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        public void stop() {
            bStop = true;
        }
    }



}
