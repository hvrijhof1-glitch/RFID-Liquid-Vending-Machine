#include <SPI.h>                    // Enables Serial Peripheral Interface protocol for RFID RC522
#include <MFRC522.h>                // Includes the core hardware driver library for the RFID module 
#include <Wire.h>                   // Enables I2C data protocol for the RGB LCD and 8x8 LED Matrix 
#include "rgb_lcd.h"                // Includes specific device definitions for the Grove RGB LCD 
#include <Grove_LED_Bar.h>          // Includes the software driver required to control the 10-segment LED Bar 
#include "grove_two_rgb_led_matrix.h" // Includes device library for the 8x8 RGB LED Matrix 
#include <Keypad.h>                 // Provides non-blocking matrix scanning logic for the 4x4 keypad 
// Music Note
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880

// Pin Assignments
#define RST_PIN         9           // Allocates Digital Pin 9 for the RFID hardware reset control 
#define SS_PIN          10          // Assigns Digital Pin 10 as the SPI Slave Select (SS) line for RFID 
#define SPEAKER_PIN     3           // Allocates Digital PWM Pin 3 to pipe audio frequencies to the speaker 
#define LED_BAR_CLK     5           // Assigns Digital Pin 5 as the timing Clock line for the LED Bar 
#define LED_BAR_DATA    4           // Assigns Digital Pin 4 as the Serial Data line for the LED Bar


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Initializes RFID instance with its select and reset pins 
rgb_lcd lcd;                        // Instantiates the I2C-controlled RGB Liquid Crystal Display 
Grove_LED_Bar bar(LED_BAR_CLK, LED_BAR_DATA, 0); // Instantiates the LED Bar with normal counting orientation 
GroveTwoRGBLedMatrixClass matrix;   // Instantiates the 8x8 RGB LED Matrix driver over I2C 

// Keypad Configuration 
const int ROWS = 4;                 // Defines row dimension boundary of the keypad
const int COLS = 4;                 // Defines column dimension boundary of the keypad 
char keys[ROWS][COLS] = {           // Maps the layout of your modified custom keypad
  {'0', '1', '2', 'A'},
  {'3', 'T', 'Q', 'M'},
  {'J', 'S', 'W', 'E'},
  {'#', 'D', '&', 'C'}
};
byte rowPins[ROWS] = {2, 6, 7, 8};  // Assigns Arduino Pins 2, 6, 7, 8 to keypad rows 
byte colPins[COLS] = {14, 15, 16, 17}; // Assigns Pins A0-A3 as Digital 14-17 for keypad columns
Keypad keypad4x4 = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Creates the Keypad management object 

// Authorized IUD
String authorizedUIDcarte = "AB 33 E0 AC"; // Stores the card UID 
String authorizedUIDtag = "67 E4 80 04";   // Stores your tag UID 

// Global Variables
int totalAttempts = 0;              // Counter tracking daily card verification attempts (Max 3) 
int ordersDelivered = 0;            // Counter tracking daily successful product deliveries (Max 3) 
unsigned long startTime;            // Variable tracking the time
bool sessionActive = false;         // Logic flag active during an approved vending countdown window 
bool accessApproved = false;        // Tracks whether the UID is approved or not 
char globalKey = NO_KEY;            // Variable holding the latest key character

// Buffers
String currentInput = "";           // String tracking the combined keypad button entries 
String lastSelectedLiquid = "";      // Stores the liquid type 

