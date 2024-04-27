/*---------------- Include du cours  ----------------*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/*---------------- Include ajouté  ----------------*/
#include <ToneESP32.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <map>
#include <string>

#include "song.h" //notre fichier song.h

const char* nom_Wifi = "PC-POIPOU"; //Le nom de votre wifi
const char* mdp_Wifi = "Trou2balle"; //Le mot de passe de votre wifi

const char* mqttServer = "192.168.137.133"; //A MODIFIER SELON LIP DU BROKER MQTT qui se trouve sur la raspberry
const int mqttPort = 1883;
const char* mqttSubTopic = "ESP32"; //A changer selon le topic d'ecoute de la raspberry

const char* mqttPubTopic = "Pi"; //A changer selon le topic d'ecoute de votre site web.
unsigned long tempsDebut;

String name = "Marie"; // NOM DE L'ESP32, Nommez le comme vous voulez, il peut y en avoir des identiques c'est pas grave



/*---------------- Define  ----------------*/
#define MOTION_PIN 13
#define BUTTON_PIN 23
#define PR_PIN A0
//LE PIN DU BUZZER DOIT ETRE MODIFIER DANS SONG.h


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    16 // Reset pin # (or -1 if sharing Arduino reset pin)


#define MAX_WAIT_FOR_TIMER 3
unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int motionStatus = 0;
int buttonStatus = 0;
String message = "{";       // NE PAS MODIFIER CETTE LIGNE

String macAddress = WiFi.macAddress();

const char* mqttClientId = macAddress.c_str();

std::vector<std::string> melody;

WiFiClient espClient;
PubSubClient client(espClient);

/*---------------- Structure  ----------------*/
enum {EMPTY, FULL}; 

struct mailbox_s {
  int state; 
  int value;
};

struct mailbox_s mb_msg = {.state = EMPTY};
struct mailbox_s mb_music = {.state = EMPTY};
struct mailbox_s mb_oled = {.state = FULL};

struct melody_s {
  int cpt; 
  String curr_name;
  String prev_name;
  int size;
  std::string text;
  int note;
  float duree;
  float delais;
  int song_pp;
  int redo;
  int progress;
  int random;
};

struct melody_s music = {.cpt = -3, .prev_name = "empty"};

struct Lum {                              
  int pin;                                                
  int value;
  int timer;
};

struct Lum Photores;
struct Lum bp_s;

struct oled_s {                              
  int wifi;
  int mqtt;
  int button;
  int photores;
  String music_name;
  int music_play_pause;

};

struct oled_s struct_oled = {.wifi = 0};

/*---------------- Detecteur de mouvement ----------------*/
void motion_on(){
    motionStatus = !motionStatus;
}

void setup_motion(){
  pinMode(MOTION_PIN, INPUT);
  Serial.println("Configuration du detecteur de mouvement . . .");
  while(digitalRead(MOTION_PIN) != 0){
    Serial.println("Veuillez mettre le capteur dans un endroit sans mouvement pendant 10 secondes.");
    delay(1000);
  }
  Serial.println("Detecteur de mouvement congifurer.");
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN), motion_on, CHANGE);
}
/*---------------- Photoresistance ----------------*/
void setup_photores( struct Lum* ctx, byte pin) {
  ctx->pin    = pin;
  ctx->value  = 0;
  pinMode(pin,INPUT);
}

void loop_photores(struct Lum* ctx) {
  if (mb_msg.state != EMPTY) return;
  ctx->value = analogRead(ctx->pin);
  mb_msg.value = map(ctx->value, 0, 4096, 100, 0);
  mb_msg.state = FULL;

  if(waitFor(0, 10000000) >= 1){
    mb_oled.state= FULL;
    struct_oled.photores = mb_msg.value;
  }

}
/*---------------- Bouton poussoir ----------------*/
void bp(){
  if(waitFor(1, 1000000) >= 1){
    buttonStatus = !buttonStatus;
    struct_oled.button = buttonStatus;
    mb_oled.state = FULL;
    return;
  }
}
void setup_bp(){
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), bp, HIGH);
}

/*---------------- OLED ----------------*/
void setup_oled() {
  //
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.display();
  delay(2000);

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(F("Starting"));
  
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  delay(2000);
}

void oledPrint(const char* msg) {

  display.fillRect(0, 32, SCREEN_WIDTH, 32, BLACK);
  display.setCursor(0,32);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(msg);

  display.display();
}

