// PowerGuard: Smart Auto Power Cut System
// Libraries
#include <LiquidCrystal_I2C.h>

// Pin Definitions
const int VOLTAGE_SENSOR_PIN = A0;    // Analog pin for voltage sensor
const int CURRENT_SENSOR_PIN = A1;    // Analog pin for current sensor
const int RELAY_PIN = 7;              // Digital pin for relay control
const int LED_PIN = 13;               // Built-in LED for status indication

// Constants
const float VOLTAGE_THRESHOLD = 4.2;  // Voltage threshold for full charge (adjust as needed)
const float CURRENT_THRESHOLD = 0.1;  // Current threshold to detect charging completion
const unsigned long CHECK_INTERVAL = 1000; // Check every second

// Global Variables
float currentVoltage = 0.0;
float currentAmperage = 0.0;
bool isCharging = false;
unsigned long lastCheckTime = 0;

// Initialize LCD (I2C address might need adjustment)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initial display
  lcd.setCursor(0, 0);
  lcd.print("PowerGuard v1.0");
  delay(2000);
  
  // Turn on power initially
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check conditions every CHECK_INTERVAL milliseconds
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;
    
    // Read sensors
    readSensors();
    
    // Update display
    updateDisplay();
    
    // Check if charging is complete
    if (isChargingComplete()) {
      cutPower();
    }
    
    // Print debug information to Serial
    printDebugInfo();
  }
}

void readSensors() {
  // Read voltage sensor
  int voltageRaw = analogRead(VOLTAGE_SENSOR_PIN);
  currentVoltage = convertToVoltage(voltageRaw);
  
  // Read current sensor
  int currentRaw = analogRead(CURRENT_SENSOR_PIN);
  currentAmperage = convertToCurrent(currentRaw);
  
  // Determine if device is charging
  isCharging = (currentAmperage > 0.1);
}

float convertToVoltage(int raw) {
  // Convert analog reading to voltage
  // This conversion depends on your voltage divider setup
  return raw * (5.0 / 1023.0) * 2; // Adjust multiplier based on voltage divider
}

float convertToCurrent(int raw) {
  // Convert analog reading to current
  // This conversion depends on your current sensor specifications
  return (raw - 512) * 0.0049; // Example conversion for ACS712
}

bool isChargingComplete() {
  return currentVoltage >= VOLTAGE_THRESHOLD && currentAmperage <= CURRENT_THRESHOLD;
}

void cutPower() {
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Charging Complete");
  lcd.setCursor(0, 1);
  lcd.print("Power Cut Off");
  
  delay(5000); // Display message for 5 seconds
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String(currentVoltage, 1) + "V " + String(currentAmperage, 1) + "A");
  lcd.setCursor(0, 1);
  lcd.print(isCharging ? "Charging..." : "Not Charging");
}

void printDebugInfo() {
  Serial.print("Voltage: ");
  Serial.print(currentVoltage);
  Serial.print("V, Current: ");
  Serial.print(currentAmperage);
  Serial.print("A, Status: ");
  Serial.println(isCharging ? "Charging" : "Not Charging");
}