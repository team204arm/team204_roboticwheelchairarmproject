// ==================================================
// Last updated: 3/30/2026
// - Wrist and gripper functionality
// - Button swap between modes and fixed custom funcs 
// ==================================================

#include <ArduinoJson.h>
#include <Preferences.h>
#include <float.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <Preferences.h>

// ================= FLASH STORAGE ==================
Preferences prefs;

// ==================================================
// ================= DISPLAY =========================
// ==================================================
constexpr uint8_t TFT_CS = 13;
constexpr uint8_t TFT_DC = 12;

Adafruit_ST7796S tft(TFT_CS, TFT_DC);

// Theme Colors
#define MAROON        0x8000
#define ORANGE        0xFD20
#define DARK_ORANGE   0xFB00
#define SOFT_ORANGE   0xFD80

// ==================================================
// ================= JOYSTICK =======================
// ==================================================
int varX = A3;
int varY = A2;
int varZ = A4;
const int buttonPin = A5;
bool gripperMode = false;
bool lastGripButtonState = HIGH;
unsigned long gripDebounceTime = 0;
const unsigned long gripDebounceDelay = 50;

// ==================================================
// ================= MODE SWITCH ====================
// ==================================================
const int switchPin = 32;

bool menuMode = false;
bool lastMenuMode = false;

bool switchReading = HIGH;
bool switchState = HIGH;
bool lastSwitchReading = HIGH;

unsigned long switchDebounceTime = 0;
const unsigned long switchDebounceDelay = 50;

// ==================================================
// ================= EMERGENCY STOP =================
// ==================================================
const int estopPin = 14;     // Red Arcade Button
bool estopActive = false;
bool lastEstopState = false;

bool estopReading = HIGH;
bool estopButtonState = HIGH;
bool lastEstopReading = HIGH;

unsigned long estopDebounceTime = 0;
const unsigned long estopDebounceDelay = 50;

// ==================================================
// ============== CUSTOM FUNCTION ===================
// ==================================================
int maxSavedPoints = 3;
int pointsPerFunction[3] = {3, 3, 3};
int MaxChange = 0;

float xPoints[3][10];
float yPoints[3][10];
float zPoints[3][10];

int currentFunction = -1;   // 0,1,2 for custom 1,2,3

int pointCount = 0;
//bool recordModeActive = false;

bool lastButtonState = HIGH;
bool currentButtonState = HIGH;

const int record = 0; // 0 means false, record is not running, 1 means true, record is running
// ===========================
// ====== SAFETY LIMITS ======
// ===========================
float X_MIN = -465;
float X_MAX =  465;

float Y_MIN = -445;
float Y_MAX =  445;

float Z_MIN =   -280;
float Z_MAX =  460;

float XY_MAX_RADIUS = 480;  // start slightly conservative
float XYZ_MAX_RADIUS = 500;


// ==================================================
// ================= ARM VARIABLES ==================
// ==================================================
int num1, num2, num3;
float X, Y, Z;
float wrist;

int max_joystickY = 6;
int min_joystickY = -4;
int max_joystickX = max_joystickY;
int min_joystickX = min_joystickY;
int max_joystickZ = max_joystickY;
int min_joystickZ = min_joystickY;

float curxpos, curypos, curzpos, curtpos, curgpos;

int home_x = 46;
int home_y = 3;
int home_z = 60;
const float WRIST_OUT = 3.9; // flat wrist value
const float rotate_min = 2.6; // minimum wrist value
const float rotate_max = 5.0; // maximum wrist value
const float Gripper_open = 2400; // gripper's widest opening
const float Gripper_closed = 3130; // grippers  tightest closing

// ==================================================
// ================= MENU VARIABLES =================
// ==================================================
const int maxMenuItems = 9;
int menuIndex = 0;
bool inMenuScreen = true;

unsigned long lastScrollTime = 0;
unsigned long lastButtonTime = 0;

const int scrollDelay = 250;
const int buttonDelay = 300;

