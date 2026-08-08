# Arduino ESP32-S3 + Edge Impulse : trouver la bonne taille de Tensor Arena

# 🚀 Un point de départ pour l’IA embarquée sur ESP32-S3
**Guide accessible aux débutants pour la classification d’images avec Edge Impulse et TensorFlow Lite Micro**

Ce guide constitue un point de départ pratique pour toute personne souhaitant commencer à développer des applications d’IA embarquée sur un ESP32-S3, y compris les débutants complets.

Il explique l’ensemble du flux de travail, depuis la création et l’annotation d’un jeu de données d’images dans Edge Impulse jusqu’à l’entraînement, l’exportation et le déploiement d’un modèle TensorFlow Lite Micro sur un ESP32-S3. Il présente également les concepts essentiels pour comprendre le fonctionnement d’un projet de classification d’images sur un appareil embarqué aux ressources limitées.

Ce projet permet de répondre à trois questions essentielles avant de déployer un modèle d’IA :

1. **Quel modèle d’IA peut fonctionner sur l’ESP32-S3 ?**
2. **Quelle quantité de mémoire TensorFlow Lite Micro Tensor Arena faut-il allouer à ce modèle ?**
3. **Combien d’images par seconde peut-on espérer traiter ?**

L’exemple utilise un modèle de classification d’images de dé, mais la même méthode peut être appliquée à de nombreux autres projets d’IA embarquée utilisant une caméra.

---

## 👋 Introduction

Ce dépôt contient un **sketch Arduino** qui mesure la **Tensor Arena de TensorFlow Lite Micro** utilisée par un **modèle Edge Impulse** sur un **ESP32-S3** équipé de PSRAM.

Au-delà des applications habituelles pour les passionnés et les makers, l’ESP32-S3 permet également d’explorer des projets d’IA directement sur l’appareil, originaux et intéressants, à un coût raisonnable. Son processeur double cœur, sa PSRAM disponible sur certaines cartes, sa connectivité Wi-Fi et Bluetooth et la prise en charge d’instructions orientées IA et DSP constituent une base pratique pour la classification d’images, la reconnaissance sonore, la détection de gestes, l’analyse de capteurs et les appareils d’IA edge connectés.

