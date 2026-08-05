# 🤖 Arduino ESP32-S3 + Edge Impulse: Find the Right Tensor Arena Size

## 👋 Introduction

This repository contains an **Arduino sketch** that measures the **TensorFlow Lite Micro tensor arena** used by an **Edge Impulse model** on an **ESP32-S3** with PSRAM.

Beyond the usual hobbyist and maker applications, the ESP32-S3 also makes it possible to explore original and engaging on-device AI projects at a modest cost. Its dual-core processor, optional PSRAM, Wi-Fi and Bluetooth connectivity, and support for AI and DSP-oriented instructions provide a practical base for image classification, sound recognition, gesture detection, sensor analysis, and connected edge-AI devices.

Together with **[Edge Impulse](https://edgeimpulse.com/)**, ESP32-S3 provides a workflow for collecting data, training a model, exporting an Arduino library, and running inference on a device without having to build the entire machine-learning deployment stack from scratch.

**This repository is intended for Arduino hobbyists and makers who have created an AI model with Edge Impulse and want to deploy it reliably on an ESP32-S3 as an Arduino library.**

> [!NOTE]
> The example used in this repository is an image-classification model created in Edge Impulse to recognize each face of a die with 224x224 images.
>![Dice 224x224](images/dice_224x224.jpg)
>
> > [!NOTE]
> <table>
>   <tr>
>     <td valign="top">
>       The example used in this repository is an image-classification model created in Edge Impulse to recognize each face of a die.
>
>       The model uses 224×224 pixel images.
>     </td>
>     <td width="240">
>       <img src="images/dice_224x224.jpg"
>            alt="Yellow die used by the image-classification model"
>            width="224">
>     </td>
>   </tr>
> </table>

## 🔍 What the sketch does

The Arduino sketch in this repository:

- Prints free internal SRAM heap.
- Prints the largest available contiguous SRAM heap block.
- Prints free and total PSRAM.
- Allocates the Edge Impulse input feature buffer in PSRAM.
- Runs `run_classifier()` once with a zero-filled test input.
- Prints memory usage before and after inference.
- Prints classification output and timing.
- Displays the TFLite Micro tensor-arena measurement after a small debug addition inside the Edge Impulse SDK.

The sketch is a diagnostic and sizing tool. It does **not** replace the camera or sensor code in a final project.

## 📖 A few technical terms

Before continuing, here are two technical terms used throughout this guide:

**TensorFlow Lite Micro** (also called **TFLite Micro** or **TFLM**) is a lightweight C++ runtime designed to run trained machine-learning models directly on microcontrollers and other embedded devices with limited memory.

A **tensor arena** is a single block of RAM reserved in advance for TensorFlow Lite Micro. During inference, the runtime uses this memory to store model inputs, outputs, intermediate activation data, persistent model data, and temporary working buffers.

The arena must be large enough for all these allocations. If it is too small, TensorFlow Lite Micro cannot allocate the model tensors and inference fails. If it is unnecessarily large, it reserves internal SRAM that could otherwise be used by the camera, display, Wi-Fi, or application code.

## 🧠 Understanding ESP32-S3 Memory

The ESP32-S3 uses different types of memory, each with a different role:
- **Flash memory** is long-term storage. It contains the compiled Arduino program, the Edge Impulse library, the TensorFlow Lite Micro model, and other static files. Flash keeps its contents when the board is powered off.

- **Internal SRAM** is the ESP32-S3's fast working memory. It is used while the program runs for variables, stacks, temporary buffers, and—by default—the TensorFlow Lite Micro tensor arena. Internal SRAM is limited, so the tensor arena size must be chosen carefully.

- **PSRAM** is additional external working memory available on some ESP32-S3 boards. It is larger than internal SRAM but generally slower. In this sketch, PSRAM is used for the large input image buffer, preserving internal SRAM for the TensorFlow Lite Micro tensor arena and the rest of the application.

Both internal SRAM and PSRAM are volatile: their contents are cleared when the board is reset or powered off.

---

## 🎯 Why this repository exists

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
- Neural-network classification time: approximately **428 ms**
- Total inference time: approximately **499 ms**
- Practical rate: just under **2 images per second**

For this die-recognition project, a lower image resolution would probably have been sufficient. The purpose of configuring the Edge Impulse impulse at 224 x 224 was to explore the highest practical image resolution that could be processed on an ESP32-S3 for more demanding future projects.

In Edge Impulse, the image width and height selected in **Impulse Design > Create Impulse** define the image size processed by the Image data block and expected by the exported classifier. In this project, the impulse is configured for 224 x 224 RGB images.

The `96x96` part of the `MobileNetV1 96x96 0.25` name refers to the model's pre-training and nominal input resolution. Edge Impulse documents 96 x 96 as the model's optimal input resolution, but it also allows another image resolution to be selected in the impulse. Using 224 x 224 therefore increases the amount of input data, preprocessing work, memory use, and usually inference cost compared with a 96 x 96 impulse.

MobileNetV1 96x96 0.25 was the most suitable model tested for this ESP32-S3 project. It provided a workable balance between classification accuracy, memory use, and inference time.

The `0.25` value is MobileNet's width multiplier. It reduces the number of channels in the neural network, reducing the model size, RAM requirement, and computation compared with wider MobileNet variants.

This does not mean that MobileNetV1 96x96 0.25 is automatically the best choice for every ESP32-S3 project. The appropriate model depends on the required accuracy, the image size configured in the impulse, the number of classes, the available memory, and the acceptable neural-network inference time.

Here, **neural-network inference time** is the time required by the ESP32-S3 to execute the trained neural network and produce the classification probabilities.

It is reported separately from the DSP time in Edge Impulse:

- **DSP time**: image preprocessing and feature preparation, such as converting the captured image into the numerical input buffer required by the classifier.
- **NN time** (or classification time): execution of the neural network itself, from the prepared input tensor to the final class probabilities.

For the reference test in this repository:

```text
DSP time: 71 ms
NN time: 428 ms
```

The complete classification processing time was therefore approximately:

```text
71 ms + 428 ms = 499 ms
```

This measurement does not necessarily include camera capture, image acquisition, screen updates, Wi-Fi communication, logging, or other code running in the final application.

---

## 🧠 The tensor arena problem

TensorFlow Lite Micro uses a **tensor arena**: a block of RAM reserved for tensors, intermediate activations, and other model allocations required during inference.

The arena must be large enough before TensorFlow Lite Micro creates the interpreter.

If it is too small:

- `AllocateTensors()` can fail.
- `run_classifier()` can return an error such as `-3`.
- The serial monitor can show messages such as `AllocateTensors() failed`.
- The device can become unstable if the application is already close to its RAM limits.

If it is much larger than necessary:

- Internal SRAM is reserved unnecessarily.
- Less memory remains for the camera, display, Wi-Fi, application logic, and other buffers.

The correct approach is:

1. Start with a deliberately large tensor arena.
2. Measure the actual arena use.
3. Add a safety margin.
4. Set the final arena size.
5. Compile and test again.

> [!IMPORTANT]
> The key line produced by this repository is:
>
> ```text
> DEBUG: Tflite arena used bytes: <M>
> ```
>
> `<M>` is the measured minimum tensor-arena usage for your model. Never configure the final arena below this value.

For example:

```text
DEBUG: Tflite arena used bytes: 536059
```

means that TensorFlow Lite Micro actually used:

```text
M = 536059 bytes
```

for that specific model.

---

## ✅ Requirements

You need:

- An **ESP32-S3 N16R8 development board** with PSRAM.

  This repository was tested with a **Goouuu ESP32-S3 N16R8**, a DevKitC-1–compatible board equipped with **16 MB of flash memory** and **8 MB of PSRAM**.

  Other ESP32-S3 boards with PSRAM may also work, such as an ESP32-S3-DevKitC-1 N16R8 or another development board based on an ESP32-S3-WROOM-1 N16R8 module.

> [!NOTE]
> To keep this guide focused and reproducible, all Arduino IDE settings and technical recommendations in this repository are based on the **ESP32-S3 N16R8 configuration**: **16 MB flash** and **8 MB PSRAM**.
>
> If you use another ESP32-S3 board or another memory configuration, the sketch may still work, but you must adapt the Arduino IDE settings—especially **Board**, **Flash Size**, **PSRAM mode**, and **Partition Scheme**—to match your hardware.

- Arduino IDE 2.x.
- Espressif's ESP32 board package installed in Arduino IDE.
- An Edge Impulse project exported as an Arduino library using TensorFlow Lite.
- The `.ino` sketch from this repository.

---

## ⚠️ Step 1: Export TensorFlow Lite

### 🧩 Use TensorFlow Lite, not EON

In Edge Impulse Studio, export the project as an **Arduino library** using **TensorFlow Lite**.

For this procedure, do **not** use the EON compiler / EON-optimized variant.

1. Open your Edge Impulse project.
2. Go to **Deployment**.
3. Select **Arduino library**.
4. Set the inference engine or model optimization to **TensorFlow Lite**.
5. Do not select the EON-optimized version for this workflow.
6. Click **Build**.
7. Download the generated `.zip` Arduino library.

> [!WARNING]
> This sketch measures `tflite::MicroInterpreter::arena_used_bytes()`, so it applies to the TensorFlow Lite Micro interpreter path. EON and TensorFlow Lite Micro are different deployment approaches. As of August 2026, use the standard TensorFlow Lite Micro path for this Arduino ESP32-S3 debugging and arena-sizing workflow.

---

## 💾 Step 2: Install the Edge Impulse library

1. Open **Arduino IDE 2.x**.
2. Select:

   ```text
   Sketch > Include Library > Add .ZIP Library...
   ```

3. Select the Edge Impulse `.zip` file downloaded from Edge Impulse Studio.
4. Arduino IDE installs the library.

The installed library is usually located under:

```text
Documents/Arduino/libraries/<your-project-name>/
```

On Windows, the full path is often similar to:

```text
C:\Users\<your-user-name>\Documents\Arduino\libraries\<your-project-name>\
```

Inside this library you will find:

```text
src/
src/<your-model>_inferencing.h
src/tflite-model/
src/edge-impulse-sdk/
```

These folders are important later in the process.

---

## 🛠️ Step 3: Configure Arduino IDE

Connect your ESP32-S3 and check the **Tools** menu in Arduino IDE.

| Arduino IDE menu | Recommended setting | Why it matters |
|---|---|---|
| **Tools > Board** | Select your actual ESP32-S3 board, for example `ESP32S3 Dev Module` or `XIAO ESP32S3 Sense` | The selected board controls available flash, PSRAM, USB, upload, and partition options |
| **Tools > Port** | Select the serial port assigned to the ESP32-S3 | Required for upload and Serial Monitor output |
| **Tools > Flash Size** | `8MB (64Mb)` if your board has 8 MB flash | Must match the installed ESP32-S3 module |
| **Tools > PSRAM** | Enable the correct PSRAM option, often `OPI PSRAM` | The sketch allocates the input buffer in PSRAM |
| **Tools > Partition Scheme** | `8M with spiffs (3MB APP / 1.5MB SPIFFS)` as a starting point | Provides approximately 3 MB for firmware and 1.5 MB for SPIFFS files |
| **Tools > Core Debug Level** | `None` or `Error` | Optional; reduces unrelated serial messages |

### 🗂️ About `8M with spiffs`

For most Edge Impulse projects on an ESP32-S3 with 8 MB flash, this is a reasonable initial choice:

```text
8M with spiffs (3MB APP / 1.5MB SPIFFS)
```

It provides approximately:

- **3 MB APP**: application firmware, including the Edge Impulse model and libraries.
- **1.5 MB SPIFFS**: optional flash filesystem for images, configuration files, logs, and other files.

It is not necessarily the best choice for every project:

- Choose a larger application partition if the Arduino compiler reports that the sketch is too large.
- Keep SPIFFS storage if your project needs files stored in flash.
- Use a custom `partitions.csv` later if you need a different flash-memory layout.
- Use an OTA-oriented partition scheme only if the project needs over-the-air firmware updates.

> [!TIP]
> The tensor arena uses **RAM**, not flash partition space. Changing the partition scheme does not automatically make the tensor arena larger.

---

## ✏️ Step 4: Adapt the sketch

Open the sketch from this repository.

Near the beginning, find these two placeholder lines:

```cpp
#include <YOURMODEL_inferencing.h>          // <-- CHANGE THIS
extern const size_t tflite_learn_XXXXXXX_ZZ_arena_size;   // <-- CHANGE THIS
```

You must replace both placeholders with names from your own Edge Impulse Arduino library.

### 📄 Find your inference header

Open this folder:

```text
...\Arduino\libraries\<your-project-name>\src\
```

Look for a file ending with:

```text
_inferencing.h
```

For example:

```text
Dice_inferencing.h
```

Then modify the sketch:

```cpp
#include <Dice_inferencing.h>
```

The filename depends on your own Edge Impulse project.

### 🔎 Find your arena symbol

Open:

```text
...\Arduino\libraries\<your-project-name>\src\tflite-model\
```

Look for a file similar to:

```text
tflite_learn_1077716_45.h
```

Open it and search for:

```text
arena_size
```

You should find something similar to:

```cpp
const size_t tflite_learn_1077716_45_arena_size = 894991;
```

Copy the exact symbol name into the Arduino sketch:

```cpp
extern const size_t tflite_learn_1077716_45_arena_size;
```

> [!NOTE]
> The numbers and names are model-specific. Do not copy the example numbers unless your own exported library uses them.

---

## 📏 Step 5: Set a large temporary arena

Stay in the same model header:

```text
...\Arduino\libraries\<your-project-name>\src\tflite-model\
tflite_learn_XXXXXXX_ZZ.h
```

Search for the arena size definitions.

They may initially look like this:

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1077716_45_ARENA_SIZE 40960
const size_t tflite_learn_1077716_45_arena_size = 40960;
```

For the first test, set **both values** to a deliberately large temporary value:

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1077716_45_ARENA_SIZE 1000000
const size_t tflite_learn_1077716_45_arena_size = 1000000;
```

This gives TensorFlow Lite Micro enough memory to allocate the model tensors and print the actual usage.

This is only temporary.

> [!IMPORTANT]
> Both lines must use exactly the same number.

If the temporary value does not compile or the ESP32-S3 cannot start due to insufficient internal memory, use the largest value your board can support, then investigate other static allocations. The test configuration in this project was able to start with 894991 bytes configured for the arena.

---

## 🧪 Step 6: Add the debug line

Open this Edge Impulse SDK file:

```text
...\Arduino\libraries\<your-project-name>\src\
edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
```

On Windows, it may look like:

```text
C:\Users\<your-user-name>\Documents\Arduino\libraries\<your-project-name>\
src\edge-impulse-sdk\classifier\inferencing_engines\tflite_micro.h
```

Search for:

```cpp
TfLiteStatus allocate_status = interpreter->AllocateTensors(true);
if (allocate_status != kTfLiteOk) {
    ei_printf("AllocateTensors() failed");
    return EI_IMPULSE_TFLITE_ERROR;
}
```

Immediately after that `if` block, add:

```cpp
// Debug: real tensor arena usage for this model.
// Printed after TFLite Micro has allocated the model tensors.
ei_printf("DEBUG: Tflite arena used bytes: %d\n",
          (int)interpreter->arena_used_bytes());
```

The full section should become:

```cpp
TfLiteStatus allocate_status = interpreter->AllocateTensors(true);
if (allocate_status != kTfLiteOk) {
    ei_printf("AllocateTensors() failed");
    return EI_IMPULSE_TFLITE_ERROR;
}

// Debug: real tensor arena usage for this model.
// Printed after TFLite Micro has allocated the model tensors.
ei_printf("DEBUG: Tflite arena used bytes: %d\n",
          (int)interpreter->arena_used_bytes());
```

`interpreter` is a `tflite::MicroInterpreter` object. Its `arena_used_bytes()` method reports the tensor-arena use after tensor allocation.

`ei_printf()` is sent to the Arduino serial output, so the result appears in the Arduino IDE Serial Monitor.

> [!TIP]
> If you cannot find the correct location, use your editor's global search / “Find in Files” function and search for `AllocateTensors(true)`.

---

## 🚀 Step 7: Compile, upload, and read the key value

1. Open the `.ino` sketch in Arduino IDE.
2. Click **Verify** to compile.
3. Click **Upload** to flash the ESP32-S3.
4. Open **Tools > Serial Monitor**.
5. Set the baud rate to **115200**.
6. If no output appears, press the board's **Reset** button.
7. Look for this key line:

```text
DEBUG: Tflite arena used bytes: <M>
```

For example, the test model used in this project displayed:

```text
DEBUG: Tflite arena used bytes: 536059
```

That means:

```text
M = 536059 bytes
```

This is the measured tensor arena use for that model. Do not assume your own model has the same value.

---

## ➕ Step 8: Calculate the final arena size

Use this rule:

```text
final arena size = M + safety margin
```

A margin of 16 kB to 64 kB is usually a practical starting range:

| Margin | Bytes | Use case |
|---|---:|---|
| 16 kB | 16,384 bytes | Tighter memory budget after validation |
| 40 kB | 40,960 bytes | Moderate buffer |
| 64 kB | 65,536 bytes | Comfortable buffer |

For the example measurement `M = 536059`:

| Safety margin | Calculation | Final arena size |
|---|---:|---:|
| 16 kB | 536059 + 16384 | 552443 bytes |
| 40 kB | 536059 + 40960 | 577019 bytes |
| 64 kB | 536059 + 65536 | 601595 bytes |

You can also choose an easy-to-read binary-aligned number above the measured requirement.

For example:

```text
564 kB = 577536 bytes
```

That provides:

```text
577536 - 536059 = 41477 bytes
```

of safety margin.

For the example model, a readable final configuration could be:

```cpp
#define EI_CLASSIFIER_TFLITE_LEARN_1077716_45_ARENA_SIZE 577536
const size_t tflite_learn_1077716_45_arena_size = 577536;
```

Replace the temporary `1000000` values in the same `tflite_learn_XXXXXXX_ZZ.h` file.

Then compile and upload the sketch again.

A successful final test should still display:

```text
run_classifier() return code  : 0 (OK)
```

and:

```text
DEBUG: Tflite arena used bytes: <M>
```

> [!WARNING]
> Never set the final arena size below `M`. If you do, TensorFlow Lite Micro may fail to allocate tensors and `run_classifier()` can return `-3`.

---

## 🖥️ Example serial output

A successful measurement run can look like this:

```text
Arduino ESP32-S3 + Edge Impulse - Tensor Arena Size Finder
------------------------------------------------------------
=== ESP32-S3 Memory Test ===
Heap free (internal SRAM)    : 342912 bytes
Heap max contiguous block    : 286708 bytes
PSRAM free                   : 8386076 bytes
PSRAM total                  : 8388608 bytes

=== Current Edge Impulse Arena ===
Arena configured in tflite_learn_xxx.h : 894991 bytes

--- Allocating NN input buffer in PSRAM ---
Planned input buffer size     : 602112 bytes
Input buffer allocated in PSRAM: 602112 bytes OK

--- Running single inference (run_classifier) ---
DEBUG: Tflite arena used bytes: 536059

run_classifier() return code  : 0 (OK)
Heap used during inference    : 432 bytes
PSRAM used during inference   : 0 bytes
Heap free after inference     : 342216 bytes
PSRAM free after inference    : 7783948 bytes

DSP time: 71 ms, NN time: 428 ms
```

### 📖 How to read this output

| Output | Meaning |
|---|---|
| `PSRAM total : 8388608 bytes` | 8 MB PSRAM is available |
| `Input buffer allocated in PSRAM` | The large input feature buffer is stored in PSRAM |
| `DEBUG: Tflite arena used bytes: 536059` | This is the measured tensor arena requirement `M` |
| `run_classifier() return code : 0 (OK)` | Inference completed successfully |
| `DSP time: 71 ms, NN time: 428 ms` | Timing for preprocessing and neural-network classification |
| `Arena configured ... : 894991 bytes` | The configured arena is larger than required and can be reduced after adding a margin |

The input buffer was allocated in PSRAM. Therefore, its approximately 602 kB allocation does not consume the internal SRAM heap reported by:

```cpp
ESP.getFreeHeap()
```

The tensor arena is normally reserved statically in internal SRAM by the Edge Impulse / TensorFlow Lite Micro code.

---

## 🩺 Troubleshooting

### ⚡ PSRAM total is zero

Example:

```text
PSRAM total : 0 bytes
```

PSRAM is not detected or is not enabled.

Check:

1. Your board actually includes PSRAM.
2. **Tools > PSRAM** is enabled.
3. The selected PSRAM mode matches the board, often `OPI PSRAM` for ESP32-S3 boards.
4. The selected board definition is correct.

### ❌ `AllocateTensors() failed`

This usually means the tensor arena is too small.

1. Return to `tflite_learn_XXXXXXX_ZZ.h`.
2. Set both arena values back to a large temporary number, such as `1000000`.
3. Confirm that the macro and `const size_t` use the same number.
4. Compile and upload again.
5. Check for the `DEBUG: Tflite arena used bytes: ...` line.

### ❌ `run_classifier()` returns `-3`

This is commonly related to an insufficient tensor arena.

Check:

- The configured arena size is not below the measured value `M`.
- Both arena definitions use the same value.
- You selected TensorFlow Lite during Edge Impulse deployment.
- The `tflite_learn_*.h` file being edited belongs to the same library installed and compiled by Arduino IDE.

### 🔇 No `DEBUG: Tflite arena used bytes` line

Check the following:

1. You added the `ei_printf(...)` line to `tflite_micro.h`.
2. The new line is after the successful `AllocateTensors()` check.
3. You saved `tflite_micro.h`.
4. You edited the library copy used by Arduino IDE.
5. Serial Monitor is open at `115200` baud.
6. You recompiled and uploaded after changing the library file.

### 📁 `YOURMODEL_inferencing.h` not found

Replace:

```cpp
#include <YOURMODEL_inferencing.h>
```

with the exact name found in:

```text
...\Arduino\libraries\<your-project-name>\src\
```

The filename must end in:

```text
_inferencing.h
```

Example:

```cpp
#include <Dice_v4_inferencing.h>
```

Case matters on some operating systems.

### 🔗 Undefined arena symbol

Make sure this line:

```cpp
extern const size_t tflite_learn_XXXXXXX_ZZ_arena_size;
```

matches the exact symbol inside `tflite_learn_*.h`.

Copy it character for character.

Example:

```cpp
extern const size_t tflite_learn_1077716_45_arena_size;
```

### 💽 Sketch is too large for the partition

This is a flash-partition problem, not a tensor-arena problem.

Choose a partition scheme with a larger APP partition, or create a custom `partitions.csv` file.

Remember:

- The **APP partition** stores the compiled program.
- The **tensor arena** uses RAM at runtime.
- Increasing the APP partition does not increase internal SRAM or PSRAM.

---

## 📝 Keep notes when you update the model

The changes in this guide are made inside the exported Edge Impulse Arduino library.

If you export a new version of the Edge Impulse library, your modifications can be overwritten.

For each model version, keep a note of:

- Edge Impulse project and export date.
- Model architecture and input size.
- Measured value `M`.
- Final tensor-arena value.
- Safety margin used.
- ESP32-S3 board name.
- Flash size.
- PSRAM mode.
- Arduino ESP32 core version.
- Partition scheme.
- Measured DSP and classification timing.

This makes it easier to reproduce results and diagnose changes after a future model export.

---

## 📚 References

- [Edge Impulse - Transfer learning for images](https://docs.edgeimpulse.com/studio/projects/learning-blocks/blocks/transfer-learning-images)
- [Edge Impulse - Image classification tutorial](https://docs.edgeimpulse.com/tutorials/end-to-end/image-classification)
- [Edge Impulse - EON Compiler](https://docs.edgeimpulse.com/studio/projects/deployment/eon-compiler)
- [Edge Impulse - Run Arduino library with Arduino IDE 2.x](https://docs.edgeimpulse.com/hardware/deployments/run-arduino-2-0)
- [Espressif Arduino-ESP32 - Partition tables](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html)

---

## 📄 License

Add the license you want to apply to this repository, for example an MIT License.
