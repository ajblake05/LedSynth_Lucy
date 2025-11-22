// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE
// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE
// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE

int chaseLEDselected = 2; // LED channel selection
float chaseLEDattValue = 1; // LED attenuation value [log] (0=max, 1=1/10, 2=1/100, ...)
unsigned long follow_start_buffer = 250; // buffer between TrigIN start to LED ON
unsigned long follow_end_buffer   = 250; // buffer between LED OFF and TrigIN end 

long lead_full_cycle_duration = 10000;
long lead_LED_on              =  2000;
long lead_LED_off             =  5000;
long lead_TRIG_on             =  2500;
long lead_TRIG_off            =  5500;


void chaseHelp(String inStr) {
  if (inStr.substring(0, 4) != "help" ) { return; }
  
  Serial.println("- HELP CHASE -------------------------------------------------------------------------------------------------------------");
  Serial.println("  Input 'report' to see current values");
  Serial.println();
  Serial.println("  The CHASE protocol builder is designed for testing the timings with periferal devices.");
  Serial.println("  Two modes are available: FOLLOW and LEAD.");
  Serial.println();
  Serial.println("  FOLLOW mode: after a TrigIN signal goes to 3.3 V the LED will turn ON after a specified buffer period and turn off");
  Serial.println("    giving another buffer before the TrigIN signal goes to 0 V.");
  Serial.println("  LEAD mode: the LED and TrigOUT will turn ON and OFF at specified times in a cycle of defined length.");
  Serial.println();
  Serial.println("  Commands:");
  Serial.println("    set c +X      LED channel selection");
  Serial.println("    set a +F.F    LED attenuation value [log] (0=max, 1=1/10, 2=1/100, ...)");
  Serial.println("    set f s +X    follow mode start buffer [µs] (buffer from TrigIN start to LED ON)");
  Serial.println("    set f e +X    follow mode  end buffer [µs] (buffer from LED OFF to TrigIN end");
  Serial.println("    set l f +X    lead mode full cycle duration [µs]");
  Serial.println("    set l a +X    lead mode   LED   ON  point [µs]");
  Serial.println("    set l b +X    lead mode   LED   OFF point [µs]");
  Serial.println("    set l c +X    lead mode TrigOUT ON  point [µs]");
  Serial.println("    set l d +X    lead mode TrigOUT OFF point [µs]");
  Serial.println("    run f         start the FOLLOW protocol");
  Serial.println("    run l         start the LEAD protocol");
  Serial.println("    stop          stop the protocol");
  Serial.println();
  Serial.println("  Input 'exit' to exit the Chase protocol builder");
  Serial.println("  Input 'help' to see this again");
  Serial.println("------------------------------------------------------------------------------------------------------------------- END -");
} // end protHelp

void chaseReport(String inStr) {
  if (inStr.substring(0, 6) != "report") { return; }
  
  Serial.println("Chase settings --------------------------------------------------------");
  Serial.println("  LED settings:");
  Serial.printf ("    LED selected:          %d\n", chaseLEDselected);
  Serial.printf ("    LED attenuation [log]: %5.4f\n", chaseLEDattValue);
  Serial.println("  FOLLOW mode:");
  Serial.printf( "    LED ON  buffer  [µs]: %d\n", follow_start_buffer);
  Serial.printf( "    LED OFF buffer [µs]: %d\n", follow_end_buffer);
  Serial.println("  LEAD mode:");
  Serial.printf( "    Full cycle duration [µs]: %d\n", lead_full_cycle_duration);
  Serial.printf( "      LED   ON   time   [µs]: %d\n", lead_LED_on);
  Serial.printf( "      LED   OFF  time   [µs]: %d\n", lead_LED_off);
  Serial.printf( "    TrigOUT ON   time   [µs]: %d\n", lead_TRIG_on);
  Serial.printf( "    TrigOUT OFF  time   [µs]: %d\n", lead_TRIG_off);
  Serial.println("-----------------------------------------------------------------------");
  Serial.println();
} // end protReport

