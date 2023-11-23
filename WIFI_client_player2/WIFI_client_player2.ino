#include <WiFi.h>
WiFiClient client;

const char* serverIP = "172.20.10.9"; // Remplacez par l'adresse IP du serveur ESP32
const int serverPort = 80; // Port du serveur

void setup() {
  Serial.begin(115200);
  WiFi.begin("iPhone de Arthur", "1jusqua8"); // Connectez-vous au même réseau WiFi que le serveur

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }

  Serial.println("Connecté au réseau WiFi");

  // Connexion au serveur
  Serial.println("Tentative de connexion au serveur...");
  int attempts = 0;
  while (!client.connect(serverIP, serverPort)) {
    attempts++;
    Serial.println("Impossible de se connecter au serveur !");
    Serial.println("Nouvelle tentative dans 1 seconde...");
    delay(1000);
    if (attempts > 10) {
      Serial.println("Échec de la connexion après plusieurs tentatives.");
      break;
    }
  }
    
  if (client.connected()) {
    Serial.println("Connecté au serveur !");
  } else {
    Serial.println("Échec de la connexion au serveur !");
  }
}

int num_paquet = 0;
void loop() {
  num_paquet+=1;
  if (client.connected()) {
    client.print(String(2) + " " + String(6) + " num paquet : "+ num_paquet + "\n");
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
  Serial.println("loop");
  delay(100);
}
