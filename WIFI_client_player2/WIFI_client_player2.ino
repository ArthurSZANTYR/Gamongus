#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// Configuration de la connexion au serveur
WiFiClient client;
const char* serverIP = "172.20.10.11"; // Adresse IP du serveur ESP32
const int serverPort = 80; // Port du serveur

// Paramètres OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define OLED_RESET -1

// Création de l'affichage OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

// Positions et mouvements des joueurs et de la balle
int client_player2_X = 3 * SCREEN_WIDTH / 4;
int client_player2_Y = SCREEN_HEIGHT / 2;

int server_player1_Y = SCREEN_HEIGHT / 2; // Position du joueur serveur
int server_player1_X = SCREEN_WIDTH / 4; // Position du joueur serveur

int paddle_size = 5;
int deltaX = 0;
int deltaY = 0;
uint8_t vitesse_delta = 4;

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

  // Connexion au serveur
  connectToServer();
}

// Boucle principale
void loop() {
  char key = keypad.getKey();
  processKeypadInput(key);
  communicateWithServer();
  updateDisplay();
  delay(100);
}

// Connexion au réseau WiFi
void connectToWiFi() {
  WiFi.begin("iPhone de Arthur", "1jusqua8");
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
  delay(2000);
}

// Connexion au serveur
void connectToServer() {
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

// Traitement des entrées du clavier
void processKeypadInput(char key) {
  if (key) {
    switch (key) {
      case 'U': deltaY = -1 * vitesse_delta; deltaX = 0; break;
      case 'D': deltaY = 1 * vitesse_delta; deltaX = 0; break;
      default: deltaX = 0; deltaY = 0; break;
    }
    client_player2_Y = constrain(client_player2_Y + deltaY, paddle_size, SCREEN_HEIGHT - paddle_size);
  }
}

// Communication avec le serveur
void communicateWithServer() {
  int num_paquet = 0;
  num_paquet += 1;
  if (client.connected()) {
    client.print(String(client_player2_Y) + "; num paquet : "+ num_paquet + "\n");

    String receivedData = "";
    if (client.available() > 0){
      while (client.available() > 0) {
        char receivedChar = client.read(); // Données reçues de l'autre joueur
        Serial.print(receivedChar);
        receivedData += receivedChar;
      }
    }

    int pos = receivedData.indexOf(";");
    if (pos != -1) {
      String new_server_player1_Y = receivedData.substring(0, pos);
      Serial.println(new_server_player1_Y);
      server_player1_Y = new_server_player1_Y.toInt();
      receivedData = receivedData.substring(pos + 1);

      pos = receivedData.indexOf(";");
      String new_ball_Y = receivedData.substring(0, pos); // Extraction de la position de la balle en Y
      Serial.println(new_ball_Y);
      ball_y = new_ball_Y.toInt();
      receivedData = receivedData.substring(pos + 1);

      pos = receivedData.indexOf(";");
      String new_ball_X = receivedData.substring(0, pos); // Extraction de la position de la balle en X
      Serial.println(new_ball_X);
      ball_x = new_ball_X.toInt();
    } 
  } else {
    client.connect(serverIP, serverPort);
  }
}

// Mise à jour de l'affichage
void updateDisplay() {
  display.clearDisplay();
  display.drawFastVLine(server_player1_X, server_player1_Y, paddle_size, SSD1306_WHITE);
  display.drawFastVLine(client_player2_X, client_player2_Y, paddle_size, SSD1306_WHITE);
  display.drawPixel(ball_y, ball_x, WHITE);
  display.display();
}