void chaseSetFromSerial(String command) {
  if (command.substring(0, 3) != "set") { return; }
  
  
  if (command.substring(0, 5) == "set c") {    chaseLEDselected = constrain(command.substring(5).toInt(), 0, D_NLS);                  Serial.println("Channel: " + String(chaseLEDselected)); }
  if (command.substring(0, 5) == "set a") {    chaseLEDattValue = constrain(command.substring(5).toFloat(), 0.0, float(MAX_ATT_VALUE));        Serial.println("Attenuation: " + String(chaseLEDattValue) + " log"); }
  
  if (command.substring(0, 7) == "set f s") {       follow_start_buffer = command.substring(7).toInt();     Serial.println("Follow start buffer: " + String(follow_start_buffer) + " µs"); }
  if (command.substring(0, 7) == "set f e") {       follow_end_buffer   = command.substring(7).toInt();     Serial.println("Follow  end  buffer: " + String(follow_end_buffer  ) + " µs"); }

  if (command.substring(0, 7) == "set l f") {       lead_full_cycle_duration = command.substring(7).toInt();     Serial.println("Lead full cycle duration: " + String(lead_full_cycle_duration  ) + " µs"); }
  if (command.substring(0, 7) == "set l a") {       lead_LED_on   = command.substring(7).toInt();     Serial.println("Lead LED  ON  time point: " + String(lead_LED_on  ) + " µs"); }
  if (command.substring(0, 7) == "set l b") {       lead_LED_off  = command.substring(7).toInt();     Serial.println("Lead LED  OFF time point: " + String(lead_LED_off  ) + " µs"); }
  if (command.substring(0, 7) == "set l c") {       lead_TRIG_on  = command.substring(7).toInt();     Serial.println("Lead TRIG ON  time point: " + String(lead_TRIG_on  ) + " µs"); }
  if (command.substring(0, 7) == "set l d") {       lead_TRIG_off = command.substring(7).toInt();     Serial.println("Lead TRIG OFF time point: " + String(lead_TRIG_off  ) + " µs"); }
} // end setFromSerial