// Emoji for the 8x8 LED Matrix ---
uint8_t emoji_attempt1[64] = {               // Byte map forming an emoji 
     0xff,0xff,0x18,0x18,0x18,0x18,0xff,0xff, 
    0xff,0x18,0x18,0x18,0x18,0x18,0x18,0xff, 
    0x18,0xff,0x18,0x18,0x18,0x18,0xff,0x18, 
    0xff,0x18,0xff,0x18,0x18,0xff,0x18,0xff, 
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18, 
    0x18,0xff,0xff,0xff,0xff,0xff,0xff,0x18, 
    0xff,0x18,0xff,0xff,0xff,0xff,0x18,0xff, 
    0xff,0xff,0x18,0x18,0x18,0x18,0xff,0xff,
};
uint8_t emoji_attempt2[64] = {                // Byte map forming the planet earth
    0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xff, 
    0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe, 
    0xfe,0xff,0xfe,0xfe,0xfe,0xfe,0xff,0xfe, 
    0xfe,0xff,0xfe,0xfe,0xfe,0xfe,0xff,0xfe, 
    0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe, 
    0xfe,0xfe,0xff,0xff,0xff,0xff,0xfe,0xfe, 
    0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe, 
    0xfe,0xfe,0xff,0xfe,0xfe,0xff,0xfe,0xfe,
};
uint8_t emoji_attempt3[64] = {               // Byte map forming a ghost 
    0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xfe, 
    0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xff, 
    0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xfe,0xff,0x12,0x12,0x12,0x12,0xff,0xfe, 
    0xfe,0xff,0x12,0x12,0x12,0x12,0xff,0xfe, 
    0xfe,0xfe,0xff,0x12,0x12,0xff,0xfe,0xfe, 
    0xfe,0xfe,0xff,0xff,0xff,0xff,0xfe,0xfe,
};
uint8_t clear_screen[64] = {                // Completely clear byte map used to reset the 8x8 LED matrix display
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

// System Reset Function  Help of AI to make this function
void ResetToHome() {
    totalAttempts = 0;              // Resets attempt loops back to defaults 
    ordersDelivered = 0;            // Clears previous oders registers 
    sessionActive = false;         // Resset of sessionActive variable 
    accessApproved = false;        // Disables validation access
    currentInput = "";              // Reset entirely the character tracking strings 
    
    bar.setLevel(0);                // Put the LED countdown bar back to 0
    matrix.displayFrames(clear_screen, 2000, true, 1); // Clears out the LED Matrix 
    
    lcd.setRGB(255, 0, 0);          // Display background red
    lcd.clear();                    
    lcd.setCursor(0, 0);
    lcd.print("System Reset..."); // Print 'System Reset...'
    delay(1000);                    // Pauses program for 1000 milliseconds
    
    lcd.setRGB(255, 255, 255);      // Restores the white background 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan RFID Card");    // Re-prints "Scan RFID Card"
}

// Arduino Initialization
void setup() {
    Serial.begin(9600);            
    SPI.begin();                    // Commences Master SPI hardware configurations on the Uno board
    mfrc522.PCD_Init();             // Sends internal startup execution codes directly into the reader chip

    lcd.begin(16, 2);               // Programme the LCD grid space to 16 columns and 2 text lines
    bar.begin();                    // Powers up connection links for the LED Level Display array 
    Wire.begin();                   // Starts standard local I2C timing loops
    matrix.resetDevice();           // Completely reset the 8x8 LED Matrix chip

    lcd.setRGB(255, 255, 255);      // Initialize the white background of the LCD 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan RFID Card");    // Prints "Scan RFID Card" at the begining of the first row
}

// Arduino Executive Loop
void loop() {
    CheckAccess();                  // Call for the CheckAccess() function to catch the UID of the cart presented
    Get_activated_key();            // Call for the Get_activated_key() function that get the character press on the keypad
    RunVendingProcess();            // Call for the RunVendingProcess() function that start the process of the vending machine
    UpdateLEDBar();                 // Call for the UpdateLEDBar() function that refreshe the time indicator bar
}

// FUNCTIONS


void Color_Attempt(int attempt, bool approved) {
    if (approved) {
        lcd.setRGB(0, 255, 0);       // green background on successful authentication
    } 
    else {
        if (attempt == 1)      
        lcd.setRGB(255, 255, 0);  // First denied access background: Yellow 
        else if (attempt == 2) 
        lcd.setRGB(255, 50, 0);   // second denied access background: Orange 
        else if (attempt == 3) 
        lcd.setRGB(255, 0, 0);    // Third denied access background: Red
    }
}


void Music_Attempt(int attempt) {
    switch (attempt) {
        case 1: { // Play Mario music on Attempt 1
            int marioMelody[9] = { NOTE_E5, NOTE_E5, 0, NOTE_E5, 0, NOTE_C5, NOTE_E5, 0, NOTE_G5 };
            int marioDurations[9] = { 100, 100, 100, 100, 100, 100, 100, 100, 200 };
            int notesCount = sizeof(marioMelody) / sizeof(int);
            
            for (int i = 0; i < notesCount; i++) {
                if (marioMelody[i] == 0) {
                    noTone(SPEAKER_PIN); // The speaker makes no sound
                } else {
                    tone(SPEAKER_PIN, marioMelody[i], marioDurations[i]); // Emit frequency pitch tone
                }
                delay(marioDurations[i] * 1.3); // gap between note so that they don't blend into each other
            }
            break;
        }
        case 2: { // Play Star Wars music on Attempt 2 
            int starwarsMelody[9] = { NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4 };
            int starwarsDurations[9] = { 300, 300, 300, 200, 100, 300, 200, 100, 400 };
            int notesCount = sizeof(starwarsMelody) / sizeof(int);
            
            for (int i = 0; i < notesCount; i++) {
                tone(SPEAKER_PIN, starwarsMelody[i], starwarsDurations[i]);
                delay(starwarsDurations[i] * 1.2);
            }
            break;
        }
        case 3: { // Play Pirates of the Caribbean sea music on Attempt 3 
            int piratesMelody[15] = { NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0, NOTE_F5, NOTE_G5, NOTE_E5, NOTE_E5, 0 };
            int piratesDurations[15] = { 120, 120, 240, 120, 40, 120, 120, 240, 120, 40, 120, 120, 240, 120, 40 };
            int notesCount = sizeof(piratesMelody) / sizeof(int);
            
            for (int i = 0; i < notesCount; i++) {
                if (piratesMelody[i] == 0) {
                    noTone(SPEAKER_PIN);
                } else {
                    tone(SPEAKER_PIN, piratesMelody[i], piratesDurations[i]);
                }
                delay(piratesDurations[i] * 1.4);
            }
            break;
        }
    }
    noTone(SPEAKER_PIN); // To be sure that the speaker produce no sound
}

void Emoji_Attempt(bool start) {
    if (start) { // Display the correct emoji depending on the attempt
        if (totalAttempts == 1)     
            matrix.displayFrames(emoji_attempt1, 2000, true, 1);
        else if (totalAttempts == 2) 
            matrix.displayFrames(emoji_attempt2, 2000, true, 1);
        else if (totalAttempts == 3) 
            matrix.displayFrames(emoji_attempt3, 2000, true, 1);
    } else {
        matrix.displayFrames(clear_screen, 2000, true, 1); // Clears the display completely when turned off
    }
}

void CheckAccess() {
    // Exit if no card are held near the RFID Module
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) 
        return;
    
    // Blocked the system if 3 attempts are reached
    if (totalAttempts >= 3) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Blocked: RA0");
        return;
    }
    
    Serial.print("UID tag :");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    totalAttempts++; // Add +1 the the attempt counter
    
    // Verifies if UID match to authorize UID
    if (content.substring(1) == authorizedUIDcarte || content.substring(1) == authorizedUIDtag) {
        accessApproved = true;          // Approve access if the condition is respected
        Color_Attempt(totalAttempts, true); // Turns screen background green 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Approved Access");    // Prints "Approved Access" at line 0 
    } 
    else {
        accessApproved = false;         // Decline access if the condition is not respected
        Color_Attempt(totalAttempts, false); // Turn the screen background to the correct color depending on the attempt
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Denied Access");     // Display "Denied Access" on the screen
        delay(1000);                    // Let the user see the text displayed                   
        EndAttempt();                   // End the attempt and call the function 
    }
    mfrc522.PICC_HaltA(); // Stop the RFID Module so that a scanned card won't use multiple tries
}

