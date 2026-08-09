# Fichier Edge Impulse `tflite_micro.h` modifié

Ce fichier est un **remplacement temporaire à usage diagnostic** du fichier original Edge Impulse `tflite_micro.h`.

Il est destiné à être utilisé avec le projet suivant :

**ESP32-S3 Arduino Tensor Arena Size Finder**  
https://github.com/zeredbox/ESP32-S3_arduino_tflite_micro_arena_size_finder

## Objectif

Edge Impulse génère automatiquement une taille de Tensor Arena pour les modèles TensorFlow Lite Micro. Dans certains cas, cette valeur peut être trop faible, ce qui provoque l'échec de `AllocateTensors()` avant même que la quantité réelle de mémoire nécessaire puisse être mesurée.

Ce fichier `tflite_micro.h` modifié remplace temporairement la taille du Tensor Arena par une valeur de diagnostic plus grande et permet d'exposer **l'utilisation réelle de la mémoire du Tensor Arena** après l'exécution réussie de `AllocateTensors()`.

Cela permet au sketch Tensor Arena Size Finder de :

- fonctionner même lorsque le Tensor Arena généré par Edge Impulse est trop petit ;
- mesurer l'utilisation réelle de la mémoire du Tensor Arena ;
- déterminer une taille de Tensor Arena plus adaptée ;
- calculer des configurations recommandées **SMALL**, **MEDIUM** et **LARGE**.

## Important

> **N'utilisez pas ce fichier modifié de manière permanente dans votre projet Edge Impulse.**

Avant de l'utiliser :

1. **Faites une sauvegarde** du fichier Edge Impulse original `tflite_micro.h`.
2. Remplacez le fichier original par cette version modifiée.
3. Exécutez le sketch de diagnostic Tensor Arena Size Finder.
4. Utilisez la recommandation obtenue pour mettre à jour la valeur du Tensor Arena dans le fichier d'en-tête de votre modèle Edge Impulse.
5. **Restaurez le fichier Edge Impulse original `tflite_micro.h`.**
6. Recompilez et testez votre projet final avec les fichiers SDK Edge Impulse d'origine.

## Compatibilité

Ce fichier modifié a été préparé pour la **version du SDK Edge Impulse utilisée par ce projet**.

Les futures versions du SDK Edge Impulse peuvent modifier l'implémentation de `tflite_micro.h`. Par conséquent, ce fichier de remplacement destiné au diagnostic peut ne plus être compatible avec les nouvelles versions du SDK.

Avant de l'utiliser, comparez toujours ce fichier avec le fichier original `tflite_micro.h` fourni avec la version actuelle de votre SDK Edge Impulse.

## Procédure recommandée

```text
Projet Edge Impulse d'origine
        │
        ▼
Sauvegarder tflite_micro.h
        │
        ▼
Installer le tflite_micro.h modifié
        │
        ▼
Exécuter Tensor Arena Size Finder
        │
        ▼
Lire la recommandation SMALL / MEDIUM / LARGE
        │
        ▼
Mettre à jour la taille du Tensor Arena
dans le fichier d'en-tête du modèle
        │
        ▼
Restaurer le tflite_micro.h original
        │
        ▼
Compiler et tester le projet final
```

## Avertissement

**Ce fichier est destiné uniquement au diagnostic.**

L'application finale doit toujours être compilée et testée avec **l'implémentation originale de `tflite_micro.h` fournie par le SDK Edge Impulse**.