void chaseRun(String command) {
  if (command.substring(0, 3) != "run") { return; }
  setRainbow(OFF_LOG_VALUE);
  
  if (command.substring(0, 5) == "run f") {
    Serial.println("Starting FOLLOW protocol, input <stop> to stop.");

    setOe(0);
    tlc.setlog(chaseLEDselected, chaseLEDattValue);

    unsigned long update_length = 0; // time in microseconds to update TLC5948 settings, set dynamically after first loop
    unsigned long prev_trigFallingTime = 0; // time of the falling edge from the previous cycle
    //unsigned long LED_on_time_start = 0; // time the led starts turning on
    unsigned long LED_on_time_end = 0; // time the led finishes turning on
    unsigned long LED_off_time_start = 0; // time the led starts turning off
    unsigned long LED_off_time_end = 0; // time the led finishes turning on
    unsigned long TRIG_LOW_LED_OFF_duration = 0; // duration where trigger signal is low and the LED is off
    unsigned long TRIG_HIGH_LED_OFF_S_duration = 0; // duration where trigger signal is high and the LED is off (start buffer)
    unsigned long TRIG_HIGH_LED_ON_duration = 0; // duration where trigger signal is high and the LED is on
    unsigned long TRIG_HIGH_LED_OFF_E_duration = 0; // duration where trigger signal is high and the LED is off (end buffer)
    unsigned long flash_duration = 10; // duration of the flash in FOLLOW mode, set dynamically after first loop
    // target time for LED to turn on/off, set dynamically after first loop,
    unsigned long LED_on_target = micros() + follow_start_buffer;
    unsigned long LED_off_target = LED_on_target + flash_duration;

    enum LED_on_flag { BEFORE_ON = 1, AFTER_ON = 2} ;
    enum LED_off_flag { BEFORE_OFF = 1, AFTER_OFF = 2 };
    enum SerialOutput { BEFORE_SO = 1, AFTER_SO = 2 };
    LED_on_flag LED_on = BEFORE_ON;
    LED_off_flag LED_off = BEFORE_OFF;
    SerialOutput serialOutput = AFTER_SO;

    while (command.substring(0, 4) != "stop") {

      // Turn on the LED after the start buffer
      if (LED_on == BEFORE_ON && micros() >= LED_on_target) {
        //LED_on_time_start = micros(); // time the LED starts turning on
        setOe(1);   // turn on the LED after the start buffer
        LED_on_time_end = micros(); // time the LED finishes turning on
        LED_on = AFTER_ON; // update LED on flag
        // Serial.println("1");
      }

      // Turn off the LED after the start buffer
      if (LED_off == BEFORE_OFF && micros() >= LED_off_target) { 
        LED_off_time_start = micros(); // time the LED starts turning off
        setOe(0);   // turn on the LED after the start buffer
        LED_off_time_end = micros(); // time the LED finishes turning off
        LED_off = AFTER_OFF; // update LED off flag
        // Serial.println("2");
      }

      // Calculate timings, and reset flag for the next cycle
      // "trigFallingTime != prev_trigFallingTime" to ensure this only runs once per cycle
      if (trigState == TRIG_LOW && LED_off == AFTER_OFF 
        && serialOutput == AFTER_SO && trigFallingTime != prev_trigFallingTime) {
        // "prev_trigFallingTime != 0" to prevent calculation on first cycle
        if (trigRisingTime != 0 && prev_trigFallingTime != 0) {
          // calculate duration of trigger low, LED off
          TRIG_LOW_LED_OFF_duration = trigRisingTime - prev_trigFallingTime;
          // calculate duration of trigger high, LED off (start buffer)
          TRIG_HIGH_LED_OFF_S_duration = LED_on_time_end - trigRisingTime;
          // calculate duration of trigger high, LED off (start buffer)
          TRIG_HIGH_LED_ON_duration = LED_off_time_end - LED_on_time_end;
          // calculate duration of trigger high, LED off (end buffer)
          TRIG_HIGH_LED_OFF_E_duration = trigFallingTime - LED_off_time_end;
          // calculate the time difference for TLC5948 update
          update_length = LED_off_time_end - LED_off_time_start; 
          // calculate the flash duration for the next cycle using the time of the rising 
          // and falling edges and then  shortening the period by the follow buffers.
          flash_duration = trigFallingTime - trigRisingTime - follow_start_buffer - follow_end_buffer;
          if (flash_duration < 0) flash_duration = 0; // prevent negative flash durations
          // calculate the time the LED should turn on and off in the next cycle
          LED_on_target = trigFallingTime + TRIG_LOW_LED_OFF_duration + follow_start_buffer - update_length;
          LED_off_target = LED_on_target + flash_duration;
        }
         // store previous falling edge time
        prev_trigFallingTime = trigFallingTime;
        // reset flags for the next cycle
        LED_on = BEFORE_ON; // set LED on flag to before
        LED_off = BEFORE_OFF; // set LED off flag to before
        serialOutput = BEFORE_SO; // set serial output to before
        // Serial.println("3");
      }

      // Serial output of timings after each cycle
      if (trigState == TRIG_LOW && LED_on == BEFORE_ON && serialOutput == BEFORE_SO) {
        serialOutput = AFTER_SO; // reset the serial output to finished
        Serial.print("Trigger pin  low, ------- [µs]: ");
        Serial.println(TRIG_LOW_LED_OFF_duration);
        Serial.print("Trigger pin high, LED off [µs]: ");
        Serial.println(TRIG_HIGH_LED_OFF_S_duration);
        Serial.print("Trigger pin high, LED  on [µs]: ");
        Serial.println(TRIG_HIGH_LED_ON_duration);
        Serial.print("Trigger pin high, LED off [µs]: ");
        Serial.println(TRIG_HIGH_LED_OFF_E_duration);
        // Serial.print("rising time [µs]: ");
        // Serial.println(trigRisingTime);
        // Serial.print("falling time [µs]: ");
        // Serial.println(trigFallingTime);
        // Serial.print("LED on time [µs]: ");
        // Serial.println(LED_on_time_end);
        // Serial.print("LED off time [µs]: ");
        // Serial.println(LED_off_time_end);
        // Serial.print("flash duration [µs]: ");
        // Serial.println(flash_duration);
        // Serial.print("LED target on time [µs]: ");
        // Serial.println(LED_on_target);
        // Serial.print("LED target off time [µs]: ");
        // Serial.println(LED_off_target);
        // Serial.print("Time for LED stop command to execute [µs]: ");
        // Serial.println(update_length);
        // Serial.println("4");
      }
          
      if (Serial.available() > 0) {
        command = Serial.readStringUntil('*');
        setFromSerial(command);
      }
    }
    
    Serial.println("Stopped FOLLOW protocol");
    setOe(0); // turn off the LED
  }

  if (command.substring(0, 5) == "run l") {
    Serial.println("Starting LEAD protocol, input <stop> to stop.");

    setOe(0);
    tlc.setlog(chaseLEDselected, chaseLEDattValue);
    
    // Validate time points
    if (lead_LED_on > lead_full_cycle_duration) lead_LED_on = 0;
    if (lead_LED_off > lead_full_cycle_duration) lead_LED_off = lead_LED_on + 1;
    if (lead_TRIG_on > lead_full_cycle_duration) lead_TRIG_on = 0;
    if (lead_TRIG_off > lead_full_cycle_duration) lead_TRIG_off = lead_TRIG_on + 1;

    long cycleStartTime = micros();
    long currTime = 0;
    int chaseLeadFlags = 0b0000; // 4-bit flag: dcba (d=TRIGOUT OFF, c=TRIGOUT ON, b=LED OFF, a=LED ON)

    while (command.substring(0, 4) != "stop") {
      currTime = micros() - cycleStartTime;

      // Reset cycle if the current time exceeds the full cycle duration
      if (currTime >= lead_full_cycle_duration) {
          cycleStartTime = micros();
          currTime = 0;
          chaseLeadFlags = 0b0000; // Reset flags for the new cycle
      }

      // Perform operations based on the current time and flags
      if (currTime >= lead_LED_on && !(chaseLeadFlags & 0b0001)) {
          setOe(1); // Turn LED ON
          chaseLeadFlags |= 0b0001; // Set flag for LED ON
      }

      if (currTime >= lead_LED_off && !(chaseLeadFlags & 0b0010)) {
          setOe(0); // Turn LED OFF
          chaseLeadFlags |= 0b0010; // Set flag for LED OFF
      }

      if (currTime >= lead_TRIG_on && !(chaseLeadFlags & 0b0100)) {
          digitalWrite(TRIGOUTPIN, HIGH); // Turn TRIGOUT ON
          chaseLeadFlags |= 0b0100; // Set flag for TRIGOUT ON
      }

      if (currTime >= lead_TRIG_off && !(chaseLeadFlags & 0b1000)) {
          digitalWrite(TRIGOUTPIN, LOW); // Turn TRIGOUT OFF
          chaseLeadFlags |= 0b1000; // Set flag for TRIGOUT OFF
      }

      // Check for stop command
      if (Serial.available() > 0) {
          command = Serial.readStringUntil('*');
          setFromSerial(command);
      }
    }
  }
  setOe(0);
} // end chaseRun

