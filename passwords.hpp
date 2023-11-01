#ifndef PASSWORDSBANK_H
#define PASSWORDSBANK_H

//include any libraries that are need for hashing and encryption
//This needs to handle file io as well

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>


class PasswordsBank{
    
    public:
        PasswordsBank(std::string file, std::string masterKey);

        void CreatePassword(std::string, std::string, std::string);

        void RemovePassword(std::string);

        void EditPassword(std::string, std::string, std::string);

        //maybe replace this with a tuple later on to be more clean
        std::pair<std::string, std::string> GetLogin(std::string label){
            return passwordDictionary.find(label)->second;
        }
        //deconstruct the bank to take care of shutdown

        void SavePasswords();

        std::vector<std::string> GetLabels(){
            std::vector<std::string> labels;
            for(std::map<std::string,std::pair<std::string, std::string>>::iterator it = passwordDictionary.begin(); it != passwordDictionary.end(); ++it) {
                 labels.push_back(it->first);
            }
            return labels;
        
        }

    private:
        //create a grouping of the user name and password for a given label
        std::map<std::string,std::pair<std::string, std::string>> passwordDictionary;
        std::fstream bankFile;
        std::string filename;
        std::string masterKey;
        unsigned char * convertToPrimitive(std::string);
        std::string convertFromPrimitive(unsigned char*);

        //ecncryption functions from the openssl example
        int encrypt(unsigned char*, int, unsigned char *, unsigned char *, unsigned char *);
        int decrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
        void handleErrors(void){
            ERR_print_errors_fp(stderr);
            abort();
        }

};

#endif 
