#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <WiFi.h>

// Paramètres OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define OLED_RESET -1

// Création du serveur
WiFiServer server(80);

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
int server_player1_X = SCREEN_WIDTH / 4;  // Player 1 (serveur)
int server_player1_Y = SCREEN_HEIGHT / 2;

int client_player2_X = 3 * SCREEN_WIDTH / 4;
int client_player2_Y = SCREEN_HEIGHT / 2;  // Position du client - Player 2

int paddle_size = 5;
int deltaX = 0;
int deltaY = 0;
uint8_t vitesse_delta = 4;

uint8_t ball_x = 64, ball_y = 32;
uint8_t vitesse_ball = 2;
uint8_t ball_dir_x = 1 * vitesse_ball, ball_dir_y = 1 * vitesse_ball;

// Initialisation
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  connectToWiFi();
  displayIP();

  server.begin();
}

// Boucle principale
void loop() {
  char key = keypad.getKey();
  processKeypadInput(key);

  WiFiClient client = server.available();
  if (client && client.connected()) {
    sendDataToClient(client);
    receiveDataFromClient(client);
    updateBallPosition();
    updateDisplay();
    delay(100);
  }
}

// Connexion au réseau WiFi
void connectToWiFi() {
  WiFi.begin("iPhone de Arthur", "1jusqua8");
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connexion WiFi...");
    display.display();
    delay(500);
  }
  displayIP();
  delay(2000);
}

// Affichage de l'adresse IP
void displayIP() {
  IPAddress IP = WiFi.localIP();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecte a ");
  display.println("iPhone de Arthur");
  display.print("IP: ");
  display.println(IP);
  display.display();
}

// Traitement des entrées du clavier
void processKeypadInput(char key) {
  if (key) {
    switch (key) {
      case 'U': deltaY = -1 * vitesse_delta; deltaX = 0; break;
      case 'D': deltaY = 1 * vitesse_delta; deltaX = 0; break;
      default: deltaX = 0; deltaY = 0; break;
    }
    server_player1_Y = constrain(server_player1_Y + deltaY, paddle_size, SCREEN_HEIGHT - paddle_size);
  }
}

// Envoi de données au client
void sendDataToClient(WiFiClient &client) {
  int num_paquet = 0;
  num_paquet += 1;
  client.print(String(server_player1_Y) + ";" + String(ball_x) + ";" + String(ball_y) + "; num paquet : " + num_paquet + "\n");
}

// Réception de données depuis le client
void receiveDataFromClient(WiFiClient &client) {
  String receivedData = "";
  if (client.available() > 0) {
    while (client.available() > 0) {
      char receivedChar = client.read();
      receivedData += receivedChar;
    }
  }
  int pos = receivedData.indexOf(";");
  if (pos != -1) {
    String sub = receivedData.substring(0, pos);
    client_player2_Y = sub.toInt();
  }
}

// Mise à jour de la position de la balle
void updateBallPosition() {
  uint8_t new_x = ball_x + ball_dir_x;
  uint8_t new_y = ball_y + ball_dir_y;

  // Collision avec les murs
  if (new_x <= 0 || new_x >= 127) {
    ball_dir_x = -ball_dir_x;
    new_x += ball_dir_x + ball_dir_x;
  }
  if (new_y <= 0 || new_y >= 63) {
    ball_dir_y = -ball_dir_y;
    new_y += ball_dir_y + ball_dir_y;
  }

  // Collision avec les raquettes des joueurs
  if (new_x == server_player1_X && new_y >= server_player1_Y && new_y <= server_player1_Y + paddle_size) {
    ball_dir_x = -ball_dir_x;
    new_x += ball_dir_x + ball_dir_x;
  }
  if (new_x == client_player2_X && new_y >= client_player2_Y && new_y <= client_player2_Y + paddle_size) {
    ball_dir_x = -ball_dir_x;
    new_x += ball_dir_x + ball_dir_x;
  }

  ball_x = new_x;
  ball_y = new_y;
}

// Mise à jour de l'affichage sur l'écran OLED
void updateDisplay() {
  display.clearDisplay();
  display.drawFastVLine(server_player1_X, server_player1_Y, paddle_size, SSD1306_WHITE);
  display.drawFastVLine(client_player2_X, client_player2_Y, paddle_size, SSD1306_WHITE);
  display.drawPixel(ball_x, ball_y, SSD1306_WHITE);
  display.display();
}
