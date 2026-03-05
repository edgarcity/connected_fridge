# Frigo Connecté — Surveillance des chambres froides en temps réel

Système IoT de surveillance continue de deux chambres froides (positive et négative) déployé en cuisine professionnelle. Alertes immédiates via buzzer local et notification push en cas d'anomalie de température, 24h/24.

## Fonctionnement

- L'ESP8266 collecte les mesures des deux sondes DS18B20 toutes les 30 secondes
- Les données sont transmises via Wi-Fi à l'application Blynk (accessible depuis n'importe quel smartphone)
- En cas de dépassement de seuil, le buzzer local s'active et une notification push est envoyée

## Gestion des alertes

| État | LED | Buzzer |
|---|---|---|
| Tout fonctionne | ✅ Allumée | ❌ Éteint |
| Perte Wi-Fi | ❌ Éteinte | Bip toutes les secondes |
| Capteur déconnecté | ✅ Allumée | Bip toutes les secondes |
| Température dépassée | ✅ Allumée | Bip en continu |

## Matériel

- ESP8266 NodeMCU
- 2 sondes de température DS18B20 (précision ±0,5°C)
- Buzzer + LED de statut
- PCB soudée à la main
- Boîtier étanche IP55

## Contenu du dépôt

- `docs/` → documentation complète du projet (PDF)
- `code/` → firmware C++ pour ESP8266

## Bilan

- Déployé en cuisine professionnelle, fonctionnel 24h/24 depuis la mise en service
- Surveillance simultanée de deux chambres : −18°C et +2°C
- Zéro fausse alerte depuis le déploiement
- Coût total < 40 € vs 300-500 € pour un système commercial équivalent

## Licence

[CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/) — Utilisation libre et modification autorisées, usage commercial interdit.
