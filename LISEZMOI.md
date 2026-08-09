# Arduino ESP32-S3 + Edge Impulse : trouver la bonne taille de Tensor Arena

# 🚀 Un point de départ pour l’IA embarquée sur ESP32-S3
**Un guide accessible aux débutants pour la classification d’images avec Edge Impulse et TensorFlow Lite Micro**

Ce guide constitue un point de départ pratique pour toute personne souhaitant commencer à développer des applications d’IA embarquée sur un ESP32-S3, y compris les débutants complets.

Il présente le workflow complet, depuis la création et l’étiquetage d’un jeu de données d’images dans Edge Impulse jusqu’à l’entraînement, l’export et le déploiement d’un modèle TensorFlow Lite Micro sur un ESP32-S3. Il explique également les notions essentielles permettant de comprendre le fonctionnement d’un projet de classification d’images sur un appareil embarqué disposant de ressources limitées.

Ce projet aide à répondre à trois questions essentielles avant de déployer un modèle d’IA :

1. **Quel modèle d’IA peut fonctionner sur l’ESP32-S3 ?**
2. **Quelle quantité de mémoire faut-il allouer à la Tensor Arena de TensorFlow Lite Micro pour ce modèle ?**
3. **Combien d’images par seconde peut-on espérer traiter ?**

L’exemple utilise un modèle de classification d’images de dé, mais la même méthode peut être appliquée à de nombreux autres projets d’IA embarquée utilisant une caméra.

---

## 👋 Introduction

Ce dépôt contient un **sketch Arduino de diagnostic** et un fichier **`tflite_micro.h` temporairement modifié** qui permettent ensemble de mesurer la **Tensor Arena de TensorFlow Lite Micro** utilisée par un **modèle Edge Impulse** sur un **ESP32-S3** équipé de PSRAM.

Au-delà des applications habituelles destinées aux hobbyistes et makers, l’ESP32-S3 permet aussi d’explorer à coût modéré des projets d’IA embarquée originaux et intéressants. Son processeur double cœur, sa PSRAM optionnelle, ses connexions Wi-Fi et Bluetooth ainsi que ses instructions orientées IA et DSP en font une base pratique pour la classification d’images, la reconnaissance sonore, la détection de gestes, l’analyse de capteurs et les objets connectés utilisant de l’IA en périphérie.

