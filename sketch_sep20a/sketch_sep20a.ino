/*
 * BMW E36 Cluster - Complete Working Demo
 * Using proper BMW E36 specifications:
 * RPM: 3 pulses per revolution (6-cylinder engine)
 * Speed: 9 pulses per differential revolution
 * Smooth realistic car acceleration/deceleration
 */

#define TACHOMETER_PIN 2    // RPM signal output (12V pulse train)
#define SPEEDOMETER_PIN 3   // VSS signal output (12V pulse train)

// BMW E36 Technical Specifications with cluster-specific calibration
const float PULSES_PER_ENGINE_REV = 3.0;    // 6-cylinder = 3 pulses per revolution
const float PULSES_PER_DIFF_REV = 9.0;      // Differential sensor = 9 pulses per rev
const float TIRE_CIRCUMFERENCE = 1.88;      // meters (typical for E36 205/60R15)
const float FINAL_DRIVE_RATIO = 3.15;       // Typical E36 rear diff ratio

// Calibration factors for your specific cluster (adjust based on actual behavior)
const float RPM_CALIBRATION_FACTOR = 1.2;   // Increase calculated RPM frequency
const float SPEED_CALIBRATION_FACTOR = 1.5; // Increase calculated speed frequency

// Current vehicle state
float currentSpeed = 0.0;     // km/h
float currentRPM = 0.0;       // RPM
float targetSpeed = 0.0;      // Target speed for smooth transitions
float targetRPM = 0.0;        // Target RPM for smooth transitions

// Demo control variables
unsigned long lastUpdate = 0;
unsigned long phaseStartTime = 0;
int demoPhase = 0;
bool phaseChanged = false;

// Timing variables for pulse generation
unsigned long lastRPMPulse = 0;
unsigned long lastSpeedPulse = 0;
unsigned long rpmPulseInterval = 0;   // Microseconds between RPM pulses
unsigned long speedPulseInterval = 0; // Microseconds between speed pulses
bool rpmPulseState = false;
bool speedPulseState = false;

void setup() {
  Serial.begin(9600);
  Serial.println("BMW E36 Cluster - Complete Working Demo");
  Serial.println("Using proper BMW specifications:");
  Serial.println("- RPM: 3 pulses per engine revolution");
  Serial.println("- Speed: 9 pulses per differential revolution");
  Serial.println("- Smooth realistic acceleration/deceleration");
  
  pinMode(TACHOMETER_PIN, OUTPUT);
  pinMode(SPEEDOMETER_PIN, OUTPUT);
  
  // Start with engine off
  digitalWrite(TACHOMETER_PIN, LOW);
  digitalWrite(SPEEDOMETER_PIN, LOW);
  
  phaseStartTime = millis();
  
  Serial.println("Starting realistic driving simulation...");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Update vehicle state every 50ms for smooth transitions
  if (currentTime - lastUpdate >= 50) {
    updateDrivingPhase(currentTime);
    smoothTransitions();
    calculatePulseIntervals();
    lastUpdate = currentTime;
  }
  
  // Generate RPM pulses
  if (rpmPulseInterval > 0 && (micros() - lastRPMPulse >= rpmPulseInterval)) {
    rpmPulseState = !rpmPulseState;
    digitalWrite(TACHOMETER_PIN, rpmPulseState ? HIGH : LOW);
    lastRPMPulse = micros();
  }
  
  // Generate Speed pulses
  if (speedPulseInterval > 0 && (micros() - lastSpeedPulse >= speedPulseInterval)) {
    speedPulseState = !speedPulseState;
    digitalWrite(SPEEDOMETER_PIN, speedPulseState ? HIGH : LOW);
    lastSpeedPulse = micros();
  }
}

