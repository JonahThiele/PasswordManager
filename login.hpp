#ifndef LOGIN_HPP
#define LOGIN_HPP

#include <wx/wx.h>
#include <openssl/sha.h>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include "passwords.hpp"

class Login : public wxFrame
{
    public:
        Login(const wxString& title);

    private:
        wxPanel *panel;
        wxBoxSizer *vbox;
        wxButton *loginBtn;
        wxButton *forgotPassword;
        wxButton *createAccount;
        wxTextCtrl *masterPassword;
        wxStaticText *createAccountLabel;
        wxStaticText *masterPasswordLabel;
        wxStaticText *forgotPasswordLabel;
        std::string masterHash;
        void checkLogin(wxCommandEvent &event);
        void createAccountF(wxCommandEvent &event);
        std::string CreateHash(std::string);
        std::fstream masterHashFile;   
};

const int LOGIN_BTN_ID = 101;
const int LOGIN_FORGOT_ID = 102;
const int LOGIN_MASTERKEY = 103;
const int ACCOUNT_SETUP_PASS = 104;
const int ACCOUNT_BACKUP_QUESTION = 105;
const int ACCOUNT_BACKUP_ANSWER = 106;
const int ACCOUNT_SUBMIT_BTN = 107;
const int CURRENT_VIEWED_PASS = 108;
const int CURRENT_VIEWED_USER = 109;
const int LOGIN_CREATE_ACCNT = 110;
const int PASSWORD_STARTING_IDS = 120;


class CreateAccount : public wxFrame
{
    public:
        CreateAccount(const wxString& title, Login*);
        //I don't know if the onclose has to be private so I am making it public
        void OnClose(wxCloseEvent &event);
        
    private:
        Login *login;
        wxPanel *panel;
        wxBoxSizer *vbox;
        wxTextCtrl *masterPass;
        wxStaticText *masterPassLabel;
        wxTextCtrl *backupQuestion;
        wxStaticText *backupQuestionLabel;
        wxTextCtrl *backupAnswer;
        wxStaticText *backupAnswerLabel;
        wxButton *submit;
        void CreateHash(wxCommandEvent &event);
        std::string generateHash(std::string);

};

const int NEW_MENU_BTN = 100;
const int REM_MENU_BTN = 99;
const int FND_MENU_BTN = 98;

class PasswordHolder : public wxFrame
{
    public:
        PasswordHolder(const wxString& title, Login*, std::string masterKey);
        
        Login *login;
        wxPanel *panel;
        wxBoxSizer *vbox;
        wxMenuBar *menu;
        wxMenu *file;
        std::vector<wxButton*> accounts;
        //for the menu options
        void OnNew(wxCommandEvent &event);
        void OnRem(wxCommandEvent &event);
        void OnFnd(wxCommandEvent &event);
        
        void OnClose(wxCloseEvent &event);

        void AddLogin(std::string, std::string, std::string);
        void DialogRem(std::string);
        void EditLogin(std::string, std::string, std::string);

        std::vector<std::string> getLabels(){
            return bank.GetLabels();
        }
        
        //stores the new login and values so that we can access them later  in the event function
        void setTempLogin(std::string name, std::string user, std::string password){
            tempLogin[0] = name;
            tempLogin[1] = user;
            tempLogin[2] = password;
        }
        
        void openProfile(int);

    private:
        PasswordsBank bank;
        std::array<std::string, 3> tempLogin;
        std::vector<std::string> labels;
        void openPasswordProfile(wxCommandEvent &event);

};

//used for holding the exact username and password for each domain or label
class PasswordGroup : public wxFrame
{
    public:
        std::string label;
        PasswordHolder *parent;
        wxPanel *panel;
        wxBoxSizer *vbox;
        wxTextCtrl *username;
        wxTextCtrl *password;
        wxButton *save;
        
        void EditAndSave(wxCommandEvent &event);
        void OnClose(wxCloseEvent &event);

        PasswordGroup(const wxString& title, std::string, std::string, std::string, PasswordHolder *);
        

};

#endif

//some utility classes

class KeyCreationDialog : public wxDialog
{
    public:
        PasswordHolder *Wparent;
        wxTextCtrl *Login;
        wxTextCtrl *Username;
        wxTextCtrl *Password;
        KeyCreationDialog(const wxString& title, PasswordHolder *parent);
        void OnDialogNew(wxCommandEvent &event);
};

class KeyRemovalDialog : public wxDialog
{
    public:
        PasswordHolder *Wparent;
        wxListBox *Logins;
        KeyRemovalDialog(const wxString& title, PasswordHolder *parent);
        void OnDialogRem(wxCommandEvent &event);
};

class KeyFindDialog : public wxDialog
{
    public:
        PasswordHolder *Wparent;
        wxComboBox *comboBox;
        KeyFindDialog(const wxString& title, PasswordHolder *parent);
        void OnDialogFnd(wxCommandEvent &event);
};
