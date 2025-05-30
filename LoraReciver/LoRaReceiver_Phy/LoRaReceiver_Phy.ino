#include <SPI.h>
#include <LoRa.h>
#include "lcdgfx.h" 
#include <FastLED.h>
#include <Arduino.h> 

#define NUM_LEDS 21
#define DATA_PIN 4
#define BTN0 2

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

void setup() {
  pinMode(BTN0, INPUT_PULLUP);

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
void lcdCode(String text) {
  display.clear();
  display.setFixedFont(ssd1306xled_font6x8);
  display.setColor(WHITE);
  display.printFixed(0, 32, text.c_str(), STYLE_BOLD);
}

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
    setLEDColor("y");
  }
  return received;
}

const int nb_tab = 4;

String IDSkieur[] = {
  "2121", "2112", "1122", "2212",
  "1211", "2212", "1221", "2221",
  "2111", "1121", "1112", "2122",
  "2211", "1212", "1222", "2222"
};

String idAttribuerAvecnom[nb_tab];
String nomSkieur[nb_tab];

String verifID(String received1, String tab[], int taille,String nb_Protocol) 
{
  String protocol="";
  for (int i = 0; i < taille; i++) {
    String ligne = tab[i];
    int separateur = ligne.indexOf(':');
    
    if (separateur != -1) {
      String id = ligne.substring(separateur + 1); // récupère l'ID (avec protocole)
      protocol=nb_Protocol+id;
      
      if (protocol == received1) {  // compare directement avec received1 complet
        lcdCode("test 1 :"+id);
        return nb_Protocol+id; // retourne l'ID trouvé
      }
    }
  }
  return ""; 
}
void reinitialiserTableaux() 
{
  for (int i = 0; i < nb_tab; i++) 
  {
    nomSkieur[i] = "";
    idAttribuerAvecnom[i] = "";
  }
}

void sauvGardeClient(String nom) 
{
  // Vérifie si le nom est déjà présent → on ne le rajoute pas
  for (int i = 0; i < nb_tab; i++) 
  {
    if (nomSkieur[i] == nom) {
      return;
    }
  }

  // Ajoute le nom dans la première case vide
  for (int i = 0; i < nb_tab; i++) {
    if (nomSkieur[i] == "") {
      nomSkieur[i] = nom;
      break;
    }
  }

  // Vérifie si le tableau nomSkieur[] est plein
  bool nomPlein = true;
  for (int i = 0; i < nb_tab; i++) {
    if (nomSkieur[i] == "") {
      nomPlein = false;
      break;
    }
  }

  // Vérifie si le tableau idAttribuerAvecnom[] est plein
  bool idPlein = true;
  for (int i = 0; i < nb_tab; i++) {
    if (idAttribuerAvecnom[i] == "") {
      idPlein = false;
      break;
    }
  }

  //  réinitialiser tabeau
  if (nomPlein || idPlein) {
    lcdCode("Tableaux pleins : reset");
    delay(2000);
    reinitialiserTableaux();
  }
}

void etat_nomSkieur() {
  for (int i = 0; i < nb_tab; i++) {
    lcdCode("nom " + String(i) + ":" + nomSkieur[i]);
    delay(1000);
  }
}

void etat_idAttribuerAvecnom() {
  for (int i = 0; i < nb_tab; i++) {
    lcdCode("ID_attribuer: " + String(i) + ":" + idAttribuerAvecnom[i]);
    delay(1000);
  }
}

String getRandomID() {
  int size = sizeof(IDSkieur) / sizeof(IDSkieur[0]);
  int index = random(size);
  return IDSkieur[index];
}

String attributionID(String nom2) {
  int tailleNom = nb_tab;

  // Vérifie si un ID a déjà été attribué à ce nom
  for (int i = 0; i < tailleNom; i++) 
  {
    if (idAttribuerAvecnom[i].startsWith(nom2 + ":")) 
    {
      String idExistant = idAttribuerAvecnom[i].substring(nom2.length() + 1);
      lcdCode("Deja attribue: " + idExistant);
      delay(2000);
      return idExistant;
    }
  }

  // Essaye de générer un ID aléatoire non encore attribué
  for (int essais = 0; essais < 100; essais++) {
    String randomID = getRandomID();

    bool dejaAttribue = false;
    for (int j = 0; j < tailleNom; j++) {
      if (idAttribuerAvecnom[j].endsWith(":" + randomID)) {
        dejaAttribue = true;
        break;
      }
    }

    if (!dejaAttribue) {
      // Trouve la position du nom pour attribuer l'ID
      for (int k = 0; k < tailleNom; k++) {
        if (nomSkieur[k] == nom2) {
          idAttribuerAvecnom[k] = nom2 + ":" + randomID;
          lcdCode("Attribue: " + randomID);
          delay(2000);
          return randomID;
        }
      }
    }
  }

  return ""; // Aucun ID libre trouvé après plusieurs essais
}


int compteurUtilisationID[10] = {0}; // compteur pour chaque ID
bool verifierUtilisationID(String receivedID) {
  for (int i = 0; i < nb_tab; i++) {
    if (idAttribuerAvecnom[i].endsWith(":" + receivedID)) 
    {
      compteurUtilisationID[i]++;

      if (compteurUtilisationID[i] > 2) {
      
        // L’ID a été utilisé 2 fois → on le supprime
        lcdCode("ID expire");
        sendData("6IDexpire");

        nomSkieur[i] = "";
        idAttribuerAvecnom[i] = "";
        compteurUtilisationID[i] = 0;

        delay(2000);
        lcdCode(""); // Nettoyer l'écran

        return true;
      }
      return false;
      break;
    }
  }
}


void loop() {
  bool verif;
      // Lecture du bouton pour envoyer un message
      if (digitalRead(BTN0) == LOW) {
        sendData("7autorisation");
        delay(500); // anti-rebond + éviter trop d’envoi
      }

      // Lecture des messages reçus
      String received = receiveData();
      String text = verifID(received, idAttribuerAvecnom, int(sizeof(idAttribuerAvecnom)), "9");

      
      if (received != "") 
      {
        lcdCode(received);
        delay(1000);

        // Exemple d’action selon message
        if (received == "2222") 
        {
          setLEDColor("green");
          sendData("ok");  // réponse
        } 
        else if (received == "1111") 
        {
          setLEDColor("red");
          sendData("no");
        }
        else if(received== text)
        {
          String idSeulement = text.substring(1); // enlève le '9'
          verif=verifierUtilisationID(idSeulement);
          if(verif==true)
          {
            setLEDColor("red");
            sendData("6IdExpir");
          }else if(verif==false)
          {
            lcdCode("test2:" + text);
            setLEDColor("green");
            sendData("ok");  // réponse
          }
            

          /*String idSeulement = text.substring(1); // enlève le '9'
          verif=verifierUtilisationID(idSeulement);*/

        }
        else if(received[0]== '0')
        {
          String id="";
          sauvGardeClient(received);
          id=attributionID(received);
         
          //etat_nomSkieur();
          lcdCode("moi:"+received+" id:"+id);
         
          setLEDColor("green");
          sendData("9"+id);  // réponse
        }
        else if(received[0]== '1')
        {
          String nom=nomSkieur[0];
          setLEDColor("c");
          sendData("ok");  // réponse
        }
        else
        {
          setLEDColor("red");
          sendData("no");
        }
        
        delay(1000);
        lcdCode("");
      }
        
}
