# Rapport de Proj

#### ZHOU Mengzhe 
#### 21927425

## 3.1 Structure de données     
J’ai défini le paramètre raf du fichier square.geo à 0,5 , ce qui m’a permis d’obtenir le fichier square.msh comportant 12 noeuds et 26 éléments.

#### 1. Explication de la structure du fichier .msh  
Le fichier `\.msh` est composé de 3 blocs de données clés :  
- **$MeshFormat**: Déclaration du format du fichier
  ```
  $MeshFormat
  2.2 0 8
  $EndMeshFormat
  ```
  Parmi ces valeurs, **2\.2** désigne la version du format de fichier Gmsh ; **0** indique que le type de fichier est au format texte ASCII ; **8** correspond à la précision des nombres flottants.      

- **$Nodes** ：Bloc des nœuds：
  ```
  $Nodes
   12
   1 0 0 0
   2 1 0 0
   ...
   $EndNodes
  ```
  Le nombre 12 sur la première ligne représente le nombre total de nœuds\. À partir de la deuxième ligne, la première colonne correspond au numéro d\&\#39;identification du nœud \(de 1 à 12\) ; les deuxième, troisième et quatrième colonnes sont les coordonnées du nœud $(x, y, z)$.   

- **$Elements** ： Bloc des éléments finis:
  ```
    $Elements
    26
    1 15 2 0 1 1
    ...
    5 1 2 0 1 1 5
    ...
    13 2 2 0 1 6 3 11
    ...
    $EndElements
  ```
    Le nombre 26 sur la première ligne est le nombre total d'éléments.    
    À partir de la deuxième ligne, chaque ligne représente un élément, qui se divise en 3 catégories :       
    1. De 1 à 4 : ce sont des **éléments ponctuels**(15), suivis de deux étiquettes(2),l'étiquette physique et l'étiquette géométrique. L'étiquette physique est par défaut (0) ; l'étiquette géométrique est numérotée de 1 à 4 (1), selon l'ordre de génération.Chaque élément ponctuel est constitué de 1 nœud (ex : l'élément ponctuel 1 correspond au nœud 1).
    2. De 5 à 12 : ce sont des **éléments linéaires** (1), suivis de deux étiquettes (2) : étiquette physique (0 par défaut) et étiquette géométrique (numérotée de 1 à 4(1). Chaque élément linéaire est constitué de 2 nœuds (ex : l'élément linéaire 5 correspond aux nœuds 1 et 5).
    3. De 13 à 26 : ce sont des **éléments triangulaires** (2), suivis de deux étiquettes (2) : étiquette physique (0 par défaut) et étiquette géométrique (1) (car générés simultanément). Chaque élément triangulaire est constitué de 3 nœuds (ex : l'élément triangulaire 13 correspond aux nœuds 6, 3 et 11).     
#### 2. Structure de données de la classe Mesh：
Elle est constituée de 3 données :    
1. Les informations du format du maillage (version Gmsh, type de fichier, précision des données)    
2. L'ensemble des nœuds (ID du nœud, coordonnées du nœud \((x,y,z)\))   
3. L'ensemble des éléments (ID de l'élément, type d'élément, nombre d'étiquettes, étiquette physique, étiquette géométrique)           
#### 3. Expliquer comment fonctionne le programme de test.
1. Après lecture du fichier square.msh, les données sont stockées dans la classe Mesh.
2. Les données du bloc $MeshFormat sont enregistrées dans mesh.format ; les données du bloc $Nodes sont stockées dans mesh.nodes ; les données du bloc $Elements sont enregistrées dans mesh.elements.
3. Les fonctions de la classe Mesh sont exécutées pour vérifier si les données sont correctes.
4. Si les données sont valides, le programme affiche les résultats.

## 3.2 Recherche simple

## 3.3 Recherche avec hashmap
#### Vérifiez l’accélération:
si raf = 0.5 ，le temps d’exécution de la méthode simple est de 5e-7 s , celui de la méthode hashmap est également de 5e-7 s     
si raf = 0.01， le temps d’exécution de la méthode simple est de 0.0010987 s , celui de la méthode hashmap est également de  1.57e-5 s         
Donc on constate:    
- Sur un maillage grossier, les vitesses des deux algorithmes sont quasiment identiques ;
- Sur un maillage fin, la méthode hashmap est environ 70 fois plus rapide que la méthode simple.      

Alors nous pouvons en conclure que plus le maillage est fin, plus l’effet d’accélération de la méthode hashmap est important.     
En effet, la méthode hashmap ne parcourt que les triangles situés dans une petite case spatiale, alors que la méthode simple doit parcourir l’ensemble des triangles du maillage.        
Par conséquent, la méthode hashmap est toujours plus performante quelle que soit la finesse du maillage, et l’écart de temps d’exécution entre les deux méthodes s’agrandit progressivement avec l’affinement du maillage.    

## 3.4 Bibliothèques alternatives
#### Comparer les performances sur des gros maillages:    
| step | raf | hashmap | parallel-hashmap |
| :---: | :---: | :---: | :---: |
| 0.1 | 0.01 | 1.57e-5 | 2.16e-5 | 
| 0.1 | 0.05 | 5.5e-5 | 6.13e-5 |

Amélioration de l’algorithme : répéter la recherche 1 million de fois pour amplifier les différences de données：   
| step | raf | hashmap | parallel-hashmap | hashmap * 1000000 | parallel-hashmap * 1000000 | 
| :---: | :---: | :---: | :---: |:---: | :---: |
| 0.1 | 0.05 | 1.2e-6 | 1.7e-6 | 0.59 | 0.90 |
| 0.01 | 0.005 |  5.92e-5 | 7.13e-5 | 30.87 | 32.39 |

Amélioration de l’algorithme : générer 100 000 points aléatoires, rechercher le même point 10 fois, optimiser PairHash :      
| step | raf | hashmap | parallel-hashmap | hashmap * 1000000 | parallel-hashmap * 1000000 | 
| :---: | :---: | :---: | :---: |:---: | :---: |
| 0.05 | 0.005 | 1.57e-5 | 1.8e-5 | **8.06** | **7.47** |
| 0.05 | 0.002 | 1.403e-4 | 1.412e-4 | **50.89** | **44.29** |
| 0.02 | 0.002 |  2.94e-5 | 3.02e-5 | 9.488 | 8.91 | 

Observations :    
1. Sur des petits maillages et des faibles quantités de recherches , la bibliothèque standard std::unordered_map est plus rapide que parallel-hashmap.
2. Sur des gros maillages (raf >= 0.005) et un grand nombre de recherches , parallel-hashmap devient plus efficace que la bibliothèque standard.

Conclusions :   
Donc parallel-hashmap est plus efficace que unordered_map sur des gros maillages.         
Bien qu'avec une optimisation uniquement au niveau de la bibliothèque, le gain de performance par recherche individuelle ne soit pas évident, voire légèrement plus lent.     
Cependant, lors d'un grand nombre de recherches, à mesure que la précision du maillage augmente, l'avantage de parallel-hashmap devient plus marqué.      


## 3.5 Recherche des triangles voisins      
Choisissez l'élément triangle numéro 4 comme triangle cible, et mesurez le temps consommé par la fonction naïve et par la recherche avec parallel-hashmap dans des maillages de différentes précisions.      

| raf | naive / s  | parallel-hashmap / s |
| :---: | :---: | :---: |
| 0.5 | 1.84e-5 | 1.7e-6 | 
| 0.05 | 3.17e-2 | 1.8e-6 |

Donc on peut donc clairement conclure que :     
l'efficacité de la recherche avec parallel-hashmap est bien supérieure à celle avec la fonction naïve.          
De plus, cet avantage devient encore plus marqué à mesure que la finesse du maillage augmente.      


## 3.6 Visualisations     

## Annexe : Utilisation de l’intelligence artificielle     
Les prompts que j’utilise fréquemment sont les suivants :        
- Traduire en chinois, sans écrire de code  
Ce prompt est très pratique et m’aide à comprendre rapidement les sujets.    
Cependant, les traductions de l’IA sont souvent imprécises. Je dois donc vérifier les mots-clés de chaque question avec Google Traduction.    

- Vérifier les erreurs dans mon code
C’est celui que j’utilise le plus. L’IA lit le code plus rapidement et plus attentivement que moi. Après avoir écrit beaucoup de code, je passe beaucoup de temps à le réexaminer, et les erreurs sont presque toujours des détails mineurs.      
Par exemple, le numéro du triangle et l’indice du triangle ne sont pas les mêmes : il faut utiliser l’indice pour stocker dans la liste triangle, et le numéro pour l’affichage. J’oubliais parfois ce que représentait exactement ma variable (indice ou numéro). J’ai donc pris l’habitude d’utiliser tri_id exclusivement pour désigner l’indice.      
Par ailleurs, l’IA m’a aidé à sortir de certaines impasses dans ma réflexion.             
Par exemple, pour la partie 3.4, je pensais que parallel-hashmap devrait toujours être plus efficace que hashmap, quelle que soit la précision du maillage ou la taille des données traitées. Quand j’ai obtenu des résultats où parallel-hashmap était légèrement plus lent avec step = 0.01 et raf = 0.005, j’ai cru que mon code contenait des erreurs et l’ai revu plusieurs fois. L’IA m’a alors expliqué que la différence ne se voit que sur des données très volumineuses, et que hashmap est en réalité plus efficace quand le maillage n’est pas très précis. De plus, je ne devais pas réduire trop la valeur de step, car cela rendait les résultats difficiles à observer.          
Mais l’IA n’est pas parfaite.             
Par exemple, elle m’a conseillé d’ajouter static_cast<int> pour convertir le résultat de triangle.size() en entier, afin d’éviter des comparaisons erronées du type -1 < 0. Or, la variable i de type int a très peu de chances de devenir négative, du moins pas dans ce projet. Cet ajout ne fait qu’alourdir inutilement la charge du CPU. Néanmoins, j’ai quand même suivi ce conseil pour apprendre cette nouvelle notion.     

-  Comment utiliser xxx
J’utilise ce prompt pour demander l’utilisation de Git, ainsi que celle de Gnuplot pour la partie 3.6. Pour maîtriser les règles syntaxiques complètes, je clique systématiquement sur les sites référencés par l’IA pour consulter les documentations originales.            
Par ailleurs, pour éviter les oublis d’un seul outil, j’interroge plusieurs IA en même temps et compare leurs réponses.     


- Pourquoi écrire xxx comme ça     
J’utilise ce prompt en complément du précédent. L’IA introduit souvent des définitions ou des termes inconnus ; je dois donc poursuivre les questions pour en comprendre le sens profond.       

