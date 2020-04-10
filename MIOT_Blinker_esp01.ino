
#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
//#define BLINKER_ESP_SMARTCONFIG         // 进行配网

#include <Blinker.h>

char auth[] = "";
char ssid[] = "";
char pswd[] = "";


#define POWER_PIN 0
#define SWITCH_PIN 2
#define BUTTON_POWER "btn-power"


BlinkerButton btn_power(BUTTON_POWER);

bool oState = false;
bool sState = false;

  
void manualSwitchCheck(){
  if (sState != (digitalRead(SWITCH_PIN)==LOW)) { 
      Blinker.delay(150);  //延时150ms不能太少标准按键成功率
      if (sState != (digitalRead(SWITCH_PIN)==LOW)){  
        sState = (digitalRead(SWITCH_PIN)==LOW);    
        oState = !oState;
        BLINKER_LOG("local switch triggering...oState:",oState);
        bool curPowerState = digitalRead(POWER_PIN);                  
        digitalWrite(POWER_PIN,!curPowerState);
        btn_power.color(oState?"#00FF00":"#CCCCCC");
        btn_power.print(oState? "on":"off");
        }
   }
}
void btn_power_callback(const String &state)
{
  BLINKER_LOG("btn_power_callback: ", state);
  BLINKER_LOG("LED_BUILTIN pin...: ", LED_BUILTIN);
  if(state == "on" || state == "off" ){
    
    oState = state == BLINKER_CMD_ON;
    digitalWrite(POWER_PIN, !oState);
    BLINKER_LOG("get btn_power delayed...: ", state);
    btn_power.color(oState?"#00FF00":"#CCCCCC");
    btn_power.print(state);
    
  }
}

void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(POWER_PIN, LOW);
        BlinkerMIOT.powerState("on");
        btn_power.color(oState?"#00FF00":"#CCCCCC");
        BlinkerMIOT.print();

        oState = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(POWER_PIN, HIGH);
        BlinkerMIOT.powerState("off");
        btn_power.color(oState?"#00FF00":"#CCCCCC");
        BlinkerMIOT.print();

        oState = false;
    }
}

void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
    }
}

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}
 
void heartbeat() //心跳包
{
    BLINKER_LOG("heartbeating...");
//    sldr_1p.print(Delay_values[0]);
//    sldr_2p.print(Delay_values[1]);
//    sldr_auto_off.print(Delay_values[2]);
    bool b_power_state = digitalRead(POWER_PIN);
    btn_power.color(b_power_state? "#00FF00":"#CCCCCC");
    btn_power.print(b_power_state? "on":"off");
}

void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    pinMode(POWER_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(POWER_PIN, HIGH);
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    sState = (digitalRead(SWITCH_PIN)==LOW);
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachQuery(miotQuery);

    btn_power.attach(btn_power_callback);
    btn_power.color("#CCCCCC");
    
}

void loop()
{
    Blinker.run();
    manualSwitchCheck();
}
