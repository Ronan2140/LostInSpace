# Star Tracker: "Lost-In-Space" Navigation Algorithm

[**English Version**](#english)** | [Version Française](#français)**

---

<a name="english"></a>
## English

This project implements a complete Guidance, Navigation, and Control (GNC) Star Tracker system in C++. It is capable of deducing the absolute orientation (Attitude) of a spacecraft in the void of space from a single simulated image of unidentified stars.

### Core Features

* **3D Stellar Engine:** Parses a real astronomical catalog and projects terrestrial spherical coordinates into normalized 3D vectors.
* **Optical Simulator (Pinhole Camera):** Generates simulated photographs of the sky (rendered in `.ppm` format) applying random rotation matrices.
* **Topological Database:** Offline generation of a ~16 million stellar triangle database filtered by Field of View (FOV). It uses a strict topological sorting (opposite side method) to guarantee a mathematically invariant alignment.
* **"Lost-In-Space" Recognition:** A blazing-fast algorithm using binary search (`std::lower_bound`) and tolerance window scanning. It can identify a stellar pattern among millions of possibilities in a fraction of a second, while filtering out false positives via combinatorial exploration (RANSAC-like approach).
* **Attitude Solver:** Solves **Wahba's Problem** using Singular Value Decomposition (SVD - Kabsch Algorithm) to extract the exact rotation matrix between the camera frame and the universal reference frame.

### Prerequisites and Dependencies

* **Compiler:** C++20 / C++23 compatible.
* **Math Library:** [Eigen](https://eigen.tuxfamily.org/) (Used for linear algebra, quaternions, and SVD).

### Installation & Database (Important)

The star catalog used for the simulation is too large to be directly included in this repository via standard Git. You must download it manually before running the program.

1. Download the star catalog CSV file here:  
   [**Download stars.csv**](https://codeberg.org/astronexus/hyg) *data/athyg_v3/hyglike_from_athyg_v32.csv.gz* 
2. Create a `data` folder at the root of your project if it doesn't exist.
3. Place the downloaded file inside, ensuring the relative path matches: `../data/stars.csv` (relative to your executable).
4. Create an `images` folder at the root to store the simulated camera outputs.

### Usage

Once compiled, the `main.cpp` entry point executes the full end-to-end sequence:

1. Loads the CSV into memory.
2. Generates the ~566k triangles database.
3. Randomly rotates the satellite and captures a simulated photo (saved as `.ppm`).
4. Analyzes the image pixels (geometric extraction).
5. Queries the topological database.
6. Calculates and displays the deduced rotation matrix (to be compared with the original simulator matrix).

### Architecture

* `StarCatalogue`: RAM management, CSV parsing, and magnitude filtering.
* `TriangleDatabase`: Generator of pre-calculated topological structures.
* `CameraSimulator`: Pinhole optical rendering engine.
* `StarMatcher`: The algorithmic core of the AI (Spatial recognition and SVD resolution).

### Example Output

```text
# Example console output for a successful run:
Database imported: 5036 stars
Generating triangles...
Database generated: 16347495 triangles.
Searching for a valid combination among captured stars...
>>> Triangle recognized using pixels 0, 1, 2 ! <<<

=== ORIGINAL ROTATION MATRIX (Simulator) ===
 0.499428   0.518252   0.694253
-0.806668 -0.0141182   0.590836
 0.316003  -0.855112   0.411006

=== CALCULATED ROTATION MATRIX (Star Tracker) ===
 0.499428   0.518252   0.694253
-0.806669 -0.0141182   0.590836
 0.316003  -0.855112   0.411007
```
<a name="français"></a>
## Français
# Star Tracker: Algorithme de Navigation "Lost-In-Space"

Ce projet implémente un système complet de guidage et de navigation spatiale (Star Tracker) en C++. Il est capable de déduire l'orientation absolue (Attitude) d'un engin spatial dans le vide intersidéral à partir d'une simple image d'étoiles non identifiées.

### Fonctionnalités Principales

* **Moteur Stellaire 3D :** Parsing d'un catalogue astronomique réel et projection des coordonnées sphériques terrestres vers des vecteurs 3D normalisés.
* **Simulateur Optique (Pinhole Camera) :** Génération de photographies simulées du ciel (rendu au format `.ppm`) avec application de matrices de rotation aléatoires.
* **Base de Données Topologique :** Génération hors-ligne d'un catalogue de ~16 millions de triangles stellaires filtrés par champ de vue (FOV), avec un tri topologique strict (méthode du côté opposé) pour garantir un alignement mathématique invariant.
* **Reconnaissance "Lost-In-Space" :** Algorithme de recherche par dichotomie (`std::lower_bound`) et scan de fenêtre de tolérance, capable d'identifier un motif stellaire parmi des millions de possibilités en une fraction de seconde, tout en filtrant les faux positifs via exploration combinatoire.
* **Solveur d'Attitude :** Implémentation du **Problème de Wahba** via la Décomposition en Valeurs Singulières (SVD - Algorithme de Kabsch) pour extraire la matrice de rotation exacte entre le repère caméra et le repère universel.

### Prérequis et Dépendances

* **Compilateur :** Compatible C++20 / C++23.
* **Librairie Mathématique :** [Eigen](https://eigen.tuxfamily.org/) (Utilisée pour l'algèbre linéaire, les quaternions et la SVD).

### Installation et Base de Données (Important)

Le catalogue des étoiles utilisé pour la génération spatiale est trop volumineux pour être inclus directement dans ce dépôt via Git. Vous devez le télécharger manuellement avant de lancer le programme.

1. Téléchargez le fichier du catalogue des étoiles ici :  
   [**Télécharger stars.csv**](https://codeberg.org/astronexus/hyg) *data/athyg_v3/hyglike_from_athyg_v32.csv.gz* 
2. Créez un dossier `data` à la racine de votre projet s'il n'existe pas.
3. Placez le fichier téléchargé à l'intérieur, en vérifiant que le chemin correspond à :
   `../data/stars.csv` (relatif à votre exécutable).
4. Créez également un dossier `images` à la racine pour stocker les sorties de la caméra simulée.

### Utilisation

Une fois compilé, le point d'entrée `main.cpp` exécute la séquence complète de bout en bout :

1. Chargement du CSV en mémoire.
2. Génération des ~16 millions de triangles.
3. Rotation aléatoire du satellite et capture d'une photo simulée (sauvegardée en `.ppm`).
4. Analyse des pixels de l'image (extraction géométrique).
5. Interrogation de la base de données topologique.
6. Calcul et affichage de la matrice de rotation déduite (à comparer avec la matrice originale du simulateur).

### Architecture du Code

* `StarCatalogue` : Gestion de la RAM, parsing du CSV et filtrage des magnitudes.
* `TriangleDatabase` : Générateur de structures topologiques pré-calculées.
* `CameraSimulator` : Moteur de rendu optique sténopé.
* `StarMatcher` : Cœur algorithmique de l'IA (Reconnaissance spatiale et résolution SVD).

### Exemple de Sortie

```text
# Exemple de sortie console lors d'un test réussi :
Base importée : 5036 étoiles
Génération des triangles...
Base générée : 16347495 triangles.
Recherche d'une combinaison valide parmi les étoiles capturées...
>>> Triangle reconnu avec les pixels 0, 1, 2 ! <<<

=== MATRICE DE ROTATION ORIGINALE (Simulateur) ===
 0.499428   0.518252   0.694253
-0.806668 -0.0141182   0.590836
 0.316003  -0.855112   0.411006

=== MATRICE DE ROTATION CALCULÉE (Star Tracker) ===
 0.499428   0.518252   0.694253
-0.806669 -0.0141182   0.590836
 0.316003  -0.855112   0.411007
```
