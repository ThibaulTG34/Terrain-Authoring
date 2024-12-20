# Projet 3D : Terrain Authoring

## Introduction
De nos jours, les terrains sont beaucoup utilisés dans de nombreux domaines, comme la simulation ou les jeux vidéo, il est donc nécessaire de trouver une solution pour les stocker, car les modèles 3D deviennent très lourd selon la taille des terrains. Une solution, qui est la nôtre, est d'avoir un terrain à partir de différentes cartes. Celles-ci décriront différents aspects du terrain et pourront, lors de la génération, être utilisées pour générer le terrain final en n'ayant stocké seulement quatre images. 

De plus, avec l'utilisation de cartes, il est possible de non seulement générer, mais aussi de modifier le terrain comme bon nous semble.
C'est dans ce cadre que s'inscrit notre projet. Il aura la forme d'une application développée en Qt C++, utilisant le système de cartes énoncé et permettant la visualisation du terrain ainsi que la modification de celui-ci en temps réel et en 3D. Les changements peuvent être appliqués aux cartes afin de pouvoir les sauvegarder  et les utiliser dans divers contextes et applications, prenant en charge la génération de terrain par cartes.

## Présentation du projet
Dans ce projet en binome, nous avions implémenté une interface graphique avec Qt pour l'édition de terrain. Avec cette interface il est possible d'impoter plusieurs cartes (hauteurs, biomes,...), qui sont en réalité des images, afin de modifier le terrain. Ensuite, à l'aide d'outil de sélection (par exemple un cercle), l'utilisateur peut modifier le terrain avec les différents outils d'édition mis à disposition (modification des hauteurs, ajout/modification de biomes, ajout/suppression de végétations). Concernant la programmation, nous avions utilisé du C++ et la librairie OpenGL pour le rendu et Qt pour la partie interface. Voici un aperçu de l'interface graphique.

<div align="center"><img src="https://github.com/ThibaulTG34/Terrain-Authoring/blob/dev/Resultats/Interface.png" alt="image" style="width:500px;height:auto;"></div>

## Outils d'édition
### Augmentation/Réduction des hauteurs
Pour cette partie, nous avions choisi d'utiliser deux types de fonctions pour créer des hauteurs au terrain. Dans un premier temps, nous avons une fonction basée sur la distance (entre le centre du cercle et les points situés à l'intérieur de celui-ci) et le rayon du cercle. 

<div align="center"><img src="https://github.com/ThibaulTG34/Terrain-Authoring/blob/dev/Resultats/Interface.png" alt="image" style="width:500px;height:auto;"></div>

Ensuite, nous avons une fonction basée sur l'équation de la fonction gaussienne.

<div align="center"><img src="https://github.com/ThibaulTG34/Terrain-Authoring/blob/dev/Resultats/Interface.png" alt="image" style="width:500px;height:auto;"></div>

## Installation sur Linux
Ouvrir un terminal

Cloner le projet dans le répertoire souhaité
```bash
  git clone https://github.com/fcaball/Terrain.git
```
Déplacez-vous dans le répertoire de l'application
```bash
  cd App
```
Installez assimp si ce n'est pas fait
```bash
  sudo apt-get update
  sudo apt-get -y install assimp-utils
```

ou 

(/!\ Cette méthode requiert cmake)
```bash
  git clone https://github.com/assimp/assimp.git
  cd assimp
  cmake CMakeLists.txt
  cmake --build .
  make
```
Si vous utilisez cette méthode il faut ajouter au fichier App.pro 
les lignes suivantes :
```bash
LIBS+= -L/chemin/absolu/vers/assimp/bin -lassimp

INCLUDEPATH += assimp/include
  ```
Ensuite une fois revenu dans le dossier App (cd ..), si vous n'y êtes pas déja, il vous reste plus qu'à faire :
```bash
  make clean
  qmake
  make
  ./App
```

  
