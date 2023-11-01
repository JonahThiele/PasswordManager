#include "passwords.hpp"  

PasswordsBank::PasswordsBank(std::string file, std::string masterKey){
    
    this->masterKey = masterKey;
    this->filename = file;
    //key should be a 256 bit hash of the master password enforce this at a higher level
    unsigned char *key = convertToPrimitive(masterKey);
    
    //randomly create the nonce/iv should be 128 bits according to the docs
    
    unsigned char nonce[128];
    int rc = RAND_bytes(nonce, sizeof(nonce));
    unsigned long err = ERR_get_error();

    if(rc != 1) {
       /* handle error by returning a message to the user through a dialog box*/
    }
     
    //open the file and load the contents into the map
    bankFile.open(file);
    
    std::string login;
    while(std::getline(bankFile,login)){
        //data should be TITLE:USERNAME:PASSWORD NONCE when decrypted 
        //split the login by the spaces
        std::string field;
        std::string strNonce;
        
        
        std::cin >> field;
        std::cin >> strNonce;
        
        //text should not be more than 128 bits 
        unsigned char decryptedText[128];
        unsigned char *nonce = convertToPrimitive(strNonce);
        unsigned char *cipherText = convertToPrimitive(field);
            
        int decryptedTextLen = decrypt(cipherText, sizeof(cipherText), key, nonce, decryptedText); 
            
        //add a null terminator and convert to c++ string
        decryptedText[decryptedTextLen] = '\0';
        std::string s( reinterpret_cast< char const* >(decryptedText));

        //split the strings into the seperate fields after decrypting
        std::string fieldList[3];
        std::stringstream fields(s);
        std::string decryptedField;
        int fieldIndex = 0;
        while(std::getline(fields, decryptedField, ':')){

            fieldList[fieldIndex++] = decryptedField;
        }

        passwordDictionary.insert({fieldList[0], {fieldList[1], fieldList[2]}}); 
    }
   bankFile.close(); 

}

void PasswordsBank::CreatePassword(std::string label, std::string username, std::string password){
    std::pair <std::string, std::string> newLogin;
    newLogin.first = username;
    newLogin.second = password;
   passwordDictionary.insert({label, newLogin});

}

void PasswordsBank::RemovePassword(std::string label){

    if(passwordDictionary.find(label) != passwordDictionary.end()){
        //remove the login
        passwordDictionary.erase(label);

    }else{
        //set a popup with an error code that the login was not found for some reason
    }

}

void PasswordsBank::EditPassword(std::string label, std::string user, std::string password){
    std::pair <std::string, std::string> newLogin;
    newLogin.first = user;
    newLogin.second = password;
    passwordDictionary[label] = newLogin;


}

void PasswordsBank::SavePasswords(){
     
    bankFile.open(filename);
    for(const auto &login : passwordDictionary){
         unsigned char nonce[128];
         int rc = RAND_bytes(nonce, sizeof(nonce));
         unsigned long err = ERR_get_error(); 

        if(rc != 1) {
           /* handle error by returning a message to the user through a dialog box*/
        }
        
        //add all fields together
        std::string plainText = login.first + ":" + login.second.first + ":" + login.second.first;
        
        unsigned char ciphertext[128];
        
        encrypt(convertToPrimitive(plainText), plainText.size(), convertToPrimitive(masterKey), nonce, ciphertext);
        
        //convert c-style ciphertext into c++ strings for storing in the file
        std::string fileNonce = convertFromPrimitive(nonce);
        std::string fileCipherText = convertFromPrimitive(ciphertext); 
        
        bankFile << fileCipherText << " " << fileNonce << '\n';
    }

    bankFile.close();

      
}

std::string PasswordsBank::convertFromPrimitive(unsigned char* primitive){
    std::string s( reinterpret_cast< char const* >(primitive));
    return s;

}

unsigned char * PasswordsBank::convertToPrimitive(std::string inStr){
    return reinterpret_cast<unsigned char*>(const_cast<char*>(inStr.c_str()));
}


//These two functions are straight from the wiki for this kind of things
int PasswordsBank::encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int PasswordsBank::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

