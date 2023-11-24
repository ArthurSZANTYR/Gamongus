#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

WiFiClient client;

const char* serverIP = "172.20.10.10"; // Remplacez par l'adresse IP du serveur ESP32
const int serverPort = 80; // Port du serveur

// Paramètres OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define OLED_RESET -1

// Création de l'affichage OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Paramètres de la clé
#define O_1 20
#define O_2 10
#define O_3 0
#define I_1 7
#define I_2 8
#define I_3 9
const byte ROWS = 3;
const byte COLS = 3;
byte rowPins[ROWS] = {O_1, O_2, O_3};
byte colPins[COLS] = {I_1, I_2, I_3};
char keys[ROWS][COLS] = {
  {'U', 'L', 'D'},
  {'R', 'S', 'A'},
  {'B', 'X', 'Y'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Positions et mouvements
int x = 3*SCREEN_WIDTH / 4 ;
int y = SCREEN_HEIGHT / 2;

int server_playerY = 0; // Position du serveur
int server_playerX = SCREEN_WIDTH / 4; // Position du serveur

int radius = 5;
int deltaX = 0;
int deltaY = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);


  WiFi.begin("iPhone de Arthur", "1jusqua8"); // Connectez-vous au même réseau WiFi que le serveur

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Connexion WiFi...");
    display.display();
  }

  Serial.println("Connecté au réseau WiFi");
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Connecte a ");
  display.println("iPhone de Arthur");
  display.display();

  delay(2000); // Afficher l'état pendant 2 secondes

  // Connexion au serveur
  Serial.println("Tentative de connexion au serveur...");
  int attempts = 0;
  while (!client.connect(serverIP, serverPort)) {
    Serial.println("Impossible de se connecter au serveur !");
    Serial.println("Nouvelle tentative dans 1 seconde...");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connexion au serveur...");
    display.display();

    delay(1000);
  }
    
  if (client.connected()) {
    Serial.println("Connecté au serveur !");
  } else {
    Serial.println("Échec de la connexion au serveur !");
  }

  
}

int num_paquet = 0;
void loop() {
   char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'U': deltaY = -1; deltaX = 0; break;
      case 'D': deltaY = 1; deltaX = 0; break;
      //case 'L': deltaX = -1; deltaY = 0; break;
      //case 'R': deltaX = 1; deltaY = 0; break;
      default: deltaX = 0; deltaY = 0; break;
    }
  }
  //x = constrain(x + deltaX, radius, SCREEN_WIDTH - radius);
  y = constrain(y + deltaY, radius, SCREEN_HEIGHT - radius);


  num_paquet+=1;
  if (client.connected()) {
    client.print(String(y) + " num paquet : "+ num_paquet + "\n");
    //while (client.available()) {
    //  int data = client.parseInt(); // Lire les données envoyées par le client
    //  Serial.println(data);
    while (client.available() > 0) {
      char receivedChar = client.read();
      Serial.print(receivedChar);
      }
    
  } else {
    Serial.println("Connexion perdue. Tentative de reconnexion...");
    client.connect(serverIP, serverPort);
  }

  updateDisplay();

  Serial.println("loop");
  delay(100);
}

void updateDisplay() {
  display.clearDisplay();
  display.drawLine(server_playerX - radius, server_playerY, server_playerX + radius, server_playerY, SSD1306_WHITE);
  display.drawLine(server_playerX, server_playerY - radius, server_playerX, server_playerY + radius, SSD1306_WHITE);
  display.drawCircle(x, y, radius, SSD1306_WHITE);
  display.display();
}
