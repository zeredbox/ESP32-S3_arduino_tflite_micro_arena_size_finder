/**
 ****************************************************************************************************
 * SKETCH TITLE : ESP32-S3 + Edge Impulse Tensor Arena Size Finder
 * FILE NAME    : ESP32-S3_arduino_tflite_micro_arena_size_finder.ino
 * HARDWARE     : ESP32-S3 board with PSRAM
 *                Tested on Goouuu ESP32-S3 N16R8 (16MB Flash / 8MB OPI PSRAM)
 * DATE         : August 2026
 * AUTHOR       : Zeredbox
 * WEBSITE      : https://github.com/zeredbox/ESP32-S3_arduino_tflite_micro_arena_size_finder
 * LICENSE      : MIT license
 *
 * DESCRIPTION:
 * TensorFlow Lite Micro tensor-arena sizing and memory diagnostic tool
 * for Edge Impulse models running on an ESP32-S3 with PSRAM.
 *
 * IMPORTANT:
 * Read the project README carefully before using this sketch:
 * https://github.com/zeredbox/ESP32-S3_arduino_tflite_micro_arena_size_finder
 *
 * FRAMEWORK    : ESP32 Arduino Core v3.3.x
 *
 * IDE BOARD CONFIGURATION (Arduino IDE 2.3.x):
 *
 * Physical Connection: Connect the USB-C cable to the OTG (USB) port
 *
 * Board                    : "ESP32S3 Dev Module"
 * USB CDC On Boot          : "Enabled"
 * USB Firmware MSC On Boot : "Disabled"
 * Flash Size               : "16MB (128Mb)"
 * PSRAM                    : "OPI PSRAM"   <-- CRITICAL for N16R8 boards
 * Partition Scheme         : "16M Flash (3MB APP / 9.9MB FATFS)"
 * Core Debug Level         : "None"
 * USB DFU On Boot          : "Disabled"
 *
 ****************************************************************************************************
 */

// ============================================================================
// USER CONFIGURATION
//
// Before using this sketch, carefully read:
// https://github.com/zeredbox/ESP32-S3_arduino_tflite_micro_arena_size_finder
//
// The modified tflite_micro.h supplied with this project must first replace
// the original file in your Edge Impulse Arduino library.
//
// Only the line marked "CHANGE THIS" needs to be modified in this sketch.
// ============================================================================

// Temporary Tensor Arena size used only by this diagnostic sketch.
// IMPORTANT: this must be defined BEFORE the Edge Impulse inference header.
//
// 1000000 bytes is a practical starting value for most models.
// If run_classifier() fails with an allocation-related error (for example -3),
// increase this diagnostic value and run the test again.
// Example: 1500000 or 2000000 bytes.
//
// This value is only used during the diagnostic test. It is NOT the final
// Tensor Arena size that should be used in your Edge Impulse model header.
#define EI_TFLITE_ARENA_DIAGNOSTIC_SIZE 1000000

// Replace YOUR_LIBRARY_inferencing.h with the inference header generated
// by your own Edge Impulse Arduino library.
// Example: #include <Dice_inferencing.h>
#include <YOUR_LIBRARY_inferencing.h>  // <-- CHANGE THIS

// ============================================================================
// INTERNAL - DO NOT MODIFY
// ============================================================================

#include <Arduino.h>
#include "esp_heap_caps.h"

// These values are filled automatically by the modified tflite_micro.h.
size_t ei_tflite_arena_used_bytes = 0;
size_t ei_tflite_model_arena_configured_bytes = 0;

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------

void waitForSerial();
void printMemoryInfo(uint32_t &heapFree,
                     uint32_t &psramFree,
                     uint32_t &psramTotal);
void printDiagnosticArenaInfo();
float *allocateInputBuffer(size_t featuresSize);
EI_IMPULSE_ERROR runInference(float *features);
void printArenaSizingGuidance();

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    waitForSerial();

    Serial.println();
    Serial.println("SETUP STARTING");
    Serial.println();
    Serial.println("=== ESP32-S3 + Edge Impulse Tensor Arena Size Finder ===");
    Serial.println();

    uint32_t heapFree = 0;
    uint32_t psramFree = 0;
    uint32_t psramTotal = 0;

    printMemoryInfo(heapFree, psramFree, psramTotal);

    if (psramTotal == 0) {
        Serial.println();
        Serial.println("ERROR: PSRAM not detected.");
        Serial.println("Check Tools > PSRAM and select the correct PSRAM mode.");

        while (true) {
            delay(1000);
        }
    }

    printDiagnosticArenaInfo();

    const size_t featuresSize =
        EI_CLASSIFIER_NN_INPUT_FRAME_SIZE * sizeof(float);

    float *features = allocateInputBuffer(featuresSize);

    if (features == nullptr) {
        Serial.println();
        Serial.printf("ERROR: Unable to allocate %u bytes for the input buffer in PSRAM.\n",
                      (unsigned)featuresSize);

        while (true) {
            delay(1000);
        }
    }

    memset(features, 0, featuresSize);

    EI_IMPULSE_ERROR err = runInference(features);

    if (err == EI_IMPULSE_OK) {
        printArenaSizingGuidance();
    }
    else {
        Serial.println();
        Serial.println("TEST FAILED");
        Serial.println("Tensor Arena recommendations cannot be calculated.");
    }

    heap_caps_free(features);
}

// ----------------------------------------------------------------------------
// Serial initialization
// ----------------------------------------------------------------------------

