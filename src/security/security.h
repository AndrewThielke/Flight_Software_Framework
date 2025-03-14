#ifndef SECURITY_H
#define SECURITY_H

#include <openssl/evp.h>
#include <string>

class Security {
public:
    void initialize(const std::string& telemetryData);
    void monitor(const std::string& telemetryData);
    
    // Encrypts and decrypts telemetry data dynamically
    std::string encryptTelemetry(const std::string& telemetryData);
    std::string decryptTelemetry();

private:
    unsigned char stored_key[32];     // AES-256 key
    unsigned char stored_iv[12];      // AES-GCM IV
    unsigned char stored_ciphertext[256]; // Ciphertext storage
    unsigned char stored_tag[16];     // Authentication tag
    int stored_ciphertext_len;        // Ciphertext length
};

#endif
