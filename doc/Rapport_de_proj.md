# Rapport de Proj

#### ZHOU Mengzhe 
#### 21927425

## 3.1 Structure de données     
J’ai défini le paramètre raf du fichier square.geo à 0,5 , ce qui m’a permis d’obtenir le fichier square.msh comportant 12 noeuds et 26 éléments.

#### 1. 解释 .msh 文件的结构：  
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
| 0.05 | 0.005 | 1.57e-5 | 1.8e-5 | 8.06 | 7.47 |
| 0.05 | 0.002 | 1.403e-4 | 1.412e-4 | 50.89 | 44.29 |
| 0.02 | 0.002 |  2.94e-5 | 3.02e-5 | 9.488 | 8.91 | 

Observations :    
1. Sur des petits maillages et des faibles quantités de recherches , la bibliothèque standard std::unordered_map est plus rapide que parallel-hashmap.
2. Sur des gros maillages (raf >= 0.005) et un grand nombre de recherches , parallel-hashmap devient plus efficace que la bibliothèque standard.

Conclusions :   
Donc parallel-hashmap est plus efficace que unordered_map sur des gros maillages.         
Bien qu'avec une optimisation uniquement au niveau de la bibliothèque, le gain de performance par recherche individuelle ne soit pas évident, voire légèrement plus lent.     
Cependant, lors d'un grand nombre de recherches, à mesure que la précision du maillage augmente, l'avantage de parallel-hashmap devient plus marqué.      


## 3.5 Recherche des triangles voisins      
选择编号为4的三角形元素作为目标三角形，测试在不同精度的maillages 中 fonction naive 和 使用parallel-hashmap搜索消耗的时间分别是多少：      
| raf | naive / s  | parallel-hashmap / s |
| :---: | :---: | :---: |
| 0.5 | 4.1e-6 | 1.7e-6 | 
| 0.05 | 3.92e-5 | 1.8e-6 |
| 0.005 | 3.48e-3 | 2.9e-6 |
所以很明显可以得出结论：    
使用 parallel-hashmap 搜索的效率比使用 fonction naive 高得多。      
并且随着精细度提高，这种优势更加明显。      


## 3.6 Visualisations     