Avec **[Edge Impulse](https://edgeimpulse.com/)**, l’ESP32-S3 offre un flux de travail permettant de collecter des données, d’entraîner un modèle, d’exporter une bibliothèque Arduino et d’exécuter des inférences sur l’appareil sans devoir construire toute la chaîne de déploiement du machine learning à partir de zéro.

**Ce dépôt s’adresse aux passionnés d’Arduino et aux makers qui ont créé un modèle d’IA avec Edge Impulse et souhaitent le déployer de manière fiable sur un ESP32-S3 sous forme de bibliothèque Arduino.**

> [!NOTE]
> <table>
> <tr>
> <td valign="top">
>
> L’exemple utilisé dans ce dépôt est un modèle de classification d’images créé dans Edge Impulse pour reconnaître chaque face d’un dé.
>
> Le modèle utilise des images de 224×224 pixels au format couleur RGB, capturées par le module caméra OV2640 connecté à l’ESP32-S3.
>
> </td>
> <td width="240">
> <img src="images/dice_224x224.jpg" alt="Dé jaune utilisé par le modèle de classification d’images" width="224">
> </td>
> </tr>
> </table>

## 🔍 Ce que fait le sketch

Le sketch Arduino de ce dépôt :

- Affiche la quantité de heap de SRAM interne disponible.
- Affiche la plus grande zone contiguë de heap de SRAM disponible.
- Affiche la PSRAM libre et totale.
- Alloue le tampon de caractéristiques d’entrée Edge Impulse dans la PSRAM.
- Exécute `run_classifier()` une fois avec une entrée de test remplie de zéros.
- Affiche l’utilisation mémoire avant et après l’inférence.
- Affiche le résultat de classification et les temps d’exécution.
- Affiche la mesure de la Tensor Arena TFLite Micro après l’ajout d’une petite ligne de débogage dans le SDK Edge Impulse.

Le sketch est un outil de diagnostic et de dimensionnement. Il **ne remplace pas** le code de la caméra ou des capteurs dans un projet final.

## 📖 Quelques notions techniques

Avant de poursuivre, voici deux termes techniques utilisés tout au long de ce guide :

**TensorFlow Lite Micro** (également appelé **TFLite Micro** ou **TFLM**) est un moteur d’exécution C++ léger conçu pour exécuter directement des modèles de machine learning entraînés sur des microcontrôleurs et autres appareils embarqués disposant de peu de mémoire.

Une **Tensor Arena** est un bloc unique de RAM réservé à l’avance pour TensorFlow Lite Micro. Pendant l’inférence, le moteur utilise cette mémoire pour stocker les entrées et sorties du modèle, les données d’activation intermédiaires, les données persistantes du modèle et les tampons de travail temporaires.

La Tensor Arena doit être suffisamment grande pour toutes ces allocations. Si elle est trop petite, TensorFlow Lite Micro ne peut pas allouer les tenseurs du modèle et l’inférence échoue. Si elle est inutilement grande, elle réserve de la SRAM interne qui pourrait être utilisée par la caméra, l’écran, le Wi-Fi ou le code de l’application.

## 💾 Comprendre la mémoire de l’ESP32-S3

L’ESP32-S3 utilise plusieurs types de mémoire, chacun ayant un rôle différent :
- **La mémoire Flash** est un stockage à long terme. Elle contient le programme Arduino compilé, la bibliothèque Edge Impulse, le modèle TensorFlow Lite Micro et d’autres fichiers statiques. La Flash conserve son contenu lorsque la carte est hors tension.

- **La SRAM interne** est la mémoire de travail rapide de l’ESP32-S3. Elle est utilisée pendant l’exécution du programme pour les variables, les piles, les tampons temporaires et, par défaut, la Tensor Arena TensorFlow Lite Micro. La SRAM interne étant limitée, sa taille doit être choisie avec soin.

- **La PSRAM** est une mémoire de travail externe supplémentaire disponible sur certaines cartes ESP32-S3. Elle est plus grande que la SRAM interne, mais généralement plus lente. Dans ce sketch, la PSRAM est utilisée pour le grand tampon d’image d’entrée, afin de préserver la SRAM interne pour la Tensor Arena TensorFlow Lite Micro et le reste de l’application.

La SRAM interne et la PSRAM sont toutes deux volatiles : leur contenu est effacé lorsque la carte est réinitialisée ou mise hors tension.

- **La carte microSD** est un stockage de fichiers amovible et non volatil. Elle peut contenir de grands jeux de données, des images capturées par la caméra, des journaux, des fichiers de configuration ou d’autres fichiers utilisateur, et conserve son contenu lorsque la carte est hors tension. Contrairement à la Flash, elle ne stocke ni n’exécute normalement le programme Arduino ; contrairement à la SRAM et à la PSRAM, elle ne sert pas de mémoire de travail pour les variables ou la Tensor Arena TensorFlow Lite Micro. L’ESP32-S3 lit et écrit les fichiers sur la carte via une interface SDMMC ou SPI.

---

## 🎯 A quoi sert ce dépôt ?

Lors du développement et du déploiement d’un projet d’IA sur l’appareil sur un ESP32-S3, deux étapes du processus nécessitent une attention particulière :

1. Choisir un modèle d’IA dans Edge Impulse capable de fonctionner sur un ESP32-S3. Edge Impulse propose de nombreux blocs d’apprentissage, architectures de modèles et options de déploiement, mais toutes les combinaisons ne sont pas adaptées aux limites de mémoire et de performances de l’ESP32-S3.

2. Déployer le modèle sous forme de bibliothèque Arduino et sélectionner la taille correcte de la Tensor Arena TensorFlow Lite Micro.

**Ce dépôt fournit des informations concrètes et mesurées sur ces deux points critiques : choisir un modèle d’IA Edge Impulse capable de fonctionner sur un ESP32-S3 et configurer la mémoire nécessaire à la Tensor Arena TensorFlow Lite Micro. Il permet d’éviter qu’un modèle s’entraîne correctement mais ne puisse pas être déployé ou exécuté de manière fiable sur l’ESP32-S3.**

De nombreux tests ont été réalisés afin de trouver une combinaison fonctionnelle de modèle, de taille d’entrée, de paramètres de déploiement, de configuration mémoire et de taille de Tensor Arena. La publication de ces résultats a pour objectif de faire gagner du temps aux passionnés et aux makers qui souhaitent démarrer un projet similaire.

## 📌 Modèle de référence pratique pour l’ESP32-S3

L’exemple à l’origine de ce dépôt est un projet Edge Impulse de classification d’images conçu pour reconnaître chaque face d’un dé. Après de nombreux tests, la combinaison suivante a été trouvée fonctionnelle sur un ESP32-S3 :

- Taille des images de l’Impulse : **224 x 224 RGB**
- Bloc d’apprentissage Edge Impulse : **Transfer Learning (Images)**
- Modèle de base : **MobileNetV1 96x96 0.25**
- Temps DSP : environ **71 ms**
- Temps de classification du réseau neuronal : environ **429 ms**
- Temps total d’inférence : environ **500 ms**
- Débit d’inférence théorique : environ **2 images par seconde**
- Débit pratique : proche de **2 images par seconde**, avant de prendre en compte la capture par la caméra, la conversion des images, l’affichage, le Wi-Fi, la journalisation et les autres tâches de l’application.

Pour ce projet de reconnaissance de dé, une résolution d’image plus faible ou des images en niveaux de gris au lieu du RGB auraient probablement suffi, ce qui aurait réduit les besoins mémoire du modèle et la charge d’inférence. L’objectif du réglage de l’Impulse Edge Impulse à 224 x 224 RGB était d’explorer la résolution d’image pratique la plus élevée pouvant être traitée par un ESP32-S3 pour de futurs projets plus exigeants.

Dans Edge Impulse, la largeur et la hauteur d’image sélectionnées dans **Impulse Design > Create Impulse** définissent la taille des images traitées par le bloc de traitement Image et attendues par le classifieur exporté. Dans ce projet, l’Impulse est configuré pour des images RGB de 224 x 224 pixels.

La partie `96x96` du nom `MobileNetV1 96x96 0.25` fait référence à la résolution d’entrée nominale et à celle utilisée lors du pré-entraînement du modèle. Edge Impulse indique que 96 x 96 est la résolution optimale du modèle, mais permet également de sélectionner une autre résolution d’image dans l’Impulse. Utiliser 224 x 224 augmente donc la quantité de données d’entrée, le travail de prétraitement, l’utilisation mémoire et généralement le coût d’inférence par rapport à un Impulse en 96 x 96.
La valeur `0.25` correspond au multiplicateur de largeur de MobileNet. Elle réduit le nombre de canaux du réseau neuronal, ce qui diminue la taille du modèle, les besoins en RAM et la quantité de calcul par rapport aux variantes MobileNet plus larges.

> [!NOTE]
> **MobileNetV1 96x96 0.25 était le modèle le plus adapté parmi ceux testés pour ce projet ESP32-S3. Il offrait un compromis exploitable entre précision de classification, utilisation mémoire et temps d’inférence. Cela ne signifie pas que MobileNetV1 96x96 0.25 est automatiquement le meilleur choix pour tous les projets ESP32-S3.**

Le modèle approprié dépend de la précision recherchée, de la taille d’image configurée dans l’Impulse, du nombre de classes, de la mémoire disponible et du temps d’inférence du réseau neuronal acceptable. Le **temps d’inférence du réseau neuronal (temps NN)** correspond au temps nécessaire à l’ESP32-S3 pour exécuter le réseau neuronal entraîné et produire les probabilités de classification.

Il est indiqué séparément du temps DSP dans Edge Impulse :

- **Temps DSP** : prétraitement de l’image et préparation des caractéristiques, notamment la conversion de l’image capturée en tampon numérique d’entrée requis par le classifieur.
- **Temps NN** (ou temps de classification) : exécution du réseau neuronal lui-même, depuis le tenseur d’entrée préparé jusqu’aux probabilités finales des classes.

Pour le test de référence de ce dépôt :
```text
Temps DSP : 71 ms
NN time: 429 ms
```

Le temps complet de traitement de la classification était donc d’environ :
```text
71 ms + 429 ms = 500 ms
```

**Cela correspond approximativement à un maximum théorique de 2 inférences par seconde.**

Le débit réel de bout en bout peut être inférieur, car un projet complet doit également capturer une image, convertir les pixels, mettre à jour un écran, écrire des journaux, communiquer via Wi-Fi ou effectuer d’autres tâches entre les inférences.

---

## 🚧 Le dimensionnement de la Tensor Arena

TensorFlow Lite Micro utilise une **Tensor Arena** : un bloc de RAM réservé aux tenseurs, aux activations intermédiaires et aux autres allocations nécessaires au modèle pendant l’inférence.

La Tensor Arena doit être suffisamment grande avant que TensorFlow Lite Micro ne crée l’interpréteur.

Si elle est trop petite :

- `AllocateTensors()` peut échouer.
- `run_classifier()` peut renvoyer une erreur telle que `-3`.
- Le Moniteur série peut afficher des messages tels que `AllocateTensors() failed`.
- L’appareil peut devenir instable si l’application est déjà proche des limites de sa RAM.

Si elle est beaucoup plus grande que nécessaire :

- De la SRAM interne est réservée inutilement.
- Il reste moins de mémoire pour la caméra, l’écran, le Wi-Fi, la logique de l’application et les autres tampons.

La bonne méthode consiste à :

1. Commencer avec une Tensor Arena volontairement grande.
2. Mesurer l’utilisation réelle de la Tensor Arena.
3. Ajouter une marge de sécurité.
4. Définir la taille finale de la Tensor Arena.
5. Recompiler et tester à nouveau.

> [!IMPORTANT]
> La ligne clé produite par ce dépôt est :
>
> ```text
> DEBUG: Tflite arena used bytes: <M>
> ```
>
> `<M>` correspond à l’utilisation minimale mesurée de la Tensor Arena pour votre modèle. Ne configurez jamais la Tensor Arena finale en dessous de cette valeur.

Par exemple :

```text
DEBUG: Tflite arena used bytes: 536044
```

signifie que TensorFlow Lite Micro a réellement utilisé :

```text
M = 536044 bytes
```

pour ce modèle précis.

---

## ✅ Prérequis

Vous avez besoin de :

- Une **carte de développement ESP32-S3 N16R8** avec PSRAM.

  Ce dépôt a été testé avec un **Goouuu ESP32-S3 N16R8**, une carte compatible DevKitC-1 équipée de **16 Mo de mémoire Flash** et de **8 Mo de PSRAM**.
  D’autres cartes ESP32-S3 N16R8 équipées de PSRAM peuvent également fonctionner, comme un module ESP32-S3-WROOM-1 N16R8.

> [!NOTE]
> Pour conserver un guide ciblé et reproductible, tous les paramètres de l’Arduino IDE et les recommandations techniques de ce dépôt sont basés sur la **configuration ESP32-S3 N16R8** : **16 Mo de Flash** et **8 Mo de PSRAM**.
>
> Si vous utilisez une autre carte ESP32-S3 ou une autre configuration mémoire, le sketch peut également fonctionner, mais vous devez adapter les paramètres de l’Arduino IDE — notamment **Board**, **Flash Size**, le **mode PSRAM** et le **schéma de partition** — à votre matériel.

- Arduino IDE 2.x.
- Le paquet de cartes ESP32 d’Espressif installé dans l’Arduino IDE.
- Un projet Edge Impulse exporté sous forme de bibliothèque Arduino utilisant TensorFlow Lite.
- Le sketch `ESP32-S3_arduino_tflite_micro_arena_size_finder.ino` de ce dépôt.

---

## 🏗️ Étape 1 : créer et exporter le modèle Edge Impulse

### 1.1 🆕 Créer le projet Edge Impulse

Dans Edge Impulse Studio, cliquez sur **Create new project**, donnez un nom au projet, puis sélectionnez **Espressif ESP-EYE (ESP32 240MHz)** comme appareil cible.

Bien que ce projet fonctionne sur une carte caméra ESP32-S3 plutôt que sur l’ESP-EYE d’origine, cette cible constitue un point de départ pratique dans Edge Impulse pour obtenir des estimations de performances orientées ESP32. La compatibilité finale et les besoins mémoire doivent néanmoins être vérifiés sur l’ESP32-S3 réel.

### 1.2 📸 Créer le jeu de données d’images

![Créer le jeu de données avec des images 224x224](images/dataset.PNG)

Avant de créer l’Impulse, constituez et annotez le jeu de données qui servira à entraîner le modèle.

Dans Edge Impulse Studio, ouvrez la page **Data acquisition**. Vous pouvez capturer des images directement avec une caméra connectée ou importer des fichiers image existants. Pour chaque image, sélectionnez la bonne étiquette et assurez-vous que chaque face du dé possède sa propre classe.

Pour ce projet de reconnaissance de dé, créez les classes suivantes : `one`, `two`, `three`, `four`, `five`, `six`, `background`.

La classe `background` est importante. Elle contient des images dans lesquelles aucune face de dé ne doit être reconnue — par exemple un fond blanc vide, la table, la vue de la caméra sans dé ou d’autres objets sans rapport. Elle aide le modèle à apprendre à distinguer le dé de son environnement et réduit les fausses détections.

Pour une classification d’objet en gros plan, faites occuper au dé environ 60 % à 80 % de l’image. Il doit être suffisamment grand pour que ses points et ses contours restent nets après redimensionnement, tout en laissant apparaître une partie du fond autour de lui.

> [!TIP]
> Si possible, utilisez un fond simple, neutre et constant, par exemple une feuille de papier blanche. Capturez les images de fond dans les mêmes conditions d’éclairage et de caméra que celles utilisées en fonctionnement normal. Incluez ce fond comme classe de reconnaissance distincte.

Pour chaque classe correspondant à une face du dé, capturez au moins 30 à 50 images pour commencer ; davantage d’images produisent généralement un modèle plus fiable. Conservez approximativement le même nombre d’images dans chaque classe, y compris la classe `background`.

Faites varier volontairement les images :

- Changez la position et la rotation du dé.
- Déplacez légèrement le dé vers la caméra ou éloignez-le.
- Faites varier l’intensité et la direction de l’éclairage.
- Introduisez de petites variations dans les ombres et le cadrage.
- Gardez le dé clairement visible et net.
- Utilisez des images capturées avec la même caméra OV2640 et la même configuration que celles qui seront utilisées sur l’ESP32-S3.

N’utilisez pas uniquement des photographies presque identiques. Le jeu de données doit représenter les conditions réelles dans lesquelles le modèle fonctionnera. Un modèle entraîné uniquement avec un dé parfaitement centré sur un fond blanc peut échouer lorsque le dé est tourné, déplacé, partiellement ombré ou observé sous un éclairage différent.

Utilisez la majorité des images pour les **données d’entraînement** et réservez des images distinctes pour les **données de test**. Une répartition initiale pratique est d’environ 80 % pour l’entraînement et 20 % pour les tests.

### 1.3 🧩 Créer l’Impulse

Après avoir créé et annoté votre jeu de données d’images dans la page **Data acquisition**, ouvrez **Impulse #x > Create impulse** pour configurer votre Impulse de classification d’images.

![Create Impulse 224x224](images/Edge-Impulse_Create-Impulse.PNG)

1. Ouvrez **Impulse #x > Create impulse**.
2. Réglez **Image width** et **Image height** sur `224`.
3. Ajoutez le bloc de traitement **Images**.
4. Ajoutez le bloc d’apprentissage **Transfer Learning (Images)**.
5. Cliquez sur **Save impulse**.
6. Ouvrez la page **Image**, sélectionnez la profondeur de couleur appropriée pour votre caméra (généralement **RGB**), puis enregistrez les paramètres.
7. Sur la même page, ouvrez l’onglet **Generate features** et cliquez sur **Generate features**.
8. Ouvrez l’onglet **Transfer Learning**, configurez le modèle et entraînez-le avec vos images annotées.

Le modèle obtenu attend des images de 224×224 pixels capturées par la caméra connectée à l’ESP32-S3.

### 1.4 🧠 Configurer le modèle de Transfer Learning

<table>
<tr>
<td valign="top">

Ouvrez l’onglet **Transfer Learning** pour configurer et entraîner le modèle de classification d’images.

Les paramètres ci-dessous constituent un bon point de départ, et non des exigences fixes. Entraînez d’abord le modèle, examinez ses performances, puis ajustez si nécessaire le nombre de cycles d’entraînement, le taux d’apprentissage, les paramètres d’augmentation des données ou l’architecture du modèle.

- **Nombre de cycles d’entraînement :** commencez avec `50`.
- **Taux d’apprentissage :** commencez avec `0.001`.
- **Processeur d’entraînement :** sélectionnez **CPU**.
- **Augmentation des données :** activez cette option si vous disposez de peu d’images d’entraînement — par exemple 15 à 20 images par classe. Elle crée des variantes des images d’entraînement, par exemple avec de petites modifications de position, de recadrage ou de luminosité, afin d’aider le modèle à mieux généraliser. Elle ne remplace pas la collecte d’images réelles et variées.
- Sous **Architecture du réseau neuronal**, cliquez sur **Choose a different model** et sélectionnez **MobileNetV1 96x96 0.25**. Ce modèle compact convient bien à l’ESP32-S3 car il utilise nettement moins de RAM et de mémoire Flash que des architectures plus importantes.

Cliquez sur **Save & train** pour lancer l’entraînement du modèle.

</td>
<td width="360" valign="top">

<img src="images/Transfer_learning.PNG"
     alt="Edge Impulse Transfer Learning configuration"
     width="340">

</td>
</tr>
</table>

> [!IMPORTANT]
> La qualité de la reconnaissance d’images dépend principalement du jeu de données : nombre d’images par classe, précision des annotations, netteté, conditions d’éclairage, angle de la caméra, distance par rapport au dé, variété des arrière-plans et adéquation entre les images d’entraînement et les conditions réelles d’utilisation.
>
> Le modèle joue également un rôle important : il s’agit de l’architecture du réseau neuronal qui apprend les motifs visuels à partir des images d’entraînement et les utilise pour classer de nouvelles images capturées par la caméra. Un modèle plus grand peut potentiellement reconnaître des motifs plus complexes, mais il nécessite davantage de mémoire Flash, de RAM et de temps de calcul sur l’ESP32-S3.
>
> **MobileNetV1 96×96 0.25** est une architecture de réseau neuronal compacte choisie comme compromis pratique entre les performances de reconnaissance et les ressources limitées d’un appareil embarqué. Selon Edge Impulse, elle nécessite environ 105,9 Ko de RAM et 301,6 Ko de ROM. Elle est donc bien adaptée au déploiement sur l’ESP32-S3 tout en laissant de la mémoire disponible pour le tampon d’image de la caméra, la Tensor Arena TensorFlow Lite Micro et le reste de l’application.
>
> **L’augmentation des données** améliore la robustesse en créant des exemples d’entraînement modifiés, par exemple avec de petites variations de recadrage, de position, de luminosité ou d’orientation. Elle est particulièrement utile avec un petit jeu de données, mais ne peut pas compenser des images mal annotées, trop similaires, floues ou peu représentatives des conditions réelles.

### 1.5 📤 Sélectionner la cible de déploiement

Sur la page **Déploiement**, configurez l’exportation comme suit :

1. Sous **Deployment target**, sélectionnez **Arduino library**.
2. Sous **Inference engine**, sélectionnez **TensorFlow Lite**.
3. Sous **Model optimizations and performance**, sélectionnez **Quantized (int8)**.
4. Cliquez sur **Build** pour générer et télécharger le fichier ZIP de la bibliothèque Arduino.

N’extrayez pas le fichier ZIP téléchargé. Il sera importé directement dans l’Arduino IDE à l’étape suivante.

![La cible de déploiement](images/Deployment_target.PNG)

> [!IMPORTANT]
> À partir d’août 2026, sélectionnez **TensorFlow Lite** et **n’utilisez pas** le **compilateur EON** ni sa variante optimisée EON pour ce flux de travail Arduino avec ESP32-S3. Les déploiements EON ont provoqué des problèmes de compatibilité et de gestion mémoire dans des projets ESP32-S3 basés sur Arduino, notamment des résultats d’inférence incorrects, des plantages, des boucles de redémarrage et des erreurs de heap corrompu. **TensorFlow Lite Micro est l’option la plus fiable pour cette procédure.**
> Ce dépôt mesure également la Tensor Arena TensorFlow Lite Micro avec `arena_used_bytes()`. Le résultat mesuré s’applique donc uniquement au modèle et au moteur d’exécution TensorFlow Lite, et non à un modèle compilé avec EON.
>
> Sélectionnez **Quantized (int8)**, et non **Unoptimized (float32)**. Un modèle int8 utilise des valeurs entières sur 8 bits au lieu de valeurs en virgule flottante sur 32 bits. Il est nettement plus petit et généralement plus rapide sur un ESP32-S3, tout en nécessitant moins de mémoire Flash et de RAM.
La taille de la Tensor Arena dépend du modèle déployé exact. Si vous passez de int8 à float32, modifiez la taille de l’image, l’architecture du modèle ou le nombre de classes, générez une nouvelle bibliothèque et mesurez à nouveau la Tensor Arena.

---

## 📥 Étape 2 : installer votre projet Edge Impulse comme bibliothèque Arduino

Importez dans l’Arduino IDE le fichier ZIP de bibliothèque Arduino généré par Edge Impulse.

1. Ouvrez l’**Arduino IDE**.
2. Sélectionnez **Sketch > Include Library > Add .ZIP Library…**
3. Sélectionnez le fichier ZIP téléchargé depuis la page **Deployment** d’Edge Impulse.
4. Attendez le message confirmant que la bibliothèque a été installée avec succès.

N’extrayez pas le fichier ZIP avant de l’importer. L’Arduino IDE installe directement la bibliothèque depuis l’archive téléchargée.

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

## ⚙️ Étape 3 : configurer l’Arduino IDE

Connectez votre ESP32-S3 et vérifiez le menu **Tools** de l’Arduino IDE.

| Menu Arduino IDE | Valeurs recommandées | Informations |
|---|---|---|
| **Tools > Board** | Sélectionnez votre carte ESP32-S3 réelle, par exemple `ESP32S3 Dev Module` ou `XIAO ESP32S3 Sense` | La carte sélectionnée détermine les options disponibles pour la Flash, la PSRAM, l’USB, le téléversement et les partitions |
| **Tools > Port** | Sélectionnez le port série attribué à l’ESP32-S3 | Nécessaire pour le téléversement et les messages du Moniteur série |
| **Tools > USB CDC On Boot** | `Enabled` | Si cette option est désactivée, la sortie série peut ne pas être visible via USB sur les cartes disposant d’un USB natif |
| **Tools > Flash Size** | `16MB (128Mb)` for an ESP32-S3 N16R8 board | `N16R8` indicates 16 MB of mémoire Flash and 8 MB of PSRAM |
| **Tools > PSRAM** | Activez l’option PSRAM appropriée, souvent `OPI PSRAM` | Le sketch alloue le tampon d’entrée dans la PSRAM |
| **Tools > schéma de partition** | `16M Flash (3MB APP / 9.9MB FATFS)` | Ensures that the partition table is configured for the board’s full 16 MB of mémoire Flash |
| **Tools > Core Debug Level** | `None` ou `Error` | Facultatif ; réduit les messages série sans rapport avec le diagnostic |

> [!TIP]
> La Tensor Arena utilise de la **RAM**, et non l’espace de la partition Flash. Modifier le schéma de partition n’augmente pas automatiquement la taille de la Tensor Arena.

---

## 🛠️ Étape 4 : adapter le sketch à votre bibliothèque Arduino Edge Impulse

Ouvrez le sketch de ce dépôt dans l’Arduino IDE.

Au début du sketch, recherchez ces deux lignes contenant les valeurs provisoires :

```cpp
#include <YOUR_LIBRARY_inferencing.h>                    // <-- CHANGE THIS
extern const size_t tflite_learn_XXXXXXX_ZZ_arena_size;  // <-- CHANGE THIS
```
Ces deux lignes doivent être adaptées à la bibliothèque Arduino exportée depuis **votre propre projet Edge Impulse**.

### 4.1 🔗 Inclure votre en-tête d’inférence

Remplacez `YOUR_LIBRARY` par le nom exact de l’en-tête d’inférence généré par votre projet Edge Impulse.

Par exemple, si votre bibliothèque Arduino Edge Impulse s’appelle **Dice**, son fichier d’en-tête d’inférence est :
```text
Dice_inferencing.h
```

Replace:
```cpp
#include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS
```

with:
```cpp
#include <Dice_inferencing.h>
```

Pour trouver le nom exact du fichier, ouvrez ce dossier sur votre ordinateur :
```text
...\Arduino\libraries\<your-project-name>\src\
```

### 4.2 🏷️ Déclarer le symbole de taille de la Tensor Arena

Remplacez ensuite le symbole de Tensor Arena temporaire :
```cpp
extern const size_t tflite_learn_XXXXXXX_ZZ_arena_size;  // <-- CHANGE THIS
```

Le symbole exact est défini dans l’en-tête du modèle généré par Edge Impulse. Ouvrez :
```text
...\Arduino\libraries\<your-project-name>\src\tflite-model\
```

Recherchez un fichier similaire à :
```text
tflite_learn_1072522_47.h
```

Copiez le nom exact du symbole dans le sketch Arduino :
```cpp
extern const size_t tflite_learn_1072522_47_arena_size;
```

> [!IMPORTANT]
> N’inventez pas et ne simplifiez pas le symbole de taille de Tensor Arena généré. Copiez-le exactement depuis votre propre fichier `tflite_learn_*.h`, en conservant chaque chiffre et chaque underscore. Chaque exportation Edge Impulse peut générer un nom de symbole différent.

### 4.3 📏 Définir temporairement la taille de la Tensor Arena

Restez dans le même dossier :
```text
...\Arduino\libraries\<your-project-name>\src\tflite-model\
```

Ouvrez le fichier d’en-tête du modèle :
```text
tflite_learn_XXXXXXX_ZZ.h
```

In this example, the file is:
```text
tflite_learn_1072522_47.h
```

Recherchez les deux définitions de taille de Tensor Arena. Au départ, elles peuvent ressembler à ceci :
```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 519232
const size_t tflite_learn_1072522_47_arena_size = 519232;
```

Pour le premier test, remplacez **les deux valeurs** par une valeur temporaire volontairement grande :
```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 1000000
const size_t tflite_learn_1072522_47_arena_size = 1000000;
```

Cela donne à TensorFlow Lite Micro suffisamment de mémoire pour allouer les tenseurs du modèle et indiquer l’utilisation réelle de la Tensor Arena. Si `1000000` ne compile pas, ou si l’ESP32-S3 ne peut pas démarrer parce qu’il ne reste pas suffisamment de SRAM interne disponible, utilisez la plus grande valeur que votre carte peut supporter. Examinez ensuite les autres allocations mémoire statiques ou envisagez de réduire les besoins mémoire du modèle.

> [!IMPORTANT]
> Cette configuration est temporaire. Les deux lignes doivent utiliser **exactement la même valeur**.

---

## 🐞 Étape 5 : ajouter une ligne de débogage pour afficher l’utilisation réelle de la Tensor Arena

Ouvrez ce fichier du SDK Edge Impulse :

```text
...\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
```

Recherchez :
```cpp
TfLiteStatus allocate_status = interpreter->AllocateTensors(true);
if (allocate_status != kTfLiteOk) {
    ei_printf("AllocateTensors() failed");
    return EI_IMPULSE_TFLITE_ERROR;
}
```

Immédiatement après ce bloc `if`, ajoutez :
```cpp
// Débogage : utilisation réelle de la Tensor Arena pour ce modèle.
// Affiché après l’allocation des tenseurs par TFLite Micro.
ei_printf("DEBUG: Tflite arena used bytes: %d\n",
          (int)interpreter->arena_used_bytes());
```

La section complète doit devenir :
```cpp
TfLiteStatus allocate_status = interpreter->AllocateTensors(true);
if (allocate_status != kTfLiteOk) {
    ei_printf("AllocateTensors() failed");
    return EI_IMPULSE_TFLITE_ERROR;
}

// Débogage : utilisation réelle de la Tensor Arena pour ce modèle.
// Affiché après l’allocation des tenseurs par TFLite Micro.
ei_printf("DEBUG: Tflite arena used bytes: %d\n",
          (int)interpreter->arena_used_bytes());
```

`interpreter` est un objet `tflite::MicroInterpreter`. Sa méthode `arena_used_bytes()` indique l’utilisation de la Tensor Arena après l’allocation des tenseurs.

`ei_printf()` est envoyé vers la sortie série Arduino ; le résultat apparaît donc dans le Moniteur série de l’Arduino IDE.

---

## ⚡ Étape 6 : compiler, téléverser et lire la valeur Tensor arena

1. Ouvrez le sketch `.ino` modifié dans l’Arduino IDE.
2. Cliquez sur **Upload** pour téléverser le programme sur l’ESP32-S3.
4. Ouvrez **Tools > Moniteur série** et réglez la vitesse sur **115200** bauds.
6. Si aucune sortie n’apparaît, appuyez sur le bouton **Reset** de la carte.

> [!NOTE]
> La première compilation peut prendre plus de 10 minutes, car l’Arduino IDE doit compiler la bibliothèque Edge Impulse, TensorFlow Lite Micro et leurs dépendances. Les compilations suivantes sont généralement plus rapides.

Recherchez cette ligne clé :
```text
DEBUG: Tflite arena used bytes: <M>
```

Par exemple, le modèle de test utilisé dans ce projet a affiché :

```text
DEBUG: Tflite arena used bytes: 536044
```

Cela signifie :

```text
M = 536044 bytes
```

Il s’agit de l’utilisation mesurée de la Tensor Arena pour ce modèle. Ne supposez pas que votre propre modèle aura la même valeur.

---

## 🧮 Étape 7 : calculer la taille finale de la Tensor Arena

Utilisez cette règle :
```text
taille finale de la Tensor Arena = M + marge de sécurité
```

Pour le test de référence de ce dépôt, la valeur mesurée est :
```text
DEBUG: Tflite arena used bytes: 536044
```

Therefore:
```text
M = 536044 bytes
```

Une marge de 16 à 64 Ko constitue généralement un bon point de départ pratique :

| Marge | Octets | Cas d’utilisation |
|---|---:|---|
| 16 Ko | 16 384 octets | Peu de marge |
| 40 Ko | 40 960 octets | Marge moyenne |
| 64 Ko | 65 536 octets | Marge confortable |

Pour la mesure de référence `M = 536044` :

| Marge de sécurité | Calcul | Taille finale de la Tensor Arena |
|---|---:|---:|
| 16 Ko | 536044 + 16384 | 552428 octets |
| 40 Ko | 536044 + 40960 | 577004 octets |
| 64 Ko | 536044 + 65536 | 601580 octets |

Vous pouvez également choisir une valeur alignée sur une puissance de deux, facile à lire, et supérieure au besoin mesuré.

Par exemple :
```text
564 Ko = 577536 octets
```

Cela donne :
```text
577536 - 536044 = 41492 bytes
```
of marge de sécurité, which is slightly more than 40 kB.

> [!IMPORTANT]
> La valeur à conserver pour ce modèle de référence est :
> ```text
> M = 536044 bytes
> ```
> Ne définissez jamais la taille finale de la Tensor Arena en dessous de `M`. Dans le cas contraire, TensorFlow Lite Micro peut échouer à allouer les tenseurs et `run_classifier()` peut renvoyer `-3`.
> Pour le modèle de référence, une configuration finale lisible pourrait être :
>```cpp
>#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 577536
>const size_t tflite_learn_1072522_47_arena_size = 577536;
>```

## 💾 Étape 8 : enregistrer la valeur finale de la Tensor Arena dans l’en-tête du modèle

À la section 4.3, la Tensor Arena avait été temporairement réglée à `1000000` octets afin de garantir à TensorFlow Lite Micro suffisamment de mémoire pour allouer tous les tenseurs du modèle.

Revenez maintenant au même fichier d’en-tête du modèle :

```text
...\Arduino\libraries\<your-project-name>\src\tflite-model\tflite_learn_XXXXXXX_YY.h
```

Trouvez les deux définitions temporaires :

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 1000000
const size_t tflite_learn_1072522_47_arena_size = 1000000;
```

Remplacez **les deux valeurs** par la taille finale choisie pour la Tensor Arena. Dans notre cas, `577536` octets constituent une valeur facile à retenir. Cela correspond
à 564 Ko et inclut une marge de sécurité de 41492 octets :

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 577536
const size_t tflite_learn_1072522_47_arena_size = 577536;
```
Enregistrez le fichier d’en-tête, puis recompilez et téléversez à nouveau le sketch Arduino.

Le test final doit toujours afficher :
```text
run_classifier() return code  : 0 (OK)
```

and:
```text
DEBUG: Tflite arena used bytes: 536044
```

La valeur DEBUG reste `536044`, car il s’agit de la mémoire réellement utilisée par le modèle. La Tensor Arena configurée est maintenant de `577536` octets, ce qui fournit une marge de sécurité de `41492` octets.

---

## 🖥️ Exemple de sortie série

Une exécution de mesure réussie pour le modèle de référence ressemble à ceci :

```text
SETUP STARTING

Arduino ESP32-S3 + Edge Impulse - Tensor Arena Size Finder
------------------------------------------------------------
=== ESP32-S3 Memory Test ===
Heap free (internal SRAM)    : 342912 bytes
Heap max contiguous block    : 286708 bytes
PSRAM free                   : 8386076 bytes
PSRAM total                  : 8388608 bytes

=== Current Edge Impulse Arena ===
Arena configured in tflite_learn_xxx.h : 1000000 bytes
Arena / (heap free + arena)  : 74.5%
Arena / max contiguous block : 348.8%

Note:
  - La Tensor Arena est réservée statiquement en SRAM par le SDK EI.
  - Our PSRAM measurements only see dynamic allocations, such as
    the input feature buffer we allocate below.

--- Allocating NN input buffer in PSRAM ---
Planned input buffer size     : 602112 bytes
Input buffer allocated in PSRAM: 602112 bytes OK

--- Running single inference (run_classifier) ---
DEBUG: Tflite arena used bytes: 536044

run_classifier() return code  : 0 (OK)
Heap used during inference    : 432 bytes
PSRAM used during inference   : 0 bytes
Heap free after inference     : 342216 bytes
PSRAM free after inference    : 7783948 bytes

--- Classification result (dummy input) ---
  0: 0.934
  1: 0.059
  2: 0.000
  3: 0.004
  4: 0.000
  5: 0.000
  6: 0.004

DSP time: 71 ms, NN time: 429 ms
```

Les lignes importantes sont :
```text
DEBUG: Tflite arena used bytes: 536044
run_classifier() return code  : 0 (OK)
```

La première ligne donne le besoin réel en Tensor Arena pour ce modèle. La seconde confirme que l’inférence s’est terminée avec succès.

La Tensor Arena configurée est temporairement fixée à `1000000` octets pour cette mesure. Après avoir mesuré le besoin réel, elle peut être réduite à une valeur finale telle que `577536` octets (564 Ko), tout en conservant une marge de sécurité.

### 📖 Comment lire cette sortie

| Output | Meaning |
|---|---|
| `PSRAM total : 8388608 bytes` | 8 MB de PSRAM sont disponibles |
| `Input buffer allocated in PSRAM: 602112 bytes OK` | Le tampon de données d’entrée de 602112 octets a été alloué avec succès dans la PSRAM |
| `DEBUG: Tflite arena used bytes: 536044` | Il s’agit du besoin mesuré en Tensor Arena `M` pour le modèle de référence |
| `run_classifier() return code : 0 (OK)` | L’allocation des tenseurs et l’inférence se sont terminées avec succès |
| `DSP time: 71 ms, NN time: 429 ms` | Le prétraitement DSP a pris 71 ms ; la classification par le réseau neuronal a pris 429 ms |
| `Arena configured in tflite_learn_xxx.h : 1000000 bytes` | La Tensor Arena est temporairement surdimensionnée pour la mesure et peut maintenant être réduite après ajout d’une marge de sécurité |
| `Heap free after inférence : 342216 bytes` | Le heap de SRAM interne reste disponible pour les allocations dynamiques après l’inférence |
| `PSRAM free after inférence : 7783948 bytes` | La PSRAM reste disponible après l’allocation du tampon de données d’entrée et l’exécution de l’inférence |

Le tampon d’entrée a été alloué dans la PSRAM. Therefore, its `602112`-byte allocation
ne consomme pas le heap de SRAM interne indiqué par :
```cpp
ESP.getFreeHeap()
```

La Tensor Arena est normalement réservée statiquement dans la SRAM interne par le code Edge Impulse / TensorFlow Lite Micro. C’est pourquoi la taille configurée de la Tensor Arena n’apparaît pas comme une allocation dynamique de PSRAM dans ce test.

Pour cette mesure, la Tensor Arena a volontairement été configurée à :
```text
1000000 bytes
```

Le besoin réel mesuré était de :
```text
536044 bytes
```

La Tensor Arena finale peut donc être réduite à une valeur supérieure à `536044` octets, avec une marge de sécurité. Par exemple, `577536` octets (564 Ko) laisse une marge de
`41492` bytes.

> [!IMPORTANT]
> ## 🎯 La valeur à conserver
>
> Pour ce modèle de référence, la ligne la plus importante du Moniteur série est :
> ```text
> DEBUG: Tflite arena used bytes: 536044
> ```
>
> Cela signifie que le besoin minimal réel en Tensor Arena TensorFlow Lite Micro est de :
>
> ```text
> M = 536044 bytes
> ```
> **Ne configurez pas la Tensor Arena finale en dessous de `536044` octets pour ce modèle précis. Une valeur finale pratique pour ce test est `577536` octets (564 Ko), ce qui conserve une marge de sécurité de `41492` octets.

---

## 🩺 Dépannage

### ⚠️ Compatibilité de la caméra et dépannage

Ce dépôt se concentre sur le dimensionnement de la Tensor Arena TensorFlow Lite Micro. Il suppose que la caméra est déjà capable de capturer des images de manière fiable avant l’ajout du traitement de machine learning.

Avant d’appeler `run_classifier()`, validez séparément la caméra avec un sketch minimal :

1. Initialisez la caméra OV2640 avec le brochage GPIO exact de votre carte.
2. Capturez et retournez plusieurs images de la caméra.
3. Vérifiez que la taille d’image et le format de pixels choisis fonctionnent de manière fiable.
4. Ajoutez seulement ensuite la conversion d’image, le redimensionnement et l’inférence TensorFlow Lite Micro.

> [!WARNING]
> Une taille d’entrée du modèle telle que 96×96 ou 224×224 ne correspond pas nécessairement à un mode de capture de la caméra. La caméra peut capturer une image source dans une taille prise en charge, puis l’application convertit et redimensionne cette image dans le tampon d’entrée du modèle.
>
> Ne supposez pas qu’une carte équipée d’un OV2640 prend en charge de manière fiable toutes les résolutions ou tous les formats de pixels bruts. Vérifiez le JPEG, le RGB565 et la taille d’image sélectionnée avec la carte exacte, le pilote de caméra, la version du cœur Arduino-ESP32 et la configuration des broches avant d’intégrer Edge Impulse.

### 🔇 Aucune sortie série / la carte semble bloquée

Si votre ESP32-S3 semble se bloquer au démarrage et que rien n’apparaît dans le Moniteur série :

1. **Vérifiez USB CDC On Boot :** dans l’Arduino IDE, assurez-vous que `Tools > USB CDC On Boot` est réglé sur **Enabled** si votre carte est connectée via son port USB natif.
2. **Boucle While infinie :** assurez-vous que votre sketch `.ino` n’utilise pas un `while(!Serial);` bloquant sans délai d’expiration. Sur certaines cartes ESP32-S3, cette instruction attendra indéfiniment un signal DTR du moniteur. Utilisez plutôt un délai d’attente non bloquant :
   ```cpp
   unsigned long start = millis();
   while (!Serial && (millis() - start < 2000)) {
       delay(10);
   }
   ```

### ⚡ La PSRAM totale est à zéro

Example:

```text
PSRAM totale : 0 octet
```

La PSRAM n’est pas détectée ou n’est pas activée. Vérifiez :

1. Vérifiez que votre carte dispose bien de PSRAM.
2. Vérifiez que **Tools > PSRAM** est activé.
3. Le mode PSRAM sélectionné correspond à la carte, souvent `OPI PSRAM` pour les cartes ESP32-S3.
4. La définition de carte sélectionnée est correcte.

### ❌ `AllocateTensors() failed`

Cela signifie généralement que la Tensor Arena est trop petite.

1. Revenez à `tflite_learn_XXXXXXX_ZZ.h`.
2. Réattribuez aux deux valeurs de Tensor Arena un nombre temporairement élevé, par exemple `1000000`.
3. Vérifiez que la macro et le `const size_t` utilisent le même nombre.
4. Recompilez et téléversez à nouveau.
5. Recherchez la ligne `DEBUG: Tflite arena used bytes: ...`.

### ❌ `run_classifier()` renvoie `-3`

Cela est généralement lié à une Tensor Arena insuffisante. Vérifiez :

- La taille configurée de la Tensor Arena n’est pas inférieure à la valeur mesurée `M`.
- Both arena definitions use the same value.
- Vous avez sélectionné TensorFlow Lite lors du déploiement Edge Impulse.
- Le fichier `tflite_learn_*.h` modifié appartient bien à la même bibliothèque que celle installée et compilée par l’Arduino IDE.

### 🔇 Aucune ligne `DEBUG: Tflite arena used bytes`

Vérifiez les points suivants :

1. Vous avez ajouté la ligne `ei_printf(...)` dans `tflite_micro.h`.
2. La nouvelle ligne se trouve après le contrôle de réussite de `AllocateTensors()`.
3. Vous avez enregistré `tflite_micro.h`.
4. Vous avez modifié la copie de la bibliothèque utilisée par l’Arduino IDE.
5. Le Moniteur série est ouvert à `115200` bauds.
6. Vous avez recompilé et téléversé après avoir modifié le fichier de bibliothèque.

### 📁 `YOURMODEL_inferencing.h` introuvable

Remplacez :

```cpp
#include <YOURMODEL_inferencing.h>
```

par la valeur exacte trouvée dans :

```text
...\Arduino\libraries\<your-project-name>\src\
```

Le nom du fichier doit se terminer par :

```text
_inferencing.h
```

Example:

```cpp
#include <Dice_v4_inferencing.h>
```

La casse est importante sur certains systèmes d’exploitation.

### 🔗 Symbole de Tensor Arena non défini

Vérifiez que cette ligne :

```cpp
extern const size_t tflite_learn_XXXXXXX_ZZ_arena_size;
```

correspond exactement au symbole présent dans `tflite_learn_*.h`. Copiez-le caractère par caractère.

Example:

```cpp
extern const size_t tflite_learn_1072522_47_arena_size;
```

### 💽 Le sketch est trop volumineux pour la partition

Il s’agit d’un problème de partition Flash, et non d’un problème de Tensor Arena.

Choisissez un schéma de partition avec une partition APP plus grande, ou créez un fichier `partitions.csv` personnalisé.

Remember:

- La **partition APP** contient le programme compilé.
- La **Tensor Arena** utilise de la RAM pendant l’exécution.
- Augmenter la partition APP n’augmente ni la SRAM interne ni la PSRAM.

---

## 📝 Conserver une trace lors de chaque mise à jour du modèle

Les modifications décrites dans ce guide sont effectuées à l’intérieur de la bibliothèque Arduino Edge Impulse exportée. Si vous exportez une nouvelle version de la bibliothèque Edge Impulse, vos modifications peuvent être écrasées.

Pour chaque version du modèle, conservez une note indiquant :

- Projet Edge Impulse et date d’exportation.
- Architecture du modèle et taille d’entrée.
- Measured value `M`.
- Valeur finale de la Tensor Arena.
- Marge de sécurité utilisée.
- Nom de la carte ESP32-S3.
- Taille de la Flash.
- mode PSRAM.
- Arduino ESP32 core version.
- Partition scheme.
- Temps DSP et temps de classification mesurés.

Cela facilite la reproduction des résultats et le diagnostic des changements après une future exportation du modèle.

---

## 📚 Références

- [Edge Impulse - Transfer learning for images](https://docs.edgeimpulse.com/studio/projects/learning-blocks/blocks/transfer-learning-images)
- [Edge Impulse - Image classification tutorial](https://docs.edgeimpulse.com/tutorials/end-to-end/classification d’images)
- [Edge Impulse - EON Compiler](https://docs.edgeimpulse.com/studio/projects/deployment/eon-compiler)
- [Edge Impulse - Run bibliothèque Arduino with Arduino IDE 2.x](https://docs.edgeimpulse.com/hardware/deployments/run-arduino-2-0)
- [Espressif Arduino-ESP32 - Partition tables](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html)

---

## ⚖️ Licence

Ce projet est distribué sous **licence MIT** — voir le fichier [LICENSE](LICENSE) pour plus de détails.