void Get_activated_key() {
    char key = keypad4x4.getKey(); // Get the correct character assigned to the button pressed

    if (key != NO_KEY) {            // Executes only if a key is pressed
        globalKey = key;            // To have a global key accesible for each function
        Serial.println((String) "The activated key is " + globalKey); // Outputs the key activated in the serial monitor
        
        // Emits a sound whenever a button is pressed
        int music4[3] = {500};
        for (int i = 0; i < 3; i++) {
                tone(SPEAKER_PIN, music4[i], 100);
                delay(50);
            }
        noTone(SPEAKER_PIN);
    }
}

void RunVendingProcess() {
    if (globalKey == NO_KEY) 
    return; // Filters if no button updates are made
    char key = globalKey;            // Buffers for the press character
    globalKey = NO_KEY;              // Resets variable to prepare for next button presses

    // Delete button
    if (key == 'D') {                // Character comparison 
        currentInput = "";           // Clear buffered variables
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("Deleted!");        //Print "Deleted!" on the LCD 
        delay(1000);
        lcd.clear();
        return;                   
    }

    currentInput += key;             // Add the new key into the current input

    if (currentInput.indexOf("000") >= 0) { 
        ResetToHome();               // Direct to the reset function if "000" sequence is made
        return; 
    }

    
    char first = currentInput[0]; // Get the first character of the variable
    if (first != 'A' && first != '#' && first != '&' && first != 'T' && first != 'Q' && first != 'M' && first != '0') {// Check if the starting character is one of the starting character otherwise it wipes it 
        currentInput = ""; 
        return;            
    }
    // Another chack but for the color or Emoji modifications
    if ((first == '#' || first == '&') && currentInput.length() >= 2) {
        if (currentInput[1] != 'C' && currentInput[1] != 'E') {
            currentInput = ""; 
            return;
        }
    }

    String targetNum = String(totalAttempts); // Getting the number of attempt in the correct type to use it
    // Attempt check
    if (currentInput == "A" + targetNum) {    // Check the exact attempt indexes is pressed
        sessionActive = true;                 // Activate the session
        startTime = millis();                 // Get the starting time
        bar.setLevel(10);                     // Sets on all of the 10 segment of the LED bar 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Timer Started");
        currentInput = "";
    } 
    else if (currentInput.startsWith("A") && currentInput.length() >= 2) {
        currentInput = "";                    // Filters if wrong input is made
    }

    // Background color check
    else if (currentInput == "#C" + targetNum) { // Display the green background if the condition are meet
        Color_Attempt(totalAttempts, accessApproved);
        currentInput = "";
    } 
    else if (currentInput == "&C" + targetNum) { // Resets background colors to white
        lcd.setRGB(255, 255, 255); 
        currentInput = "";
    } 
    else if ((currentInput.startsWith("#C") || currentInput.startsWith("&C")) && currentInput.length() >= 3) {
        currentInput = "";
    }

    // Type of liquid check
    else if (currentInput == "TJ" || currentInput == "TS" || currentInput == "TW") {
        if (sessionActive && accessApproved) { // Verifies if sessionActive and accessApproved are true 
            lcd.clear();
            lcd.setCursor(0, 0);
            if (currentInput == "TJ") { 
                lcd.print("Selected: Juice"); 
                lastSelectedLiquid = "Juice"; } 
            if (currentInput == "TS") { 
                lcd.print("Selected: Soda "); 
                lastSelectedLiquid = "Soda";  } 
            if (currentInput == "TW") { 
                lcd.print("Selected: Water"); 
                lastSelectedLiquid = "Water"; } 
        currentInput = "";
    } 
    else if (currentInput.startsWith("T") && currentInput.length() >= 2) {
        currentInput = "";
    }

    // Quantity Check
    else if (currentInput == "Q100" || currentInput == "Q200" || currentInput == "Q300") {
        if (sessionActive && accessApproved) {
            String val = currentInput.substring(1); // Strips 'Q' prefix to render the number smoothly
            lcd.setCursor(0, 1);
            lcd.print("Quantity: " + val + "ml "); // Prints the quantity selected in row line 2 
        }
        currentInput = "";
    } 
    else if (currentInput.startsWith("Q") && currentInput.length() >= 4) {
        currentInput = "";
    }

    // Audio check
    else if (currentInput == "M" + targetNum) { // Check the exact music indexes is pressed
        if (sessionActive && accessApproved) {
            Music_Attempt(totalAttempts);       // Play the corresponding music
        }
        currentInput = "";
    } 
    else if (currentInput.startsWith("M") && currentInput.length() >= 2) {
        currentInput = "";
    }

    // Emoji check
    else if (currentInput == "#E" + targetNum) { // Check the exact emoji indexes is pressed
        if (sessionActive && accessApproved) {
            Emoji_Attempt(true);               // Lights up the corresponding emoji
        }
        currentInput = "";
    } 
    else if (currentInput == "&E" + targetNum) { // Clear the LED Matric 
        if (sessionActive && accessApproved) {
            Emoji_Attempt(false);              
            CompleteOrder();                   // Triggers the validation of the order
        }
        currentInput = "";
    } 
    else if ((currentInput.startsWith("#E") || currentInput.startsWith("&E")) && currentInput.length() >= 3) {
        currentInput = "";
    }
    if (currentInput.length() > 4) {
        currentInput = "";                     // Clear if string is too long
    }
}

void UpdateLEDBar() {
    if (!sessionActive) 
        return;               // Ignores updating sequences if no loops run 
    
    unsigned long timer = (millis() - startTime) / 1000; // Calculate the time in live
    if (timer >= 120) {                       // Automatic timeout at 120 seconds
        bar.setLevel(0);                      // Resets bar level to 0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.setRGB(255, 0, 0);                // Turn background to red 
        lcd.print("Time Over!");              // Display "Time Over!"
        EndAttempt();                         // End the attempt by calling the function
        return;
    }
    int level = 10 - (timer / 12);            // Calculates the number of bar to turn off
    bar.setLevel(level);                      // Display the correct amount of bar 
}

void CompleteOrder() {
    ordersDelivered++;                        // Increments the number of order
    lastSelectedLiquid = "";                  // Clears the variable
    EndAttempt();                             // End the attempt
}

void EndAttempt() {
    sessionActive = false;                    // Deactivates the session
    delay(2000);                              
    lcd.setRGB(255, 255, 255);                // Restores white background of the LCD
    matrix.displayFrames(clear_screen, 2000, true, 1); // Clear the LED Matrix
    
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("RA");
    lcd.print(3 - totalAttempts);             // Prints remaining attempt
    
    lcd.setCursor(0, 1);
    lcd.print("ORDER ");
    lcd.print(ordersDelivered);               // Prints the number of delivered order
    if (ordersDelivered == 3) {
        lcd.print("-MAX QUOTA");              // If there is 3 order print additionaly "-MAX QUOTA"
    } 
    bar.setLevel(0);                          // Reset the LED Bar
    currentInput = "";                        // Clear the variable
    delay(2000);                            
    
    if (totalAttempts < 3) {                  // Checks the number of attemps to know if it is the maximu or not 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan RFID Card");          // Print "Scan RFID Card"
    }
}