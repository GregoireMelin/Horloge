# My project's README






MISE EN PLACE DE l'INTERRUPTION PAR OVERFLOW

CLK = 16 MHz

------------
Timer1 16 bit : Fréquence actuelle = 1000Hz ; préscaler = 1 ; top value =15999 (doit être <65536)
------------
16000000/1 =16000000 Hz (frequence d'incrementation)
1/16000000 = 6.25e-08 sec (incrementation toutes les 6.25e-08 sec)
La top value est fixée par OCR1A = 15999= (16000000) / (1*1000) - 1 
avec 1000 = fréquence désirée
Arrivé à 15999, le timer overflow :
L'overflow se fera en 15999*6.25e-08 = 0.0009999375 s ~ 1ms


------------
Timer0 8 bit : Fréquence actuelle = 125Hz ; préscaler = 1024 ; top value = 124 (doit être <256)
------------
16000000/1024 =15625.0 Hz (frequence d'incrementation)
1/15625.0 = 6.4e-05 sec (incrementation toutes les6.4e-05 sec)
La top value est fixée par OCR0A =124= (16000000) / (1024*125) - 1 
avec 125 = fréquence désirée 
Arrivé à 124, le timer overflow :
L'overflow se fera en 125*6.4e-05 = 8 ms 


## CONNEXION BLUETOOTH
### Connexion
  - Connexion sur le serial port via blueman-manager
  - blueman-manager indique une connexion sur le port : PORT = /dev/rfcomm1

### Verification de la validite de la connexion
Commandes permettant de vérifier :

--> ls /dev|grep $PORT
Renvoie : $PORT

--> sudo cu -l /dev/$PORT --speed 38400
Renvoie : cu: open (/dev/$PORT): Permission denied
          cu: /dev/rfcomm0: Line in use

Pour corriger l'erreur "cu: open (/dev/$PORT): Permission denied" :
cd /dev
sudo chmod 777 $PORT