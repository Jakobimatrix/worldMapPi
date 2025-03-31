/**
* @file LEDstripe.cpp
* @brief Programms GPIO 26 (PIN 37) (wPi 25) to transmit data to the led strip according to the choosen program.

* @date 30.03.2025
* @author Jakob Wandel
* @version 1.0
**/


extern "C" {
#include <wiringPi.h>//ansprechen der GPIO
}

#include <system/system.hpp>
#include <color/color.hpp>

#include <type_traits>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <csignal>


/**
 * @brief A tightly packed struct representing a 24-bit GRB value for WS2812B LEDs.
 */
struct [[gnu::packed]] GRB {
    uint8_t g = 0; ///< Green component (Most Significant Byte in WS2812B)
    uint8_t r = 0; ///< Red component
    uint8_t b = 0; ///< Blue component

    constexpr GRB(){}

    /**
     * @brief Constructor to initialize the GRB color.
     * @param red The red component.
     * @param green The green component.
     * @param blue The blue component.
     */
    constexpr GRB(uint8_t red, uint8_t green, uint8_t blue) 
        : g(green), r(red), b(blue) {}
    
    /**
     * @brief Constructor to initialize the GRB color.
     * @param color The Color as RGB<uint8_t>
     */
    constexpr GRB(const color::RGB<uint8_t>& color): g(color.g), r(color.r), b(color.b) {}
        
    template <class ColorType>
    constexpr GRB(const ColorType& color) : GRB(color::RGB<uint8_t>(color)){}
};
static_assert(sizeof(GRB) == 3, "GRB struct must be exactly 3 bytes");



bool EXIT = false;
const std::string PROCESS_NAME = "LEDstripe";
constexpr int PIN = 25;
constexpr size_t NUM_LEDS = 10;

std::array<GRB, NUM_LEDS> globalLEDdata;



void signalHandler(int signum)
{
    EXIT = true;   
}


/**
 * @brief Sets a GRB color at a given index to globalLEDdata.
 * 
 * @param index The index where the GRB value should be set.
 * @param color The color object from the `color` library.
 */
template <class ColorType>
void setLEDColorToIndex(size_t index, const ColorType& color) {
    if (index < NUM_LEDS) {
        globalLEDdata[index] = GRB(color);
    }
}

/**
 * @brief Sets a GRB color to all indices of globalLEDdata.
 * 
 * @param color The color object from the `color` library.
 */
void setLEDColorToAll(const color::RGB<uint8_t>& color){
    for(size_t i = 0; i < NUM_LEDS; i = i + 2){
        globalLEDdata[i] = GRB(color);
    }
}

/**
 * @brief Delays execution for a given number of nanoseconds.
 * @param ns The number of nanoseconds to delay.
 */
inline void delayNanos(int ns) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now() - start)
               .count() < ns) {
        // Busy-waiting until the required time has elapsed
    }
}

inline void resetAllLEDs() {
    digitalWrite(PIN, 0);
    std::this_thread::sleep_for(std::chrono::microseconds(300));
    digitalWrite(PIN, 1);
}

/**
 * @brief Sends a single bit to the LED strip.
 * @param bit The bit to send (0 or 1).
 */
inline void sendBit(bool bit) {
    // https://www.mikrocontroller.net/articles/WS2812_Ansteuerung
    // https://cpldcpu.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/
    if (bit) {
        // Send '1'
        digitalWrite(PIN, 1);
        delayNanos(9);
        digitalWrite(PIN, 0);
        delayNanos(350);
    } else {
        // Send '0'
        digitalWrite(PIN, 1);
        delayNanos(350);
        digitalWrite(PIN, 0);
        delayNanos(9);
    }
}

/**
 * @brief Sends one full LED's GRB data.
 * @param led The GRB struct containing the color.
 */
void sendLED(const GRB& led) {
    for (int8_t i = 7; i >= 0; --i) sendBit(led.g & (1 << i)); // Send GREEN first
    for (int8_t i = 7; i >= 0; --i) sendBit(led.r & (1 << i)); // Send RED
    for (int8_t i = 7; i >= 0; --i) sendBit(led.b & (1 << i)); // Send BLUE
}

/**
 * @brief Writes the LED data to the WS2812B strip.
 * 
 * This function sends all LED colors stored in `globalLEDdata` to the LED strip
 * using precise bit-banging.
 */
void writeToLEDStrip() {
    pinMode(PIN, OUTPUT);
    resetAllLEDs();
    for (const auto& led : globalLEDdata) {
        sendLED(led);
    }
}


void programRGB(){
    color::RGB<uint8_t> current_color(0, 0, 0);
    const color::RGB<uint8_t> black(0, 0, 0);
    while(42){
        if(EXIT){
	    resetAllLEDs();
            return;   
        }
        setLEDColorToAll(current_color);
        color::RGB<uint8_t> next_color(current_color.g, current_color.b, current_color.r);
        current_color = next_color;
        writeToLEDStrip();
        sleep(5);
        if(EXIT){
            return;   
        }
        setLEDColorToAll(current_color);
        next_color = color::RGB<uint8_t>(current_color.g, current_color.b, current_color.r);
        current_color = next_color;
        writeToLEDStrip();
        sleep(5);
        if(EXIT){
            return;   
        }
        setLEDColorToAll(current_color);
        next_color = color::RGB<uint8_t>(current_color.g, current_color.b, current_color.r);
        current_color = next_color;
        writeToLEDStrip();
        sleep(5);
        if(EXIT){
            return;   
        }
        setLEDColorToAll(black);
        writeToLEDStrip();
        sleep(1);
    }
}


int main(){
    if(isProcessRunningMoreThanOnce(PROCESS_NAME)){
        return 0;
    }

    EXIT = false;
    
    const int wiringPIstatus = wiringPiSetup();//setuo GPIO!! must be done before comunication with GPIO
    if(wiringPIstatus == -1){
        return -1;
    }

    //interrupthandler in case the system decides to shut down
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);    
    programRGB();
    
    return 0;
}
