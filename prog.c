#include <Arduino.h>  
#include <U8g2lib.h>  
#include <SoftwareSerial.h>  
#include <DFRobot_SIM808.h>  
// Pin definitions  
const int ENA = 5; // PWM pin for motor speed control  
const int IN1 = 7; // Motor direction pin  
const int IN2 = 8; // Motor direction pin  
const int buzzer = 13; // Buzzer pin  
const int sensorPin = A0; // Alcohol sensor pin  
// Threshold value for alcohol detection  
const int thresholdValue = 500;  
// Initialize U8g2 for the 4-pin LCD screen with 1-page buffer (less 
memory)  
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, /* 
reset=*/ U8X8_PIN_NONE);  
// Create a SoftwareSerial object for SIM808 communication on pins 10 
(RX) and 11 (TX)  
SoftwareSerial sim808Serial(10, 11);  
// Initialize SIM808 object with SoftwareSerial instance  
DFRobot_SIM808 sim808(&sim808Serial);  
// Variables  
int motorSpeed = 255; // Initial motor speed  
bool alcoholDetected = false; // Track if alcohol has been detected  
void setup() {  
// Set pin modes  
pinMode(ENA, OUTPUT);  
pinMode(IN1, OUTPUT);  
pinMode(IN2, OUTPUT);  
pinMode(buzzer, OUTPUT);  
pinMode(sensorPin, INPUT);  
// Initialize motor direction  
digitalWrite(IN1, HIGH);  
digitalWrite(IN2, LOW);  
// Initialize LCD screen with reduced memory configuration  
u8g2.begin();  
// Start motor at full speed  
analogWrite(ENA, motorSpeed);  
// Initialize SIM808 module using SoftwareSerial  
sim808Serial.begin(9600);  
Serial.begin(9600);  
if (sim808.init()) {  
Serial.println("SIM808 initialized successfully!");  
} else {  
Serial.println("Failed to initialize SIM808!");  
}  
}  
void loop() {  
// Read alcohol sensor value  
int alcoholValue = analogRead(sensorPin);  
// Display the values on the screen  
displayValues(alcoholValue, thresholdValue, motorSpeed);  
// Check if alcohol is detected above the threshold and if it has not been 
detected previously  
if (alcoholValue >= thresholdValue && !alcoholDetected) {  
alcoholDetected = true; // Set flag to true to indicate alcohol was 
detected  
// Ring buzzer for 10 seconds  
tone(buzzer, 1000); // Start buzzer at 1000 Hz  
delay(10000); // Wait for 10 seconds  
noTone(buzzer); // Stop buzzer  
// Slowly decrease motor speed from 255 to 0  
for (motorSpeed = 255; motorSpeed >= 0; motorSpeed--) {  
analogWrite(ENA, motorSpeed); // Set motor speed  
displayValues(alcoholValue, thresholdValue, motorSpeed); // Update 
screen  
delay(39); // 10 seconds divided by 255 = ~39 ms delay for each 
decrement  
}  
// Send SMS alert  
sendAlertSMS("+918885361029", "Alcohol over limit detected!");  
// Ensure motor is completely off  
analogWrite(ENA, 0);  
motorSpeed = 0; // Set motor speed to 0  
}  
// Refresh the display every 0.5 seconds  
delay(500);  
}  
// Function to display alcohol value, threshold value (or motor speed if 
detected) on LCD screen  
void displayValues(int alcoholValue, int threshold, int speed) {  
u8g2.clearBuffer(); // Clear the internal memory of the display  
u8g2.setFont(u8g2_font_6x10_tr); // Use a smaller font to save memory  
// Display alcohol value  
u8g2.setCursor(0, 7);  
u8g2.print("Alcohol Value: ");  
u8g2.print(alcoholValue);  
// Display threshold or motor speed based on alcohol detection  
u8g2.setCursor(0, 15);  
if (alcoholValue < threshold) {  
u8g2.print("Threshold: ");  
u8g2.print(threshold);  
} else {  
u8g2.print("Motor Speed: ");  
u8g2.print(speed);  
}  
u8g2.sendBuffer(); // Send buffer content to the display  
}  
// Function to send an SMS alert using SIM808  
void sendAlertSMS(const char* phoneNumber, const char* message) {  
// Ensure SIM808 is in text mode  
sim808Serial.print("AT+CMGF=1\r");  
delay(1000);  
// Specify the recipient's phone number  
sim808Serial.print("AT+CMGS=\"");  
sim808Serial.print(phoneNumber);  
sim808Serial.print("\"\r");  
delay(1000);  
// Send the message content  
sim808Serial.print(message);  
delay(1000);  
// Send Ctrl+Z character to send the SMS  
sim808Serial.write(26);  
delay(5000);  
Serial.println("SMS sent successfully!");  
} 