void waitForSerial() {
    delay(2000);

    unsigned long start = millis();

    while (!Serial && (millis() - start < 2000)) {
        delay(10);
    }

    delay(500);
}

// ----------------------------------------------------------------------------
// Memory check
// ----------------------------------------------------------------------------

void printMemoryInfo(uint32_t &heapFree,
                     uint32_t &psramFree,
                     uint32_t &psramTotal) {
    Serial.println("[1] MEMORY CHECK");

    heapFree = ESP.getFreeHeap();
    psramFree = ESP.getFreePsram();
    psramTotal = ESP.getPsramSize();

    Serial.printf("Internal heap free : %u bytes\n", heapFree);
    Serial.printf("PSRAM free         : %u bytes\n", psramFree);
    Serial.printf("PSRAM total        : %u bytes\n", psramTotal);
    Serial.printf("PSRAM              : %s\n",
                  psramTotal > 0 ? "OK" : "NOT DETECTED");
}

// ----------------------------------------------------------------------------
// Diagnostic Tensor Arena
// ----------------------------------------------------------------------------

void printDiagnosticArenaInfo() {
    Serial.println();
    Serial.println("[2] DIAGNOSTIC CONFIGURATION");
    Serial.printf("Diagnostic arena   : %u bytes\n",
                  (unsigned)EI_TFLITE_ARENA_DIAGNOSTIC_SIZE);
    Serial.println();
    Serial.println("The diagnostic arena temporarily overrides the Edge Impulse");
    Serial.println("arena size for this test only.");
}

// ----------------------------------------------------------------------------
// Input buffer
// ----------------------------------------------------------------------------

float *allocateInputBuffer(size_t featuresSize) {
    float *features = (float *)heap_caps_aligned_alloc(
        16,
        featuresSize,
        MALLOC_CAP_SPIRAM
    );

    return features;
}

// ----------------------------------------------------------------------------
// Inference
// ----------------------------------------------------------------------------

EI_IMPULSE_ERROR runInference(float *features) {
    Serial.println();
    Serial.println("[3] INFERENCE TEST");
    
    const size_t featuresSize =
        EI_CLASSIFIER_NN_INPUT_FRAME_SIZE * sizeof(float);

    Serial.printf("Input buffer       : %u bytes in PSRAM\n",
                  (unsigned)featuresSize);

    ei::signal_t signal;

    numpy::signal_from_buffer(
        features,
        EI_CLASSIFIER_NN_INPUT_FRAME_SIZE,
        &signal
    );

    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR err =
        run_classifier(&signal, &result, false);

    if (err == EI_IMPULSE_OK) {
        Serial.println("run_classifier()   : OK");
    }
    else {
        Serial.printf("run_classifier()   : ERROR (%d)\n", err);
    }

    if (ei_tflite_arena_used_bytes > 0) {
        Serial.printf("Tensor Arena used  : %u bytes\n",
                      (unsigned)ei_tflite_arena_used_bytes);
    }

    return err;
}

// ----------------------------------------------------------------------------
// Tensor Arena recommendation
// ----------------------------------------------------------------------------

void printArenaSizingGuidance() {
    const uint32_t measuredArenaBytes =
        (uint32_t)ei_tflite_arena_used_bytes;

    const uint32_t modelConfiguredArenaBytes =
        (uint32_t)ei_tflite_model_arena_configured_bytes;

    Serial.println();
    Serial.println("[4] ARENA RECOMMENDATION");

    if (measuredArenaBytes == 0) {
        Serial.println("ERROR: Tensor Arena usage was not captured.");
        Serial.println("Make sure the modified tflite_micro.h supplied with this");
        Serial.println("project is installed in your Edge Impulse Arduino library.");
        return;
    }

    Serial.printf("Edge Impulse arena : %u bytes\n",
                  modelConfiguredArenaBytes);
    Serial.printf("Measured usage (M) : %u bytes\n",
                  measuredArenaBytes);

    if (modelConfiguredArenaBytes > 0 &&
        modelConfiguredArenaBytes < measuredArenaBytes) {
        Serial.println();
        Serial.println("WARNING: The arena generated by Edge Impulse is smaller");
        Serial.println("than the measured arena usage.");
    }

    const uint32_t smallArenaSize =
        measuredArenaBytes + (16 * 1024);

    const uint32_t mediumArenaSize =
        measuredArenaBytes + (32 * 1024);

    const uint32_t largeArenaSize =
        measuredArenaBytes + (64 * 1024);

    Serial.println();
    Serial.printf("SMALL  : %u bytes  (+16 KiB)\n",
                  smallArenaSize);
    Serial.printf("MEDIUM : %u bytes  (+32 KiB)  <-- RECOMMENDED\n",
                  mediumArenaSize);
    Serial.printf("LARGE  : %u bytes  (+64 KiB)\n",
                  largeArenaSize);

    Serial.println();
    Serial.printf("Recommended value: %u bytes\n",
                  mediumArenaSize);

Serial.println();
Serial.println("Update BOTH of these lines in your Edge Impulse model header file:");
Serial.println("  src/tflite-model/tflite_learn_XXXXXXX_ZZ.h");
Serial.println();

Serial.printf(
    "  #define EI_CLASSIFIER_TFLITE_LEARN_XXXXXXX_ZZ_ARENA_SIZE %u\n",
    mediumArenaSize
);

Serial.printf(
    "  const size_t tflite_learn_XXXXXXX_ZZ_arena_size = %u;\n",
    mediumArenaSize
);

    Serial.println();
    Serial.println("TEST COMPLETE");
}

// ----------------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------------

void loop() {
    delay(5000);
}