String menuItems[maxMenuItems] = {
  "Say hi!",
  "Stow away",
  "Bring cup",
  "Elevator Press",
  "Custom 1",
  "Custom 2",
  "Custom 3",
  "Record Custom Func",
  "Rename Custom Func"
};
String customNames[3] = {"Custom 1", "Custom 2", "Custom 3"};
// ==================================================
// ================= SETUP ==========================
// ==================================================
void setup() {
  
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(varX, INPUT);
  pinMode(varY, INPUT);
  pinMode(varZ, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(estopPin, INPUT_PULLUP);

  SPI.begin(SCK, -1, MOSI);
  // delay(3000);

  tft.init(320, 480, 0, 0, ST7796S_BGR);
  tft.setRotation(0);  // Portrait
  tft.invertDisplay(true);
  tft.setSPISpeed(16000000);

  while (!Serial1.available()) {
    delay(10);
  }

  curxpos = 150;
  curypos = 0;
  curzpos = 150;
  curtpos = 3.24;
  curgpos = 2700; //Fully open

  // Initialize flash storage
  prefs.begin("customFns", false);  // namespace, read/write
  loadCustomFunctions();

  drawArmManualScreen();
}

// ==================================================
// ================= E-STOP CHECK ===================
// ==================================================
void checkEstop() {

  estopReading = digitalRead(estopPin);

  if (estopReading != lastEstopReading) {
    estopDebounceTime = millis();
  }

  if ((millis() - estopDebounceTime) > estopDebounceDelay) {

    if (estopReading != estopButtonState) {

      estopButtonState = estopReading;

      if (estopButtonState == LOW) {
        estopActive = !estopActive;
      }
    }
  }

  lastEstopReading = estopReading;
}

// ===============================================
// ================= Menu Check ==================
// ===============================================
void checkModeSwitch() {

  switchReading = digitalRead(switchPin);

  if (switchReading != lastSwitchReading) {
    switchDebounceTime = millis();
  }

  if ((millis() - switchDebounceTime) > switchDebounceDelay) {

    if (switchReading != switchState) {
      switchState = switchReading;

      // ONLY trigger on press 
      if (switchState == LOW) {
        menuMode = !menuMode;   
      }
    }
  }

  lastSwitchReading = switchReading;
}
// ==================================================
// ================ ARM SCREEN =======================
// ==================================================
void drawArmManualScreen() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(4);
  tft.setCursor(50, 180);
  tft.println("MANUAL");

  tft.setTextSize(3);
  tft.setCursor(70, 230);
  tft.println("CONTROL");
}

// ==================================================
// ================ GRIPPER SCREEN ==================
// ==================================================
void drawGripperScreen() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(4);
  tft.setCursor(50, 180);
  tft.println("GRIPPER");

  tft.setTextSize(3);
  tft.setCursor(70, 230);
  tft.println("CONTROL");
}

// ==================================================
// ================= MENU SCREEN =====================
// ==================================================
void drawMenu() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.setCursor(60, 30);
  tft.println("ARM OPTIONS");

  tft.drawFastHLine(20, 70, 280, ORANGE);

  tft.setTextSize(2);

  for (int i = 0; i < maxMenuItems; i++) {

    int yPos = 100 + (i * 45);

    if (i == menuIndex) {
      tft.fillRoundRect(20, yPos - 5, 280, 35, 8, DARK_ORANGE);
      tft.setTextColor(MAROON);
    } else {
      tft.setTextColor(SOFT_ORANGE);
    }

    tft.setCursor(40, yPos);
    tft.println(menuItems[i]);
  }
}

// ==================================================
// ================= E-STOP SCREEN ==================
// ==================================================
void drawEstopScreen() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(4);
  tft.setCursor(20, 150);
  tft.println("EMERGENCY");

  tft.setCursor(70, 200);
  tft.println("STOP");

  tft.drawFastHLine(20, 250, 280, ORANGE);

  tft.setTextSize(2);
  tft.setCursor(40, 290);
  tft.println("Emergency Stop Pressed");

  tft.setCursor(60, 320);
  tft.println("Arm Motion Frozen");
}

// ==================================================
// ============== SELECTED SCREEN ====================
// ==================================================
void showSelectedScreen() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.setCursor(60, 150);
  tft.println("EXECUTING");

  tft.setTextSize(2);
  tft.setCursor(60, 200);
  tft.println(menuItems[menuIndex]);
}

// ==================================================
// =======Record Function Screen Instructions =======
// ==================================================
void showRecordInstructions() {

  tft.fillScreen(MAROON);

  // Title
  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.setCursor(40, 60);
  tft.print("Custom Function          ");
  tft.println(currentFunction + 1);

  // Info
  tft.setTextSize(2);
  tft.setCursor(60, 140);
  tft.print("Waypoints: ");
  tft.println(maxSavedPoints);

  tft.setCursor(60, 190);
  tft.println("Press button to");

  tft.setCursor(60, 220);
  tft.println("start moving arm");

  // Footer
  tft.setTextColor(SOFT_ORANGE);
  tft.setCursor(50, 320);
  tft.println("Then press to save");
  tft.setTextColor(SOFT_ORANGE);
  tft.setCursor(50, 350);
  tft.println("each point");
}

