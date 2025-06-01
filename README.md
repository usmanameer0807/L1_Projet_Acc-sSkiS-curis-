## L1_Projet_Acces-Ski-Securité-

# 🎿 Accès Ski Sécurisé – Gestion d'accès aux remontées mécaniques

## 📌 Présentation

Ce projet a pour objectif de **sécuriser et automatiser l’accès aux remontées mécaniques** à l’aide de la technologie **LoRa** et d’un système embarqué. Il répond à la problématique de lutte contre la fraude et les incidents liés à l’accès non autorisé.

## 🛠 Fonctionnalités principales

- Simulation d'un badge RFID via bouton poussoir.
- Transmission des identifiants via **LoRa** à un serveur distant.
- Gestion des forfaits côté serveur.
- Validation ou refus local de l'accès via :
  - LED RGB (statut d’accès)
  - Buzzer (signal sonore)
  - Écran OLED (informations)
  - Servo-moteur (barrière d'accès)

## 🧰 Matériel utilisé

- Carte UCA (client et serveur)
- Module LoRa SX1276
- Bouton poussoir
- LED RGB
- Buzzer
- Écran OLED
- Servo-moteur

## 🔄 Schéma de fonctionnement
Utilisateur (badge simulé)
        ↓
Carte UCA (Client)
        ↓
   Envoi via LoRa
        ↓
    Serveur distant
        ↓
   Réponse LoRa (OK/NON)
        ↓
Actions locales : LED / Buzzer / OLED / Barrière


## 📅 Planning de réalisation

1. **Semaine 1** : Création du prototype (client).
2. **Semaine 2** : Mise en place de la communication LoRa bidirectionnelle.
3. **Semaine 3** : Développement de la gestion des forfaits côté serveur.
4. **Semaine 4** : Intégration finale et démonstration.

## 🧪 Démo

- **BTN0** : S’inscrire et recevoir un ID unique.
- **BTN1** : S’identifier et demander l’accès.
- **Serveur** : Vérification ID et envoi de l’autorisation.
- **Client** : Déclenchement des actions selon réponse (OK/NON).

## 🔗 Lien GitHub

[Accès Ski Sécurisé – GitHub Repo](https://github.com/usmanameer0807/L1_Projet_Acc-sSkiS-curis-)

## 🚀 Perspectives

- Intégration d’un **véritable lecteur RFID**.
- Renforcement de la sécurité.
