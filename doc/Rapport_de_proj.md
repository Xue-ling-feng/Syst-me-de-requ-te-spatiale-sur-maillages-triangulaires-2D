# Rapport de Proj

#### ZHOU Mengzhe 
#### 21927425

## 3.1 Structure de données     
我将 square.geo 文件中的 raf 参数设置为 0.5 ，最后得到了12个节点，26个单元的 square.msh 文件。

#### 1. 解释 .msh 文件的结构：  
.msh 文件由3个关键字段快组成：  
- **$MeshFormat**: 文件格式声明：
  ```
  $MeshFormat
  2.2 0 8
  $EndMeshFormat
  ```
  其中， 2.2 指的是Gmsh文件的格式版本；0指的是文件类型是ASCII文本格式；8指的是浮点数精度。      

- **$Nodes** ：节点块：
  ```
  $Nodes
   12
   1 0 0 0
   2 1 0 0
   ...
   $EndNodes
  ```
  其中第一行的12是节点总数。从第二行开始，第一列是节点序号，从1到12；第二、三、四列是节点坐标(x,y,z)。

- **$Elements** ： 单元块：
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
    其中第一行的26是单元总数。  
    从第二行开始，每一行代表一个单元，其中又分为3类：   
    1. 从1到4，都是**点单元**（15），后面跟两个标签（2），分别是物理标签和几何标签：物理标签是默认的（0）；几何标签：按生成顺序编号从1到4（1），由1个节点组成，点单元1是节点（1）。
    2. 从5到12，都是**线单元** （1），后面跟两个标签（2），分别是物理标签和几何标签：物理标签是默认的（0）；几何标签：按生成顺序编号从1到4（1），由2个节点组成，线单元5是节点（1 5）。
    3. 从13到26，都是**三角形单元**（2），后面跟两个标签（2），分别是物理标签和几何标签：物理标签是默认的（0）；几何标签：按生成顺序编号1（1），因为是同时生成的，所以只有1，由三个节点组成，三角形单元13是节点（6 3 11），     
#### 2. Mesh类数据结构：
有3个数据构成：
1. 网格格式信息（Gmsh版本，文件类型，数据精度）
2. 节点集合（节点ID，节点坐标（x,y,z））
3. 单元集合（单元ID，单元类型，标签数量，物理标签，几何标签）       
#### 3. Expliquer comment fonctionne le programme de test.
1. 在读取 square.msh 文件之后，把数据装进 Mesh类中。       
2. 将 $MeshFormat 中的数据储存到 mesh.format ； $Nodes 中的数据储存到 mesh.nodes ； $Element 中的数据储存到 mesh.elements 。   
4. 运行 Mesh类中的函数，判断数据是否正确无误。     
5. 如果正确，输出数据。

## 3.2 Recherche simple

## 3.3 Recherche avec hashmap

## 3.4 Bibliothèques alternatives

## 3.5 Recherche des triangles voisins

## 3.6 Visualisations