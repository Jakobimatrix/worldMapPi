/**
* @file statusLED.cpp
* @brief Shows the status of the raspberry PI using on LED on GPIO27 on Pin 13 using WiringPI -> corresponding Pin 2. 
* If the program already runs and is called a second time, it recognizes it and kills the secont proram to avoid interference. 
* If the system sends SIGTERM it recognizes it and stops running as soon as possible.
* The following things are checked:
* Temperature below treshhold? -> //blinking fast and consistent _-_- //will shut down the system if T>secureShutdownTemp 
* Check wheather Apache is running -> //blinking slowly and consistent ___---___--- //tries to start apache 
* Check if online -> //LED on _______ //it will curse my Internetprovider
* Check if there is enough free space avaiable -> //blinking slow and inconsistent: ____--__--

* @date 11.09.2016
* @author Jakob Wandel
* @version 1.0
**/

extern "C" {
#include <wiringPi.h>//ansprechen der GPIO
}
#include <system/system.hpp>

#include <stdio.h>
#include <cstdlib> //exit()
#include <csignal>
#include <tr1/memory> //shared_ptr
#include <string.h>
#include <stdlib.h> // strtof 
#include <ctime> //time()

bool EXIT;
const std::string PROCESS_NAME = "statusLED";


void signalHandler( int signum)
{
    EXIT = true;   
}
void reportERROR(std::string ERROR, std::time_t date){

}
void tryingToFixProblemLED(){
        digitalWrite(2, 1);//an
        delay(40);
        digitalWrite(2, 0);//aus
        delay(40);
}
/*APACHE*/
bool isAppacheRunning(){
    //apatche is running if "pgrep apache" returns something else than ""
    if(exec("pgrep apache").compare("") == 0){
        return false;
    }
    return true;
}
void ApacheNotRunning(std::time_t endtime){
    std::time_t t = std::time(NULL);
    unsigned int attempt = 1;
    unsigned int maxAttempts = 5;
    unsigned int secondsBetweenAtempts = 10; 
    while(!isAppacheRunning() and std::time(NULL)<endtime and !EXIT){
        if(std::time(NULL)>t+attempt*secondsBetweenAtempts){
            attempt++;
            system("service apache2 start &");
        }
        if(attempt > maxAttempts){
            digitalWrite(2, 1);//an
            delay(2000);
            digitalWrite(2, 0);//aus
            delay(2000);
        }else{
            tryingToFixProblemLED();
        }
    }
}
bool shouldAppacheBeRunning(){
    //todo 
    return true;
}
/*APACHE
/*TEMPERATUR*/

float getTemp(){
    //"vcgencmd measure_temp" returns string like: temp=40.8'C
    std::string temp = exec("vcgencmd measure_temp");
    temp = temp.substr(5,temp.size());
    //temp = temp.substr(0,temp.size()-3);//-3 weil ' + C + NULLcharakter
    return std::strtof(temp.c_str(),NULL);
}
void TempToHeigh(int treshholdTemp, int suttdownTemp, std::time_t endtime){    
    while(getTemp()>treshholdTemp and std::time(NULL)<endtime and !EXIT){
        if(getTemp()>suttdownTemp){
            EXIT = true;
            std::system("shutdown -h now &");
        }
        digitalWrite(2, 1);//an
        delay(200);
        digitalWrite(2, 0);//aus
        delay(200);
    }
}
/*TEMPERATUR*/
/*ONLINESTATUS*/
bool isOnline(){
    std::string online = exec("ping -c 1 -W 1 8.8.8.8 >/dev/null 2>&1 && echo \"0\" || echo \"1\"");
    //retrun value: 0\n OR 1\n   = [connected,disconnected]
    if(!online.empty() && online[0]=='0'){
        return true;
    }
    return false;
}
void PiNotOnline(std::time_t endtime){
    while(!isOnline() and !EXIT and std::time(NULL)<endtime){
        digitalWrite(2, 1);//an
        delay(10000);
    }
}
/*ONLINESTATUS*/
/*Speicherstatus*/
bool isOnShortStorage(float treshholdStorage){
    /* df -h
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/root        28G  3.9G   22G  15% /<----SystemSpeicher
    devtmpfs        459M     0  459M   0% /dev
    tmpfs           463M     0  463M   0% /dev/shm
    tmpfs           463M  6.3M  457M   2% /run
    tmpfs           5.0M  4.0K  5.0M   1% /run/lock
    tmpfs           463M     0  463M   0% /sys/fs/cgroup
    /dev/mmcblk0p6   63M   20M   44M  32% /boot<----BootPartition
    tmpfs            93M     0   93M   0% /run/user/1000
    /dev/mmcblk0p5   30M  398K   28M   2% /media/pi/SETTINGS
    */
    //--------------------------TABLE |  the Use%column |looks for %|del found %| sorts      |cuts last|get the cutof from f1 (last cut)
    std::string storage = exec("df -h | awk '{print $5}' | grep % | grep -v Use | sort -n | tail -1 | cut -d \"%\" -f1 -");
    //Command returns the highst percentage under Use%
    float UsedStorage = std::strtof(storage.c_str(),NULL);
    if(UsedStorage > treshholdStorage){
        return true;
    }
    return false;
}
void PiOnShortStorage(std::time_t endtime){    
    while(std::time(NULL)<endtime and !EXIT){
        digitalWrite(2, 1);//an
        delay(2000);
        digitalWrite(2, 0);//aus
        delay(500);
        digitalWrite(2, 1);//an
        delay(1000);
        digitalWrite(2, 0);//aus
        delay(500);
    }
}
/*Speicherstatus*/
int main(){
    if(isProcessRunningMoreThanOnce(PROCESS_NAME)){
        return 0;
    }

    EXIT = false;

    signal(SIGTERM, signalHandler);  //interrupthandler in case the system decides to shut down

    unsigned int detectedErrors = 0;
    constexpr std::time_t maxExecutionTime = 290;//secondsDer Cronjob wird alle 5 Minuten ausgeführt... 10 sekunden pufferzeit
    const std::time_t starttime = std::time(NULL);
    const std::time_t endtime = starttime + maxExecutionTime;

    constexpr float treshholdTemp = 60;//C
    constexpr float secureShutdownTemp = 70;//C
    constexpr float treshholdStorage = 80;//%

    int wiringPIstatus = wiringPiSetup();//setuo GPIO!! must be done before comunication with GPIO
    if(wiringPIstatus == -1){
        return -1;
    }

    pinMode(2, OUTPUT);
    digitalWrite(2, 1);//an

    //blinking fast and consistent //will shut down the system if T>secureShutdownTemp _-_-
    if(treshholdTemp<getTemp()){        
        TempToHeigh(treshholdTemp, secureShutdownTemp, endtime);
        detectedErrors++;
    }
    //blinking slowly and consistent //tries to start apache if it should be ___---___---
    /*
    if(!isAppacheRunning()){            
        ApacheNotRunning(endtime);
        detectedErrors++;
    }
    */
    //LED on //it will curse my Internetprovider_______
    if(!isOnline()){                    
        PiNotOnline(endtime);
        detectedErrors++;
    }
    //blinking slow and inconsistent: ____--__--
    if(isOnShortStorage(treshholdStorage)){
        PiOnShortStorage(endtime);
        detectedErrors++;
    }

    digitalWrite(2, 0);//aus    
    return 0;
}

