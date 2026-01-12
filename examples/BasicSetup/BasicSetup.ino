#include <PSci.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Initialize P-Sci library
    psci.setup();
}

void loop() {
    // Run P-Sci in the main loop
    psci.run();
    
    // Your application code here
    delay(10);
}
