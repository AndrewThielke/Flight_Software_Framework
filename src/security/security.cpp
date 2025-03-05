#include "security.h"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>

void Security::initialize() {
    std::cout << "\nSecurity System Initialized" << std::endl;
}

void Security::monitor() {
    std::cout << "\nMonitoring System for Intrusions..." << std::endl;
}

// Encrypt telemetry data using the AES-256-GCM approach (New OpenSSL API)
void Security::encryptTelemetry() {
    unsigned char key[32];   // AES-256 key (this is 256-bit)
    unsigned char iv[12];    // IV (GCM mode, this requires 12 bytes)
    unsigned char plaintext[] = "Telemetry Data"; // Example data (This is temporary)
    unsigned char ciphertext[128];
    unsigned char tag[16];   // The authentication tag

    RAND_bytes(key, sizeof(key));  // Generate a random encryption key
    RAND_bytes(iv, sizeof(iv));    // Generate a random IV

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Error initializing cipher context." << std::endl;
        return;
    }

    // Initialize encryption operation with AES-256-GCM
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv) != 1) {
        std::cerr << "Error initializing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    int len;
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, sizeof(plaintext)) != 1) {
        std::cerr << "Error encrypting data." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        std::cerr << "Error finalizing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Get authentication tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) {
        std::cerr << "Error getting authentication tag." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    EVP_CIPHER_CTX_free(ctx);

    std::cout << "🛡️ Encrypted Telemetry Data: ";
    for (int i = 0; i < 16; i++) {
        printf("%02x", ciphertext[i]);
    }
    std::cout << " (AES-256-GCM Encrypted)" << std::endl;
}