// ==================================================
// ======= BRING CUP ALIGNMENT SCREEN ==============
// ==================================================
void drawBringCupAlignScreen() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.setCursor(70, 30);
  tft.println("BRING CUP");

  tft.drawFastHLine(20, 70, 280, ORANGE);

  tft.setTextSize(3);
  tft.setCursor(40, 120);
  tft.println("ALIGN CUP");

  tft.setTextSize(2);
  tft.setCursor(40, 170);
  tft.println("Use joystick to");
  tft.setCursor(40, 200);
  tft.println("move the arm");

  tft.fillRoundRect(30, 260, 260, 200, 12, DARK_ORANGE);

  tft.setTextColor(MAROON);
  tft.setTextSize(2);
  tft.setCursor(60, 285);
  tft.println("Press button");

  tft.setCursor(60, 315);
  tft.println("to CONFIRM GRIP");

  tft.setTextSize(2);
  tft.setCursor(45, 345);
  tft.println("(Gripper will CLOSE)");
}

// ==================================================
// ======= Elevator press screen  ===================
// ==================================================
void drawelevator() {

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.setCursor(70, 30);
  tft.println("Elevator");

  tft.drawFastHLine(20, 70, 280, ORANGE);

  tft.setTextSize(3);
  tft.setCursor(40, 120);
  tft.println("ALIGN ARM");

  tft.setTextSize(2);
  tft.setCursor(40, 170);
  tft.println("Use joystick to");
  tft.setCursor(40, 200);
  tft.println("move the arm");

  tft.fillRoundRect(30, 260, 260, 200, 12, DARK_ORANGE);

  tft.setTextColor(MAROON);
  tft.setTextSize(2);
  tft.setCursor(60, 285);
  tft.println("Press button");

  tft.setCursor(60, 315);
  tft.println("to continue");

  tft.setTextSize(2);
  tft.setCursor(75, 345);
  tft.println("(Arm will stow)");
}


// ==================================================
// ================= ARM CONTROL ====================
// ==================================================
void manual_mode() {
  checkEstop();
  if (estopActive) return;   // Immediately exit movement

  num1 = analogRead(varX);
  num2 = analogRead(varY);
  num3 = analogRead(varZ);

  X = map(num1, 0, 4095, min_joystickX, max_joystickX);
  Y = map(num2, 0, 4095, min_joystickY, max_joystickY);
  Z = map(num3, 0, 4095, min_joystickZ, max_joystickZ);

  if (abs(X) < 2) X = 0;
  if (abs(Y) < 2) Y = 0;
  if (abs(Z) < 2) Z = 0;

 if (gripperMode) {
  // ===== GRIPPER + WRIST CONTROL =====

  if(Y < -1 || Y > 1){
    curgpos += 3*Y;
    clampGripper(curgpos, curtpos);

    StaticJsonDocument<200> curposdoc;
    curposdoc["T"] = 1013;
    curposdoc["spd"] = 500;
    curposdoc["acc"] = 0;
    curposdoc["OpenCloseVal"] = curgpos;
    serializeJson(curposdoc, Serial1);
    Serial1.println();
    delay(50);
  }

  if(X < -1 || X > 1){
    wrist = X / 100;
    curtpos += wrist;
    clampGripper(curgpos, curtpos);

    StaticJsonDocument<200> curposdoc;
    curposdoc["T"] = 1041;
    curposdoc["x"] = curxpos;
    curposdoc["y"] = curypos;
    curposdoc["z"] = curzpos;
    curposdoc["t"] = curtpos;

    serializeJson(curposdoc, Serial1);
    Serial1.println();
    delay(50);
  }

} else {
  // ===== NORMAL ARM CONTROL =====

    curxpos += X;
    curypos -= Y;
    curzpos += Z;

    // Apply circular clamp
    clampXYZToSphere(curxpos, curypos, curzpos);

  StaticJsonDocument<200> curposdoc;
  curposdoc["T"] = 1041;
  curposdoc["x"] = curxpos;
  curposdoc["y"] = curypos;
  curposdoc["z"] = curzpos;
  curposdoc["t"] = curtpos;

  serializeJson(curposdoc, Serial1);
  Serial1.println();

  delay(50);
}
}
// ================================
// ========== Containment =========
// ================================
void clampXYZToSphere(float &x, float &y, float &z) {
  float r = sqrt(x*x + y*y + z*z);

  if (r > XYZ_MAX_RADIUS) {
    float scale = XYZ_MAX_RADIUS / r;
    x *= scale;
    y *= scale;
    z *= scale;
  }
}

