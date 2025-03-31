/**
 * @file shutdownButton.cpp
 * @brief Provides a convenient shutdown button for Raspberry Pi.
 * 
 * GPIO22 (Pin 15) -> Button (WiringPi pin 3)
 * GPIO27 (Pin 13) -> LED (WiringPi pin 2)
 * 
 * Press & release within 3 sec -> Shutdown
 * Press & release within 3-6 sec -> Reboot
 * Press >6 sec -> Do nothing
 * 
* @date 31.03.2025
* @author Jakob Wandel
* @version 2.0
 */

 extern "C" {
    #include <wiringPi.h>
    }
    #include <cstdlib> // system()
    #include <csignal>
    #include <ctime> // time()
    #include <unistd.h> // sleep()
    
    // GPIO PINS
    constexpr int GPIOInterrupt = 3;
    constexpr int GPIOLED = 2;
    
    // Signal handler
    void signalHandler(int signum) {
        exit(0);
    }
    
    // Button press interrupt handler
    void myInterrupt() { 
        time_t startTime = std::time(nullptr);
        pinMode(GPIOLED, OUTPUT);
        digitalWrite(GPIOLED, HIGH);
        sleep(1);  // Prevent shutdown from voltage fluctuations
    
        if (digitalRead(GPIOInterrupt) == LOW) { // Button still pressed
            while (std::time(nullptr) < startTime + 6) {
                if (std::time(nullptr) < startTime + 3) {  // Shutdown condition
                    if (digitalRead(GPIOInterrupt) == HIGH) {  // Button released
                        digitalWrite(GPIOLED, LOW);
                        system("/sbin/shutdown -h now");  // Shutdown
                        exit(0);
                    } else {
                        digitalWrite(GPIOLED, HIGH);
                        delay(50);
                        digitalWrite(GPIOLED, LOW);
                        delay(50);
                    }
                } else {  // Reboot condition
                    if (digitalRead(GPIOInterrupt) == HIGH) {  // Button released
                        digitalWrite(GPIOLED, LOW);
                        system("/sbin/reboot");  // Reboot
                        exit(0);
                    } else {
                        digitalWrite(GPIOLED, HIGH);
                        delay(200);
                        digitalWrite(GPIOLED, LOW);
                        delay(200);
                    }
                }
            }
        }
        digitalWrite(GPIOLED, LOW);
    }
    
    int main() {
        // Handle SIGTERM
        signal(SIGTERM, signalHandler);
    
        if (wiringPiSetup() == -1) {
            return -1;
        }
    
        // Setup interrupt on falling edge
        wiringPiISR(GPIOInterrupt, INT_EDGE_FALLING, &myInterrupt);
    
        // Infinite loop to keep the service alive
        while (true) {
            sleep(1);
        }
    }
    