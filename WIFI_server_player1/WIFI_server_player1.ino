#include <WiFi.h>
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin("iPhone de Arthur", "1jusqua8");
  Serial.println("hello");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi");
    delay(500);
  }

  IPAddress IP = WiFi.localIP();
  Serial.print("Adresse IP du serveur : ");
  Serial.println(IP);



  server.begin();
}

int num_paquet = 0;
void loop() {
  num_paquet+=1;
  WiFiClient client = server.available();
  if (client && client.connected()) {
    Serial.println("Client connecté !");
    //while (client.available()) {
    //  int data = client.parseInt(); // Lire les données envoyées par le client
    //  Serial.println(data);
    //}
    while (client.available() > 0) {
      char receivedChar = client.read();
      Serial.print(receivedChar);
    }
    client.print(String(1) + " " + String(4) + " num paquet : "+ num_paquet + "\n"); // Envoyer des données au client
    Serial.print("loop");
  }
  delay(100);
}
