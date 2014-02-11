#ifndef CRYPTO_HPP
#define CRYPTO_HPP


#include <string>

namespace CoreLib {
    class Crypto;
}

class CoreLib::Crypto
{
private:
    struct Impl;

public:
    static bool Encrypt(const char *plainText, std::string &out_encodedText);
    static bool Encrypt(const std::string &plainText, std::string &out_encodedText);
    static bool Encrypt(const char *plainText, std::string &out_encodedText,
                        std::string &out_error);
    static bool Encrypt(const std::string &plainText, std::string &out_encodedText,
                        std::string &out_error);
    static bool Decrypt(const char *cipherText, std::string &out_recoveredText);
    static bool Decrypt(const std::string &cipherText, std::string &out_recoveredText);
    static bool Decrypt(const char *cipherText, std::string &out_recoveredText,
                        std::string &out_error);
    static bool Decrypt(const std::string &cipherText, std::string &out_recoveredText,
                        std::string &out_error);
    static bool GenerateHash(const char *text, std::string &out_digest);
    static bool GenerateHash(const std::string &text, std::string &out_digest);
    static bool GenerateHash(const char *text, std::string &out_digest,
                             std::string &out_error);
    static bool GenerateHash(const std::string &text, std::string &out_digest,
                             std::string &out_error);
    static int Base64Decode(char value);
    static int Base64Decode(const char *code, const int length, char *out_plainText);
    static void Base64Decode(std::istream &inputStream, std::ostream &outputStream);
    static int Base64Encode(char value);
    static int Base64Encode(const char *code, const int length, char *out_plainText);
    static int Base64EncodeBlockEnd(char *out_plainText);
    static void Base64Encode(std::istream &inputStream, std::ostream &outputStream);
};


#endif /* CRYPTO_HPP */

