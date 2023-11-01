#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "login.hpp"

Login::Login(const wxString& title)
    : wxFrame((wxFrame *) NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 250))
{
    //load in the master hash if one exist i.e they have an account
    //I have no idea what the file type for this would be
    
    masterHashFile.open("hash.hs");

    masterHashFile >> masterHash;

    masterHashFile.close();
    
    //set up gui
    panel = new wxPanel(this, wxID_ANY);
    
    vbox = new wxBoxSizer(wxVERTICAL);
    
    loginBtn = new wxButton(panel, LOGIN_BTN_ID, wxT("LOGIN"));
    Connect(loginBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(Login::checkLogin));

    //forgotPassword = new wxButton(panel, LOGIN_FORGOT_ID, wxT("Reset Login"));
    //forgotPasswordLabel = new wxStaticText(panel, wxID_ANY, wxT("Forgot your account Login"));

    createAccount = new wxButton(panel, LOGIN_CREATE_ACCNT, wxT("Create new Account"));
    Connect(createAccount->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Login::createAccountF));
    createAccountLabel = new wxStaticText(panel, wxID_ANY, wxT("Register"));

    masterPassword = new wxTextCtrl(panel, LOGIN_MASTERKEY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    masterPasswordLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter the master Password"));
    
    vbox->Add(masterPasswordLabel, 0.25, wxCENTER, 5);
    vbox->Add(masterPassword,0.5, wxCENTER, 5);
    vbox->AddSpacer(10);
    vbox->Add(loginBtn, 0.25, wxCENTER, 2.5);
    vbox->AddSpacer(25);
    //vbox->Add(forgotPasswordLabel, 0.25, wxCENTER, 2.5);
    //vbox->Add(forgotPassword, 0.1, 5);
    vbox->Add(createAccountLabel, 0.25, wxCENTER, 2.5);
    vbox->Add(createAccount, 0.25, 5);

    panel->SetSizer(vbox);

}

void Login::createAccountF(wxCommandEvent &event){
    //load the frame so someone can set up an account
    CreateAccount *cAccount = new CreateAccount(wxT("create a new account"), this);
    cAccount->Show(true);
    this->Iconize();
}

std::string Login::CreateHash(std::string login){
    //add the salt
    std::string saltedLogin = login + "Run Forest Run";
    unsigned char *data = reinterpret_cast<unsigned char*>(const_cast<char*>(saltedLogin.c_str())); 
    size_t length = sizeof(data);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data, length, hash);

    std::string generatedHash(reinterpret_cast<char const*>(hash));
    return generatedHash;

}

void Login::checkLogin(wxCommandEvent &WXUNUSED(event)){
    //convert the wxString into a normal std string
    std::string login = masterPassword->GetValue().ToStdString();

    //generate the hash from the login plaintext
    std::string hash = CreateHash(login);
    if(hash == masterHash){
       PasswordHolder *Pholder = new PasswordHolder(wxT("Password Bank"), this, hash);
       Pholder->Show(true);
       this->Iconize();
    }else{
    //create a modal window that tells the user the password is incorrect
    //clear the text box as well
        wxMessageDialog *incorPassMsg = new wxMessageDialog(NULL, wxT("Password is incorrect"), wxT("Error"), wxOK | wxICON_ERROR);
        incorPassMsg->ShowModal();
        masterPassword->Clear();
    }
}

void CreateAccount::OnClose(wxCloseEvent &event){
    login->Maximize(false);
    this->Destroy();
}