void clampGripper(float &g, float &t) {
  g = constrain(g, Gripper_open, Gripper_closed);
  t = constrain(t, rotate_min, rotate_max);
}

// ==================================================
// ================= SPEED CONTROL ==================
// ==================================================
float maxMoveStep = 4.0;   // Smaller = slower & smoother
int moveDelay = 20;        // ms between steps

// ==================================================
// ================= ARM MOVEMENT ===================
// ==================================================
void ReturnHome(int homeX, int homeY, int homeZ) {
  moveDelay = 40; // Both the step size and delay are slowed down due to one waypoint to ensure safety of the user
  maxMoveStep = 5.0;
  moveTo(homeX, homeY, homeZ, 4);
  moveDelay = 20;
  maxMoveStep = 4.0;
}

void moveTo(float targetX, float targetY, float targetZ, float targetT) {
  checkEstop();
  targetZ = constrain(targetZ, Z_MIN, Z_MAX);

  // Clamp XY target to circle
  // clampXYToCircle(targetX, targetY);
  if (estopActive) return;   // Immediately exit movement

  float dx = targetX - curxpos;
  float dy = targetY - curypos;
  float dz = targetZ - curzpos;
  float dt = targetT - curtpos;

  float distance = sqrt(dx*dx + dy*dy + dz*dz);

  // If already very close, just send final command
  if (distance < 1.0) {
    checkEstop();
    if (estopActive) return;
    curxpos = targetX;
    curypos = targetY;
    curzpos = targetZ;
    curtpos = targetT;
  }
  else {

    int steps = distance / maxMoveStep;
    if (steps < 1) steps = 1;

    float stepX = dx / steps;
    float stepY = dy / steps;
    float stepZ = dz / steps;
    float stepT = dt / steps;

    for (int i = 0; i < steps; i++) {
      checkEstop();
      if (estopActive) return;

      curxpos += stepX;
      curypos += stepY;
      curzpos += stepZ;

      clampXYZToSphere(curxpos, curypos, curzpos);
      curtpos += stepT;

      StaticJsonDocument<200> curposdoc;
      curposdoc["T"] = 1041;
      curposdoc["x"] = curxpos;
      curposdoc["y"] = curypos;
      curposdoc["z"] = curzpos;
      curposdoc["t"] = curtpos;

      serializeJson(curposdoc, Serial1);
      Serial1.println();

      delay(moveDelay);
    }
  }

  checkEstop();
  if (estopActive) return;   // Immediately exit movement
  // Final exact correction
  curxpos = constrain(targetX, X_MIN, X_MAX);
  curypos = constrain(targetY, Y_MIN, Y_MAX);
  curzpos = constrain(targetZ, Z_MIN, Z_MAX);
  curtpos = targetT;

  StaticJsonDocument<200> finaldoc;
  finaldoc["T"] = 1041;
  finaldoc["x"] = curxpos;
  finaldoc["y"] = curypos;
  finaldoc["z"] = curzpos;
  finaldoc["t"] = curtpos;

  serializeJson(finaldoc, Serial1);
  Serial1.println();
}

// =================================================
// ============= SETTING GRIPPER VALUE =============
// =================================================
void setGripper(int value) {
  curgpos = value;

  StaticJsonDocument<200> doc;
  doc["T"] = 1013;
  doc["spd"] = 1000;
  doc["acc"] = 0;
  doc["OpenCloseVal"] = curgpos;

  serializeJson(doc, Serial1);
  Serial1.println();
}

// ==================================================
// ================= FUNCTIONS ======================
// ==================================================
void sayHello() {
  checkEstop();
  if (estopActive) return;
  moveTo(150, 0, 434, 3);
  delay(1000);
  moveTo(150, 0, 434, 4);
  delay(1300);
  moveTo(150, 0, 434, 3);
  delay(1300);
  moveTo(150, 0, 434, 4);
  delay(1300);
  moveTo(150, 0, 434, 3);
  delay(1300);
  moveTo(128, 17, 114, 3);
}

