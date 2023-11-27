#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#include <WiFi.h>
WiFiServer server(80);

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
int server_player1_X = SCREEN_WIDTH / 4;  //player 1 (server)
int server_player1_Y = SCREEN_HEIGHT / 2;

int client_player2_X = 3*SCREEN_WIDTH / 4 , client_player2_Y = SCREEN_HEIGHT / 2; // Position du client - player 2
int paddle_size = 5;
int deltaX = 0;
int deltaY = 0;

uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;


void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  WiFi.begin("iPhone de Arthur", "1jusqua8");
  Serial.println("hello");
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Connexion WiFi...");
    display.display();
    Serial.println("Not connected to WiFi");
    delay(500);
  }


  IPAddress IP = WiFi.localIP();
  Serial.print("Adresse IP du serveur : ");
  Serial.println(IP);

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Connecte a ");
  display.println("iPhone de Arthur");
  display.print("IP: ");
  //display.println(WiFi.localIP());
  display.println(IP);
  display.display();

  delay(2000); // Afficher l'adresse IP pendant 2 secondes

  server.begin();
}

int num_paquet = 0;
void loop() {
  num_paquet+=1;

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
  //x = constrain(x + deltaX, paddle_size, SCREEN_WIDTH - paddle_size);
  server_player1_Y = constrain(server_player1_Y + deltaY, paddle_size, SCREEN_HEIGHT - paddle_size);


  WiFiClient client = server.available();
  if (client && client.connected()) {
    //Serial.println("Client connecté !");
    //while (client.available()) {
    //  int data = client.parseInt(); // Lire les données envoyées par le client
    //  Serial.println(data);
    //}
    client.print(String(server_player1_Y) + ";" + String(ball_x) + ";" + String(ball_y) + "; num paquet : "+ num_paquet + "\n");

    String receivedData = "";
    while (client.available() > 0) {
      char receivedChar = client.read();
      Serial.print(receivedChar);
      receivedData += receivedChar; // Ajouter le caractère à la variable
    }

    int pos = receivedData.indexOf(";");   //on recupere seulement la donnée Y de l'autre joueur
    if (pos != -1) {
    String sub = receivedData.substring(0, pos); // Extraction de la sous-chaîne après ';'
    Serial.println(sub);
    client_player2_Y = sub.toInt();

  }


  //ball update
  uint8_t new_x = ball_x + ball_dir_x;
  uint8_t new_y = ball_y + ball_dir_y;
  // Check if we hit the vertical walls
  if(new_x == 0 || new_x == 127) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
  }
  // Check if we hit the horizontal walls.
  if(new_y == 0 || new_y == 63) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
  }
  ball_x = new_x;
  ball_y = new_y;

  updateDisplay();
  delay(100);
}
}

void updateDisplay() {
  display.clearDisplay();
  //display.drawLine(server_player1_X - paddle_size, server_player1_Y, server_player1_X + paddle_size, server_player1_Y, SSD1306_WHITE);
  //display.drawLine(server_player1_X, server_player1_Y - paddle_size, server_player1_X, server_player1_Y + paddle_size, SSD1306_WHITE);
  display.drawFastVLine(server_player1_X, server_player1_Y, paddle_size, SSD1306_WHITE);
  display.drawFastVLine(client_player2_X, client_player2_Y, paddle_size, SSD1306_WHITE);
  display.drawPixel(ball_x, ball_y, WHITE);
  display.display();
}