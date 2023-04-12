#ifndef CRYPTOADAPTER_H
#define CRYPTOADAPTER_H
#include "Crypto.h"
#include <QString>

class CryptoAdapter
{
public:
    CryptoAdapter();

    int Init(unsigned char * key_data, int key_data_len);
    char * Encrypt(char *plaintext, int *len);
    char * Decrypt(char *ciphertext, int *len);

    int Init(const QString& key);
    QByteArray Encrypt(const QString& input);
    QString Decrypt(const QByteArray& input);

    QString LicenseEncrypt(const QString& input);
private:
    CCrypto m_crypto;
};

#endif // CRYPTOADAPTER_H