void loop_oled(struct oled_s *oled, struct mailbox_s *mb){
  if(mb->state != FULL) return;
  mb->state = EMPTY;
  display.fillRect(0, 0, SCREEN_WIDTH, 32, BLACK); //j'efface les 4 premieres lignes
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  //Ligne 1
  display.print(F("Wifi:"));
  if(oled->wifi == 1){
    display.print(F("on"));
  } else display.print(F("off"));
  display.setCursor(63, 0);
  display.print(F("MQTT:"));
  if(oled->mqtt == 1){
    display.print(F("on"));
  } else display.print(F("off"));

  //Ligne 2
  display.setCursor(0, 8);
  display.print(F("btn:"));
  if(oled->button == 1){
    display.print(F("on"));
  }else display.print(F("off"));
  display.setCursor(63, 8);
  display.print(F("mouv:"));
  display.print(F("off"));

  //Ligne 3
  display.setCursor(0, 16);
  display.print(F("Luminosite:"));
  if(oled->photores == NULL){
    display.print(F("off"));
  }else{
    display.print(oled->photores);
    display.print(F("%"));
  }
  
  //Ligne 4
  display.setCursor(0, 24);
  display.print(F("song:"));
  if(oled->music_name == "null" || oled->music_name == ""){
    display.print("none");
  }else{
    display.print(oled->music_name);
  }
  

  display.display();
}
/*---------------- WIFI ----------------*/
void setup_wifi(){
  WiFi.begin(nom_Wifi, mdp_Wifi);
  while (WiFi.status() != WL_CONNECTED){
    if(mb_oled.state != FULL) return;
    mb_oled.state = FULL;
    struct_oled.wifi = 0;
    delay(1000);
    Serial.println("Connexion en cours au WiFi...");
  }
  Serial.println("Connecté au WiFi.");
  struct_oled.wifi = 1;
}

/*---------------- MQTT ----------------*/
void setup_mqtt(){
  client.setServer(mqttServer, mqttPort);
  client.setSocketTimeout(10000);
  while (!client.connected()) {
    Serial.println("Connexion au serveur MQTT...");
    struct_oled.mqtt = 0;
    if (client.connect(mqttClientId)) {
      Serial.println("Connecté au serveur MQTT !");
      client.subscribe(mqttSubTopic);
      struct_oled.mqtt = 1;
    } else {
      Serial.print("Échec de la connexion au serveur MQTT. Code d'erreur = ");
      Serial.println(client.state());
      delay(4000);
    }
  }
}