CreateAccount::CreateAccount(const wxString& title, Login *login): wxFrame((wxFrame *) NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 250), wxDEFAULT_FRAME_STYLE){
    //set up the master password and also the extra security questions
    this->login = login;
    panel =  new wxPanel(this, wxID_ANY);
    vbox = new wxBoxSizer(wxVERTICAL);
    
    // I will figure out the back end for password change or retrieve in a bit after I complete the normal functionality
    masterPass = new wxTextCtrl(this->panel, ACCOUNT_SETUP_PASS, wxT(""));
    masterPassLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter a master password"));
    backupQuestion = new wxTextCtrl(this->panel, ACCOUNT_BACKUP_QUESTION, wxT(""));
    backupQuestionLabel = new wxStaticText(panel, wxID_ANY, wxT("Security Backup Question"));
    backupAnswer = new wxTextCtrl(this->panel, ACCOUNT_BACKUP_ANSWER, wxT(""));
    backupAnswerLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter an answer to the security question above"));
    submit = new wxButton(panel, ACCOUNT_SUBMIT_BTN, wxT("SUBMIT"));
    Connect(submit->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(CreateAccount::CreateHash)); 
    
    vbox->Add(masterPassLabel, 0.5, wxCENTER, 10);
    vbox->Add(masterPass,0.5, wxCENTER, 10);
    vbox->AddSpacer(30);
    vbox->Add(backupQuestionLabel, 0.5, wxCENTER, 10);
    vbox->Add(backupQuestion, 0.5, wxCENTER, 10);
    vbox->AddSpacer(10);
    vbox->Add(backupAnswerLabel, 0.5, wxCENTER,10);
    vbox->Add(backupAnswer, 0.5, wxCENTER, 10);
    vbox->AddSpacer(10);
    vbox->Add(submit, 0.5, wxCENTER, 10);
    
    
    panel->SetSizer(vbox);
}

std::string CreateAccount::generateHash(std::string strIn){
    std::string saltedStr = strIn + "Run Forest Run";
    unsigned char *data = reinterpret_cast<unsigned char*>(const_cast<char*>(saltedStr.c_str()));
    size_t length = sizeof(data);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data, length, hash);
    std::string generatedHash(reinterpret_cast<char const*>(hash));
      
    return generatedHash;
}

 void CreateAccount::CreateHash(wxCommandEvent &event){
    std::string login = masterPass->GetValue().ToStdString();
    std::string backupQuest = backupQuestion->GetValue().ToStdString();
    std::string ansQuest = backupAnswer->GetValue().ToStdString();
    
    //reset the masterhash file every single time the account is reset
    std::ofstream masterHashFile("hash.hs", std::ios::out | std::ios::trunc);
    masterHashFile << generateHash(login) << '\n';

    //have not written code to handle the forgetting passwords yet
    masterHashFile << generateHash(backupQuest) << '\n';
    masterHashFile << generateHash(ansQuest) << '\n';
    masterHashFile.close();

    //close the window
    this->Close(true);
    //maximize the parent window then
 

}

PasswordHolder::PasswordHolder(const wxString& title, Login* login, std::string masterKey): wxFrame((wxFrame *) NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 250), wxDEFAULT_FRAME_STYLE), bank("passwords.hs", masterKey){
    //we need to figure out to enumerate the list of passwords and generate a button for each in a semi elegant way
    
    this->login = login;
    
    //bank("hash2.hs", masterKey);
    labels = bank.GetLabels();
    
    menu = new wxMenuBar(); 
    file = new wxMenu();
    file->Append(NEW_MENU_BTN, wxT("&New"));
    Connect(NEW_MENU_BTN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PasswordHolder::OnNew));
    
    file->Append(REM_MENU_BTN, wxT("&Remove"));
    Connect(REM_MENU_BTN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PasswordHolder::OnRem));

    
    file->Append(FND_MENU_BTN, wxT("&Find"));
    Connect(FND_MENU_BTN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PasswordHolder::OnFnd));
    
    file->AppendSeparator();
    menu->Append(file, wxT("&Entries"));    
    SetMenuBar(menu);

    panel = new wxPanel(this, wxID_ANY);
    vbox = new wxBoxSizer(wxVERTICAL);
    
    //create a button for each label and 
    int idOffset = 0;
    //this might be the cause of a bunch of memory leaks
    for(std::string label :labels){
        wxString labelStr(label.c_str(), wxConvUTF8);
        wxButton *btn = new wxButton(panel,PASSWORD_STARTING_IDS + idOffset, labelStr);
        vbox->Add(btn, 0.5, wxCENTER, 10);
        
        //connect to correct info each time
        Connect(btn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PasswordHolder::openPasswordProfile));
        accounts.push_back(btn);
        idOffset++;
    }

    //a menu path to create more entries

    panel->SetSizer(vbox);
    
}

void PasswordHolder::OnClose(wxCloseEvent &event){
    //handle the close by callling the deconstructor method on the password bank...
    //call this before closing down
    bank.SavePasswords();
    login->Maximize(false);
    this->Destroy();
}

