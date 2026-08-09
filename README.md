# Arduino ESP32-S3 + Edge Impulse: Find the Right Tensor Arena Size

# 🚀 A Starting Point for Embedded AI on ESP32-S3
**A beginner-friendly guide to image classification with Edge Impulse and TensorFlow Lite Micro**

This guide is a practical starting point for anyone who wants to begin building embedded AI applications on an ESP32-S3, including complete beginners.

It explains the full workflow, from creating and labeling an image dataset in Edge Impulse to training, exporting, and deploying a TensorFlow Lite Micro model on an ESP32-S3. Along the way, it explains the key concepts needed to understand how an image-classification project works on a constrained embedded device.

This project helps answer three crucial questions before deploying an AI model:

1. **Which AI model can run on the ESP32-S3?**
2. **How much TensorFlow Lite Micro tensor arena memory must be allocated for that model?**
3. **How many images per second can you expect to process?**

The example uses a dice-image-classification model, but the same method can be applied to many other camera-based embedded AI projects.

---

## 👋 Introduction

This repository contains an **Arduino diagnostic sketch** and a **temporary modified `tflite_micro.h` file** that together measure the **TensorFlow Lite Micro tensor arena** used by an **Edge Impulse model** on an **ESP32-S3** with PSRAM.

Beyond the usual hobbyist and maker applications, the ESP32-S3 also makes it possible to explore original and engaging on-device AI projects at a modest cost. Its dual-core processor, optional PSRAM, Wi-Fi and Bluetooth connectivity, and support for AI and DSP-oriented instructions provide a practical base for image classification, sound recognition, gesture detection, sensor analysis, and connected edge-AI devices.

