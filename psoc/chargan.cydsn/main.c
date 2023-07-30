#include <project.h>
#include <stdlib.h>

// 16 bit quadrature decoder... the middle is 0x8000
#define QD_RESET (0x8000)
// Scale the QD to RPMs
#define QD_SCALE (5)
// The max compare value for the motor PWM (i.e. full speed)
#define MAXSPEED (100)

int processTachFlag = 0;
int tachNotify = 0;
int nameNotify = 0;

int tachLeft = 0;
int tachRight = 0;
int8 speedLeft = 0;
int8 speedRight = 0;
uint8 command[30];
uint8 cmdLength;
uint8 status[30];

uint8 sendNotifications;
    
    
 uint8 ble_state;
#define ADVERTISING (0u)
#define CONNECTED (1u)
#define DISCONNECTED (2u)
#define ERROR (3u)


int motorFlag = 0; // this flag is used to turn the motors on/off



// This ISR handles button presses on the user button on the board. This will toggle the motors on/off
CY_ISR(swisr)
{
    motorFlag = 1;
    button_ClearInterrupt();
    
}

typedef enum motor {
    LEFT,
    RIGHT
} motor;

void copyInt(int n, uint8* bytes ){
bytes[0] = (n >> 24) & 0xFF;
bytes[1] = (n >> 16) & 0xFF;
bytes[2] = (n >> 8) & 0xFF;
bytes[3] = n & 0xFF;  
    
}

// This function updates the GATT database with the current speed
void updateStatus()
{
    status[0]=0x44;
    status[1]=0x1;
    status[2] = speedLeft;
    status[3] = speedRight;
    copyInt(tachLeft,&status[4]);
    copyInt(tachRight,&status[8]);
    
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;

    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;
    
    tempHandle.attrHandle = CYBLE_CHARGAN_COMMAND_CHAR_HANDLE;
    tempHandle.value.val = status;
    tempHandle.value.len = 20;
   
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, 0);
    CyBle_GattsNotification(cyBle_connHandle, &tempHandle);
       
 
    
    
}

// Configure the speed into the correct PWM, then call the GATT database speed update function
void setSpeed(motor m, int speed)
{
    int s = abs(speed);
    int dir = (s == speed)?0:1;
    
    if(s > MAXSPEED)
        return;
    
    switch (m)
    {
        case LEFT:
            DIRLEFT_Write(dir);
            PWMLEFT_WriteCompare(s);
            speedLeft = speed;         
        break;
        case RIGHT:
            DIRRIGHT_Write(dir);
            PWMRIGHT_WriteCompare(s);
            speedRight = speed;
        break;
    }
    
}

// This ISR is connected to the periodic interrupt source of 187ms
CY_ISR(tachISR)
{
    processTachFlag = 1;
    tachPwm_ReadStatusRegister(); // clear the interrupt
    
}


void processCmd(){
    speedLeft = command[2];
    speedRight = command[3];
    setSpeed(LEFT,speedLeft);
    setSpeed(RIGHT,speedRight);
    updateStatus();      // Update the speeed in the GATT database
    
}

   




// This function is called a few times/second and calculates the actual RPMs of the motor
// by reading the quadrature encoder and calculating the RPMs
inline void processTach()
{
    processTachFlag = 0;
    tachLeft =  ( QDLEFT_ReadCounter()  - QD_RESET ) * QD_SCALE;
    tachRight = ( QDRIGHT_ReadCounter() - QD_RESET ) * QD_SCALE;
    QDLEFT_WriteCounter(QD_RESET);
    QDRIGHT_WriteCounter(QD_RESET);
    updateStatus(); // Update the GATT database with the new RPM values
   
}

// This function handles the BLE Stack
void BleCallBack(uint32 event, void* eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    int i;
    
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            tachNotify = 0;
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            IND_RED_Write(0);
        break;
            
        case CYBLE_EVT_GATT_CONNECT_IND:
            updateStatus();
            IND_BLU_Write(0);
            IND_RED_Write(1);
        break;
            
         case CYBLE_EVT_GATTS_WRITE_REQ:
  
	    case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
	        wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			
	        /* Handling Notification Enable */
			if(wrReqParam->handleValPair.attrHandle == CYBLE_CHARGAN_COMMAND_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
				CYBLE_GATT_HANDLE_VALUE_PAIR_T    NotificationCCDHandle;
				uint8 CCDValue[2];
				
	            /* Extract CCCD Notification enable flag */
	            sendNotifications = wrReqParam->handleValPair.value.val[0];
                if (sendNotifications){
                    IND_GRN_Write(0);
                    IND_BLU_Write(0);
                    IND_RED_Write(1);
                }
                else {
                    IND_GRN_Write(1);
                    IND_BLU_Write(0);
                    IND_RED_Write(0);
                }
				
				/* Write the present I2C notification status to the local variable */
				CCDValue[0] = sendNotifications;
				
				CCDValue[1] = 0x00;
				
				/* Update CCCD handle with notification status data*/
				NotificationCCDHandle.attrHandle = CYBLE_CHARGAN_COMMAND_CHAR_HANDLE;
				
				NotificationCCDHandle.value.val = CCDValue;
				
				NotificationCCDHandle.value.len = 2;
				
				/* Report data to BLE component for sending data when read by Central device */
				CyBle_GattsWriteAttributeValue(&NotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);			
	        }
				
	        /* Handling Write data from Client */
	        else if(wrReqParam->handleValPair.attrHandle == CYBLE_CHARGAN_COMMAND_CHAR_HANDLE)
	        {
					
		        /*The data received from ble is extracted */
					
                for(i=0;i<(wrReqParam->handleValPair.value.len);i++)	
		        	command[i] = wrReqParam->handleValPair.value.val[i];
			    cmdLength = wrReqParam->handleValPair.value.len;
		         processCmd();
                  
                
 	        }
	            
	        if (event == CYBLE_EVT_GATTS_WRITE_REQ)
			{
	            CyBle_GattsWriteRsp(cyBle_connHandle);
			}
	        break;

            
        default:
        break;
    }
}
    
// This function will turn the motors on/off if the user presses the button    
void handleMotor()
{
    static int motorState = 1;
    if(motorFlag)
    {
        if(motorState)
        {
            PWMLEFT_Stop();
            PWMRIGHT_Stop();
        }
        else
        {
            PWMLEFT_Start();
            PWMRIGHT_Start(); 
        }
        motorState = !motorState;
        motorFlag = 0;
    } 
}

int main()
{
           
    CyGlobalIntEnable; /* Enable global interrupts. */

    swint_StartEx(swisr); // start the interrupt handler for the switch
    
    // Get the Motor PWMs going
    PWMLEFT_Start();
    PWMRIGHT_Start();
    setSpeed(LEFT,0);
    setSpeed(RIGHT,0);
    
    
    // Start the quadrature encoders
    QDLEFT_Start();
    QDRIGHT_Start();
    QDLEFT_TriggerCommand(QDLEFT_MASK, QDLEFT_CMD_RELOAD);
    QDRIGHT_TriggerCommand(QDRIGHT_MASK, QDRIGHT_CMD_RELOAD);
    
    // Start the timer and ISR for the tachometer
    tachPwm_Start();
    tachTimer_StartEx(tachISR);
    CyBle_GapSetLocalName("chargan");
  
    
    // Start the BLE
    CyBle_Start(BleCallBack);

    for(;;)
    {      
        handleMotor();
        
        if(processTachFlag)
            processTach();
            
            
        CyBle_ProcessEvents();
        CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
    }
}

/* [] END OF FILE */
