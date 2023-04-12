/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <openssl/evp.h>
#include <QString>

#define AES_BLOCK_SIZE 128

class CCrypto
{
public:
    CCrypto(void);
    ~CCrypto(void);

    int Init(unsigned char * key_data, int key_data_len);
    char * Encrypt(char *plaintext, int *len);
    char * Decrypt(char *ciphertext, int *len);   

    int Init(const QString& key);
    QByteArray Encrypt(const QString& input);
    QString Decrypt(const QByteArray& input);

private:
    int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt);
    unsigned char * aes_encrypt(unsigned char *plaintext, int *len);
    unsigned char * aes_decrypt(unsigned char *ciphertext, int *len);

    EVP_CIPHER_CTX* m_ctxEncode = nullptr;
    EVP_CIPHER_CTX* m_ctxDecode = nullptr;

    bool m_bInit;
};