void bring_cup() {
  checkEstop();
  if (estopActive) return;

  delay(500);
  float t = WRIST_OUT;   // <-- lock wrist direction
  float g = Gripper_open;

  setGripper(g);
  delay(500);   // give time to actuate

  // Open gripper
  curtpos = 4;
  moveTo(220, 0, 180, curtpos);
  delay(2000);

  moveTo(220, 0, 0, curtpos);
  delay(2000);

  checkEstop();
  if (estopActive) return;   // Immediately exit movement

  // Alignment Screen
  drawBringCupAlignScreen();

  while (digitalRead(buttonPin) == HIGH) {

  checkEstop();
  if (estopActive) return;

  manual_mode();
  }
  // Close gripper
  setGripper(Gripper_closed);
  delay(500);   // give time to actuate

  curtpos = 3.24;
  moveTo(curxpos,curypos,curzpos, curtpos);

  delay(2500);
  checkEstop();
  if (estopActive) return;
  moveTo(220, 0, 40, curtpos);
  delay(1500);

  moveTo(220, 0, 180, curtpos);
  delay(1500);
  checkEstop();
  if (estopActive) return;

  moveTo(130, -200, 200, curtpos);
  delay(600);
  checkEstop();
  if (estopActive) return;
}

void elevator_setup() {
  checkEstop();
  if (estopActive) return;

  float t = WRIST_OUT;   // <-- lock wrist direction
  float g = Gripper_closed;

  setGripper(g);
  delay(500);   // give time to actuate

  // Approach position
  moveTo(150, 0, 150, t);
  delay(800);

  moveTo(200, 0, 200, t);
  delay(500);

  moveTo(400, 0, 150, t);
  delay(500);

  checkEstop();
  if (estopActive) return;

  // Alignment screen
  drawelevator();

  // Manual fine alignment (BUT KEEP WRIST LOCKED)
  while (digitalRead(buttonPin) == HIGH) {

    checkEstop();
    if (estopActive) return;

    // Override wrist so joystick can't mess it up
    curtpos = t;

    manual_mode();
  }

  checkEstop();
  if (estopActive) return;

  delay(1000);

  // Return home with neutral wrist
  ReturnHome(home_x, home_y, home_z);
}

// =========================
// ==== Recording Function =
// =========================

void RecordFunction() {
  // maxSavedPoints = pointsPerFunction[currentFunction];

  if (currentFunction < 0) return;
  
  // Wait for button release BEFORE starting
  while (digitalRead(buttonPin) == LOW);
  delay(200);  // debounce safety


  for (int i = 0; i < maxSavedPoints; i++) {

    // Wait for intentional press
    while (digitalRead(buttonPin) == HIGH) {
      manual_mode();
    }

    // Save CURRENT ARM POSITION
    xPoints[currentFunction][i] = curxpos;
    yPoints[currentFunction][i] = curypos;
    zPoints[currentFunction][i] = curzpos;

    // Visual feedback
    tft.fillScreen(MAROON);

    // BIG NUMBER
    tft.setTextColor(ORANGE);
    tft.setTextSize(6);
    tft.setCursor(80, 120);
    tft.print(i + 1);

    // Label
    tft.setTextSize(3);
    tft.setCursor(60, 200);
    tft.println("POINT SAVED");

    delay(400);

    // Wait for release before next loop
    while (digitalRead(buttonPin) == LOW);
    delay(200);
  }

  tft.fillScreen(MAROON);

  tft.setTextColor(ORANGE);
  tft.setTextSize(4);
  tft.setCursor(40, 160);
  tft.println("RECORDING");

  tft.setCursor(70, 210);
  tft.println("COMPLETE");

  saveCustomFunctions(maxSavedPoints);   // save to flash

  tft.setTextColor(ORANGE);
  tft.setTextSize(2);
  tft.setCursor(40, 260);
  tft.println("Press button to return");

  // Wait for user to press to begin
  while (digitalRead(buttonPin) == HIGH);
  while (digitalRead(buttonPin) == LOW);
  delay(200);
}