Together with **[Edge Impulse](https://edgeimpulse.com/)**, ESP32-S3 provides a workflow for collecting data, training a model, exporting an Arduino library, and running inference on a device without having to build the entire machine-learning deployment stack from scratch.

**This repository is intended for Arduino hobbyists and makers who have created an AI model with Edge Impulse and want to deploy it reliably on an ESP32-S3 as an Arduino library.**

> [!NOTE]
> <table>
> <tr>
> <td valign="top">
>
> The example used in this repository is an image-classification model created in Edge Impulse to recognize each face of a die.
>
> The model uses 224×224 pixel images in RGB color format, captured by the OV2640 camera module connected to the ESP32-S3.
>
> </td>
> <td width="240">
> <img src="images/dice_224x224.jpg" alt="Yellow die used by the image-classification model" width="224">
> </td>
> </tr>
> </table>

## 🔍 What the sketch does

The Arduino sketch in this repository:

- Checks that PSRAM is available.
- Prints free internal SRAM heap and free/total PSRAM.
- Uses a temporary **diagnostic Tensor Arena** of `1000000` bytes by default.
- Allocates the Edge Impulse input feature buffer in PSRAM.
- Runs `run_classifier()` once with a zero-filled test input.
- Reads the Tensor Arena size originally configured by Edge Impulse.
- Measures the Tensor Arena actually used by TensorFlow Lite Micro after tensor allocation.
- Warns if the Edge Impulse-generated arena is smaller than the measured usage.
- Automatically calculates three suggested final arena sizes:
  - **SMALL = M + 16 KiB**
  - **MEDIUM = M + 32 KiB** — recommended starting point
  - **LARGE = M + 64 KiB**
- Prints the exact two model-header lines that must be updated with the selected value.

The sketch is a diagnostic and sizing tool. It does **not** replace the camera or sensor code in a final project.

## 🔄 Workflow

The complete Tensor Arena sizing workflow is:

1. Create, train, and export your Edge Impulse model as an **Arduino library** using **TensorFlow Lite**.
2. Install the exported ZIP library in Arduino IDE.
3. Configure Arduino IDE for your ESP32-S3 board and PSRAM.
4. In the diagnostic sketch, change only the Edge Impulse inference-header include:
   ```cpp
   #include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS
   ```
5. Back up the original Edge Impulse file:
   ```text
   src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
   ```
6. Temporarily replace it with the modified file supplied by this repository:
   ```text
   edge-impulse-sdk-patch/tflite_micro.h
   ```
7. Compile and upload the Tensor Arena Size Finder.
8. Read the automatically measured value `M` and the generated **SMALL / MEDIUM / LARGE** recommendations in Serial Monitor.
9. Update **both** Tensor Arena definitions in:
   ```text
   src/tflite-model/tflite_learn_XXXXXXX_ZZ.h
   ```
10. Restore the **original Edge Impulse `tflite_micro.h`**.
11. Compile and test your **final application** using the original Edge Impulse SDK file and the new Tensor Arena value.

> [!IMPORTANT]
> The modified `tflite_micro.h` is only a diagnostic helper. Once the correct Tensor Arena value has been written into your model header, restore the original Edge Impulse file before returning to your final application.
>
> If you export a new model or a new Edge Impulse Arduino library, repeat the measurement because the required Tensor Arena size can change.

---

## 📖 A few technical terms

Before continuing, here are two technical terms used throughout this guide:

**TensorFlow Lite Micro** (also called **TFLite Micro** or **TFLM**) is a lightweight C++ runtime designed to run trained machine-learning models directly on microcontrollers and other embedded devices with limited memory.

A **tensor arena** is a single block of RAM reserved in advance for TensorFlow Lite Micro. During inference, the runtime uses this memory to store model inputs, outputs, intermediate activation data, persistent model data, and temporary working buffers.

The arena must be large enough for all these allocations. If it is too small, TensorFlow Lite Micro cannot allocate the model tensors and inference fails. If it is unnecessarily large, it reserves internal SRAM that could otherwise be used by the camera, display, Wi-Fi, or application code.

## 💾 Understanding ESP32-S3 Memory

The ESP32-S3 uses different types of memory, each with a different role:
- **Flash memory** is long-term storage. It contains the compiled Arduino program, the Edge Impulse library, the TensorFlow Lite Micro model, and other static files. Flash keeps its contents when the board is powered off.

- **Internal SRAM** is the ESP32-S3's fast working memory. It is used while the program runs for variables, stacks, temporary buffers and, by default, the TensorFlow Lite Micro tensor arena. Internal SRAM is limited, so the tensor arena size must be chosen carefully.

- **PSRAM** is additional external working memory available on some ESP32-S3 boards. It is larger than internal SRAM but generally slower. In this sketch, PSRAM is used for the large input image buffer, preserving internal SRAM for the TensorFlow Lite Micro tensor arena and the rest of the application.

Both internal SRAM and PSRAM are volatile: their contents are cleared when the board is reset or powered off.

- **microSD card** is removable, non-volatile file storage. It can hold large datasets, captured camera images, logs, configuration files, or other user files, and it keeps its contents when the board is powered off. Unlike Flash, it does not normally store or run the Arduino program; unlike SRAM and PSRAM, it is not working memory for variables or the TensorFlow Lite Micro tensor arena. The ESP32-S3 reads and writes files on the card through an SDMMC or SPI interface.

---

## 🎯 What is this repository for?

When developing and deploying an on-device AI project on an ESP32-S3, two stages of the process require particularly careful choices:

1. Choosing an AI model in Edge Impulse that can actually run on an ESP32-S3. Edge Impulse offers many learning blocks, model architectures, and deployment options, but not every combination is suitable for the memory and performance limits of the ESP32-S3 board.

2. Deploying the model as an Arduino library and selecting the correct TensorFlow Lite Micro tensor arena size.

**This repository provides concrete, measured information about both critical points: choosing an Edge Impulse AI model that can run on an ESP32-S3, and configuring the required TensorFlow Lite Micro tensor arena memory. It helps avoid a situation where a model trains successfully but cannot be deployed or run reliably on the ESP32-S3.**

It is the result of many tests carried out to find a working combination of model, input size, deployment settings, memory configuration, and tensor arena size. By making this work public, the goal is to save time for hobbyists and makers who want to start a similar project.

## 📌 A practical ESP32-S3 model reference

The example behind this repository is an Edge Impulse image-classification project designed to recognize each face of a die. After many tests, the following combination was found to run on an ESP32-S3:

- Impulse image size: **224 x 224 RGB**
- Edge Impulse learning block: **Transfer Learning (Images)**
- Base model: **MobileNetV1 96x96 0.25**
- DSP time: approximately **71 ms**
- Neural-network classification time: approximately **429 ms**
- Total inference time: approximately **500 ms**
- Theoretical inference rate: approximately **2 images per second**
- Practical rate: close to **2 images per second**, before including camera capture, image conversion, display, Wi-Fi, logging, or other application tasks.

For this dice recognition project, a lower image resolution or grayscale images instead of RGB would probably have been sufficient, reducing the model's memory requirements and inference workload. The purpose of configuring the Edge Impulse impulse at 224 x 224 RGB was to explore the highest practical image resolution that could be processed on an ESP32-S3 for more demanding future projects.

In Edge Impulse, the image width and height selected in **Impulse Design > Create Impulse** define the image size processed by the Image data block and expected by the exported classifier. In this project, the impulse is configured for 224 x 224 RGB images.

The `96x96` part of the `MobileNetV1 96x96 0.25` name refers to the model's pre-training and nominal input resolution. Edge Impulse documents 96 x 96 as the model's optimal input resolution, but it also allows another image resolution to be selected in the impulse. Using 224 x 224 therefore increases the amount of input data, preprocessing work, memory use, and usually inference cost compared with a 96 x 96 impulse.
The `0.25` value is MobileNet's width multiplier. It reduces the number of channels in the neural network, reducing the model size, RAM requirement, and computation compared with wider MobileNet variants.

> [!NOTE]
> **MobileNetV1 96x96 0.25 was the most suitable model tested for this ESP32-S3 project. It provided a workable balance between classification accuracy, memory use, and inference time. This does not mean that MobileNetV1 96x96 0.25 is automatically the best choice for every ESP32-S3 project.**

The appropriate model depends on the required accuracy, the image size configured in the impulse, the number of classes, the available memory, and the acceptable neural-network inference time. The **Neural-network inference time (NN time)** is the time required by the ESP32-S3 to execute the trained neural network and produce the classification probabilities.

It is reported separately from the DSP time in Edge Impulse:

- **DSP time**: image preprocessing and feature preparation, such as converting the captured image into the numerical input buffer required by the classifier.
- **NN time** (or classification time): execution of the neural network itself, from the prepared input tensor to the final class probabilities.

For the reference test in this repository:
```text
DSP time: 71 ms
NN time: 429 ms
```

The complete classification processing time was therefore approximately:
```text
71 ms + 429 ms = 500 ms
```

**This corresponds roughly to a theoretical maximum of 2 inferences per second.**

The real end-to-end rate can be lower because a complete project may also need to capture an image, convert pixels, update a display, write logs, communicate over Wi-Fi, or perform other work between inferences.

---

## 🚧 The tensor arena problem

TensorFlow Lite Micro uses a **tensor arena**: a block of RAM reserved for tensors, intermediate activations, persistent model data, and temporary buffers required during inference.

The arena must be large enough before TensorFlow Lite Micro can successfully allocate the model tensors.

If it is too small:

- `AllocateTensors()` can fail.
- `run_classifier()` can return an error such as `-3`.
- Inference cannot start correctly.

If it is much larger than necessary:

- More RAM is reserved than the model actually needs.
- Less memory remains available for the camera, display, Wi-Fi, application logic, and other buffers.

This repository avoids the old trial-and-error workflow of manually changing the model header to a large temporary value first.

Instead, the diagnostic sketch defines:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

before including the Edge Impulse inference header. The modified `tflite_micro.h` supplied with this repository uses that value **temporarily for the diagnostic run**, while still preserving the arena size originally generated by Edge Impulse for comparison.

After TensorFlow Lite Micro successfully allocates its tensors, the modified SDK file captures:

```cpp
interpreter->arena_used_bytes()
```

and passes that measured value back to the Arduino sketch.

The sketch then reports:

```text
Tensor Arena used  : <M> bytes
```

`M` is the measured Tensor Arena usage for that exact exported model and runtime configuration.

For the reference model used in this repository:

```text
Tensor Arena used  : 536044 bytes
```

so:

```text
M = 536044 bytes
```

The sketch then automatically adds safety margins and prints the recommended final values.

> [!IMPORTANT]
> Never configure the final Tensor Arena below the measured value `M`.
>
> The suggested margins are practical starting points, not universal guarantees. Re-test the final application after applying the selected value.

---

## ✅ Requirements

You need:

- An **ESP32-S3 N16R8 development board** with PSRAM.

  This repository was tested with a **Goouuu ESP32-S3 N16R8**, a DevKitC-1-compatible board equipped with **16 MB of flash memory** and **8 MB of PSRAM**.
  Other ESP32-S3 N16R8 boards with PSRAM may also work, such as an ESP32-S3-WROOM-1 N16R8 module, XIAO ESP32S3 Sense, other ESP32-S3 development boards exposing sufficient PSRAM.

> [!NOTE]
> To keep this guide focused and reproducible, all Arduino IDE settings and technical recommendations in this repository are based on the **ESP32-S3 N16R8 configuration**: **16 MB flash** and **8 MB PSRAM**.
>
> If you use another ESP32-S3 board or another memory configuration, the sketch may still work, but you must adapt the Arduino IDE settings — especially **Board**, **Flash Size**, **PSRAM mode**, and **Partition Scheme** — to match your hardware.

- Arduino IDE 2.x.
- Espressif's ESP32 board package installed in Arduino IDE.
- An Edge Impulse project exported as an Arduino library using **TensorFlow Lite**.
- The `ESP32-S3_arduino_tflite_micro_arena_size_finder.ino` sketch from this repository.
- The modified diagnostic file supplied in:
  ```text
  edge-impulse-sdk-patch/tflite_micro.h
  ```
- A backup of the original Edge Impulse `tflite_micro.h` before replacing it temporarily.

---

## 🏗️ Step 1: Create and Export the Edge Impulse Model

### 1.1 🆕 Create the Edge Impulse project

In Edge Impulse Studio, click **Create new project**, give the project a name, then select **Espressif ESP-EYE (ESP32 240MHz)** as the target device.

Although this project runs on an ESP32-S3 camera board rather than the original ESP-EYE, this target is used as a practical starting point in Edge Impulse to obtain ESP32-oriented performance estimates. The final compatibility and memory requirement must still be verified on the actual ESP32-S3 board.

### 1.2 📸 Create the image dataset

![Create the dataset with 224x224 pictures](images/dataset.PNG)

Before creating the impulse, build and label the dataset that will be used to train the model.

In Edge Impulse Studio, open the **Data acquisition** page. You can capture images directly from a connected camera or upload existing image files. For every image, select the correct label and make sure that each die face has its own class.

For this dice-recognition project, create the following classes: `one`, `two`, `three`, `four`, `five`, `six`, `background`.

The `background` class is important. It contains images in which no die face should be recognized—for example, an empty white background, the table, the camera view without a die, or other irrelevant objects. It helps the model learn the difference between a die and its surroundings, reducing false detections.

For close-up object classification, make the die occupy approximately 60% to 80% of the image. It should be large enough for its dots and edges to remain clear after the image is resized, but leave some visible background around it.

> [!TIP]
> If possible, use a simple, neutral, and consistent background, such as a white sheet of paper. Capture background images under the same lighting and camera conditions used during normal operation. Include this background as its own recognition class.

For each die-face class, capture at least 30 to 50 images as a starting point; more images generally produce a more reliable model. Keep roughly the same number of images in every class, including the `background` class.

Vary the images deliberately:

- Change the die position and rotation.
- Move the die slightly closer to or farther from the camera.
- Vary the lighting intensity and direction.
- Include small changes in shadows and framing.
- Keep the die clearly visible and in focus.
- Use images captured with the same OV2640 camera and setup that will be used on the ESP32-S3.

Do not use only nearly identical photographs. The dataset must represent the real conditions in which the model will run. A model trained only with a perfectly centered die on a white background may fail when the die is rotated, shifted, partially shadowed, or viewed under different lighting.

Use most images for **Training data** and reserve separate images for **Test data**. A practical starting split is approximately 80% training and 20% testing images.

### 1.3 🧩 Create the impulse

After creating and labeling your image dataset in the **Data acquisition** page, open **Impulse #x > Create impulse** to configure your image-classification impulse.

![Create Impulse 224x224](images/Edge-Impulse_Create-Impulse.PNG)

1. Open **Impulse #x > Create impulse**.
2. Set both **Image width** and **Image height** to `224`.
3. Add the **Images** processing block.
4. Add the **Transfer Learning (Images)** learning block.
5. Click **Save impulse**.
6. Open the **Image** page, select the appropriate color depth for your camera (usually **RGB**), then save the parameters.
7. On the same page, open the **Generate features** tab and click **Generate features**.
8. Open the **Transfer Learning** tab, configure the model, and train it with your labeled images.

The resulting model expects 224×224 pixel images captured by the camera connected to the ESP32-S3.

### 1.4 🧠 Configure the Transfer Learning model

<table>
<tr>
<td valign="top">

Open the **Transfer Learning** tab to configure and train the image-classification model.

The settings below are a useful starting point, not fixed requirements. Train the model first, review its performance, then adjust the number of training cycles, learning rate, augmentation settings, or model architecture if necessary.

- **Number of training cycles:** start with `50`.
- **Learning rate:** start with `0.001`.
- **Training processor:** select **CPU**.
- **Data augmentation:** enable this option if you have only a small number of training images—such as 15 to 20 images per class. It creates modified versions of the training images, for example with small changes in position, crop, or brightness, helping the model generalize better. It does not replace collecting real, varied images.
- Under **Neural network architecture**, click **Choose a different model** and select **MobileNetV1 96x96 0.25**. This compact model is a good choice for an ESP32-S3 because it uses substantially less RAM and flash memory than larger architectures.

Click **Save & train** to start training the model.

</td>
<td width="360" valign="top">

<img src="images/Transfer_learning.PNG"
     alt="Edge Impulse Transfer Learning configuration"
     width="340">

</td>
</tr>
</table>

> [!IMPORTANT]
> Image-recognition quality depends primarily on the dataset: the number of images per class, accurate labels, sharpness, lighting conditions, camera angle, distance to the die, background variation, and how closely the training images match the real conditions in which the ESP32-S3 camera will be used. Include representative examples of every die face and avoid using near-identical images only.
>
> The model also plays an important role: it is the neural-network architecture that learns visual patterns from the training images and uses them to classify new camera images. A larger model can potentially recognize more complex patterns, but it requires more Flash memory, RAM, and processing time on the ESP32-S3.
>
> **MobileNetV1 96×96 0.25** is a compact neural-network architecture chosen as a practical compromise between recognition performance and the limited resources of an embedded device. According to Edge Impulse, it requires approximately 105.9 KB of RAM and 301.6 KB of ROM. This makes it well suited to deployment on the ESP32-S3 while leaving memory available for the camera image buffer, the TensorFlow Lite Micro tensor arena, and the rest of the application.
>
> **Data augmentation** improves robustness by creating modified training examples, such as small changes in crop, position, brightness, or orientation. It is especially useful with a small dataset, but it cannot compensate for images that are poorly labeled, too similar, blurry, or unrepresentative of real operating conditions.

### 1.5 📤 Select the deployment target

On the **Deployment** page, configure the export as follows:

1. Under **Deployment target**, select **Arduino library**.
2. Under **Inference engine**, select **TensorFlow Lite**.
3. Under **Model optimizations and performance**, select **Quantized (int8)**.
4. Click **Build** to generate and download the Arduino-library ZIP file.

Do not extract the downloaded ZIP file. It will be imported directly into the Arduino IDE in the next step.

![Deployment target](images/Deployment_target.PNG)

> [!IMPORTANT]
> As of August 2026, select **TensorFlow Lite** and do **not** use the **EON compiler** / EON-optimized variant for this Arduino ESP32-S3 workflow. EON deployments have caused compatibility and memory-management problems in Arduino-based ESP32-S3 projects, including incorrect inference results, crashes, reboot loops, and corrupted-heap errors. **TensorFlow Lite Micro is the more reliable option for this procedure.**
> This repository also measures the TensorFlow Lite Micro tensor arena with `arena_used_bytes()`. The measured result therefore applies only to the TensorFlow Lite model and runtime, not to an EON-compiled model.
>
> Select **Quantized (int8)**, not **Unoptimized (float32)**. An int8 model uses 8-bit integer values instead of 32-bit floating-point values. It is substantially smaller and generally faster on an ESP32-S3, requiring less Flash memory and RAM.
The tensor-arena size depends on the exact deployed model. If you switch between int8 and float32, change the image size, model architecture, or number of classes, build a new library and measure the tensor arena again.

---

## 📥 Step 2: Install your Edge Impulse project as an Arduino library

Import the Arduino library ZIP file generated by Edge Impulse into the Arduino IDE.

1. Open the **Arduino IDE**.
2. Select **Sketch > Include Library > Add .ZIP Library…**
3. Select the ZIP file downloaded from the Edge Impulse **Deployment** page.
4. Wait for the confirmation message indicating that the library was installed successfully.

Do not extract the ZIP file before importing it. The Arduino IDE installs the library directly from the downloaded archive.

On Windows, the full path is often similar to:

```text
C:\Users\<your-user-name>\Documents\Arduino\libraries\<your-project-name>\
```
Inside this library you will find:

```text
src\
src\<your-model>_inferencing.h
src\tflite-model\
src\edge-impulse-sdk\
```
These folders are important later in the process.

---

## ⚙️ Step 3: Configure Arduino IDE

Connect your ESP32-S3 and check the **Tools** menu in Arduino IDE.

| Arduino IDE menu | Recommended setting | Meaning |
|---|---|---|
| **Tools > Board** | Select your actual ESP32-S3 board, for example `ESP32S3 Dev Module` or `XIAO ESP32S3 Sense` | The selected board controls available flash, PSRAM, USB, upload, and partition options |
| **Tools > Port** | Select the serial port assigned to the ESP32-S3 | Required for upload and Serial Monitor output |
| **Tools > USB CDC On Boot** | `Enabled` | If disabled, Serial output may not be visible over USB on boards with native USB support |
| **Tools > Flash Size** | `16MB (128Mb)` for an ESP32-S3 N16R8 board | `N16R8` indicates 16 MB of Flash memory and 8 MB of PSRAM |
| **Tools > PSRAM** | Enable the correct PSRAM option, often `OPI PSRAM` | The sketch allocates the input buffer in PSRAM |
| **Tools > Partition Scheme** | `16M Flash (3MB APP / 9.9MB FATFS)` | Ensures that the partition table is configured for the board’s full 16 MB of Flash memory |
| **Tools > Core Debug Level** | `None` or `Error` | Optional; reduces unrelated serial messages |

> [!TIP]
> The tensor arena uses **RAM**, not flash partition space. Changing the partition scheme does not automatically make the tensor arena larger.

---

## 🛠️ Step 4: Adapt the sketch to your Edge Impulse Arduino library

Open the sketch from this repository in Arduino IDE.

Near the beginning of the sketch, the user-configuration block contains the diagnostic Arena definition followed by the Edge Impulse inference header:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000

#include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS
```

Only the line marked `CHANGE THIS` normally needs to be adapted.

### 4.1 🔗 Include your inference header

Replace `YOUR_LIBRARY_inferencing.h` with the exact inference-header filename generated by your Edge Impulse project.

For example, if your Edge Impulse Arduino library is named **Dice**, use:

```cpp
#include <Dice_inferencing.h>
```

To find the exact filename, open:

```text
...\Arduino\libraries\<your-project-name>\src\
```

and look for the file ending in:

```text
_inferencing.h
```

> [!IMPORTANT]
> The diagnostic macro must remain **before** the Edge Impulse inference header:
>
> ```cpp
> #define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
> #include <YOUR_LIBRARY_inferencing.h>
> ```
>
> The modified `tflite_micro.h` reads this macro while the Edge Impulse library is being compiled.

### 4.2 📏 Diagnostic Arena size

The default diagnostic size is:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

This value is temporary. It does **not** replace the final Tensor Arena value in your model header.

For the reference project, `1000000` bytes is large enough to let TensorFlow Lite Micro allocate the model and measure its real usage.

If `run_classifier()` fails with an allocation-related error such as `-3`, and the Tensor Arena usage is not captured, you can try a larger diagnostic value, for example:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1500000
```

or:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 2000000
```

Do not increase this value blindly. If the board cannot compile, boot, or allocate the diagnostic Arena, reduce the model's memory requirements or investigate the board's memory configuration.

---

## 🧩 Step 5: Temporarily install the modified `tflite_micro.h`

The repository supplies a ready-to-use modified Edge Impulse SDK file:

```text
edge-impulse-sdk-patch/tflite_micro.h
```

It replaces the old procedure of manually editing several lines inside the Edge Impulse SDK.

### 5.1 💾 Back up the original file

Locate the original file installed with your Edge Impulse Arduino library:

```text
...\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
```

Before replacing it, make a backup. For example, copy it somewhere safe or rename a copy to:

```text
tflite_micro.h.original
```

### 5.2 🔄 Replace it temporarily

Copy the repository version:

```text
edge-impulse-sdk-patch/tflite_micro.h
```

over the original file in:

```text
...\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\
```

The filename used by the library must remain:

```text
tflite_micro.h
```

The modified file performs three diagnostic tasks:

1. It temporarily uses `EI_TFLITE_ARENA_DIAGNOSTIC_SIZE` instead of the model-header Arena size.
2. It records the Arena size originally configured by Edge Impulse.
3. After `AllocateTensors()` succeeds, it records `interpreter->arena_used_bytes()` so the sketch can calculate the recommended final values automatically.

All project-specific modifications inside the supplied file are clearly marked with comments such as:

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
> This is a **temporary diagnostic replacement**, not a permanent replacement for the Edge Impulse SDK file.
>
> After you have written the selected Tensor Arena value into your model header, restore the original `tflite_micro.h` before compiling your final application.
>
> The modified file is based on the Edge Impulse SDK version used for this project. If a future Edge Impulse export contains a substantially different `tflite_micro.h`, compare the versions before replacing it.

---

## ⚡ Step 6: Compile, upload, and run the diagnostic

1. Open the adapted `.ino` sketch in Arduino IDE.
2. Confirm that the modified `tflite_micro.h` is installed in the Edge Impulse library.
3. Click **Upload** to compile and flash the ESP32-S3.
4. Open **Tools > Serial Monitor** and set the baud rate to **115200**.
5. If no output appears, press the board's **Reset** button.

> [!NOTE]
> The first compilation can take more than 10 minutes because Arduino IDE must compile the Edge Impulse library, TensorFlow Lite Micro, and their dependencies. Later compilations are usually faster.

A successful run is organized into four sections:

```text
[1] MEMORY CHECK
[2] DIAGNOSTIC CONFIGURATION
[3] INFERENCE TEST
[4] ARENA RECOMMENDATION
```

The most important measurement is:

```text
Tensor Arena used  : <M> bytes
```

For the reference model:

```text
Tensor Arena used  : 536044 bytes
```

which means:

```text
M = 536044 bytes
```

The sketch also reports the Arena originally generated by Edge Impulse:

```text
Edge Impulse arena : 519232 bytes
Measured usage (M) : 536044 bytes
```

Because `519232 < 536044`, the sketch prints:

```text
WARNING: The arena generated by Edge Impulse is smaller
than the measured arena usage.
```

This is exactly the type of mismatch the diagnostic is designed to detect.

---

## 🧮 Step 7: Read the automatically calculated final Arena sizes

The sketch performs the safety-margin calculations automatically.

It uses:

```text
SMALL  = M + 16 KiB
MEDIUM = M + 32 KiB
LARGE  = M + 64 KiB
```

For the reference measurement:

```text
M = 536044 bytes
```

the sketch calculates:

| Recommendation | Calculation | Final Arena size |
|---|---:|---:|
| **SMALL** | 536044 + 16384 | **552428 bytes** |
| **MEDIUM** | 536044 + 32768 | **568812 bytes** |
| **LARGE** | 536044 + 65536 | **601580 bytes** |

The Serial Monitor displays:

```text
SMALL  : 552428 bytes  (+16 KiB)
MEDIUM : 568812 bytes  (+32 KiB)  <-- RECOMMENDED
LARGE  : 601580 bytes  (+64 KiB)

Recommended value: 568812 bytes
```

For this project, **MEDIUM** is the recommended starting point because it provides a useful safety margin without keeping the full temporary diagnostic Arena.

> [!IMPORTANT]
> The measured value `M` is the lower bound observed for this exact model/runtime configuration. Do not set the final Arena below it.
>
> SMALL / MEDIUM / LARGE are suggested safety margins. They are not universal guarantees. Always compile and test the final application after applying the selected value.

---

## 💾 Step 8: Save the final Tensor Arena value and restore the original SDK file

The diagnostic sketch tells you exactly which model-header file to update:

```text
src/tflite-model/tflite_learn_XXXXXXX_ZZ.h
```

Open the matching file inside your installed Edge Impulse Arduino library:

```text
...\Arduino\libraries\<your-project-name>\
src\tflite-model\tflite_learn_XXXXXXX_ZZ.h
```

Find the two Tensor Arena definitions. For the reference model used in this repository, the original Edge Impulse export contains:

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 519232
const size_t tflite_learn_1072522_47_arena_size = 519232;
```

> [!NOTE]
> These names are only examples from the reference model used in this repository.
> Your own Edge Impulse export will normally contain a different generated model identifier, so the filename, macro name, and variable name may be different.
>
> Do not copy the `1072522_47` identifier from this example. Always modify the two Tensor Arena definitions that already exist in the `tflite_learn_*.h` file generated for your own Edge Impulse model.

The diagnostic recommends:

```text
MEDIUM : 568812 bytes  (+32 KiB)  <-- RECOMMENDED
```

Update **both** lines to the same selected value:

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1072522_47_ARENA_SIZE 568812
const size_t tflite_learn_1072522_47_arena_size = 568812;
```

Save the model header.

### 8.1 ♻️ Restore the original `tflite_micro.h`

The measurement is now complete.

Remove the diagnostic replacement and restore the original Edge Impulse file that you backed up in Step 5:

```text
src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
```

Your final project should therefore use:

- the **updated Tensor Arena value** in `tflite_learn_XXXXXXX_ZZ.h`;
- the **original Edge Impulse `tflite_micro.h`**.

---

## 🚀 Step 9: Return to your original project

**The Tensor Arena sizing process is now complete !**

You now have a measured and properly sized Tensor Arena for your Edge Impulse model.

At this stage:

1. Keep the new Tensor Arena value in your model header:
   ```text
   ...\src\tflite-model\tflite_learn_XXXXXXX_ZZ.h
   ```

2. Make sure the **original Edge Impulse `tflite_micro.h`** has been restored:
   ```text
   ...\src\edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
   ```

3. Return to your original Arduino project.

4. Compile and test your **final application** again.

> [!IMPORTANT]
> The Tensor Arena diagnostic is now finished.
> You can continue developing your original project — camera capture, image processing, display, Wi-Fi, sensors, application logic, or any other features — using a Tensor Arena size that has been measured on the ESP32-S3 instead of estimated.
>
> If you later retrain the model, change its input dimensions, architecture, quantization, or export a new Edge Impulse library, run the Tensor Arena Size Finder again because the required Arena size may change.

---

# 📎 Appendices

The main Tensor Arena sizing procedure is now complete.

The following sections provide additional reference information, example output, and troubleshooting guidance.

---

## 🖥️ Appendix A: Example serial output

A successful diagnostic run for the reference model looks like this:

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

### 📖 How to read this output

| Output | Meaning |
|---|---|
| `PSRAM : OK` | PSRAM was detected and is available for the diagnostic sketch |
| `Diagnostic arena : 1000000 bytes` | Temporary Arena used only for the measurement run |
| `Input buffer : 602112 bytes in PSRAM` | The Edge Impulse input feature buffer was successfully allocated in PSRAM |
| `run_classifier() : OK` | Tensor allocation and the test inference completed successfully |
| `Tensor Arena used : 536044 bytes` | Measured Tensor Arena usage `M` for the reference model |
| `Edge Impulse arena : 519232 bytes` | Arena size originally generated by Edge Impulse |
| `WARNING: ... smaller than the measured arena usage` | The original `519232`-byte Arena is below the measured `536044`-byte usage |
| `MEDIUM : 568812 bytes (+32 KiB) <-- RECOMMENDED` | Recommended starting value for this reference model |
| `TEST COMPLETE` | The diagnostic completed and the model-header values can now be updated |

The important distinction is:

```text
Edge Impulse arena : 519232 bytes
Diagnostic arena   : 1000000 bytes
Measured usage (M) : 536044 bytes
Recommended value  : 568812 bytes
```

These values have different roles:

- **519232 bytes** is the value originally generated in the model header.
- **1000000 bytes** is the temporary diagnostic Arena used only by the size finder.
- **536044 bytes** is the measured Tensor Arena usage.
- **568812 bytes** is the recommended MEDIUM value for the final model header.

> [!IMPORTANT]
>
> ## 🎯 The values to keep
>
> For this exact reference model:
>
> ```text
> Measured usage (M) : 536044 bytes
> Recommended value  : 568812 bytes
> ```
>
> Do not configure the final Arena below `536044` bytes. The recommended starting value produced by the current sketch is `568812` bytes, which adds a `32 KiB` safety margin.

---

## 🩺 Appendix B: Troubleshooting

The items below are ordered roughly by when they are most likely to appear during the workflow: **compilation first, then startup and memory detection, then inference and measurement, and finally camera integration in the original project.**

### 📁 1. `YOUR_LIBRARY_inferencing.h` not found

This error normally appears during compilation if the placeholder inference header has not been replaced.

Replace:

```cpp
#include <YOUR_LIBRARY_inferencing.h>
```

with the exact filename found in:

```text
...\Arduino\libraries\<your-project-name>\src\
```

The filename must normally end in:

```text
_inferencing.h
```

Example:

```cpp
#include <Dice_inferencing.h>
```

Case matters on some operating systems.

---

### 🔗 2. Errors involving `ei_tflite_arena_used_bytes` or `ei_tflite_model_arena_configured_bytes`

These variables are part of the diagnostic connection between the sketch and the modified `tflite_micro.h`.

If compilation or linking reports one of these names:

- Confirm that you are using the current sketch and the current modified `tflite_micro.h` from the same repository version.
- Do not mix an older intermediate patch with the current sketch.
- Replace the SDK file again with:
  ```text
  edge-impulse-sdk-patch/tflite_micro.h
  ```
- Recompile the sketch.

---

### 💽 3. Sketch is too large for the partition

This error appears during compilation or upload when the selected APP partition is too small for the compiled program.

This is a flash-partition problem, not a Tensor Arena problem.

Choose a partition scheme with a larger APP partition, or create a custom `partitions.csv` file.

Remember:

- The **APP partition** stores the compiled program.
- The **Tensor Arena** uses RAM at runtime.
- Increasing the APP partition does not increase internal SRAM or PSRAM.

---

### 🔇 4. No Serial output / board seems frozen

If the sketch uploads successfully but nothing appears in Serial Monitor:

1. Check **Tools > USB CDC On Boot** and set it to **Enabled** when using the board's native USB port.
2. Confirm that Serial Monitor is set to **115200 baud**.
3. Press the board's **Reset** button after opening Serial Monitor.
4. Avoid a blocking `while (!Serial);` without a timeout.

The project sketch already uses a timeout:

```cpp
unsigned long start = millis();
while (!Serial && (millis() - start < 2000)) {
    delay(10);
}
```

---

### ⚡ 5. `PSRAM : NOT DETECTED`

If the sketch starts but the memory check shows:

```text
PSRAM              : NOT DETECTED
```

check:

1. Your board actually includes PSRAM.
2. **Tools > PSRAM** is enabled.
3. The selected PSRAM mode matches the board, often `OPI PSRAM` for ESP32-S3 N16R8 boards.
4. The selected board definition is correct.

The diagnostic stops if PSRAM is not detected because the large input feature buffer is intentionally allocated there.

---

### ❌ 6. `AllocateTensors() failed` or `run_classifier()` returns `-3`

An allocation-related `-3` can mean that the diagnostic Tensor Arena is still too small.

The default is:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000
```

You can try a larger diagnostic value, for example:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1500000
```

or:

```cpp
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 2000000
```

Then compile and run the diagnostic again.

Also check:

- The modified repository version of `tflite_micro.h` is installed in the Edge Impulse library.
- `EI_TFLITE_ARENA_DIAGNOSTIC_SIZE` is defined **before** the Edge Impulse inference header.
- You selected **TensorFlow Lite**, not EON, during Edge Impulse deployment.
- The Edge Impulse library being compiled is the same library you modified.
- PSRAM is enabled and detected.

> [!WARNING]
> Do not keep increasing the diagnostic Arena indefinitely. If a larger value prevents compilation, boot, or allocation, investigate the model size and the board's memory usage instead.

---

### ❌ 7. `Tensor Arena usage was not captured`

If `run_classifier()` completes but the sketch reaches the recommendation stage without a measured Tensor Arena value, the modified SDK file is probably missing, outdated, or not the copy being compiled.

Check:

1. The file installed at:
   ```text
   src/edge-impulse-sdk/classifier/inferencing_engines/tflite_micro.h
   ```
   is the modified file supplied by this repository.
2. You saved the file before recompiling.
3. Arduino IDE is compiling the expected Edge Impulse library.
4. The modified file contains the project blocks marked:
   ```cpp
   // MODIFICATION - ESP32-S3 Tensor Arena Size Finder
   ```
5. Recompile and upload the sketch after replacing the SDK file.

---

### ⚠️ 8. Camera compatibility and troubleshooting

This issue normally belongs to the **original application**, after the Tensor Arena diagnostic itself has been completed.

This repository focuses on TensorFlow Lite Micro Tensor Arena sizing. The diagnostic sketch does not require live camera input because it runs inference with a zero-filled test buffer.

Before calling `run_classifier()` with real camera data in your final application, validate the camera separately with a minimal sketch:

1. Initialize the OV2640 camera with the exact GPIO pin mapping for your board.
2. Capture and return camera frames repeatedly.
3. Confirm that the chosen frame size and pixel format work reliably.
4. Only then add image conversion, resizing, and TensorFlow Lite Micro inference.

> [!WARNING]
> A model input size such as 96×96 or 224×224 is not necessarily a camera capture mode. The camera may capture a supported source frame size, then the application converts and resizes that frame into the model input buffer.
>
> Do not assume that an OV2640 board supports every resolution or raw pixel format reliably. Verify JPEG, RGB565, and the selected frame size with the exact board, camera driver, Arduino-ESP32 core version, and pin configuration before integrating Edge Impulse.

---

## 📚 References

- [Edge Impulse - Transfer learning for images](https://docs.edgeimpulse.com/studio/projects/learning-blocks/blocks/transfer-learning-images)
- [Edge Impulse - Image classification tutorial](https://docs.edgeimpulse.com/tutorials/end-to-end/image-classification)
- [Edge Impulse - EON Compiler](https://docs.edgeimpulse.com/studio/projects/deployment/eon-compiler)
- [Edge Impulse - Run Arduino library with Arduino IDE 2.x](https://docs.edgeimpulse.com/hardware/deployments/run-arduino-2-0)
- [Espressif Arduino-ESP32 - Partition tables](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html)

## 🌐 Resources

- [Edge Impulse Forum](https://forum.edgeimpulse.com/)

---

## ⚖️ License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.
