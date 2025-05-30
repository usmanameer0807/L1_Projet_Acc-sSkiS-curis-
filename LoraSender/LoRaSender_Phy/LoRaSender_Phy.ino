#include <SPI.h>
#include <LoRa.h>
#include "lcdgfx.h" 
#include <FastLED.h>
#include <Servo.h>

#define NUM_LEDS 21
#define DATA_PIN 4
#define PIN_BUZZER 7
#define NOTE_BIP_VALID 1000  // fréquence en Hz pour bip valide
#define NOTE_BIP_INVALID 200  // fréquence en Hz pour bip invalide
#define DUREE_BIP 100
#define PAUSE 100
#define BTN0 2
#define BTN1 3

Servo servo_9;  // Servo moteur
int pos = 0; // Position du servo moteur
DisplaySSD1306_128x64_I2C display(-1); // OLED
CRGB leds[NUM_LEDS];

int counter = 0;
int txPower = 14;
int spreadingFactor = 12;
long signalBandwidth = 125E3;
int codingRateDenominator=5;
int preambleLength=8;

#define SS 10
#define RST 8
#define DI0 6
#define BAND 865E6


//Fonction pour configurer  les périphériquse au démarraage
void setup() {
  
  // Initialisation du servo
  servo_9.attach(A2);  
  pinMode(BTN0, INPUT_PULLUP);
  pinMode(BTN1, INPUT_PULLUP);

  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  display.begin();
  display.fill(0x00);
  display.setFixedFont(ssd1306xled_font6x8);

  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa bidirectional");

  SPI.begin();
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  LoRa.setTxPower(txPower, 1);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(signalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
}

// Fonction pour changer la couleur des LEDs 
void setLEDColor(String color) {
  if (color == "red") {
    leds[15] = CRGB(0, 255, 0);  // VERT
    FastLED.show();
    delay(1000);                // Allumé pendant 1 seconde
    leds[15] = CRGB::Black;     // Puis s’éteint
    FastLED.show();
    
  } else if (color == "green") {
    leds[15] = CRGB(255, 0, 0);  // ROUGE
    FastLED.show();
    delay(1000);
    leds[15] = CRGB::Black;
    FastLED.show();

  } else if (color == "blue") {
    leds[15] = CRGB(0, 0, 255);  // BLEU
    FastLED.show();
    delay(1000);
    leds[15] = CRGB::Black;
    FastLED.show();

  } else if (color == "black") {
    leds[15] = CRGB::Black;      // ÉTEINT
    FastLED.show();
  }else if (color == "y") {
    leds[15] = CRGB(255,255 , 0);  // BLEU
    FastLED.show();
    delay(1000);
    leds[15] = CRGB::Black;
    FastLED.show();

  }
  else if (color == "c") {
    leds[15] = CRGB(255,127 , 80);  // BLEU
    FastLED.show();
    delay(1000);
    leds[15] = CRGB::Black;
    FastLED.show();

  }
}

// Fonction pour afficher un texte sur l’écran OLED
void lcdCode(String text) {
  display.clear();
  display.setFixedFont(ssd1306xled_font6x8);
  display.setColor(WHITE);
  display.printFixed(0, 32, text.c_str(), STYLE_BOLD);
}

// Fonction qui emet un son  : "valid" (bip rapide) ou "invalid" (bip d’erreur)
void emettreSon(String type) {
    if (type == "valid") {
        tone(PIN_BUZZER, NOTE_BIP_VALID, DUREE_BIP);
        delay(DUREE_BIP + PAUSE);
        tone(PIN_BUZZER, NOTE_BIP_VALID, DUREE_BIP);
        delay(DUREE_BIP + PAUSE);
        noTone(PIN_BUZZER);
    } else if (type == "invalid") {
       for (int i = 0; i < 2; i++) {
          tone(PIN_BUZZER, 800, 150);
          delay(200);
          tone(PIN_BUZZER, 400, 150);
          delay(200);
        }
noTone(PIN_BUZZER);

    }
}

// Fonction pour ouvrir la barrière (mouvement du servo)
void ouvrirBarriere() {
    for (pos = 0; pos <= 180; pos += 1) {
        servo_9.write(pos);
        delay(15);  // Attente pour que le servo bouge
    }
    for (pos = 180; pos >= 0; pos -= 1) {
        servo_9.write(pos);
        delay(15);  // Attente pour que le servo revienne à sa position initiale
    }
}

// Fonction pour envoyer les donée
void sendData(String payload) {
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();
  
  setLEDColor("blue");

  Serial.print("Sent: ");
  Serial.println(payload);
  counter++;
  delay(300);  // anti-rebond
}


//Fonction pour recevoire les donnée
String receiveData() {
  String received = "";
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received: ");
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      Serial.print(c);
      received += c;
    }
    Serial.println();

    if(received != "")
    {
      setLEDColor("Y");
    }
    
  }
  return received;
}
// Fonction pour saisir un identifiant à 4 chiffres à l’aide de 2 boutons
String saisieBouton() { 
  lcdCode("Saisie votre ID:");
  int count = 0;
  String id = "";
  bool btn0Prev = HIGH, btn1Prev = HIGH;

  while (count < 4) {
    int btn0Now = digitalRead(BTN0);
    int btn1Now = digitalRead(BTN1);

    if (btn0Now == HIGH && btn0Prev == LOW) {
      id += "1";  // BTN0 correspond au chiffre "1"
       lcdCode("1");
      count++;
    }
    if (btn1Now == HIGH && btn1Prev == LOW) {
      id += "2";  // BTN1 correspond au chiffre "2"
       lcdCode("2");
      count++;
    }

    btn0Prev = btn0Now;
    btn1Prev = btn1Now;

    delay(50);  // Anti-rebond
  }

  lcdCode("Id:"+id);

  return "9"+id;
}

