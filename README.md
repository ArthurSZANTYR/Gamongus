# Gamongus : Projet Pong Multiplayer

Ce projet consiste en un jeu Pong multijoueur utilisant une connexion WiFi. Les joueurs contrôlent leur raquette à l'aide de deux boutons et tentent de marquer des points en renvoyant une balle derrière la raquettes adverses.

## Matériel requis

- boitier GAMONGUS :
    -> un PCB avec esp32
    -> un Shield avec 8 boutons et un écran OLED
    -> un cable USB C vers USB

## Dépendances

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
- [Keypad Library](https://github.com/Chris--A/Keypad)

## Branches du projet

- **main:** Contient le code principal du jeu.

## Configuration du matériel

- Connectez le cable au port UsbC de la console GAMONGUS
- Connectez Le cable à un ordinateur munis du logiciel ArduinoIDE
- telechargez les codes depuis le github

## Configuration WiFi

Modifiez les paramètres WiFi en remplaçant les variables `ssid` et `password` par les informations de votre réseau WiFi.

```cpp
const char *ssid = "Votre_SSID";
const char *password = "Votre_Mot_de_Passe";
```

## Démarrage du jeu

1. Chargez le code sur votre Arduino.
2. Laisser le boitier GAMONGUS brancher

## Fonctionnalités principales

- Multijoueur via WiFi : Permet à deux joueurs de s'affronter en temps réel.
- Affichage du score : Le score de chaque joueur est affiché au centre de l'écran.
- Découverte automatique des clients : Le serveur envoie une requête de découverte et récupère l'adresse IP du client.

## Auteur

Ce projet à été réalisé par :
Sébastien LAM
Maëlle ROCHE
Arthur SZANTYR