void chaseEnvironment() {
  Serial.println(" _______  __   __  _______  _______  _______     _______  ______    _______  _______  _______  _______  _______  ___     ");
  Serial.println("|       ||  | |  ||   _   ||       ||       |   |       ||    _ |  |       ||       ||       ||       ||       ||   |    ");
  Serial.println("|       ||  |_|  ||  |_|  ||  _____||    ___|   |    _  ||   | ||  |   _   ||_     _||   _   ||       ||   _   ||   |    ");
  Serial.println("|       ||       ||       || |_____ |   |___    |   |_| ||   |_||_ |  | |  |  |   |  |  | |  ||       ||  | |  ||   |    ");
  Serial.println("|      _||       ||       ||_____  ||    ___|   |    ___||    __  ||  |_|  |  |   |  |  |_|  ||      _||  |_|  ||   |___ ");
  Serial.println("|     |_ |   _   ||   _   | _____| ||   |___    |   |    |   |  | ||       |  |   |  |       ||     |_ |       ||       |");
  Serial.println("|_______||__| |__||__| |__||_______||_______|   |___|    |___|  |_||_______|  |___|  |_______||_______||_______||_______|");
  Serial.println();
  Serial.println("  Input 'help' for instructions and 'exit' to exit the Chase protocol builder");
  
  // House cleaning
  command = "";
  change = false;
  setOe(0);
  setRainbow(OFF_LOG_VALUE); // update this name

  while (command != "exit") {
    if (Serial.available() > 0) {
      command = Serial.readStringUntil('*');
      change = true;
    }

    if (change) {
      change = false;
      
      chaseReport( command );
      chaseHelp( command );
      chaseSetFromSerial( command );

      chaseRun( command );
    }

    trigReceived = false;
  }
  Serial.println("Closed Chase protocol builder");
} // end