void PasswordHolder::AddLogin(std::string name, std::string username, std::string password){
    wxString label(name);
    wxButton *Nbtn = new wxButton(panel, PASSWORD_STARTING_IDS + accounts.size(),label);
    Connect(Nbtn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PasswordHolder::openPasswordProfile));
    accounts.push_back(Nbtn);
    //calls bank so why is it empty
    bank.CreatePassword(name, username, password);
    vbox->Add(Nbtn, 0.5, wxCENTER, 10);
    panel->Layout();
}

void PasswordHolder::DialogRem(std::string remStr){
    //should be similar to the find function

      for(int i = 0; i < accounts.size(); i++){
          std::string name = accounts[i]->GetLabel().ToStdString();
          if(name == remStr){
              bank.RemovePassword(name);
              accounts[i]->Destroy();
              panel->Layout();
              //accounts.erase(accounts.begin() + i);
              break;
          }
      }
}

void PasswordHolder::OnNew(wxCommandEvent &WXUNUSED(event)){
    KeyCreationDialog *createDialog = new KeyCreationDialog(wxT("Add a new login"), this);
    createDialog->ShowModal();
}

void PasswordHolder::OnRem(wxCommandEvent &WXUNUSED(event)){
    KeyRemovalDialog *removeDialog = new KeyRemovalDialog(wxT("Remove an existing entry"),this);
    removeDialog->ShowModal();
}

void PasswordHolder::OnFnd(wxCommandEvent &WXUNUSED(event)){
    KeyFindDialog *findDialog = new KeyFindDialog(wxT("Search for an existing entry"), this);
    findDialog->ShowModal();
}

void PasswordHolder::EditLogin(std::string label, std::string user, std::string password){
    //shouldn't have to reset the other values
    bank.EditPassword(label, user, password);
}
void KeyCreationDialog::OnDialogNew(wxCommandEvent& WXUNUSED(event)){
    //set up the creation of new widgets
    //somehow get the strings
    Wparent->AddLogin(Login->GetValue().ToStdString(), Username->GetValue().ToStdString(), Password->GetValue().ToStdString());
    this->Destroy();

}
void KeyRemovalDialog::OnDialogRem(wxCommandEvent& WXUNUSED(event)){
    Wparent->DialogRem(Logins->GetStringSelection().ToStdString());
    this->Destroy();

}
void KeyFindDialog::OnDialogFnd(wxCommandEvent& WXUNUSED(event)){
    //find an entry in the password list
    std::string selection = comboBox->GetValue().ToStdString();
    for(auto acc : Wparent->accounts){
        if(acc->GetLabel().ToStdString() == selection){
            // open up the account
            Wparent->openProfile(acc->GetId());
            break;            
        }
    }
    this->Destroy();

}
void PasswordHolder::openProfile(int id){
    std::string label = getLabels()[id - PASSWORD_STARTING_IDS];
    std::pair<std::string, std::string> accountInfo = bank.GetLogin(label);
    //this needs to get converted to a starting wxstring
    wxString wxlabelStr(label.c_str(), wxConvUTF8);
    PasswordGroup *PGroup = new PasswordGroup(wxlabelStr,label, accountInfo.first, accountInfo.second, this);
    PGroup->Show(true);
    this->Iconize();

}

void PasswordHolder::openPasswordProfile(wxCommandEvent &event){
    int id = event.GetId();
    openProfile(id);
    this->Destroy();
}

void PasswordGroup::EditAndSave(wxCommandEvent &event){
    parent->EditLogin(label, username->GetValue().ToStdString(), password->GetValue().ToStdString());
    this->Destroy();
}

void PasswordGroup::OnClose(wxCloseEvent &event){
    parent->Maximize(false);
    this->Destroy();
}

PasswordGroup::PasswordGroup(const wxString& title, std::string label, std::string strUsername, std::string strPassword, PasswordHolder *parent): wxFrame((wxFrame *) NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 250), wxDEFAULT_FRAME_STYLE){
       
      this->parent = parent;
      this->label = label;
      
      wxString PasswordStr(strPassword);
      wxString UsernameStr(strUsername);
      panel =  new wxPanel(this, wxID_ANY);
      vbox = new wxBoxSizer(wxVERTICAL);
      wxStaticText *passwordLabel = new wxStaticText(panel, wxID_ANY, wxT("Password:"));
      password = new wxTextCtrl(panel, CURRENT_VIEWED_PASS, PasswordStr);

      wxStaticText *usernameLabel = new wxStaticText(panel, wxID_ANY, wxT("Username:"));
      username = new wxTextCtrl(panel, CURRENT_VIEWED_USER, UsernameStr);
      save = new wxButton(panel, wxID_ANY, wxT("save and exit"));
      Connect(save->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PasswordGroup::EditAndSave));
      vbox->Add(usernameLabel, 0.5, wxCENTER, 1);
      vbox->Add(username, 0.5, wxCENTER, 1);
      vbox->Add(passwordLabel, 0.5, wxCENTER, 1);
      vbox->Add(password, 0.5, wxCENTER, 1);
      vbox->Add(save, 0.5, wxCENTER, 1);

      panel->SetSizer(vbox);
}

