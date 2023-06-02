# NE323_server_web


## 1. Comment utiliser le serveur web

Pour l'exécuter, on a besoin du header zlib.h

Après avoir décompressé le projet, utiliser `make install` qui permettre d'installer le projet.
En installant zlib,php-fpm et en mettant les sites aux bon endroits.

La commande pour run le serveur est `make run`, mais on peut également exécuter le script autoreload.sh pour cela.

## 2. Sites web utilisés pour le test

Nous avons pour cela les sites :

URL Version HTTP/1.0:

- localhost:8000/www.toto.com/
- localhost:8000/www.fake.com/
- localhost:8000/www.o.com/, avec une vidéo en streaming avec range
- localhost:8000/game.com/, un site avec un petit jeu installé dessus
- localhost:8000/post.php, un php utilisé pour le test de la prise en charge de fcgi php-fpm (POST/GET).
- localhost:8000/info.php, un php utilisé pour le test de la prise en charge des fichiers longs.

## 3. Fonctionnalités implémentées

- [x] Gestion des requêtes GET
- [x] Gestion des requêtes HEAD
- [x] Gestion des requêtes POST php
- [x] Prise en charge du chunked
- [x] Prise en charge du PHP dans le /var/www/
- [x] Prise en charge des fichiers de différents mime types
- [x] Prise en charge du streaming vidéo avec le header range
- [x] Prise en charge du multisite
- [x] Prise en charge de Gros fichiers php et fermeture de la socket fcgi

## 4. Leaks

Il n'y a pas de leaks connus à ce jour avec Valgrind (testable avec la commande `make leaks`).

## 5. Probleme réglés

Possibilité de mettre plus de 126 caracteres dans le QUERY_STRING
