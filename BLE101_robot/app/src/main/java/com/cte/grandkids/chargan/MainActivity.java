/*
Copyright (c) 2016, Cypress Semiconductor Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


For more information on Cypress BLE products visit:
http://www.cypress.com/products/bluetooth-low-energy-ble
 */

package com.cte.grandkids.chargan;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.ToggleButton;

/**
 * This Activity provides the user interface to control the robot.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class MainActivity extends Activity implements SensorEventListener {

    // Objects to access the layout items for Tach, Buttons, and Seek bars
    private static TextView tachLeftView;
    private static TextView tachRightView;
    private static SeekBar speedSeekBar;
    private static SeekBar directionSeekBar;
    private static ToggleButton enableButton;
    private static Button testButton;

    private static boolean on;
    private static boolean toggle=false;

    // This tag is used for debug messages
    private static final String TAG = MainActivity.class.getSimpleName();

    private static String mDeviceAddress;
    private static PSoCBleRobotService mPSoCBleRobotService;

    private SensorManager mSensorManager;
    private Sensor mAccelerometer;

    /**
     * This manages the lifecycle of the BLE service.
     * When the service starts we get the service object, initialize the service, and connect.
     */
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            Log.i(TAG, "onServiceConnected");
            mPSoCBleRobotService = ((PSoCBleRobotService.LocalBinder) service).getService();
            if (!mPSoCBleRobotService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the car database upon successful start-up initialization.
            mPSoCBleRobotService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mPSoCBleRobotService = null;
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);


        // Assign the various layout objects to the appropriate variables

        final Intent intent = getIntent();
        mDeviceAddress = intent.getStringExtra(ScanActivity.EXTRAS_BLE_ADDRESS);

        // Bind to the BLE service
        Log.i(TAG, "Binding Service");
        Intent RobotServiceIntent = new Intent(this, PSoCBleRobotService.class);
        bindService(RobotServiceIntent, mServiceConnection, BIND_AUTO_CREATE);





        testButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (toggle) {
                    toggle=false;
                    Log.i(TAG,"Send off");
                    setSpeed(0,0);
                }
                else {
                    toggle=true;
                    Log.i(TAG,"Send on");
                    setSpeed(50, 50);
                }
            }
        });


        /* This will be called when the  speed seekbar is moved */
        speedSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
            }

            public void onProgressChanged(SeekBar seekBar, int speed, boolean fromUser) {
                /* Scale the speed from what the seek bar provides to what the PSoC FW expects */
                speed = scaleSpeed(speed);
              //  mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.LEFT, speed);
              //  mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.RIGHT, speed);
                Log.d(TAG, "Left Speed Change to:" + speed);
            }
        });

        /* This will be called when the right speed seekbar is moved */
        directionSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
            }

            public void onProgressChanged(SeekBar seekBar, int speed, boolean fromUser) {
                /* Scale the speed from what the seek bar provides to what the PSoC FW expects */

                //mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.RIGHT, speed);
                Log.d(TAG, "Direction to " + speed);
            }
        });
    } /* End of onCreate method */

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mRobotUpdateReceiver, makeRobotUpdateIntentFilter());
        if (mPSoCBleRobotService != null) {
            final boolean result = mPSoCBleRobotService.connect(mDeviceAddress);
            Log.i(TAG, "Connect request result=" + result);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mRobotUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mPSoCBleRobotService = null;
    }

     /**
     * Scale the speed read from the slider (0 to 20) to
     * what the car object expects (-100 to +100).
     *
     * @param speed Input speed from the slider
     * @return scaled value of the speed
     */
    private int scaleSpeed(int speed) {
        final int SCALE = 10;
        final int OFFSET = 100;

        return ((speed * SCALE) - OFFSET);
    }

    /**
     * Enable or disable the left/right motor
     *
     * @param isChecked used to enable/disable motor
     */
    private void enable(boolean isChecked) {
        Log.i(TAG,"Enable button "+isChecked);
        if (isChecked) { // Turn on the specified motor

        } else { // turn off the specified motor
              mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.LEFT, 0);
              mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.RIGHT, 0);
              speedSeekBar.setProgress(10); // Move slider to middle position
              directionSeekBar.setProgress(10); // Move slider to middle position
            }

    }

    /**
     * Handle broadcasts from the Car service object. The events are:
     * ACTION_CONNECTED: connected to the car.
     * ACTION_DISCONNECTED: disconnected from the car.
     * ACTION_DATA_AVAILABLE: received data from the car.  This can be a result of a read
     * or notify operation.
     */
    private final BroadcastReceiver mRobotUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            switch (action) {
                case PSoCBleRobotService.ACTION_CONNECTED:
                    // No need to do anything here. Service discovery is started by the service.
                    break;
                case PSoCBleRobotService.ACTION_DISCONNECTED:
                    mPSoCBleRobotService.close();
                    break;
                case PSoCBleRobotService.ACTION_DATA_AVAILABLE:
                    // This is called after a Notify completes
                    Log.i(TAG,"Nofify data available ");

                     // mTachLeftText.setText(String.format("%d", PSoCBleRobotService.getTach(PSoCBleRobotService.Motor.LEFT)));
                   // mTachRightText.setText(String.format("%d", PSoCBleRobotService.getTach(PSoCBleRobotService.Motor.RIGHT)));
                    break;
            }
        }
    };

    /**
     * This sets up the filter for broadcasts that we want to be notified of.
     * This needs to match the broadcast receiver cases.
     *
     * @return intentFilter
     */
    private static IntentFilter makeRobotUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(PSoCBleRobotService.ACTION_CONNECTED);
        intentFilter.addAction(PSoCBleRobotService.ACTION_DISCONNECTED);
        intentFilter.addAction(PSoCBleRobotService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    private void setSpeed(int left, int right){
        mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.RIGHT, right);
        mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.LEFT, left);
        mPSoCBleRobotService.sendCommand();

    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        Vector x  = new Vector (1.0,0.0,0.0);
        Vector g  = new Vector (0.0,0.0,1.0);

        synchronized (this) {
            android.hardware.Sensor sensor = event.sensor;
            {
                boolean rvalue = false;
                int j = (sensor.getType() == android.hardware.Sensor.TYPE_ACCELEROMETER) ? 1 : 0;
                if (j == 1) {

                    // since normal is landscape swap x an y
                    Vector accel = new Vector(event.values[1], event.values[0], event.values[2]);
                    double tilt = (45.0 - AccUtils.Angle(accel,g));
                    double steer = (90.0 - AccUtils.Angle(accel,x));

                    double fwdspeed = tilt ;
                    int leftspeed = (int) (fwdspeed - steer/1.5);
                    int rightspeed = (int) (fwdspeed + steer/1.5);
                  //  Log.d("CHARGAN", String.format("Tilt %4.2f Steer %4.2f  %d %d ",tilt,steer,leftspeed,rightspeed));

                    if (mPSoCBleRobotService != null) {
<<<<<<< HEAD:BLE101_robot/app/src/main/java/com/cypress/academy/ble101_robot/ControlActivity.java
                 //        mPSoCBleRobotService.setMotorSpeed(leftspeed,rightspeed);
                    }
                    else {
                        Log.e(TAG,"service is null");
=======

                        if (on) {


                        }
>>>>>>> d9ca5115f3a5e278ac87fdfe99153fa1c8010274:BLE101_robot/app/src/main/java/com/cte/grandkids/chargan/MainActivity.java
                    }

                  //      mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.RIGHT, rightspeed);
                  //      mPSoCBleRobotService.setMotorSpeed(PSoCBleRobotService.Motor.LEFT, leftspeed);
                  //  }

                    }

                }

            }

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}