void loop_mqtt() {
  while (!client.connected()) {
    if(mb_oled.state != FULL) return;
    mb_oled.state = FULL;
    struct_oled.mqtt = 0;
    Serial.print("Tentative de reconnexion au serveur MQTT...");
    setup_mqtt();
    if (client.connect(mqttClientId)) {
      Serial.println("Connecté au serveur MQTT !");
      client.subscribe(mqttSubTopic); // Abonne aux topics MQTT
      struct_oled.mqtt = 1;
    } else {
      Serial.print("Échec de la reconnexion. Code d'erreur = ");
      Serial.println(client.state());
      
    }
    delay(5000);
  }
  return;
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur le topic : ");
  Serial.println(topic);

  Serial.print("Contenu du message : ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Convertir le tableau de bytes en une chaîne de caractères
  char message[length + 1];
  strncpy(message, (char*)payload, length);
  message[length] = '\0';

  // Créer un objet JSON
  StaticJsonDocument<200> jsonDocument;

  // Désérialiser le message JSON
  DeserializationError error = deserializeJson(jsonDocument, message);

  if (error) {
    Serial.print("Erreur lors de la désérialisation JSON : ");
    Serial.println(error.c_str());
    return;
  }

  if (measureJson(jsonDocument["data"]) <= 4) return;
  // Extraire les données spécifiques
  
  const char* messageMacAddress = jsonDocument["mac_address"];

  String curr_name = jsonDocument["data"]["name"];
  const char* song_pp = jsonDocument["data"]["playpause"];
  String choosen_song = jsonDocument["data"]["song"];
  const char* oled = jsonDocument["data"]["oled"];
  const char* random = jsonDocument["data"]["random"];
  const char* redo = jsonDocument["data"]["redo"];
  
  Serial.print("song_pp = ");
  Serial.println(song_pp);
  Serial.print("curr_name = ");
  Serial.println(curr_name.length());
  const char* song = choosen_song.c_str();


  if(strcmp(messageMacAddress, macAddress.c_str()) ==0){
    if(strlen(oled) != 0){
      oledPrint(oled);
      return;
    }

    if(std::atoi(random) == 0){
      music.random = 0;
    }else{
      music.random = 1;
    } 
    if(atoi(redo) == 0 && strlen(redo) > 0){
      music.redo = 0;
      Serial.println("0");
    }else if(atoi(redo) == 1 && strlen(redo) > 0) {
      music.redo = 1;
      Serial.println("1");
      return;
    }

    if(atoi(song_pp) == 0 && strlen(song_pp) > 0){
      Serial.println("je rentre la du coup 1");
      music.song_pp = 0;
    }else if (atoi(song_pp) == 1){
      Serial.println("je rentre la du coup 4");
      if(curr_name.length() == 0 && mb_music.state == FULL){
        music.song_pp = 1;
        Serial.println("je rentre la du coup 2");
      }else if(strlen(song) > 0 ){
        if ((music.prev_name != curr_name || mb_music.state == EMPTY) && curr_name != "null" ){ //
          //Serial.println("je rentre la du coup 3");
          mb_music.state = FULL;
          melody.clear();
          char* token = strtok((char*)song, ",");
          music.cpt = -3;
          
          while (token != NULL) {
            //Serial.println(token);
            melody.push_back(token);
            token = strtok(NULL, ",");
          }
          if(melody.size() <= 1) return;
          music.size = melody.size();
          music.prev_name = curr_name;
          struct_oled.music_name = curr_name;
          mb_oled.state = FULL;
          tempsDebut = millis();
        }
      }else return;
      music.song_pp = 1;
    }
  }
}

unsigned long startTime;
unsigned long elapsedTime;
const unsigned long timerInterval = 1000; // 10 secondes

/*---------------- music ----------------*/
void play_song_2(struct melody_s *melo,struct mailbox_s *mb){
  if (melo->song_pp == 0) return;
  if (mb->state != FULL) return;
  if (melo->cpt >= (melo->size) - 3){
    if(melo->redo){
      melo->cpt = -3;
      return;
    }
    melody.clear();
    mb->state = EMPTY;
    melo->cpt = -3;
    unsigned long tempsEcoule = millis() - tempsDebut;
    Serial.print("Temps écoulé : ");
    Serial.print(tempsEcoule);
    Serial.println(" ms");
    return;
  }else{
    melo->cpt += 3;
    melo->progress = round((melo->cpt * 100)/(melo->size - 3));
    String toWebMessage = "{";
    toWebMessage += "\"mac_address\": \"" + macAddress + "\",";
    toWebMessage += "\"progress\": \"" + String(melo->progress) + "\"";
    toWebMessage += "}";
    client.publish("web", toWebMessage.c_str());
    melo->text = melody[melo->cpt]; //Je recupere la note qui est en texte
    melo->note = notesMap[melo->text]; //je la fais correspondre avec le define

    buzzer.tone(melo->note, std::stoi(melody[melo->cpt + 1]));
    delay(std::stoi(melody[melo->cpt + 2]));
    
  }
}

void loop_msg(){
  if(mb_msg.state != FULL) return;
  if(elapsedTime <= timerInterval) return;
  startTime = millis();

  message += "\"name\": \"" + name + "\", ";
  message += "\"mac_address\": \"" + macAddress + "\", ";
  message += "\"data\": {\"luminosity\": " + String(mb_msg.value) + "";
  message += ", \"button\": " + String(buttonStatus) + "";
  message += ", \"motion\": " + String(motionStatus) + "";
  message += "}}";

  if (buttonStatus == 1) {
    buttonStatus = 0;
    mb_oled.state = FULL;
    struct_oled.button = 0;
  }

  mb_msg.state = EMPTY;
  client.publish("Pi", message.c_str());
  message = "{"; //pour vider le message
}
/*---------------- main ----------------*/

void setup() {
  Serial.begin(9600);
  Wire.begin(4, 15);
  setup_wifi();
  setup_mqtt();
  client.setBufferSize(16384);
  setup_oled();
  setup_photores(&Photores, PR_PIN);
  //setup_motion(); // A Activer si on souhaite testé le detecteur de mouvement.
  setup_bp();
  client.setCallback(callback);
  startTime = millis();
}

void loop() {
  loop_oled(&struct_oled, &mb_oled);
  loop_msg();
  loop_photores(&Photores);
  client.loop();
  loop_mqtt();
  play_song_2(&music, &mb_music);
  elapsedTime = millis() - startTime;
}
