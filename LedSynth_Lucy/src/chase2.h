// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE
// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE
// CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE CHASE

int chaseLEDselected = 2; // LED channel selection
float chaseLEDattValue = 1; // LED attenuation value [log] (0=max, 1=1/10, 2=1/100, ...)
long follow_start_buffer = 10; // buffer between TrigIN start to LED ON
long follow_end_buffer   = 10; // buffer between LED OFF and TrigIN end 

long lead_full_cycle_duration = 10000;
long lead_LED_on              =  2000;
long lead_LED_off             =  5000;
long lead_TRIG_on             =  2500;
long lead_TRIG_off            =  5500;


unsigned long rising_edge_time = 0; // time of the rising edge of the TrigIN signal
unsigned long falling_edge_time = 0; // time of the falling edge of the TrigIN signal
unsigned long flash_duration = 500; // duration of the flash in FOLLOW mode, set dynamically after first flash


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
  Serial.println("    set f s +X    follow mode start buffer [us] (buffer from TrigIN start to LED ON)");
  Serial.println("    set f e +X    follow mode  end buffer [us] (buffer from LED OFF to TrigIN end");
  Serial.println("    set l f +X    lead mode full cycle duration [us]");
  Serial.println("    set l a +X    lead mode   LED   ON  point [us]");
  Serial.println("    set l b +X    lead mode   LED   OFF point [us]");
  Serial.println("    set l c +X    lead mode TrigOUT ON  point [us]");
  Serial.println("    set l d +X    lead mode TrigOUT OFF point [us]");
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
  Serial.printf( "    LED ON  buffer  [us]: %d\n", follow_start_buffer);
  Serial.printf( "    LED OFF buffer [us]: %d\n", follow_end_buffer);
  Serial.println("  LEAD mode:");
  Serial.printf( "    Full cycle duration [us]: %d\n", lead_full_cycle_duration);
  Serial.printf( "      LED   ON   time   [us]: %d\n", lead_LED_on);
  Serial.printf( "      LED   OFF  time   [us]: %d\n", lead_LED_off);
  Serial.printf( "    TrigOUT ON   time   [us]: %d\n", lead_TRIG_on);
  Serial.printf( "    TrigOUT OFF  time   [us]: %d\n", lead_TRIG_off);
  Serial.println("-----------------------------------------------------------------------");
  Serial.println();
} // end protReport

void chaseSetFromSerial(String command) {
  if (command.substring(0, 3) != "set") { return; }
  
  
  if (command.substring(0, 5) == "set c") {    chaseLEDselected = constrain(command.substring(5).toInt(), 0, D_NLS);                  Serial.println("Channel: " + String(chaseLEDselected)); }
  if (command.substring(0, 5) == "set a") {    chaseLEDattValue = constrain(command.substring(5).toFloat(), 0.0, float(MAX_ATT_VALUE));        Serial.println("Attenuation: " + String(chaseLEDattValue) + " log"); }
  
  if (command.substring(0, 7) == "set f s") {       follow_start_buffer = command.substring(7).toInt();     Serial.println("Follow start buffer: " + String(follow_start_buffer) + " us"); }
  if (command.substring(0, 7) == "set f e") {       follow_end_buffer   = command.substring(7).toInt();     Serial.println("Follow  end  buffer: " + String(follow_end_buffer  ) + " us"); }

  if (command.substring(0, 7) == "set l f") {       lead_full_cycle_duration = command.substring(7).toInt();     Serial.println("Lead full cycle duration: " + String(lead_full_cycle_duration  ) + " us"); }
  if (command.substring(0, 7) == "set l a") {       lead_LED_on   = command.substring(7).toInt();     Serial.println("Lead LED  ON  time point: " + String(lead_LED_on  ) + " us"); }
  if (command.substring(0, 7) == "set l b") {       lead_LED_off  = command.substring(7).toInt();     Serial.println("Lead LED  OFF time point: " + String(lead_LED_off  ) + " us"); }
  if (command.substring(0, 7) == "set l c") {       lead_TRIG_on  = command.substring(7).toInt();     Serial.println("Lead TRIG ON  time point: " + String(lead_TRIG_on  ) + " us"); }
  if (command.substring(0, 7) == "set l d") {       lead_TRIG_off = command.substring(7).toInt();     Serial.println("Lead TRIG OFF time point: " + String(lead_TRIG_off  ) + " us"); }
} // end setFromSerial

void chaseRun(String command) {
  if (command.substring(0, 3) != "run") { return; }
  setRainbow(OFF_LOG_VALUE);
  
  if (command.substring(0, 5) == "run f") {
    Serial.println("Starting FOLLOW protocol, input <stop> to stop.");

    bool currTrigState = digitalRead(TRIGINPIN);
    setOe(0);
    tlc.setlog(chaseLEDselected, chaseLEDattValue);



    while (command.substring(0, 4) != "stop") {
      if (currTrigState != digitalRead(TRIGINPIN)) {

        currTrigState = !currTrigState; // toggle the state
        if (currTrigState) {
          rising_edge_time = micros(); // time of the rising edge
          Serial.print("Time of rising edge [us]: ");
          Serial.println(rising_edge_time);
          delayMicroseconds(follow_start_buffer);
          setOe(1);
          unsigned long now = micros();
          Serial.print("Time of led on [us]: ");
          Serial.println(now);
          Serial.print("flash duration [us]: ");
          Serial.println(flash_duration);
          delayMicroseconds(flash_duration);
          setOe(0);
          now = micros();
          Serial.print("Time of led off [us]: ");
          Serial.println(now);
        } else {
          falling_edge_time = micros(); // time of the falling edge
          Serial.print("Time of falling edge [us]: ");
          Serial.println(falling_edge_time);
          if(rising_edge_time != 0) {
            // calculate the flash duration for the next cycle using the time of the rising and falling edges
            // and shortening the period by the follow buffers. 500 µs is the starting value.
            flash_duration = falling_edge_time - rising_edge_time - follow_start_buffer - follow_end_buffer;
            if (flash_duration < 0) flash_duration = 0;
          } else {
            flash_duration = 500; // default value if the rising edge time is not set
          }
        }
      }
      
      if (Serial.available() > 0) {
        command = Serial.readStringUntil('*');
        setFromSerial(command);
      }
    }
    rising_edge_time = 0; // reset the rising edge time
    falling_edge_time = 0; // reset the falling edge time
    flash_duration = 500; // reset the flash duration to the default value
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