//==================================
// Select Function Slot to Record
// =================================
void selectFunctionToRecord() {

  int recordIndex = 0;
  int lastRecordIndex = -1;   // force first draw
  bool selecting = true;

  // Clear once
  tft.fillScreen(MAROON);

  while (selecting) {

    int rawX = analogRead(varX);
    int mappedX = map(rawX, 0, 4095, -10, 10);

    // Scroll detection
    if (millis() - lastScrollTime > scrollDelay) {

      if (mappedX < -6) {
        recordIndex++;
        recordIndex = constrain(recordIndex, 0, 2);
        lastScrollTime = millis();
      } 
      else if (mappedX > 6) {
        recordIndex--;
        recordIndex = constrain(recordIndex, 0, 2);
        lastScrollTime = millis();
      }
    }

    // ONLY redraw if selection changed
    if (recordIndex != lastRecordIndex) {

      lastRecordIndex = recordIndex;

      tft.fillScreen(MAROON);

      tft.setTextColor(ORANGE);
      tft.setTextSize(3);
      tft.setCursor(40, 60);
      tft.println("Record To:");

      tft.setTextSize(2);

      for (int i = 0; i < 3; i++) {

        int yPos = 140 + (i * 60);

        if (i == recordIndex) {
          tft.fillRoundRect(40, yPos - 5, 240, 40, 8, DARK_ORANGE);
          tft.setTextColor(MAROON);
        } else {
          tft.setTextColor(SOFT_ORANGE);
        }

        tft.setCursor(60, yPos);
        tft.println(customNames[i]);
      }
    }

    // Confirm selection
    if (digitalRead(buttonPin) == LOW && millis() - lastButtonTime > buttonDelay) {

      lastButtonTime = millis();
      currentFunction = recordIndex;
      selecting = false;
    }
  }

    // Wait for release of selection press
  while (digitalRead(buttonPin) == LOW);
  delay(200);

  // STEP 1: Select number of waypoints
  selectNumberOfPoints();

  // Ensure clean button state
  while (digitalRead(buttonPin) == LOW);   // wait for release
  delay(200);

  // STEP 2: Show instructions
  showRecordInstructions();

  // Wait for a NEW intentional press
  while (digitalRead(buttonPin) == HIGH);  // wait for press
  delay(200);
  while (digitalRead(buttonPin) == LOW);   // wait for release
  delay(200);

  // STEP 3: Start recording
  RecordFunction();
}

// =============================
// ==== Play Back Function =====
// =============================
void playCustomFunction() {

  if (currentFunction < 0) return;

  int numPoints = pointsPerFunction[currentFunction];
  numPoints = constrain(numPoints, 0, 10);

  for (int i = 0; i < numPoints; i++) {

    moveTo(
      xPoints[currentFunction][i],
      yPoints[currentFunction][i],
      zPoints[currentFunction][i],
      curtpos
    );

    delay(500);
  }
}

// =============================
// ==== SAVE CUSTOM FUNCTIONS ==
// =============================
void saveCustomFunctions(int maxSavedPoints) {

  // Save number of points for THIS function
  pointsPerFunction[currentFunction] = maxSavedPoints;

  // Save all function point counts
  prefs.putBytes("pointsPerFn", pointsPerFunction, sizeof(pointsPerFunction));

  // Save all position arrays
  prefs.putBytes("xPts", xPoints, sizeof(xPoints));
  prefs.putBytes("yPts", yPoints, sizeof(yPoints));
  prefs.putBytes("zPts", zPoints, sizeof(zPoints));
}

// =============================
// ==== SAVE CUSTOM NAMES ======
// =============================
void saveCustomNames() {
  for (int i = 0; i < 3; i++) {
    String key = "name" + String(i);
    prefs.putString(key.c_str(), customNames[i]);
  }
  
}
// Rename Custom Function
void updateMenuNames() {
  menuItems[4] = customNames[0];
  menuItems[5] = customNames[1];
  menuItems[6] = customNames[2];
}

// =============================
// ==== LOAD CUSTOM FUNCTIONS ==
// =============================
void loadCustomFunctions() {

  if (prefs.isKey("pointsPerFn")) {
    prefs.getBytes("pointsPerFn", pointsPerFunction, sizeof(pointsPerFunction));
    prefs.getBytes("xPts", xPoints, sizeof(xPoints));
    prefs.getBytes("yPts", yPoints, sizeof(yPoints));
    prefs.getBytes("zPts", zPoints, sizeof(zPoints));
  }

  // Load names
  for (int i = 0; i < 3; i++) {
    String key = "name" + String(i);
    if (prefs.isKey(key.c_str())) {
      customNames[i] = prefs.getString(key.c_str(), customNames[i]);
    }
  }

  updateMenuNames();
}

// ===========================
// ==== Select # of Points ===
// ===========================

