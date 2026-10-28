# ☕ RFID-Controlled Liquid Vending Machine

## 📝 About The Project
This embedded software project consists of the end-to-end development of a smart liquid vending machine. The system manages user authentication, liquid selection, order limits, and provides dynamic visual and audio feedback. 

The project was implemented twice: once as a physical hardware prototype on a breadboard, and once as a full virtual simulation using Tinkercad and Wokwi.

## 🛠️ Built With
* **Language:** Embedded C++
* **Hardware Protocols:** I2C, SPI
* **Simulation Platforms:** Tinkercad, Wokwi
* **Components:** Arduino, RFID RC522, 4x4 Keypad, Grove LCD RGB, Grove LED Bar, 8x8 LED Matrix, Piezo Buzzer

## ✨ Key Features
* **RFID Access Control:** Uses an MFRC522 module communicating via SPI to grant or deny access based on the user's card/tag UID.
* **State Machine & Security:** Implements a strict 120-second countdown timer per session, a limit of 3 attempts per day, and a maximum quota of 3 orders per day.
* **Interactive UI:** Users input commands (Juice, Soda, Water & 100ml, 200ml, 300ml quantities) via a 4x4 keypad.
* **Dynamic Feedback:** 
  * **LCD (I2C):** Background color changes (Green/Yellow/Orange/Red) based on access approval and remaining attempts.
  * **LED Matrix (I2C):** Displays specific custom emojis based on the attempt number.
  * **Audio:** Plays 3 distinct iconic melodies (Mario, Star Wars, Pirates of the Caribbean) using a buzzer and MOSFET.

## ⚙️ Simulation Adaptation
Due to component unavailability in Tinkercad, the circuit was cleverly adapted: the RFID was replaced by a digital push button, and the Grove LED components were substituted with custom-programmed NeoPixel strips while maintaining 100% of the software constraints.
---


**Version Française**


# ☕ Distributeur de Boissons Contrôlé par RFID

## 📝 À Propos du Projet
Ce projet de logiciel embarqué consiste en le développement complet d'un distributeur de boissons intelligent. Le système gère l'authentification des utilisateurs, la sélection des liquides, les limites de commande et fournit un retour visuel et sonore dynamique. 

Le projet a été réalisé deux fois : une fois en tant que prototype matériel physique sur breadboard, et une fois en tant que simulation virtuelle complète en utilisant Tinkercad et Wokwi.

## 🛠️ Technologies Utilisées
* **Langage :** C++ Embarqué
* **Protocoles Matériels :** I2C, SPI
* **Plateformes de Simulation :** Tinkercad, Wokwi
* **Composants :** Arduino, RFID RC522, Clavier 4x4, Grove LCD RGB, Grove LED Bar, Matrice LED 8x8, Buzzer Piézo

## ✨ Fonctionnalités Principales
* **Contrôle d'Accès RFID :** Utilisation d'un module MFRC522 communiquant via SPI pour accorder ou refuser l'accès en fonction de l'UID de la carte ou du badge de l'utilisateur.
* **Machine à États & Sécurité :** Implémentation d'un compte à rebours strict de 120 secondes par session, d'une limite de 3 tentatives par jour et d'un quota maximum de 3 commandes par jour.
* **Interface Utilisateur Interactive :** Les utilisateurs saisissent leurs commandes (Jus, Soda, Eau & quantités 100ml, 200ml, 300ml) via un clavier matriciel 4x4.
* **Retour Dynamique :** 
  * **LCD (I2C) :** La couleur de fond change (Vert/Jaune/Orange/Rouge) en fonction de l'autorisation d'accès et des tentatives restantes.
  * **Matrice LED (I2C) :** Affiche des emojis personnalisés spécifiques en fonction du numéro de la tentative.
  * **Audio :** Joue 3 mélodies emblématiques distinctes (Mario, Star Wars, Pirates des Caraïbes) à l'aide d'un buzzer et d'un transistor MOSFET.

## ⚙️ Adaptation pour la Simulation
En raison de l'indisponibilité de certains composants dans Tinkercad, le circuit a été astucieusement adapté : le module RFID a été remplacé par un bouton poussoir numérique, et les composants LED Grove ont été substitués par des bandes NeoPixel programmées sur mesure, tout en respectant 100% des contraintes du cahier des charges logiciel.
