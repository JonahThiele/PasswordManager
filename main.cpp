#include "main.hpp"
#include "login.hpp"

IMPLEMENT_APP(PasswordManager)

bool PasswordManager::OnInit()
{
    Login *login = new Login(wxT("Password Manager Login"));
    login->Show(true);

    return true;
}