void selectNumberOfPoints() {

  // Start from existing value for this function
  maxSavedPoints = pointsPerFunction[currentFunction];

  int lastValue = -1;

  // Initial draw
  tft.fillScreen(MAROON);

  while (true) {

    int rawX = analogRead(varX);
    int mappedX = map(rawX, 0, 4095, -10, 10);

    // Adjust value
    if (millis() - lastScrollTime > scrollDelay) {

      if (mappedX < -6) {
        maxSavedPoints++;
        maxSavedPoints = constrain(maxSavedPoints, 3, 10);
        lastScrollTime = millis();
      }
      else if (mappedX > 6) {
        maxSavedPoints--;
        maxSavedPoints = constrain(maxSavedPoints, 3, 10);
        lastScrollTime = millis();
      }
    }

    // Redraw only if changed
    if (maxSavedPoints != lastValue) {

      lastValue = maxSavedPoints;

      tft.fillScreen(MAROON);

      tft.setTextColor(ORANGE);
      tft.setTextSize(3);
      tft.setCursor(30, 60);
      tft.println("Select Waypoints");

      tft.setTextSize(2);
      tft.setCursor(40, 120);
      tft.println("Move joystick U/D");

      tft.setCursor(60, 150);
      tft.println("3 to 10 points");

      // Big number
      tft.setTextSize(6);
      tft.setCursor(120, 220);
      tft.print(maxSavedPoints);

      // Footer
      tft.setTextSize(2);
      tft.setTextColor(SOFT_ORANGE);
      tft.setCursor(20, 320);
      tft.println("Press button to confirm");
    }

    // Confirm selection
    if (digitalRead(buttonPin) == LOW && millis() - lastButtonTime > buttonDelay) {
      lastButtonTime = millis();
      break;
    }
  }

  // Wait for release
  while (digitalRead(buttonPin) == LOW);
  delay(200);
}


// ===============================
// =========== Rename GUI =======
// ==============================
void renameCustomFunction() {

  int selected = 0;
  int lastSelected = -1;

  // ================================
  // STEP 1: SELECT FUNCTION
  // ================================
  while (true) {

    int mappedX = map(analogRead(varX), 0, 4095, -10, 10);

    if (millis() - lastScrollTime > scrollDelay) {
      if (mappedX < -6) selected++;
      else if (mappedX > 6) selected--;
      selected = constrain(selected, 0, 2);
      lastScrollTime = millis();
    }

    // ONLY redraw if changed
    if (selected != lastSelected) {

      lastSelected = selected;

      tft.fillScreen(MAROON);

      tft.setTextColor(ORANGE);
      tft.setTextSize(3);
      tft.setCursor(40, 60);
      tft.println("Rename Which?");

      for (int i = 0; i < 3; i++) {
        int y = 140 + i * 60;

        if (i == selected) {
          tft.fillRoundRect(40, y - 5, 240, 40, 8, DARK_ORANGE);
          tft.setTextColor(MAROON);
        } else {
          tft.setTextColor(SOFT_ORANGE);
        }

        tft.setCursor(60, y);
        tft.println(customNames[i]);
      }
    }

    if (digitalRead(buttonPin) == LOW && millis() - lastButtonTime > buttonDelay) {
      lastButtonTime = millis();
      break;
    }
  }

  // wait for release
  while (digitalRead(buttonPin) == LOW);
  delay(200);

  // ================================
  // STEP 2: NAME EDITOR
  // ================================

  const int maxLen = 10;
  char name[maxLen];
  
  // initialize blanks
  for (int i = 0; i < maxLen; i++) name[i] = ' ';

  int cursor = 0;
  int lastCursor = -1;
  char currentChar = 'A';
  char lastChar = 0;

  while (true) {

    int mappedX = map(analogRead(varX), 0, 4095, -10, 10);
    int mappedY = map(analogRead(varY), 0, 4095, -10, 10);

    // LEFT / RIGHT = move cursor
    if (millis() - lastScrollTime > scrollDelay) {

      if (mappedY > 6) cursor++;
      else if (mappedY < -6) cursor--;

      cursor = constrain(cursor, 0, maxLen - 1);

      // UP / DOWN = change letter
      if (mappedX < -6) currentChar++;
      else if (mappedX > 6) currentChar--;

      if (currentChar > 'Z') currentChar = ' ';
      if (currentChar < ' ') currentChar = 'Z';

      lastScrollTime = millis();
    }

    // ONLY redraw if something changed
    if (cursor != lastCursor || currentChar != lastChar) {

      lastCursor = cursor;
      lastChar = currentChar;

      tft.fillScreen(MAROON);

      tft.setTextColor(ORANGE);
      tft.setTextSize(3);
      tft.setCursor(40, 60);
      tft.println("Edit Name");

      // draw name slots
      tft.setTextSize(3);
      tft.setCursor(40, 140);

      for (int i = 0; i < maxLen; i++) {
        if (i == cursor) {
          tft.setTextColor(MAROON);
          tft.fillRect(40 + i * 20, 140, 18, 30, DARK_ORANGE);
        } else {
          tft.setTextColor(SOFT_ORANGE);
        }

        char displayChar = (i == cursor) ? currentChar : name[i];
        tft.setCursor(40 + i * 20, 140);
        tft.print(displayChar);
      }

      // instructions
      tft.setTextSize(2);
      tft.setTextColor(SOFT_ORANGE);
      tft.setCursor(40, 220);
      tft.println("Up/Down: Letter");
      tft.setCursor(40, 250);
      tft.println("Left/Right: Move");
      tft.setCursor(40, 280);
      tft.println("Press: Confirm");
    }

    // BUTTON = lock in letter
    if (digitalRead(buttonPin) == LOW && millis() - lastButtonTime > buttonDelay) {

      lastButtonTime = millis();

      name[cursor] = currentChar;

      // move forward automatically
      cursor++;

      if (cursor >= maxLen) break;

      delay(200);
    }
  }

  // ================================
  // SAVE NAME
  // ================================
  String finalName = "";
  for (int i = 0; i < maxLen; i++) {
    finalName += name[i];
  }

  finalName.trim();  // remove trailing spaces

  customNames[selected] = finalName;

  saveCustomNames();
  updateMenuNames();
}

