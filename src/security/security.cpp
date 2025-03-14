#include "security.h"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <cstring>

/**
 *  AES-256-GCM ENCRYPTION 
 *  Encrypts actual telemetry data and returns encrypted hex string.
 */
std::string Security::encryptTelemetry(const std::string& telemetryData) {
    unsigned char key[32];   // 256-bit key
    unsigned char iv[12];    // 96-bit IV for GCM
    unsigned char ciphertext[256]; // Buffer for encrypted data
    unsigned char tag[16];  

    // Generate random key & IV
    if (RAND_bytes(key, sizeof(key)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
        std::cerr << "[ERROR] Failed to generate cryptographic key/IV." << std::endl;
        return "";
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "[ERROR] Cipher context initialization failed." << std::endl;
        return "";
    }

    // Initialize AES-256-GCM encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv) != 1) {
        std::cerr << "[ERROR] Encryption initialization failed." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    int len;
    int ciphertext_len = 0;

    // Encrypt actual telemetry data
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, 
                          (unsigned char*)telemetryData.c_str(), telemetryData.length()) != 1) {
        std::cerr << "[ERROR] Data encryption failed." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len += len;

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        std::cerr << "[ERROR] Encryption finalization failed." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len += len;

    // Retrieve authentication tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) {
        std::cerr << "[ERROR] Failed to retrieve authentication tag." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    // Store key, IV, ciphertext, and tag for decryption
    memcpy(stored_key, key, sizeof(key));
    memcpy(stored_iv, iv, sizeof(iv));
    memcpy(stored_ciphertext, ciphertext, ciphertext_len);
    memcpy(stored_tag, tag, sizeof(tag));
    stored_ciphertext_len = ciphertext_len;

    // Convert ciphertext to a hex string for readability
    std::ostringstream hexStream;
    for (int i = 0; i < ciphertext_len; i++) {
        hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)ciphertext[i];
    }

    return hexStream.str();
}

/**
 *  AES-256-GCM DECRYPTION 
 *  Decrypts and returns plaintext string.
 */
std::string Security::decryptTelemetry() {
    unsigned char decrypted_text[256]; // Buffer for decrypted data
    int len;
    int plaintext_len = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "[ERROR] Cipher context initialization failed (Decryption)." << std::endl;
        return "";
    }

    // Initialize AES-256-GCM decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, stored_key, stored_iv) != 1) {
        std::cerr << "[ERROR] Decryption initialization failed." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Set authentication tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, stored_tag) != 1) {
        std::cerr << "[ERROR] Failed to set authentication tag for decryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Decrypt the ciphertext
    if (EVP_DecryptUpdate(ctx, decrypted_text, &len, stored_ciphertext, stored_ciphertext_len) != 1) {
        std::cerr << "[ERROR] Decryption failed." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;

    // Finalize the decryption
    if (EVP_DecryptFinal_ex(ctx, decrypted_text + len, &len) != 1) {
        std::cerr << "[ERROR] Decryption finalization failed. Data may be tampered with." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    // Null-terminate the decrypted text
    decrypted_text[plaintext_len] = '\0';

    return std::string((char*)decrypted_text);
}



/**
 *  INTRUSION DETECTION SYSTEM
 *  Encrypts and decrypts telemetry for verification.
 */
void Security::monitor(const std::string& telemetryData) {
    std::cout << "\n====================================" << std::endl;
    std::cout << "     Monitoring For Intrusions...     " << std::endl;
    std::cout << "====================================" << std::endl;

    // Encrypt telemetry and get encrypted output
    std::string encryptedData = encryptTelemetry(telemetryData);
    std::cout << "Encrypted Telemetry Data (AES-256-GCM): " << encryptedData << "\n";
    
    // Decrypt telemetry and print result
    //std::string decryptedData = decryptTelemetry();
    //std::cout << "Decrypted Telemetry Data" << std::endl;
    //std::cout << decryptedData << "\n" << std::endl;
}

/**
 *  SYSTEM INITIALIZATION
 *  Runs encryption & decryption test.
 */
void Security::initialize(const std::string& telemetryData) {
    std::cout << "\n====================================" << std::endl;
    std::cout << "     Security System Initialized     " << std::endl;
    std::cout << "====================================" << std::endl;

    // PLACEHOLDER: Utilize the initialize function for loading cryptographic keys from a config file or vault...
    // For now, I am just running a test of the encryption/decryption system.
}