void updateDrivingPhase(unsigned long currentTime) {
  unsigned long totalElapsed = currentTime - phaseStartTime;
  
  // Complete cycle takes 100 seconds 
  float cycleProgress = (float)(totalElapsed % 100000) / 100000.0;
  
  // Create smooth, continuous driving simulation with realistic gear shifts
  if (cycleProgress < 0.20) {
    // 0-20 seconds: Engine start and acceleration through gears 1-3
    float progress = cycleProgress / 0.20;
    
    // Simulate gear shifts with RPM drops
    if (progress < 0.3) {
      // 1st gear: 20-40 km/h
      float gearProgress = progress / 0.3;
      targetSpeed = 20 + (gearProgress * 20);  // Start at 20 km/h (cluster minimum)
      targetRPM = 1500 + (gearProgress * 1800); // 1500 to 3300 RPM
    }
    else if (progress < 0.35) {
      // 1st to 2nd gear shift - RPM drops
      targetSpeed = 40;
      targetRPM = 2000; // RPM drops during shift
    }
    else if (progress < 0.6) {
      // 2nd gear: 40-65 km/h  
      float gearProgress = (progress - 0.35) / 0.25;
      targetSpeed = 40 + (gearProgress * 25);
      targetRPM = 2000 + (gearProgress * 1400); // 2000 to 3400 RPM
    }
    else if (progress < 0.65) {
      // 2nd to 3rd gear shift - RPM drops
      targetSpeed = 65;
      targetRPM = 2200; // RPM drops during shift
    }
    else {
      // 3rd gear: 65-80 km/h
      float gearProgress = (progress - 0.65) / 0.35;
      targetSpeed = 65 + (gearProgress * 15);
      targetRPM = 2200 + (gearProgress * 1000); // 2200 to 3200 RPM
    }
    
    if (totalElapsed % 8000 < 100) {
      Serial.println("ACCELERATION - Shifting through gears 1-2-3");
    }
  }
  else if (cycleProgress < 0.45) {
    // 20-45 seconds: Acceleration to highway speed with 4th and 5th gear shifts
    float progress = (cycleProgress - 0.20) / 0.25;
    
    if (progress < 0.4) {
      // 3rd to 4th gear acceleration
      float gearProgress = progress / 0.4;
      targetSpeed = 80 + (gearProgress * 30); // 80 to 110 km/h
      targetRPM = 3200 + (gearProgress * 600); // Build RPM for shift
    }
    else if (progress < 0.5) {
      // 3rd to 4th gear shift - RPM drops
      targetSpeed = 110;
      targetRPM = 2600; // Big RPM drop during shift
    }
    else if (progress < 0.8) {
      // 4th gear acceleration
      float gearProgress = (progress - 0.5) / 0.3;
      targetSpeed = 110 + (gearProgress * 20); // 110 to 130 km/h
      targetRPM = 2600 + (gearProgress * 800); // 2600 to 3400 RPM
    }
    else if (progress < 0.9) {
      // 4th to 5th gear shift - RPM drops
      targetSpeed = 130;
      targetRPM = 2400; // RPM drops during final shift
    }
    else {
      // 5th gear
      float gearProgress = (progress - 0.9) / 0.1;
      targetSpeed = 130 + (gearProgress * 10); // 130 to 140 km/h
      targetRPM = 2400 + (gearProgress * 200); // 2400 to 2600 RPM
    }
    
    if (totalElapsed % 10000 < 100) {
      Serial.println("HIGHWAY ACCELERATION - Shifting 4th to 5th gear");
    }
  }
  else if (cycleProgress < 0.65) {
    // 45-65 seconds: Highway cruising with minor variations
    float progress = (cycleProgress - 0.45) / 0.20;
    targetSpeed = 138 + (sin(progress * 6 * PI) * 5); // ±5 km/h variations
    targetRPM = 2600 + (sin(progress * 8 * PI) * 150); // Minor throttle adjustments
    
    if (totalElapsed % 12000 < 100) {
      Serial.println("HIGHWAY CRUISE - 5th gear, minor throttle adjustments");
    }
  }
  else if (cycleProgress < 0.85) {
    // 65-85 seconds: Deceleration with downshifting
    float progress = (cycleProgress - 0.65) / 0.20;
    
    if (progress < 0.3) {
      // Engine braking in 5th gear
      float decelProgress = progress / 0.3;
      targetSpeed = 140 - (decelProgress * 40); // 140 to 100 km/h
      targetRPM = 2600 - (decelProgress * 400); // 2600 to 2200 RPM
    }
    else if (progress < 0.35) {
      // 5th to 4th downshift - RPM jumps up
      targetSpeed = 100;
      targetRPM = 2800; // RPM increases during downshift
    }
    else if (progress < 0.7) {
      // 4th gear engine braking
      float decelProgress = (progress - 0.35) / 0.35;
      targetSpeed = 100 - (decelProgress * 40); // 100 to 60 km/h
      targetRPM = 2800 - (decelProgress * 600); // 2800 to 2200 RPM
    }
    else if (progress < 0.75) {
      // 4th to 3rd downshift - RPM jumps up
      targetSpeed = 60;
      targetRPM = 2600; // RPM increases during downshift
    }
    else {
      // 3rd gear final deceleration
      float decelProgress = (progress - 0.75) / 0.25;
      targetSpeed = 60 - (decelProgress * 25); // 60 to 35 km/h
      targetRPM = 2600 - (decelProgress * 800); // 2600 to 1800 RPM
    }
    
    if (totalElapsed % 8000 < 100) {
      Serial.println("DECELERATION - Downshifting and engine braking");
    }
  }
  else {
    // 85-100 seconds: City driving and final stop
    float progress = (cycleProgress - 0.85) / 0.15;
    
    if (progress < 0.6) {
      // City driving with variations
      float cityProgress = progress / 0.6;
      targetSpeed = 35 - (sin(cityProgress * 2 * PI) * 10); // 25-45 km/h variations
      targetRPM = 1800 + (abs(sin(cityProgress * 2 * PI)) * 400); // 1800-2200 RPM
    }
    else {
      // Final approach to stop
      float stopProgress = (progress - 0.6) / 0.4;
      targetSpeed = 25 - (stopProgress * 5); // Slow to 20 km/h minimum
      targetRPM = 1500 - (stopProgress * 700); // Return towards idle
    }
    
    if (totalElapsed % 6000 < 100) {
      Serial.println("CITY DRIVING - Stop and go, approaching destination");
    }
  }
  
  // Ensure minimum speed of 20 km/h for cluster
  targetSpeed = max(targetSpeed, 20.0);
  
  // Reset cycle
  if (totalElapsed >= 100000) {
    phaseStartTime = currentTime;
    Serial.println("RESTARTING CYCLE - Engine start and acceleration");
  }
}