// ==================================================
// ================= LOOP ===========================
// ==================================================
void loop() {

  checkEstop();
  checkModeSwitch();
  Serial.println(digitalRead(buttonPin));

  bool currentGripButtonState = digitalRead(buttonPin);

  // Detect press (HIGH → LOW transition)
 if (!menuMode && inMenuScreen &&   // <-- KEY LINE
    lastGripButtonState == HIGH && currentGripButtonState == LOW &&
    millis() - gripDebounceTime > gripDebounceDelay) {

  gripDebounceTime = millis();

  gripperMode = !gripperMode;

  if (gripperMode) {
    drawGripperScreen();
  } else {
    drawArmManualScreen();
  }
}

lastGripButtonState = currentGripButtonState;
  // ===============================
  // E-STOP STATE CHANGE DETECTION
  // ===============================
  if (estopActive != lastEstopState) {

    if (estopActive) {
      drawEstopScreen();
    } else {
      // Restore correct screen
      if (menuMode) {
        drawMenu();
      } else {
        drawArmManualScreen();
      }
    }

    lastEstopState = estopActive;
  }
  // If E-Stop is active, freeze everything else
  if (estopActive) {
    return;
  }



if (menuMode != lastMenuMode) {
  lastMenuMode = menuMode;

  if (menuMode) {
    gripperMode = false;   // <-- FORCE EXIT
    drawMenu();
  } else {
    drawArmManualScreen();
  }
}

  if (!menuMode) {
    manual_mode();
  }
  else {

    int rawX = analogRead(varX);
    int mappedX = map(rawX, 0, 4095, -10, 10);

    if (millis() - lastScrollTime > scrollDelay) {

      if (mappedX < -6) { 
        menuIndex++;
        menuIndex = constrain(menuIndex, 0, maxMenuItems - 1);
        lastScrollTime = millis();
        drawMenu();
      }
      else if (mappedX > 6) { 
        menuIndex--;
        menuIndex = constrain(menuIndex, 0, maxMenuItems - 1);
        lastScrollTime = millis();
        drawMenu();
      }
    }

    if (digitalRead(buttonPin) == LOW && millis() - lastButtonTime > buttonDelay) {

      lastButtonTime = millis();

      if (inMenuScreen) {
        inMenuScreen = false;
        showSelectedScreen();

        switch (menuIndex) {
          case 0: gripperMode = false; sayHello(); break;
          case 1: gripperMode = false; ReturnHome(home_x, home_y, home_z); break;
          case 2: gripperMode = false; bring_cup(); break;
          case 3: gripperMode = false; elevator_setup(); break;
          case 4: gripperMode = false; // custom function 1
              currentFunction = 0;
              playCustomFunction(); break;
          case 5: gripperMode = false; // custom function 2
              currentFunction = 1;
              playCustomFunction(); break;
          case 6: gripperMode = false; // custom function 3
              currentFunction = 2;
              playCustomFunction(); break;
          case 7:  gripperMode = false; // Record function
            selectFunctionToRecord();
            break;
          case 8: renameCustomFunction();
            break;
        }

        inMenuScreen = true;
        drawMenu();
      }
    }
  }
}