//add the exit feature this weekend

//set up the custom widgets

KeyCreationDialog::KeyCreationDialog(const wxString &title, PasswordHolder *parent): wxDialog(parent, -1, title, wxDefaultPosition, wxSize(250,250)){
    
    Wparent = parent; 

    wxPanel *panel = new wxPanel(this, -1);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    
    Login  = new wxTextCtrl(panel, wxID_ANY, wxT(""));
    wxStaticText *LoginLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter a label for the login"));
    
    Username = new wxTextCtrl(panel, wxID_ANY, wxT(""));
    wxStaticText *UsernameLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter a username for the login"));

    Password = new wxTextCtrl(panel, wxID_ANY, wxT(""));
    wxStaticText *PasswordLabel = new wxStaticText(panel, wxID_ANY, wxT("Enter a password for the login"));

    wxButton *submit = new wxButton(panel, wxID_ANY, wxT("Add account"));
    Connect(submit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(KeyCreationDialog::OnDialogNew));

    
    vbox->Add(LoginLabel, 0.5, wxCENTER, 10);
    vbox->Add(Login, 0.5, wxCENTER, 10);
    vbox->Add(UsernameLabel, 0.5, wxCENTER, 10);
    vbox->Add(Username,0.5, wxCENTER, 10);
    vbox->Add(PasswordLabel, 0.5, wxCENTER, 10);
    vbox->Add(Password, 0.5, wxCENTER, 10);
    vbox->Add(submit, 0.5, wxCENTER, 10);

    panel->SetSizer(vbox);

}

KeyRemovalDialog::KeyRemovalDialog(const wxString &title, PasswordHolder *parent): wxDialog(parent, -1, title, wxDefaultPosition, wxSize(250,250)){
    //how can I return the value of the deleted logins to the 
    Wparent = parent;
    
    wxPanel *panel = new wxPanel(this, -1);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText *deleteLabel = new wxStaticText(panel, wxID_ANY, wxT("choose which entry to delete"));

    Logins = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(20, 20));

    wxButton *DelBtn = new wxButton(panel, wxID_ANY, wxT("Delete"));
    Connect(DelBtn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(KeyRemovalDialog::OnDialogRem));
    
    std::vector<std::string> labels = Wparent->getLabels();

    for( std::string login : labels){ 
        wxString wxLoginStr(login.c_str(), wxConvUTF8);
        Logins->Append(wxLoginStr);
    
    }
    
    vbox->Add(deleteLabel,0.5,wxCENTER,1);
    vbox->Add(Logins,8,wxCENTER | wxEXPAND, 8);
    vbox->Add(DelBtn,0.5,wxCENTER, 1);

    panel->SetSizer(vbox);
    ShowModal();
    Destroy();
}

KeyFindDialog::KeyFindDialog(const wxString &title, PasswordHolder *parent): wxDialog(parent ,-1, title, wxDefaultPosition, wxSize(250,250)){
    
    Wparent = parent;

    wxPanel *panel = new wxPanel(this, -1);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText *searchLabel = new wxStaticText(panel, wxID_ANY, wxT("search for a login with a regex"));

    comboBox = new wxComboBox(panel, wxID_ANY, wxT("search"));
    //add all the entries to the combo box by converison
    wxArrayString array;
    for(std::string str : parent->getLabels()){
        wxString wStr(str);
        array.Add(wStr);
    }
    comboBox->Set(array);
    wxButton *searchBtn = new wxButton(panel, wxID_ANY, wxT("Search"));
    Connect(searchBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(KeyFindDialog::OnDialogFnd));


    vbox->Add(searchLabel,0.5,wxCENTER,1);
    vbox->Add(comboBox,0.5, wxCENTER, 1);
    vbox->Add(searchBtn, 0.5, wxCENTER,1);

    panel->SetSizer(vbox);
    ShowModal();
    Destroy();
}

