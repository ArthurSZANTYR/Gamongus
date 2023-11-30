#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <SPI.h>
// Configuration de la connexion au serveur
const char* serverIP = "172.20.10.11"; // Adresse IP du serveur ESP32

const char *ssid = "Your_SSID";
const char *password = "Your_Password";
WiFiUDP udp;
unsigned int localPort = 9999;

// Paramètres OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define OLED_RESET -1
// Création de l'affichage OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drawCourt();
// Paramètres du clavier
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

int ScorePlayer1 = 0;
int ScorePlayer2 = 0;

// Positions et mouvements des joueurs et de la balle
int client_player2_X = 3 * SCREEN_WIDTH / 4;
int client_player2_Y = SCREEN_HEIGHT / 2;

int server_player1_Y = SCREEN_HEIGHT / 2; // Position du joueur serveur
int server_player1_X = SCREEN_WIDTH / 4; // Position du joueur serveur

int paddle_size = 10;
int deltaX = 0;
int deltaY = 0;
uint8_t vitesse_delta = 8;

uint8_t ball_x = 64, ball_y = 32;

// Initialisation
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  // Connexion au réseau WiFi
  connectToWiFi();
}

// Boucle principale
char memKey = '_';
void loop() {
  char key = keypad.getKey();
  processKeypadInput(key);
  communicateWithServer();
  updateDisplay();
  delay(40);
}

// Connexion au réseau WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Connexion WiFi...");
    display.display();
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Connecte a ");
  display.println(ssid);
  display.display();
  delay(2000);
}


// Traitement des entrées du clavier
void processKeypadInput(char key) {
  if (key) {
    switch (key) {
      case 'U': deltaY = -1 * vitesse_delta; deltaX = 0; break;
      case 'D': deltaY = 1 * vitesse_delta; deltaX = 0; break;
      default: deltaX = 0; deltaY = 0; break;
    }
    client_player2_Y = constrain(client_player2_Y + deltaY, 1, SCREEN_HEIGHT - paddle_size);
  }
}

// Communication avec le serveur
void communicateWithServer() {
  int num_paquet = 0;
  num_paquet += 1;
  if (WiFi.status() == WL_CONNECTED) {
    String message = String(client_player2_Y) + "; num paquet : "+ num_paquet + "\n";
    udp.beginPacket(serverIP, 9999);
    udp.print(message);
    udp.endPacket();

    int packetSize = udp.parsePacket();
    Serial.println(udp.remoteIP());
    Serial.println(packetSize);
    if (packetSize) {
      char packetData[packetSize + 1];
      udp.read(packetData, packetSize);
      packetData[packetSize] = '\0';

      String receivedData(packetData);
      Serial.println(receivedData);

      // Traitement des données reçues du serveur
      int pos = receivedData.indexOf(";");
      if (pos != -1) {
        String new_server_player1_Y = receivedData.substring(0, pos);
        server_player1_Y = new_server_player1_Y.toInt();
        receivedData = receivedData.substring(pos + 1);

        pos = receivedData.indexOf(";");
        String new_ball_Y = receivedData.substring(0, pos); // Extraction de la position de la balle en Y
        ball_y = new_ball_Y.toInt();
        receivedData = receivedData.substring(pos + 1);

        pos = receivedData.indexOf(";");
        String new_ball_X = receivedData.substring(0, pos); // Extraction de la position de la balle en X
        ball_x = new_ball_X.toInt();
        receivedData = receivedData.substring(pos + 1);

        pos = receivedData.indexOf(";");
        String new_ScorePlayer1 = receivedData.substring(0, pos); //extraction score player1
        ScorePlayer1 = new_ScorePlayer1.toInt();
        receivedData = receivedData.substring(pos + 1);

        pos = receivedData.indexOf(";");
        String new_ScorePlayer2 = receivedData.substring(0, pos); //extraction score player1
        ScorePlayer2 = new_ScorePlayer2.toInt();
      }
    }
  }
}


// Mise à jour de l'affichage
void updateDisplay() {
  display.clearDisplay();
  display.drawFastVLine(server_player1_X, server_player1_Y, paddle_size, SSD1306_WHITE);
  display.drawFastVLine(client_player2_X, client_player2_Y, paddle_size, SSD1306_WHITE);
  display.drawPixel(ball_y, ball_x, WHITE);
  drawCourt();
  displayScore();
  display.display();
}
void drawCourt() {
    display.drawRect(0, 0, 128, 64, WHITE);
}

void displayScore() {
  // Affichage du score au milieu de l'écran
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 - 18, SCREEN_HEIGHT / 2 - 8);
  display.setTextColor(SSD1306_WHITE);
  display.print(ScorePlayer1);

  // Affichage de la barre "-" entre les scores
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 - 4);
  display.setTextColor(SSD1306_WHITE);
  display.print("-");

  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 + 6, SCREEN_HEIGHT / 2 - 8);
  display.print(ScorePlayer2);
}