void smoothTransitions() {
  // Smooth acceleration/deceleration
  float speedDiff = targetSpeed - currentSpeed;
  float rpmDiff = targetRPM - currentRPM;
  
  // Adjust speed smoothly (max change per update)
  if (abs(speedDiff) > 0.5) {
    currentSpeed += (speedDiff > 0) ? 0.8 : -1.2;  // Deceleration is faster
  } else {
    currentSpeed = targetSpeed;
  }
  
  // Adjust RPM smoothly
  if (abs(rpmDiff) > 20) {
    currentRPM += (rpmDiff > 0) ? 25 : -40;  // RPM changes faster than speed
  } else {
    currentRPM = targetRPM;
  }
  
  // Ensure values stay within realistic bounds
  currentSpeed = constrain(currentSpeed, 0, 220);
  currentRPM = constrain(currentRPM, 0, 7000);
}

void calculatePulseIntervals() {
  // Calculate RPM pulse interval with calibration factor
  if (currentRPM > 100) {
    // RPM to Hz: (RPM / 60) * 3 pulses per revolution * calibration factor
    float rpmFreq = (currentRPM / 60.0) * PULSES_PER_ENGINE_REV * RPM_CALIBRATION_FACTOR;
    // Convert to pulse interval in microseconds (half period for square wave)
    rpmPulseInterval = (unsigned long)(500000.0 / rpmFreq);
  } else {
    rpmPulseInterval = 0;  // No pulses at very low RPM
  }
  
  // Calculate Speed pulse interval with calibration factor
  if (currentSpeed > 2) {
    // Speed to differential revolutions per second
    float metersPerSecond = currentSpeed / 3.6;  // Convert km/h to m/s
    float diffRevsPerSecond = metersPerSecond / TIRE_CIRCUMFERENCE;
    // Convert to pulse frequency with calibration factor
    float speedFreq = diffRevsPerSecond * PULSES_PER_DIFF_REV * SPEED_CALIBRATION_FACTOR;
    // Convert to pulse interval in microseconds
    speedPulseInterval = (unsigned long)(500000.0 / speedFreq);
  } else {
    speedPulseInterval = 0;  // No pulses at very low speed
  }
  
  // Debug output every 2 seconds
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 2000) {
    Serial.print("Target - RPM: ");
    Serial.print((int)currentRPM);
    Serial.print(", Speed: ");
    Serial.print((int)currentSpeed);
    Serial.print(" km/h | Actual frequencies - RPM: ");
    Serial.print(rpmPulseInterval > 0 ? 1000000.0 / (rpmPulseInterval * 2) : 0);
    Serial.print(" Hz, Speed: ");
    Serial.print(speedPulseInterval > 0 ? 1000000.0 / (speedPulseInterval * 2) : 0);
    Serial.println(" Hz");
    lastDebug = millis();
  }
}