Associé à **[Edge Impulse](https://edgeimpulse.com/)**, l’ESP32-S3 offre un workflow permettant de collecter des données, d’entraîner un modèle, d’exporter une bibliothèque Arduino et d’exécuter l’inférence directement sur l’appareil sans devoir construire toute la chaîne de déploiement machine learning à partir de zéro.

**Ce dépôt s’adresse aux hobbyistes et makers Arduino qui ont créé un modèle d’IA avec Edge Impulse et souhaitent le déployer de manière fiable sur un ESP32-S3 sous forme de bibliothèque Arduino.**

> [!NOTE]
> <table>
> <tr>
> <td valign="top">
>
> L’exemple utilisé dans ce dépôt est un modèle de classification d’images créé dans Edge Impulse afin de reconnaître chacune des faces d’un dé.
>
> Le modèle utilise des images RGB de 224×224 pixels, capturées avec un module caméra OV2640 connecté à l’ESP32-S3.
>
> </td>
> <td width="240">
> <img src="images/dice_224x224.jpg" alt="Dé jaune utilisé par le modèle de classification d’images" width="224">
> </td>
> </tr>
> </table>

## 🔍 Ce que fait le sketch

Le sketch Arduino de ce dépôt :

- Vérifie que la PSRAM est disponible.
- Affiche la quantité de heap SRAM interne libre ainsi que la PSRAM libre et totale.
- Utilise par défaut une **Tensor Arena de diagnostic** temporaire de `1000000` octets.
- Alloue dans la PSRAM le buffer d’entrée utilisé par Edge Impulse.
- Exécute une fois `run_classifier()` avec une entrée de test remplie de zéros.
- Lit la taille de Tensor Arena initialement configurée par Edge Impulse.
- Mesure la Tensor Arena réellement utilisée par TensorFlow Lite Micro après l’allocation des tenseurs.
- Affiche un avertissement si l’Arena générée par Edge Impulse est plus petite que l’utilisation réellement mesurée.
- Calcule automatiquement trois tailles finales suggérées :
  - **SMALL = M + 16 KiB**
  - **MEDIUM = M + 32 KiB** — valeur de départ recommandée
  - **LARGE = M + 64 KiB**
- Affiche les deux lignes exactes du header du modèle qui doivent être mises à jour avec la valeur sélectionnée.

Le sketch est un outil de diagnostic et de dimensionnement. Il ne remplace **pas** le code caméra ou capteur d’un projet final.

## 🔄 Workflow

Le workflow complet de dimensionnement de la Tensor Arena est le suivant :

1. Créez, entraînez et exportez votre modèle Edge Impulse sous forme d’**Arduino library** en utilisant **TensorFlow Lite**.
2. Installez la bibliothèque ZIP exportée dans Arduino IDE.
3. Configurez Arduino IDE pour votre carte ESP32-S3 et sa PSRAM.
4. Dans le sketch de diagnostic, modifiez uniquement l’inclusion du header d’inférence Edge Impulse :
   ```cpp
   #include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS
   ```
5. Sauvegardez le fichier Edge Impulse original :
   ```text
   src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
   ```
6. Remplacez-le temporairement par le fichier modifié fourni dans ce dépôt :
   ```text
   edge-impulse-sdk-patch/tflite_micro.h
   ```
7. Compilez et téléversez le Tensor Arena Size Finder.
8. Lisez dans le Serial Monitor la valeur `M` mesurée automatiquement ainsi que les recommandations **SMALL / MEDIUM / LARGE** générées.
9. Mettez à jour les **deux** définitions de Tensor Arena dans :
   ```text
   src/tflite-model/tflite_learn_XXXXXXX_ZZ.h
   ```
10. Restaurez le fichier **`tflite_micro.h` original d’Edge Impulse**.
11. Compilez et testez votre **application finale** avec le fichier SDK Edge Impulse original et la nouvelle valeur de Tensor Arena.

> [!IMPORTANT]
> Le fichier `tflite_micro.h` modifié sert uniquement d’outil de diagnostic. Une fois la bonne valeur de Tensor Arena enregistrée dans le header de votre modèle, restaurez le fichier Edge Impulse original avant de revenir à votre application finale.
>
> Si vous exportez un nouveau modèle ou une nouvelle version de la bibliothèque Arduino Edge Impulse, recommencez la mesure car la taille de Tensor Arena requise peut changer.

---

## 📖 Quelques termes techniques

Avant de continuer, voici deux termes techniques utilisés tout au long de ce guide :

**TensorFlow Lite Micro** (également appelé **TFLite Micro** ou **TFLM**) est un runtime C++ léger conçu pour exécuter directement des modèles de machine learning entraînés sur des microcontrôleurs et autres appareils embarqués disposant de ressources mémoire limitées.

Une **Tensor Arena** est un bloc unique de RAM réservé à l’avance pour TensorFlow Lite Micro. Pendant l’inférence, le runtime utilise cette mémoire pour stocker les entrées et sorties du modèle, les activations intermédiaires, les données persistantes du modèle ainsi que les buffers temporaires de travail.

L’Arena doit être suffisamment grande pour accueillir toutes ces allocations. Si elle est trop petite, TensorFlow Lite Micro ne peut pas allouer les tenseurs du modèle et l’inférence échoue. Si elle est inutilement grande, elle réserve de la SRAM interne qui pourrait autrement être utilisée par la caméra, l’écran, le Wi-Fi ou le code applicatif.

## 💾 Comprendre la mémoire de l’ESP32-S3

L’ESP32-S3 utilise plusieurs types de mémoire, chacun ayant un rôle différent :

- **Flash memory** : stockage à long terme. Elle contient le programme Arduino compilé, la bibliothèque Edge Impulse, le modèle TensorFlow Lite Micro et d’autres fichiers statiques. La Flash conserve son contenu lorsque la carte est éteinte.

- **Internal SRAM** : mémoire de travail rapide de l’ESP32-S3. Elle est utilisée pendant l’exécution pour les variables, les stacks, les buffers temporaires et, par défaut, la Tensor Arena de TensorFlow Lite Micro. La SRAM interne étant limitée, la taille de la Tensor Arena doit être choisie avec soin.

- **PSRAM** : mémoire de travail externe supplémentaire disponible sur certaines cartes ESP32-S3. Elle est plus importante que la SRAM interne mais généralement plus lente. Dans ce sketch, la PSRAM est utilisée pour le grand buffer d’entrée image, ce qui permet de préserver la SRAM interne pour la Tensor Arena de TensorFlow Lite Micro et le reste de l’application.

La SRAM interne et la PSRAM sont toutes deux volatiles : leur contenu est perdu lors d’un reset ou d’une coupure d’alimentation.

- **microSD card** : stockage amovible non volatil. Elle peut contenir de grands jeux de données, des images capturées par la caméra, des logs, des fichiers de configuration ou d’autres fichiers utilisateur, et conserve son contenu lorsque la carte est éteinte. Contrairement à la Flash, elle ne stocke ni n’exécute normalement le programme Arduino ; contrairement à la SRAM et à la PSRAM, elle ne sert pas de mémoire de travail pour les variables ou la Tensor Arena de TensorFlow Lite Micro. L’ESP32-S3 lit et écrit les fichiers sur la carte via une interface SDMMC ou SPI.

---

## 🎯 À quoi sert ce dépôt ?

Lors du développement et du déploiement d’un projet d’IA embarquée sur un ESP32-S3, deux étapes demandent une attention particulière :

1. Choisir dans Edge Impulse un modèle d’IA capable de réellement fonctionner sur un ESP32-S3. Edge Impulse propose de nombreux learning blocks, architectures de modèles et options de déploiement, mais toutes les combinaisons ne sont pas adaptées aux contraintes de mémoire et de performances de l’ESP32-S3.

2. Déployer le modèle sous forme de bibliothèque Arduino et choisir la bonne taille de Tensor Arena pour TensorFlow Lite Micro.

**Ce dépôt fournit des informations concrètes et mesurées sur ces deux points critiques : choisir un modèle Edge Impulse capable de fonctionner sur un ESP32-S3 et configurer la mémoire nécessaire à la Tensor Arena de TensorFlow Lite Micro. Il permet d’éviter qu’un modèle entraîné avec succès se révèle impossible à déployer ou à exécuter de manière fiable sur l’ESP32-S3.**

Ce projet est le résultat de nombreux tests visant à trouver une combinaison fonctionnelle de modèle, taille d’entrée, paramètres de déploiement, configuration mémoire et taille de Tensor Arena. En rendant ce travail public, l’objectif est de faire gagner du temps aux hobbyistes et makers souhaitant démarrer un projet similaire.

## 📌 Une configuration de référence pratique pour l’ESP32-S3

L’exemple à l’origine de ce dépôt est un projet Edge Impulse de classification d’images destiné à reconnaître chacune des faces d’un dé. Après de nombreux essais, la combinaison suivante s’est révélée fonctionnelle sur un ESP32-S3 :

- Taille d’image de l’impulse : **224 x 224 RGB**
- Learning block Edge Impulse : **Transfer Learning (Images)**
- Modèle de base : **MobileNetV1 96x96 0.25**
- Temps DSP : environ **71 ms**
- Temps de classification du réseau neuronal : environ **429 ms**
- Temps total d’inférence : environ **500 ms**
- Débit théorique : environ **2 images par seconde**
- Débit pratique : proche de **2 images par seconde**, avant d’inclure la capture caméra, la conversion d’image, l’affichage, le Wi-Fi, les logs ou les autres tâches de l’application.

Pour ce projet de reconnaissance de dé, une résolution d’image plus faible ou des images en niveaux de gris plutôt qu’en RGB auraient probablement suffi, réduisant les besoins mémoire et la charge d’inférence du modèle. Le choix d’un impulse Edge Impulse en 224 x 224 RGB avait pour objectif d’explorer la résolution d’image pratique la plus élevée pouvant être traitée sur un ESP32-S3 pour de futurs projets plus exigeants.

Dans Edge Impulse, la largeur et la hauteur sélectionnées dans **Impulse Design > Create Impulse** définissent la taille d’image traitée par le bloc **Image** et attendue par le classifieur exporté. Dans ce projet, l’impulse est configuré pour des images RGB de 224 x 224.

La partie `96x96` du nom `MobileNetV1 96x96 0.25` correspond à la préformation et à la résolution d’entrée nominale du modèle. Edge Impulse indique 96 x 96 comme résolution d’entrée optimale du modèle, mais permet également de choisir une autre résolution dans l’impulse. L’utilisation de 224 x 224 augmente donc la quantité de données d’entrée, le travail de prétraitement, l’utilisation mémoire et généralement le coût d’inférence par rapport à un impulse 96 x 96.

La valeur `0.25` est le width multiplier de MobileNet. Elle réduit le nombre de canaux du réseau neuronal, diminuant ainsi la taille du modèle, le besoin en RAM et le temps de calcul par rapport à des variantes MobileNet plus larges.

> [!NOTE]
> **MobileNetV1 96x96 0.25 a été le modèle testé le plus adapté à ce projet ESP32-S3. Il offre un compromis exploitable entre précision de classification, utilisation mémoire et temps d’inférence. Cela ne signifie pas que MobileNetV1 96x96 0.25 soit automatiquement le meilleur choix pour tous les projets ESP32-S3.**

Le choix du modèle dépend de la précision recherchée, de la taille d’image définie dans l’impulse, du nombre de classes, de la mémoire disponible et du temps d’inférence réseau acceptable. Le **Neural-network inference time (NN time)** correspond au temps nécessaire à l’ESP32-S3 pour exécuter le réseau neuronal entraîné et produire les probabilités de classification.

Il est indiqué séparément du temps DSP dans Edge Impulse :

- **DSP time** : prétraitement de l’image et préparation des features, par exemple la conversion de l’image capturée vers le buffer numérique requis par le classifieur.
- **NN time** (ou classification time) : exécution du réseau neuronal lui-même, depuis le tenseur d’entrée préparé jusqu’aux probabilités finales des classes.

Pour le test de référence de ce dépôt :

```text
DSP time: 71 ms
NN time: 429 ms
```

Le temps total de traitement de la classification est donc d’environ :

```text
71 ms + 429 ms = 500 ms
```

**Cela correspond approximativement à un maximum théorique de 2 inférences par seconde.**

Le débit réel de bout en bout peut être inférieur car un projet complet peut également devoir capturer une image, convertir les pixels, mettre à jour un écran, écrire des logs, communiquer en Wi-Fi ou effectuer d’autres tâches entre deux inférences.

---

## 🚧 Le problème de la Tensor Arena

TensorFlow Lite Micro utilise une **Tensor Arena** : un bloc de RAM réservé aux tenseurs, activations intermédiaires, données persistantes du modèle et buffers temporaires nécessaires pendant l’inférence.

L’Arena doit être suffisamment grande pour que TensorFlow Lite Micro puisse allouer correctement les tenseurs du modèle.

Si elle est trop petite :

- `AllocateTensors()` peut échouer.
- `run_classifier()` peut retourner une erreur telle que `-3`.
- L’inférence ne peut pas démarrer correctement.

Si elle est beaucoup plus grande que nécessaire :

- Une quantité excessive de RAM est réservée pour le modèle.
- Il reste moins de mémoire disponible pour la caméra, l’écran, le Wi-Fi, la logique applicative et les autres buffers.

Ce dépôt évite l’ancien workflow par essais successifs qui consistait à modifier manuellement le header du modèle avec une grande valeur temporaire avant de commencer.

À la place, le sketch de diagnostic définit :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

avant d’inclure le header d’inférence Edge Impulse. Le fichier `tflite_micro.h` modifié fourni dans ce dépôt utilise cette valeur **temporairement pendant le diagnostic**, tout en conservant la taille d’Arena initialement générée par Edge Impulse afin de pouvoir la comparer.

Une fois que TensorFlow Lite Micro a correctement alloué ses tenseurs, le fichier SDK modifié récupère :

```cpp
interpreter->arena_used_bytes()
```

et transmet cette valeur mesurée au sketch Arduino.

Le sketch affiche alors :

```text
Tensor Arena used  : <M> bytes
```

`M` est la quantité de Tensor Arena mesurée pour ce modèle exporté et cette configuration de runtime précise.

Pour le modèle de référence utilisé dans ce dépôt :

```text
Tensor Arena used  : 536044 bytes
```

ce qui donne :

```text
M = 536044 bytes
```

Le sketch ajoute ensuite automatiquement des marges de sécurité et affiche les valeurs finales recommandées.

> [!IMPORTANT]
> Ne configurez jamais la Tensor Arena finale en dessous de la valeur mesurée `M`.
>
> Les marges proposées sont des valeurs de départ pratiques et non des garanties universelles. Testez à nouveau l’application finale après avoir appliqué la valeur choisie.

---

## ✅ Prérequis

Vous avez besoin de :

- Une **carte ESP32-S3 équipée de PSRAM**.

  Ce dépôt a été testé avec une **Goouuu ESP32-S3 N16R8**, carte compatible DevKitC-1 disposant de **16 MB de Flash** et **8 MB de PSRAM**.
  D’autres cartes ESP32-S3 disposant d’une quantité suffisante de PSRAM peuvent également fonctionner, par exemple une ESP32-S3 DevKitC-1 équipée d’un module N8R8 ou N16R8, une carte basée sur un ESP32-S3-WROOM-1 avec PSRAM, une XIAO ESP32S3 Sense, ou d’autres cartes de développement ESP32-S3 disposant de PSRAM.

> [!NOTE]
> Afin de garder ce guide clair et reproductible, les paramètres Arduino IDE et recommandations techniques présentés ici sont basés sur la configuration de référence **ESP32-S3 N16R8** : **16 MB de Flash** et **8 MB de PSRAM**.
>
> Si vous utilisez une autre carte ESP32-S3 ou une autre configuration mémoire, le sketch peut tout de même fonctionner, mais vous devez adapter les paramètres Arduino IDE — en particulier **Board**, **Flash Size**, **PSRAM** et **Partition Scheme** — à votre matériel.

- Arduino IDE 2.x.
- Le package de cartes ESP32 d’Espressif installé dans Arduino IDE.
- Un projet Edge Impulse exporté sous forme de bibliothèque Arduino avec **TensorFlow Lite**.
- Le sketch `ESP32-S3_arduino_tflite_micro_arena_size_finder.ino` fourni dans ce dépôt.
- Le fichier de diagnostic modifié fourni dans :
  ```text
  edge-impulse-sdk-patch/tflite_micro.h
  ```
- Une sauvegarde du fichier Edge Impulse `tflite_micro.h` original avant son remplacement temporaire.

---

## 🏗️ Étape 1 : créer et exporter le modèle Edge Impulse

### 1.1 🆕 Créer le projet Edge Impulse

Dans Edge Impulse Studio, cliquez sur **Create new project**, donnez un nom au projet, puis sélectionnez **Espressif ESP-EYE (ESP32 240MHz)** comme target device.

Même si ce projet fonctionne sur une carte caméra ESP32-S3 et non sur l’ESP-EYE d’origine, cette cible constitue un point de départ pratique dans Edge Impulse pour obtenir des estimations de performances adaptées à l’ESP32. La compatibilité finale et les besoins mémoire doivent néanmoins être vérifiés sur la vraie carte ESP32-S3.

### 1.2 📸 Créer le jeu de données d’images

![Créer le jeu de données avec des images 224x224](images/dataset.PNG)

Avant de créer l’impulse, constituez et étiquetez le jeu de données qui servira à entraîner le modèle.

Dans Edge Impulse Studio, ouvrez la page **Data acquisition**. Vous pouvez capturer des images directement depuis une caméra connectée ou téléverser des fichiers image existants. Pour chaque image, sélectionnez le bon label et assurez-vous que chaque face du dé possède sa propre classe.

Pour ce projet de reconnaissance de dé, créez les classes suivantes : `one`, `two`, `three`, `four`, `five`, `six`, `background`.

La classe `background` est importante. Elle contient les images dans lesquelles aucune face de dé ne doit être reconnue — par exemple un fond blanc vide, la table, la vue caméra sans dé ou d’autres objets non pertinents. Elle aide le modèle à apprendre la différence entre un dé et son environnement, réduisant ainsi les faux positifs.

Pour une classification d’objets en gros plan, faites en sorte que le dé occupe environ 60 % à 80 % de l’image. Il doit être suffisamment grand pour que ses points et ses contours restent visibles après redimensionnement, tout en conservant un peu de fond autour de lui.

> [!TIP]
> Si possible, utilisez un fond simple, neutre et cohérent, comme une feuille de papier blanc. Capturez les images de fond dans les mêmes conditions d’éclairage et de caméra que celles utilisées en fonctionnement normal. Incluez ce fond comme une classe de reconnaissance à part entière.

Pour chaque classe correspondant à une face du dé, capturez au moins 30 à 50 images comme point de départ ; davantage d’images donnent généralement un modèle plus fiable. Essayez de conserver approximativement le même nombre d’images pour chaque classe, y compris `background`.

Faites varier volontairement les images :

- Modifiez la position et la rotation du dé.
- Rapprochez ou éloignez légèrement le dé de la caméra.
- Faites varier l’intensité et la direction de l’éclairage.
- Introduisez de petites variations d’ombres et de cadrage.
- Gardez le dé bien visible et net.
- Utilisez des images capturées avec la même caméra OV2640 et la même configuration que celles qui seront utilisées avec l’ESP32-S3.

N’utilisez pas uniquement des photographies presque identiques. Le jeu de données doit représenter les conditions réelles dans lesquelles le modèle sera utilisé. Un modèle entraîné uniquement avec un dé parfaitement centré sur un fond blanc peut échouer si le dé est tourné, décalé, partiellement dans l’ombre ou observé sous un éclairage différent.

Utilisez la majorité des images comme **Training data** et réservez des images distinctes comme **Test data**. Une répartition pratique de départ est d’environ 80 % pour l’entraînement et 20 % pour le test.

### 1.3 🧩 Créer l’impulse

Après avoir créé et étiqueté votre jeu de données dans la page **Data acquisition**, ouvrez **Impulse #x > Create impulse** afin de configurer votre impulse de classification d’images.

![Create Impulse 224x224](images/Edge-Impulse_Create-Impulse.PNG)

1. Ouvrez **Impulse #x > Create impulse**.
2. Réglez **Image width** et **Image height** sur `224`.
3. Ajoutez le processing block **Images**.
4. Ajoutez le learning block **Transfer Learning (Images)**.
5. Cliquez sur **Save impulse**.
6. Ouvrez la page **Image**, sélectionnez la profondeur de couleur adaptée à votre caméra — généralement **RGB** — puis enregistrez les paramètres.
7. Sur cette même page, ouvrez l’onglet **Generate features** puis cliquez sur **Generate features**.
8. Ouvrez l’onglet **Transfer Learning**, configurez le modèle puis entraînez-le avec vos images étiquetées.

Le modèle obtenu attend des images de 224×224 pixels capturées par la caméra connectée à l’ESP32-S3.

### 1.4 🧠 Configurer le modèle Transfer Learning

<table>
<tr>
<td valign="top">

Ouvrez l’onglet **Transfer Learning** afin de configurer et entraîner le modèle de classification d’images.

Les réglages ci-dessous constituent un point de départ utile, pas des exigences absolues. Entraînez d’abord le modèle, examinez ses performances, puis ajustez si nécessaire le nombre de cycles d’entraînement, le learning rate, les paramètres de data augmentation ou l’architecture du modèle.

- **Number of training cycles:** commencez avec `50`.
- **Learning rate:** commencez avec `0.001`.
- **Training processor:** sélectionnez **CPU**.
- **Data augmentation:** activez cette option si vous disposez d’un petit nombre d’images d’entraînement — par exemple 15 à 20 images par classe. Elle crée des variantes des images d’entraînement, par exemple avec de légères modifications de position, de cadrage ou de luminosité, ce qui aide le modèle à mieux généraliser. Elle ne remplace pas la collecte de vraies images variées.
- Sous **Neural network architecture**, cliquez sur **Choose a different model** et sélectionnez **MobileNetV1 96x96 0.25**. Ce modèle compact est un bon choix pour un ESP32-S3 car il utilise nettement moins de RAM et de Flash que les architectures plus lourdes.

Cliquez sur **Save & train** pour lancer l’entraînement.

</td>
<td width="360" valign="top">

<img src="images/Transfer_learning.PNG"
     alt="Configuration Transfer Learning dans Edge Impulse"
     width="340">

</td>
</tr>
</table>

> [!IMPORTANT]
> La qualité de reconnaissance dépend en premier lieu du jeu de données : nombre d’images par classe, exactitude des labels, netteté, conditions d’éclairage, angle de caméra, distance au dé, variation du fond et correspondance entre les images d’entraînement et les conditions réelles dans lesquelles la caméra ESP32-S3 sera utilisée. Incluez des exemples représentatifs de chaque face du dé et évitez de n’utiliser que des images presque identiques.
>
> Le modèle joue lui aussi un rôle important : c’est l’architecture du réseau neuronal qui apprend les motifs visuels à partir des images d’entraînement puis les utilise pour classifier de nouvelles images. Un modèle plus grand peut potentiellement reconnaître des motifs plus complexes, mais il nécessite davantage de mémoire Flash, de RAM et de temps de calcul sur l’ESP32-S3.
>
> **MobileNetV1 96×96 0.25** est une architecture compacte choisie ici comme compromis pratique entre performances de reconnaissance et ressources limitées d’un appareil embarqué. Selon Edge Impulse, elle nécessite environ 105,9 KB de RAM et 301,6 KB de ROM. Cela la rend adaptée à un déploiement sur ESP32-S3 tout en laissant de la mémoire disponible pour le buffer image de la caméra, la Tensor Arena de TensorFlow Lite Micro et le reste de l’application.
>
> **Data augmentation** améliore la robustesse en créant des exemples d’entraînement modifiés, comme de petits changements de cadrage, de position, de luminosité ou d’orientation. Elle est particulièrement utile avec un petit jeu de données mais ne peut pas compenser des images mal étiquetées, trop similaires, floues ou peu représentatives des conditions réelles.

### 1.5 📤 Sélectionner la cible de déploiement

Sur la page **Deployment**, configurez l’export comme suit :

1. Sous **Deployment target**, sélectionnez **Arduino library**.
2. Sous **Inference engine**, sélectionnez **TensorFlow Lite**.
3. Sous **Model optimizations and performance**, sélectionnez **Quantized (int8)**.
4. Cliquez sur **Build** pour générer et télécharger le fichier ZIP de la bibliothèque Arduino.

N’extrayez pas le fichier ZIP téléchargé. Il sera importé directement dans Arduino IDE à l’étape suivante.

![Deployment target](images/Deployment_target.PNG)

> [!IMPORTANT]
> À partir d’août 2026, sélectionnez **TensorFlow Lite** et n’utilisez **pas** le **EON compiler** / la variante optimisée EON pour ce workflow Arduino ESP32-S3. Les déploiements EON ont provoqué des problèmes de compatibilité et de gestion mémoire dans des projets Arduino ESP32-S3, notamment des résultats d’inférence incorrects, des plantages, des boucles de redémarrage et des erreurs de heap corrompue. **TensorFlow Lite Micro est l’option la plus fiable pour cette procédure.**
>
> Ce dépôt mesure également la Tensor Arena de TensorFlow Lite Micro avec `arena_used_bytes()`. La mesure obtenue s’applique donc uniquement au modèle et au runtime TensorFlow Lite, pas à un modèle compilé avec EON.
>
> Sélectionnez **Quantized (int8)** et non **Unoptimized (float32)**. Un modèle int8 utilise des valeurs entières 8 bits au lieu de valeurs flottantes 32 bits. Il est nettement plus compact et généralement plus rapide sur ESP32-S3, tout en utilisant moins de Flash et de RAM.
>
> La taille de la Tensor Arena dépend du modèle réellement déployé. Si vous passez de int8 à float32, changez la taille d’image, l’architecture du modèle ou le nombre de classes, générez une nouvelle bibliothèque et mesurez à nouveau la Tensor Arena.

---

## 📥 Étape 2 : installer votre projet Edge Impulse comme bibliothèque Arduino

Importez dans Arduino IDE la bibliothèque ZIP générée par Edge Impulse.

1. Ouvrez **Arduino IDE**.
2. Sélectionnez **Sketch > Include Library > Add .ZIP Library…**
3. Sélectionnez le fichier ZIP téléchargé depuis la page **Deployment** d’Edge Impulse.
4. Attendez le message confirmant que la bibliothèque a bien été installée.

N’extrayez pas le ZIP avant de l’importer. Arduino IDE installe directement la bibliothèque depuis l’archive téléchargée.

Sous Windows, le chemin complet ressemble souvent à :

```text
C:\Users\<your-user-name>\Documents\Arduino\libraries\<your-project-name>\
```

Dans cette bibliothèque, vous trouverez :

```text
src\
src\<your-model>_inferencing.h
src\tflite-model\
src\edge-impulse-sdk\
```

Ces dossiers seront importants plus loin dans la procédure.

---

## ⚙️ Étape 3 : configurer Arduino IDE

Connectez votre ESP32-S3 et vérifiez le menu **Tools** dans Arduino IDE.

| Menu Arduino IDE | Réglage recommandé | Signification |
|---|---|---|
| **Tools > Board** | Sélectionnez votre vraie carte ESP32-S3, par exemple `ESP32S3 Dev Module` ou `XIAO ESP32S3 Sense` | Le choix de la carte contrôle les options de Flash, PSRAM, USB, upload et partition disponibles |
| **Tools > Port** | Sélectionnez le port série attribué à l’ESP32-S3 | Nécessaire pour l’upload et l’affichage du Serial Monitor |
| **Tools > USB CDC On Boot** | `Enabled` | S’il est désactivé, la sortie série peut être invisible via USB sur les cartes utilisant l’USB natif |
| **Tools > Flash Size** | `16MB (128Mb)` pour une carte ESP32-S3 N16R8 | `N16R8` signifie 16 MB de Flash et 8 MB de PSRAM |
| **Tools > PSRAM** | Activez le bon mode PSRAM, souvent `OPI PSRAM` | Le sketch alloue le buffer d’entrée dans la PSRAM |
| **Tools > Partition Scheme** | `16M Flash (3MB APP / 9.9MB FATFS)` | Garantit que la table de partitions utilise les 16 MB de Flash de la carte de référence |
| **Tools > Core Debug Level** | `None` ou `Error` | Optionnel ; réduit les messages série non liés au diagnostic |

> [!TIP]
> La Tensor Arena utilise de la **RAM**, pas l’espace de partition Flash. Modifier le **Partition Scheme** n’augmente donc pas automatiquement la taille possible de la Tensor Arena.

---

## 🛠️ Étape 4 : adapter le sketch à votre bibliothèque Arduino Edge Impulse

Ouvrez le sketch fourni dans ce dépôt avec Arduino IDE.

Au début du sketch, le bloc de configuration utilisateur contient la définition de l’Arena de diagnostic suivie du header d’inférence Edge Impulse :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000

#include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS
```

Seule la ligne marquée `CHANGE THIS` doit normalement être adaptée.

### 4.1 🔗 Inclure votre header d’inférence

Remplacez `YOUR_LIBRARY_inferencing.h` par le nom exact du fichier header d’inférence généré par votre projet Edge Impulse.

Par exemple, si votre bibliothèque Arduino Edge Impulse s’appelle **Dice**, utilisez :

```cpp
#include <Dice_inferencing.h>
```

Pour trouver le nom exact, ouvrez :

```text
...\Arduino\libraries\<your-project-name>\src\
```

et cherchez le fichier dont le nom se termine par :

```text
_inferencing.h
```

> [!IMPORTANT]
> Le macro de diagnostic doit rester **avant** le header d’inférence Edge Impulse :
>
> ```cpp
> #define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
> #include <YOUR_LIBRARY_inferencing.h>
> ```
>
> Le fichier `tflite_micro.h` modifié lit ce macro pendant la compilation de la bibliothèque Edge Impulse.

### 4.2 📏 Taille de l’Arena de diagnostic

La taille de diagnostic par défaut est :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

Cette valeur est temporaire. Elle ne remplace **pas** la valeur finale de Tensor Arena enregistrée dans le header de votre modèle.

Pour le projet de référence, `1000000` octets est suffisamment grand pour permettre à TensorFlow Lite Micro d’allouer le modèle et de mesurer son utilisation réelle.

Si `run_classifier()` échoue avec une erreur liée à l’allocation, telle que `-3`, et que l’utilisation de la Tensor Arena n’est pas mesurée, vous pouvez essayer une valeur de diagnostic plus grande, par exemple :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1500000
```

ou :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 2000000
```

N’augmentez pas cette valeur sans limite. Si la carte ne compile plus, ne démarre plus ou ne parvient plus à allouer l’Arena de diagnostic, réduisez les besoins mémoire du modèle ou vérifiez la configuration mémoire de la carte.

---

## 🧩 Étape 5 : installer temporairement le fichier `tflite_micro.h` modifié

Le dépôt fournit un fichier SDK Edge Impulse prêt à l’emploi :

```text
edge-impulse-sdk-patch/tflite_micro.h
```

Il remplace l’ancienne procédure qui consistait à modifier manuellement plusieurs lignes du SDK Edge Impulse.

### 5.1 💾 Sauvegarder le fichier original

Localisez le fichier original installé avec votre bibliothèque Arduino Edge Impulse :

```text
...\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
```

Avant de le remplacer, faites-en une sauvegarde. Vous pouvez par exemple le copier dans un endroit sûr ou enregistrer une copie sous le nom :

```text
tflite_micro.h.original
```

### 5.2 🔄 Le remplacer temporairement

Copiez la version fournie dans le dépôt :

```text
edge-impulse-sdk-patch/tflite_micro.h
```

à la place du fichier original dans :

```text
...\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\
```

Le nom du fichier utilisé par la bibliothèque doit rester :

```text
tflite_micro.h
```

Le fichier modifié effectue trois tâches de diagnostic :

1. Il utilise temporairement `EI_TFLITE_ARENA_DIAGNOSTIC_SIZE` à la place de la taille d’Arena du header du modèle.
2. Il enregistre la taille d’Arena initialement configurée par Edge Impulse.
3. Après la réussite de `AllocateTensors()`, il enregistre `interpreter->arena_used_bytes()` afin que le sketch puisse calculer automatiquement les valeurs finales recommandées.

Toutes les modifications spécifiques à ce projet sont clairement délimitées dans le fichier par des commentaires tels que :

```cpp
// ============================================================================
// MODIFICATION - ESP32-S3 Tensor Arena Size Finder
// ============================================================================

// modified code

// ============================================================================
// END MODIFICATION
// ============================================================================
```

> [!WARNING]
> Ce fichier est un **remplacement temporaire destiné au diagnostic**, pas un remplacement permanent du fichier SDK Edge Impulse.
>
> Une fois la valeur de Tensor Arena choisie et écrite dans le header de votre modèle, restaurez le fichier `tflite_micro.h` original avant de compiler votre application finale.
>
> Le fichier modifié est basé sur la version du SDK Edge Impulse utilisée pour ce projet. Si une future version exportée par Edge Impulse contient un `tflite_micro.h` sensiblement différent, comparez les deux versions avant de le remplacer.

---

## ⚡ Étape 6 : compiler, téléverser et exécuter le diagnostic

1. Ouvrez le sketch `.ino` adapté dans Arduino IDE.
2. Vérifiez que le fichier `tflite_micro.h` modifié est bien installé dans la bibliothèque Edge Impulse.
3. Cliquez sur **Upload** pour compiler et téléverser le programme sur l’ESP32-S3.
4. Ouvrez **Tools > Serial Monitor** et réglez le débit sur **115200**.
5. Si aucune sortie n’apparaît, appuyez sur le bouton **Reset** de la carte.

> [!NOTE]
> La première compilation peut prendre plus de 10 minutes car Arduino IDE doit compiler la bibliothèque Edge Impulse, TensorFlow Lite Micro et leurs dépendances. Les compilations suivantes sont généralement plus rapides.

Une exécution réussie est organisée en quatre sections :

```text
[1] MEMORY CHECK
[2] DIAGNOSTIC CONFIGURATION
[3] INFERENCE TEST
[4] ARENA RECOMMENDATION
```

La mesure la plus importante est :

```text
Tensor Arena used  : <M> bytes
```

Pour le modèle de référence :

```text
Tensor Arena used  : 536044 bytes
```

ce qui signifie :

```text
M = 536044 bytes
```

Le sketch indique également l’Arena initialement générée par Edge Impulse :

```text
Edge Impulse arena : 519232 bytes
Measured usage (M) : 536044 bytes
```

Comme `519232 < 536044`, le sketch affiche :

```text
WARNING: The arena generated by Edge Impulse is smaller
than the measured arena usage.
```

C’est précisément le type d’écart que cet outil de diagnostic est destiné à détecter.

---

## 🧮 Étape 7 : lire les tailles finales calculées automatiquement

Le sketch effectue automatiquement les calculs de marge de sécurité.

Il utilise :

```text
SMALL  = M + 16 KiB
MEDIUM = M + 32 KiB
LARGE  = M + 64 KiB
```

Pour la mesure de référence :

```text
M = 536044 bytes
```

le sketch calcule :

| Recommandation | Calcul | Taille finale de l’Arena |
|---|---:|---:|
| **SMALL** | 536044 + 16384 | **552428 bytes** |
| **MEDIUM** | 536044 + 32768 | **568812 bytes** |
| **LARGE** | 536044 + 65536 | **601580 bytes** |

Le Serial Monitor affiche :

```text
SMALL  : 552428 bytes  (+16 KiB)
MEDIUM : 568812 bytes  (+32 KiB)  <-- RECOMMENDED
LARGE  : 601580 bytes  (+64 KiB)

Recommended value: 568812 bytes
```

Pour ce projet, **MEDIUM** est la valeur de départ recommandée car elle offre une marge de sécurité utile sans conserver la grande Arena temporaire de diagnostic.

> [!IMPORTANT]
> La valeur mesurée `M` est la limite basse observée pour cette configuration exacte de modèle et de runtime. Ne configurez pas l’Arena finale en dessous.
>
> SMALL / MEDIUM / LARGE correspondent à des marges de sécurité suggérées. Ce ne sont pas des garanties universelles. Compilez et testez toujours l’application finale après avoir appliqué la valeur choisie.

---

## 💾 Étape 8 : enregistrer la valeur finale de Tensor Arena et restaurer le SDK original

Le sketch de diagnostic vous indique exactement quel header du modèle doit être mis à jour :

```text
src/tflite-model/tflite_learn_XXXXXXX_ZZ.h
```

Ouvrez le fichier correspondant dans votre bibliothèque Arduino Edge Impulse installée :

```text
...\Arduino\libraries\<your-project-name>\
src\tflite-model\tflite_learn_XXXXXXX_ZZ.h
```

Cherchez les deux définitions de Tensor Arena.

Pour le modèle de référence, l’export Edge Impulse original contient :

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 519232
const size_t tflite_learn_1072522_47_arena_size = 519232;
```

Le diagnostic recommande :

```text
MEDIUM : 568812 bytes  (+32 KiB)  <-- RECOMMENDED
```

Mettez à jour les **deux** lignes avec la même valeur sélectionnée :

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 568812
const size_t tflite_learn_1072522_47_arena_size = 568812;
```

Enregistrez le header du modèle.

### 8.1 ♻️ Restaurer le fichier `tflite_micro.h` original

La mesure est maintenant terminée.

Supprimez la version de diagnostic et restaurez le fichier Edge Impulse original sauvegardé à l’étape 5 :

```text
src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
```

Votre projet final doit donc utiliser :

- la **nouvelle valeur de Tensor Arena** dans `tflite_learn_XXXXXXX_ZZ.h` ;
- le fichier **`tflite_micro.h` original d’Edge Impulse**.

Compilez et testez ensuite à nouveau votre **application finale**.

> [!IMPORTANT]
> Le Tensor Arena Size Finder lui-même dépend du fichier `tflite_micro.h` de diagnostic modifié. Si vous souhaitez relancer le size finder plus tard, réinstallez temporairement le fichier modifié.
>
> Si vous réentraînez le modèle, modifiez les dimensions d’entrée, changez d’architecture, changez la quantification ou exportez une nouvelle version de la bibliothèque Edge Impulse, mesurez à nouveau la Tensor Arena.

---

## 🖥️ Exemple de sortie série

Une exécution de diagnostic réussie pour le modèle de référence ressemble à ceci :

```text
SETUP STARTING

=== ESP32-S3 + Edge Impulse Tensor Arena Size Finder ===

[1] MEMORY CHECK
Internal heap free : 342904 bytes
PSRAM free         : 8386076 bytes
PSRAM total        : 8388608 bytes
PSRAM              : OK

[2] DIAGNOSTIC CONFIGURATION
Diagnostic arena   : 1000000 bytes

The diagnostic arena temporarily overrides the Edge Impulse
arena size for this test only.

[3] INFERENCE TEST
Input buffer       : 602112 bytes in PSRAM
run_classifier()   : OK
Tensor Arena used  : 536044 bytes

[4] ARENA RECOMMENDATION
Edge Impulse arena : 519232 bytes
Measured usage (M) : 536044 bytes

WARNING: The arena generated by Edge Impulse is smaller
than the measured arena usage.

SMALL  : 552428 bytes  (+16 KiB)
MEDIUM : 568812 bytes  (+32 KiB)  <-- RECOMMENDED
LARGE  : 601580 bytes  (+64 KiB)

Recommended value: 568812 bytes

Update BOTH of these lines in your Edge Impulse model header file:
  src/tflite-model/tflite_learn_XXXXXXX_ZZ.h

  #define EI_CLASSIFIER_TFLITE_LEARN_XXXXXXX_ZZ_ARENA_SIZE 568812
  const size_t tflite_learn_XXXXXXX_ZZ_arena_size = 568812;

TEST COMPLETE
```

### 📖 Comment lire cette sortie

| Sortie | Signification |
|---|---|
| `PSRAM : OK` | La PSRAM a été détectée et est disponible pour le sketch de diagnostic |
| `Diagnostic arena : 1000000 bytes` | Arena temporaire utilisée uniquement pendant la mesure |
| `Input buffer : 602112 bytes in PSRAM` | Le buffer d’entrée Edge Impulse de 602112 octets a été correctement alloué dans la PSRAM |
| `run_classifier() : OK` | L’allocation des tenseurs et l’inférence de test se sont terminées correctement |
| `Tensor Arena used : 536044 bytes` | Utilisation de Tensor Arena `M` mesurée pour le modèle de référence |
| `Edge Impulse arena : 519232 bytes` | Taille d’Arena initialement générée par Edge Impulse |
| `WARNING: ... smaller than the measured arena usage` | L’Arena originale de `519232` octets est inférieure à l’utilisation mesurée de `536044` octets |
| `MEDIUM : 568812 bytes (+32 KiB) <-- RECOMMENDED` | Valeur de départ recommandée pour ce modèle de référence |
| `TEST COMPLETE` | Le diagnostic est terminé et les valeurs du header du modèle peuvent maintenant être mises à jour |

La distinction importante est la suivante :

```text
Edge Impulse arena : 519232 bytes
Diagnostic arena   : 1000000 bytes
Measured usage (M) : 536044 bytes
Recommended value  : 568812 bytes
```

Ces valeurs ont des rôles différents :

- **519232 bytes** est la valeur initialement générée dans le header du modèle.
- **1000000 bytes** est l’Arena de diagnostic temporaire utilisée uniquement par le size finder.
- **536044 bytes** correspond à l’utilisation mesurée de la Tensor Arena.
- **568812 bytes** est la valeur MEDIUM recommandée pour le header final du modèle.

> [!IMPORTANT]
> ## 🎯 Les valeurs à retenir
>
> Pour ce modèle de référence précis :
>
> ```text
> Measured usage (M) : 536044 bytes
> Recommended value  : 568812 bytes
> ```
>
> Ne configurez pas l’Arena finale en dessous de `536044` octets. La valeur de départ recommandée par le sketch actuel est `568812` octets, soit une marge de sécurité de `32 KiB`.

---

## 🩺 Dépannage

### ⚠️ Compatibilité caméra et dépannage

Ce dépôt est centré sur le dimensionnement de la Tensor Arena de TensorFlow Lite Micro. Il suppose que la caméra est déjà capable de capturer des frames de manière fiable avant l’ajout du traitement machine learning.

Avant d’appeler `run_classifier()` dans une application caméra finale, validez séparément la caméra avec un sketch minimal :

1. Initialisez l’OV2640 avec le mapping GPIO exact de votre carte.
2. Capturez et retournez des frames de manière répétée.
3. Vérifiez que la taille d’image et le format de pixels choisis fonctionnent de manière fiable.
4. Ajoutez seulement ensuite la conversion d’image, le redimensionnement et l’inférence TensorFlow Lite Micro.

> [!WARNING]
> Une taille d’entrée de modèle comme 96×96 ou 224×224 ne correspond pas nécessairement à un mode de capture de la caméra. La caméra peut capturer une résolution source prise en charge, puis l’application convertit et redimensionne cette frame vers le buffer d’entrée du modèle.
>
> Ne supposez pas qu’une carte équipée d’un OV2640 prend en charge de manière fiable toutes les résolutions ou tous les formats bruts. Vérifiez JPEG, RGB565 et la taille de frame sélectionnée avec la carte, le driver caméra, la version du core Arduino-ESP32 et le mapping de broches réellement utilisés avant d’intégrer Edge Impulse.

### 🔇 Pas de sortie série / la carte semble bloquée

Si votre ESP32-S3 semble se bloquer au démarrage et que rien n’apparaît dans le Serial Monitor :

1. Vérifiez **Tools > USB CDC On Boot** et réglez-le sur **Enabled** lorsque vous utilisez le port USB natif de la carte.
2. Vérifiez que le Serial Monitor est réglé sur **115200 baud**.
3. Appuyez sur le bouton **Reset** après avoir ouvert le Serial Monitor.
4. Évitez un `while (!Serial);` bloquant sans timeout.

Le sketch du projet utilise déjà un timeout :

```cpp
unsigned long start = millis();
while (!Serial && (millis() - start < 2000)) {
    delay(10);
}
```

### ⚡ `PSRAM : NOT DETECTED`

Si la sortie affiche :

```text
PSRAM              : NOT DETECTED
```

vérifiez :

1. Que votre carte dispose réellement de PSRAM.
2. Que **Tools > PSRAM** est activé.
3. Que le mode PSRAM sélectionné correspond à votre carte, souvent `OPI PSRAM` pour les cartes ESP32-S3 N16R8.
4. Que la bonne définition de carte est sélectionnée.

Le diagnostic s’arrête si la PSRAM n’est pas détectée car le grand buffer d’entrée est volontairement alloué dans cette mémoire.

### ❌ `AllocateTensors() failed` ou `run_classifier()` retourne `-3`

Une erreur `-3` liée à l’allocation peut indiquer que la Tensor Arena de diagnostic est encore trop petite.

La valeur par défaut est :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

Vous pouvez essayer une valeur de diagnostic plus grande, par exemple :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1500000
```

ou :

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 2000000
```

Puis recompilez et relancez le diagnostic.

Vérifiez également :

- Que la version modifiée de `tflite_micro.h` fournie par le dépôt est bien installée dans la bibliothèque Edge Impulse.
- Que `EI_TFLITE_ARENA_DIAGNOSTIC_SIZE` est défini **avant** le header d’inférence Edge Impulse.
- Que vous avez sélectionné **TensorFlow Lite**, pas EON, lors du déploiement Edge Impulse.
- Que la bibliothèque Edge Impulse compilée est bien celle que vous avez modifiée.
- Que la PSRAM est activée et détectée.

> [!WARNING]
> N’augmentez pas indéfiniment la taille de l’Arena de diagnostic. Si une valeur plus grande empêche la compilation, le démarrage ou l’allocation, examinez plutôt la taille du modèle et l’utilisation mémoire de la carte.

### ❌ `Tensor Arena usage was not captured`

Si le sketch atteint l’étape de recommandation mais indique que l’utilisation de la Tensor Arena n’a pas pu être mesurée, le fichier SDK modifié est probablement absent, obsolète ou ce n’est pas celui réellement compilé.

Vérifiez :

1. Que le fichier installé dans :
   ```text
   src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
   ```
   est bien la version modifiée fournie par ce dépôt.
2. Que vous avez enregistré le fichier avant de recompiler.
3. Qu’Arduino IDE compile bien la bibliothèque Edge Impulse attendue.
4. Que le fichier modifié contient les blocs du projet marqués :
   ```cpp
   // MODIFICATION - ESP32-S3 Tensor Arena Size Finder
   ```
5. Recompilez et téléversez le sketch après avoir remplacé le fichier SDK.

### 📁 `YOUR_LIBRARY_inferencing.h` introuvable

Remplacez :

```cpp
#include <YOUR_LIBRARY_inferencing.h>
```

par le nom exact du fichier présent dans :

```text
...\Arduino\libraries\<your-project-name>\src\
```

Le nom du fichier doit normalement se terminer par :

```text
_inferencing.h
```

Exemple :

```cpp
#include <Dice_inferencing.h>
```

La casse peut être importante selon le système d’exploitation.

### 🔗 Erreurs impliquant `ei_tflite_arena_used_bytes` ou `ei_tflite_model_arena_configured_bytes`

Ces variables font partie de la liaison de diagnostic entre le sketch et le fichier `tflite_micro.h` modifié.

Si la compilation ou l’édition de liens signale l’un de ces noms :

- Vérifiez que vous utilisez bien le sketch actuel et le fichier `tflite_micro.h` actuel provenant de la même version du dépôt.
- Ne mélangez pas un ancien patch intermédiaire avec le sketch actuel.
- Remplacez à nouveau le fichier SDK par :
  ```text
  edge-impulse-sdk-patch/tflite_micro.h
  ```
- Recompilez le sketch.

### 💽 Le sketch est trop volumineux pour la partition

Il s’agit d’un problème de partition Flash, pas d’un problème de Tensor Arena.

Choisissez un **Partition Scheme** offrant une partition APP plus grande ou créez un fichier `partitions.csv` personnalisé.

Rappel :

- La partition **APP** stocke le programme compilé.
- La **Tensor Arena** utilise la RAM pendant l’exécution.
- Agrandir la partition APP n’augmente ni la SRAM interne ni la PSRAM.

---

## 📚 Références

- [Edge Impulse - Transfer learning for images](https://docs.edgeimpulse.com/studio/projects/learning-blocks/blocks/transfer-learning-images)
- [Edge Impulse - Image classification tutorial](https://docs.edgeimpulse.com/tutorials/end-to-end/image-classification)
- [Edge Impulse - EON Compiler](https://docs.edgeimpulse.com/studio/projects/deployment/eon-compiler)
- [Edge Impulse - Run Arduino library with Arduino IDE 2.x](https://docs.edgeimpulse.com/hardware/deployments/run-arduino-2-0)
- [Espressif Arduino-ESP32 - Partition tables](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html)

---

## ⚖️ Licence

Ce projet est distribué sous **licence MIT** — voir le fichier [LICENSE](LICENSE) pour plus de détails.