// Fonction pour saisir un nom court à l’aide de 2 boutons (ex. "us", "man")
String saisieNom() { 
  lcdCode("Saisie votre nom :");
  int count = 0;
  String nom = "";
  bool btn0Prev = HIGH, btn1Prev = HIGH;

  while (count < 2) {
    int btn0Now = digitalRead(BTN0);
    int btn1Now = digitalRead(BTN1);

    if (btn0Now == HIGH && btn0Prev == LOW) {
      nom += "us";  // BTN0 correspond au chiffre "1"
       lcdCode(nom);
      count++;
    }
    if (btn1Now == HIGH && btn1Prev == LOW) 
    {
      nom += "man";  // BTN1 correspond au chiffre "2"
       lcdCode(nom);
      count++;
    }

    btn0Prev = btn0Now;
    btn1Prev = btn1Now;

    delay(50);  // Anti-rebond
  }


  return "0"+nom;
}

// Fonction pour saisir un seul chiffre avec les 2 boutons
String saisiNombre() { 
  int count = 0;
  String nb = "";
  bool btn0Prev = HIGH, btn1Prev = HIGH;

  while (count < 1) {
    int btn0Now = digitalRead(BTN0);
    int btn1Now = digitalRead(BTN1);

    if (btn0Now == HIGH && btn0Prev == LOW) {
      nb += "1";  // BTN0 correspond au chiffre "1"
       lcdCode("1");
      count++;
    }
    if (btn1Now == HIGH && btn1Prev == LOW) {
      nb += "2";  // BTN1 correspond au chiffre "2"
       lcdCode("2");
      count++;
    }

    btn0Prev = btn0Now;
    btn1Prev = btn1Now;

    delay(50);  // Anti-rebond
  }

  return nb;
}

void choix()
{
  int nbchoix = 0;
  String nb = "";
  lcdCode("1:identifier 2:inscrire" );

  nb = saisiNombre();

  if (nb == "1")
  {
    nbchoix = 1;
  }
  else if (nb == "2")
  {
    nbchoix = 2;
  }

    switch (nbchoix)
    {
        case 1:
        {
          String id = saisieBouton();
          sendData(id);
          delay(500); // anti-rebond

          String received = "";
          unsigned long startTime = millis();

          // Attente de la réponse max 3 secondes (ajuste selon besoin)
          while (received == "" && (millis() - startTime < 8000)) {
            received = receiveData();
          }

          lcdCode(received);
          //lcdCode("ce que j'ai recu"+received);
          delay(1000);

          if (received != "") {
            if (received == "ok") {
              setLEDColor("green");
              emettreSon("valid");
              ouvrirBarriere(); 
            } else if (received == "no") {
              setLEDColor("red");
              emettreSon("invalid");
            }else if (received[0] == '8') {
              lcdCode(received);
              setLEDColor("red");
              emettreSon("invalid");
            }
            else if (received[0] == '6') {
              lcdCode(received);
              setLEDColor("red");
              emettreSon("invalid");
            }  
            else 
            {
              setLEDColor("red");
              emettreSon("invalid");
            }
          } else {
            // Pas de réponse reçue dans le délai
            lcdCode(received);
            setLEDColor("red");
            emettreSon("invalid");
          }
          delay(1000); // Pause avant prochain choix
          break;
        }


      case 2:
        {
          String id = saisieNom();
          sendData(id);
          delay(500); // anti-rebond

          String received = "";
          unsigned long startTime = millis();

          // Attente de la réponse max 3 secondes (ajuste selon besoin)
          while (received == "" && (millis() - startTime < 6000)) {
            received = receiveData();
          }

          lcdCode(received);

          if (received != "") {
            if (received == "ok") {
              setLEDColor("green");
              emettreSon("valid");
              //ouvrirBarriere(); // Décommente si servo voulu
            } else if (received == "no") {
              setLEDColor("red");
              emettreSon("invalid");
            } else if (received[0] == '9') {
              lcdCode("votre ID: "+received);
              setLEDColor("green");
              emettreSon("valid");
            }  
            else {
              setLEDColor("red");
              emettreSon("invalid");
            }
          } else {
            // Pas de réponse reçue dans le délai
            lcdCode("Pas de reponse");
            setLEDColor("red");
            emettreSon("invalid");
          }
          delay(1000); // Pause avant prochain choix
          break;
        }


      default:
        lcdCode("Choix invalide");
        delay(1000);
        break;
    }

}

void loop() {
    
    choix();
}
