#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include <array>

class Game {
public:
    std::array<int, 4> gameState = {0, 0, 0, 0}; // Plateau de jeu

    // Méthode pour convertir gameState en une chaîne de caractères
    String gameStateToString() {
        String gameStateString;
        for (int i = 0; i < gameState.size(); ++i) {
            gameStateString += String(gameState[i]);
            if (i != gameState.size() - 1) {
                gameStateString += ","; // Ajouter une virgule entre les éléments pour la lisibilité
            }
        }
        return gameStateString;
    }
};

Game game;

// Set your access point network credentials
const char* ssid = "ESP32-Access-Point-GAMONGUS";
const char* password = "123456789";  // Peut être laissé vide pour un AP ouvert

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


int numero_paquet = 0;
void SendToServerGameState(String GameState) {
    numero_paquet ++;
    server.on("/temperature", HTTP_GET, [GameState](AsyncWebServerRequest *request){
        request->send(200, "text/plain", GameState + numero_paquet);
    });
}

void setup() {
    // Serial port for debugging purposes
    Serial.begin(115200);
    Serial.println();

    // Setting the ESP as an access point
    Serial.print("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid);   //, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Start server
    SendToServerGameState(game.gameStateToString());

    server.begin();
}

void loop() {
    int i = 0;
    i++;

    //modification
    SendToServerGameState(String(i));

    delay(1000); // Attendre 1 seconde avant la prochaine mise à jour
}
