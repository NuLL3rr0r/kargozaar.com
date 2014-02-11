#include <stdexcept>

#if defined ( _WIN32 )
#include <windows.h>
//#include <cryptopp/dll.h>     // msvc-shared only
#endif  // defined ( _WIN32 )

#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <b64/decode.h>
#include <b64/encode.h>
#include "crypto.hpp"
#include "log.hpp"


#define     UNKNOWN_ERROR           "CoreLib::Crypto unknown error!"


using namespace std;
using namespace CryptoPP;
using namespace CoreLib;


struct Crypto::Impl
{
    // Use this nice HEX/ASCII converter and your editor's replace dialog,
    // to create your own Key and IV.
    // http://www.dolcevie.com/js/converter.html

    static constexpr unsigned char KEY[] = {
        0x4e, 0x40, 0x21, 0x6b, 0x21, 0x30, 0x0, 0x2d, 0x40, 0x6b, 0x23, 0x48, 0x2f, 0x2e, 0x3f, 0x71
    };

    static constexpr unsigned char IV[] = {
        0x53, 0x23, 0x33, 0x54, 0x34, 0x5f, 0x2b, 0x52, 0x33, 0x40, 0x48, 0x3f, 0x2e, 0x71, 0x0, 0x2f
    };
};

bool Crypto::Encrypt(const char *plainText, std::string &out_encodedText)
{
    string err;
    return Encrypt(plainText, out_encodedText, err);
}

bool Crypto::Encrypt(const std::string &plainText, std::string &out_encodedText)
{
    string err;
    return Encrypt(plainText.c_str(), out_encodedText, err);
}

bool Crypto::Encrypt(const char *plainText, std::string &out_encodedText,
                     std::string &out_error)
{
    try {
        string cipher;

        CBC_Mode<AES>::Encryption enc;
        enc.SetKeyWithIV(Impl::KEY, sizeof(Impl::KEY), Impl::IV, sizeof(Impl::IV));

        cipher.clear();
        StringSource(plainText, true,
                     new StreamTransformationFilter(enc, new StringSink(cipher)));

        out_encodedText.clear();
        StringSource(cipher, true, new HexEncoder(new StringSink(out_encodedText)));

        return true;
    }

    catch (const CryptoPP::Exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (const std::exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (...) {
        LOG_ERROR(UNKNOWN_ERROR);
        out_error.assign(UNKNOWN_ERROR);
    }

    return false;
}

bool Crypto::Encrypt(const std::string &plainText, std::string &out_encodedText,
                     std::string &out_error)
{
    return Encrypt(plainText.c_str(), out_encodedText, out_error);
}

bool Crypto::Decrypt(const char *cipherText, std::string &out_recoveredText)
{
    string err;
    return Decrypt(cipherText, out_recoveredText, err);
}

bool Crypto::Decrypt(const std::string &cipherText, std::string &out_recoveredText)
{
    string err;
    return Decrypt(cipherText.c_str(), out_recoveredText, err);
}

bool Crypto::Decrypt(const char *cipherText, std::string &out_recoveredText,
                     std::string &out_error)
{
    try {
        string cipher;

        CBC_Mode<AES>::Decryption dec;
        dec.SetKeyWithIV(Impl::KEY, sizeof(Impl::KEY), Impl::IV, sizeof(Impl::IV));

        cipher.clear();
        StringSource(cipherText, true, new HexDecoder(new StringSink(cipher)));

        out_recoveredText.clear();
        StringSource s(cipher, true,
                       new StreamTransformationFilter(dec, new StringSink(out_recoveredText)));

        return true;
    }

    catch (const CryptoPP::Exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (const std::exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (...) {
        LOG_ERROR(UNKNOWN_ERROR);
        out_error.assign(UNKNOWN_ERROR);
    }

    return false;
}

bool Crypto::Decrypt(const std::string &cipherText, std::string &out_recoveredText,
                     std::string &out_error)
{
    return Decrypt(cipherText.c_str(), out_recoveredText, out_error);
}


bool Crypto::GenerateHash(const char *text, std::string &out_digest)
{
    string err;
    return GenerateHash(text, out_digest, err);
}

bool Crypto::GenerateHash(const std::string &text, std::string &out_digest)
{
    string err;
    return GenerateHash(text.c_str(), out_digest, err);
}

bool Crypto::GenerateHash(const char *text, std::string &out_digest,
                          std::string &out_error)
{
    try {
        SHA1 hash;

        out_digest.clear();
        StringSource(text, true,
                     new HashFilter(hash, new HexEncoder(new StringSink(out_digest))));

        return true;
    }

    catch (const CryptoPP::Exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (const std::exception &ex) {
        LOG_ERROR(ex.what());
        out_error.assign(ex.what());
    }

    catch (...) {
        LOG_ERROR(UNKNOWN_ERROR);
        out_error.assign(UNKNOWN_ERROR);
    }

    return false;
}

bool Crypto::GenerateHash(const std::string &text, std::string &out_digest,
                          std::string &out_error)
{
    return GenerateHash(text.c_str(), out_digest, out_error);
}

int Crypto::Base64Decode(char value)
{
    base64::decoder decoder;
    return decoder.decode(value);
}

int Crypto::Base64Decode(const char *code, const int length, char *out_plainText)
{
    base64::decoder decoder;
    return decoder.decode(code, length, out_plainText);
}

void Crypto::Base64Decode(std::istream &inputStream, std::ostream &outputStream)
{
    base64::decoder decoder;
    decoder.decode(inputStream, outputStream);
}

int Crypto::Base64Encode(char value)
{
    base64::encoder encoder;
    return encoder.encode(value);
}

int Crypto::Base64Encode(const char *code, const int length, char *out_plainText)
{
    base64::encoder encoder;
    return encoder.encode(code, length, out_plainText);
}

int Crypto::Base64EncodeBlockEnd(char *out_plainText)
{
    base64::encoder encoder;
    return encoder.encode_end(out_plainText);
}

void Crypto::Base64Encode(std::istream &inputStream, std::ostream &outputStream)
{
    base64::encoder encoder;
    encoder.encode(inputStream, outputStream);
}

