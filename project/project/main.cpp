//----------------------------------------------------------------------------------------
#include <afxwin.h>      //MFC core and standard components
#include "resource.h"    //main symbols
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------------------
//Globals


CButton * CheckboxContainer;

string label;
CString Clabel;

string tmp;

string state;
CString Cstate;


CEdit * pChoroba;
CEdit * pLeczenie;
CEdit * pError;

CEdit * admin_id;
CEdit * admin_pw;
CString C_Id;
CString C_Pw;
bool admin = false;

CEdit * pom;

CButton * D_choroby;
CButton * D_objawy;
CButton * D_metody;

CButton * Login;

int nChorob;
int DataBase_size=10;
int nObjawow=0;
//-----------------------------------------------------------------------------------------

//*****************************************************
struct Objaw
{
	string nazwa; //nazwa objawu
	int numbObj; // numer objawu w tablicy
	void setNumbObj(int temp);
	void setNazwa(string temp);

	void show();
};

Objaw DataBase[10];

void Objaw::setNazwa(string temp)
{
	nazwa = temp;
}


void Objaw::setNumbObj(int temp)
{
	numbObj = temp;
}

void loadObjawy() // laduje objawy do chorob
{
	int n = 0;  // ile objawow
	int i = 0; // ktory to wiersz data
	int k = 0; // ktore objawy z kolei
	fstream plik;
	string linia = "", temp = "";
	plik.open("objawy.txt", ios::in);
	if (plik.good() == true)
	{
		while (getline(plik, linia))
		{
			for (unsigned int j = 0; j < linia.length(); j++)
			{
				if (linia[j] == '.')
					n++;
			}
			for (unsigned int j = 0; j <linia.length(); j++)
			{
				if (linia[j] != '.')
				{
					temp = temp + linia[j];
				}
				if (linia[j + 1] == '.')
				{
					DataBase[i].setNazwa(temp);
					DataBase[i].setNumbObj(i+1);
					
					k++;
					temp = "";
				}
			}
			i++;
			k = 0;
		}
	}
	nObjawow=n;
};



struct Choroba
{
	string nazwa; //nazwa choroby
	string *objawy; // tablica objawow
	int numbObj; // wielkosc tablicy objawow
	string leczenie; // metody leczenia
	void setNumbObj(int temp);
	void setNazwa(string temp);
	void setObjawy(string temp, int i);
	void setLeczenie(string temp);
	void show();
};
Choroba *choroby; 
void Choroba::setNazwa(string temp)
{
	nazwa = temp;
}
void Choroba::setLeczenie(string temp)
{
	leczenie = temp;
}
void Choroba::setObjawy(string temp, int i)
{
	objawy[i] = temp;
}
void Choroba::setNumbObj(int temp)
{
	numbObj = temp;
}
//rozbij sobie show w razie co


//*****************************************************

struct User
{
	int id;
	int uNumbObj;
	string *uObjawy;
	int diagnoza;
	User(int temp);
	void setUObj(string obj1, int n); // checkboxy
	void setUNumbObj(int temp); // ilosc objawow
	void setDiagnoza(int diagnoza); // diagnoza
};

User *user = new User(0);


User::User(int temp)
{
	id = temp;
}
void User::setUNumbObj(int temp)
{
	uNumbObj = temp;
	uObjawy = new string[uNumbObj];
}
//potrzebna modyfikacja do gui
void User::setUObj(string temp, int n)
{
		uObjawy[n] = temp;

}
// funkcje programowe: checki, ³adowania danych etc.
void User::setDiagnoza(int temp)
{
	diagnoza=temp;
}
//*****************************************************

int compatibility(string nazwa) //liczy linie w pliku sprawdza czy pliki wsadowe sa poprawne
{
	fstream plik;
	int i = 0;
	string temp = "";
	plik.open(nazwa, ios::in);
	if (plik.good() == true)
	{
		while (getline(plik, temp))
			i++;
	}
	else
		pError->SetWindowText(L"Problem w sprawdzaniu poprawnosci plikow\n");
	plik.close();
	return i;
};
void loadChor(Choroba *T) // laduje nazwy chorob
{
	int i = 0;
	fstream plik;
	string linia = "";
	plik.open("choroby.txt", ios::in);
	if (plik.good() == true)
	{
		while (getline(plik, linia))
		{
			T[i].setNazwa(linia);
			i++;
		}
	}
	else
		pError->SetWindowText(L"Problem z wczytaniem chorob\n");
	plik.close();
}

void loadObj(Choroba *T) // laduje objawy do chorob
{
	int n = 0;  // ile objawow
	int i = 0; // ktory to wiersz data
	int k = 0; // ktore objawy z kolei
	int tmp;
	fstream plik;
	string linia = "", temp = "";
	plik.open("objawy_chorob.txt", ios::in);
	if (plik.good() == true)
	{
		while (getline(plik, linia))
		{
			for (unsigned int j = 0; j < linia.length(); j++)
			{
				if (linia[j] == '.')
					n++;
			}
			T[i].setNumbObj(n);
			T[i].objawy = new string[n];
			for (unsigned int j = 0; j <linia.length(); j++)
			{
				if (linia[j] != '.')
				{
					temp = temp + linia[j];
				}
				if (linia[j + 1] == '.')
				{
					tmp=atoi(temp.c_str());
					
					T[i].setObjawy(DataBase[tmp-1].nazwa, k);
					k++;
					temp = "";
				}
			}
			i++;
			k = 0;
			n = 0;
		}
	}
	
};

void loadLecz(Choroba *T) //laduje metody leczenia
{
	int i = 0;
	fstream plik;
	string linia = "";
	plik.open("leczenie.txt", ios::in);
	if (plik.good() == true)
	{
		while (getline(plik, linia))
		{
			T[i].setLeczenie(linia);
			i++;
		}
	}
	else
		pError->SetWindowText(L"Problem z wczytaniem chorob\n");
	plik.close();
};

void myIllness(User *user) // wyszukiwarka
{
	double number = 0; // ilosc dopasowanych objawow
	double *tab = new double[6];
	string actual = ""; // na jakie objawy paczymy
	for (int i = 0; i < nChorob; i++) // lecimy po ilosci chorob w bazie
	{
		number = 0;
		for (int j = 0; j < user->uNumbObj; j++)
		{
			actual = user->uObjawy[j];
			for (int k = 0; k < choroby[i].numbObj; k++)
			{
				if (choroby[i].objawy[k] == actual)
					number++;
				
			}
		}
		double temp = number / choroby[i].numbObj;
			//user->uNumbObj;
		if (temp >=0.8)
			tab[i]=temp;
		else 
			tab[i]=0;
	}
	double max=0.0;
	int point=-1;
	for (int i = 0; i < nChorob; i++) 
	{
		if (tab[i]>max)
		{
			point=i;
			max=tab[i];
		}
	}
	if (point!=-1)
	{
		user->setDiagnoza(point);
		state="Byc moze to ";
		state += choroby[point].nazwa;
		Cstate.Format(_T("%S"), state.c_str());
		pChoroba->SetWindowTextW(Cstate);
	}
	else 
	{
		state="Zadna choroba nie pasuje do zadanego kryterium";
		Cstate.Format(_T("%S"), state.c_str());
		pChoroba->SetWindowTextW(Cstate);
		user->setDiagnoza(-1);
	}
}

//*****************************************************

//*************************************
struct Admin
{
	string name;
	Admin(string temp);
	void write(string napis); // admin wpisuje do kazdego pliku po jednej linii ten sam numer linii identyfikuje chorobe, objawy,leczenie
	void read(string napis);
};
Admin::Admin(string temp)
{
	name = temp;
}
// napis to nazwa pliku np. "choroby.txt"
void Admin::write(string napis)
{
	fstream plik;
	CString Cpom;
	char flag = 'y';

	plik.open(napis, ios::out | ios::app);
	pom->GetWindowTextW(Cpom);
	CT2CA pszConvertedAnsiString (Cpom);
	string temp (pszConvertedAnsiString);
	temp += ".\n";

	if (plik.good() == true)
	{
		if (napis == "choroby.txt")
		{
			plik << temp;
			pError->SetWindowText(L"dodano chorobe");
		}

		if (napis == "objawy.txt")
		{
			plik << temp;
			pError->SetWindowText(L"dodano objaw");
		}
		if (napis == "leczenie.txt")
		{
			plik << temp;
			pError->SetWindowText(L"dodano metode leczenia");
		}
	}
	else
		pError->SetWindowText(L"Write problem in ");
	plik.close();
}
void Admin::read(string napis)
{
	fstream plik;
	string linia = "";
	plik.open(napis, ios::in);
	if (plik.good() == true)
	{
		if (napis == "choroby.txt")
		{
			pError->SetWindowText(L"Wpisane choroby:\n");
		}
		if (napis == "objawy.txt")
		{
			pError->SetWindowText(L"Wpisane objawy:\n");
		}
		if (napis == "leczenie.txt")
		{
			pError->SetWindowText(L"Wpisane metody leczenia:\n");
		}
		/*while (getline(plik, linia))
		{
			cout << linia << endl;
		}*/
	}
	else
		pError->SetWindowText(L"Read problem in ");
	plik.close();
}
Admin adm("admin");

//-----------------------------------------------------------------------------------------

    
class PROJECT_FORM : public CDialog
{
    public:
    PROJECT_FORM(CWnd* pParent = NULL): CDialog(PROJECT_FORM::IDD, pParent)
    {    }
    // Dialog Data, name of dialog form
    enum{IDD = IDD_INTERFACE1};
    protected:
    virtual void DoDataExchange(CDataExchange* pDX) { CDialog::DoDataExchange(pDX); }
    //Called right after constructor. Initialize things here.
    virtual BOOL OnInitDialog() 
    { 
            CDialog::OnInitDialog();
			
			//------------------------------------------------------
			pChoroba = (CEdit *) GetDlgItem(CE_Choroba);
			pLeczenie = (CEdit *) GetDlgItem(CE_Leczenie);
			pError = (CEdit *) GetDlgItem(CE_Error);

			pom = (CEdit *) GetDlgItem(IDC_POM);

			admin_id = (CEdit *) GetDlgItem(IDC_ID);
			admin_pw = (CEdit *) GetDlgItem(IDC_PW);

			D_choroby = (CButton *) GetDlgItem(IDC_DCH);
			D_objawy = (CButton *) GetDlgItem(IDC_DOBJ);
			D_metody = (CButton *) GetDlgItem(IDC_DMET);

			Login = (CButton *) GetDlgItem(IDC_LOGIN);

			//------------------------------------------------------
			Admin *admin = new Admin("IMIE");
			
			//------------------------------------------------------
			nChorob = compatibility("choroby.txt");
			
			if (compatibility("choroby.txt") == compatibility("objawy_chorob.txt") && compatibility("objawy_chorob.txt") == compatibility("leczenie.txt"))			
			{
				loadObjawy();
				choroby= new Choroba[nChorob];
				loadChor(choroby);
				loadLecz(choroby);
				loadObj(choroby);
				CheckboxContainer = new CButton[DataBase_size];
				
				
				CButton *button = new CButton;
				for (int i=0; i<nObjawow; i++)
				{
					
					CheckboxContainer[i].Create(_T("&objaw"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(10, 25+30*i, 150, 40+30*i), this, 0x11);
					label = DataBase[i].nazwa;
					Clabel.Format(_T("%S"), label.c_str());
					CheckboxContainer[i].SetWindowTextW(Clabel);
				}

				state="Wczytano ";
				sprintf((char*)tmp.c_str(), "%d", nObjawow);
				state += tmp.c_str();
				state +=" objawow, oraz ";
				sprintf((char*)tmp.c_str(), "%d", nChorob);
				state += tmp.c_str();
				state +=" chorob z pliku";
				Cstate.Format(_T("%S"), state.c_str());
				pError->SetWindowTextW(Cstate);
				

			}
			else
				pError->SetWindowText(L"Blad danych wejsciowych\n");
            
            return true; 

    }
public:
DECLARE_MESSAGE_MAP()
afx_msg void OnBnClickedStart();
afx_msg void OnBnClickedAdmin();
afx_msg void OnBnClickedLogin();
afx_msg void OnBnClickedDobj();
afx_msg void OnBnClickedDch();
afx_msg void OnBnClickedDmet();
};
//-----------------------------------------------------------------------------------------
class Project : public CWinApp
{
	public:
	Project() 
	{  

	}
public:
virtual BOOL InitInstance()
    {
   CWinApp::InitInstance();
   SetRegistryKey(_T("Illness"));
   PROJECT_FORM dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();
   return FALSE;
} //close function
};
//-----------------------------------------------------------------------------------------
//Need a Message Map Macro for both CDialog and CWinApp
BEGIN_MESSAGE_MAP(PROJECT_FORM, CDialog)

	ON_BN_CLICKED(CB_START, &PROJECT_FORM::OnBnClickedStart)
	ON_BN_CLICKED(IDC_ADMIN, &PROJECT_FORM::OnBnClickedAdmin)
	ON_BN_CLICKED(IDC_LOGIN, &PROJECT_FORM::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_DOBJ, &PROJECT_FORM::OnBnClickedDobj)
	ON_BN_CLICKED(IDC_DCH, &PROJECT_FORM::OnBnClickedDch)
	ON_BN_CLICKED(IDC_DMET, &PROJECT_FORM::OnBnClickedDmet)
END_MESSAGE_MAP()
//-----------------------------------------------------------------------------------------
Project theApp;  //Starts the Application

void PROJECT_FORM::OnBnClickedStart()
{
	int n=0; // ilosc zadeklarowanych objawow
	for (int i=0; i<nObjawow; i++)
	{
		if (CheckboxContainer[i].GetCheck()==BST_CHECKED)
		{
			n++;
		}
	}
	user->setUNumbObj(n);
	state="Uzytkownik zadeklarowal ";
	sprintf((char*)tmp.c_str(), "%d", user->uNumbObj);
	state += tmp.c_str();
	state +="  posiadanych objawow";
	Cstate.Format(_T("%S"), state.c_str());
	pError->SetWindowTextW(Cstate);
	
	n=0;
	for (int i=0; i<nObjawow; i++)
	{
		if (CheckboxContainer[i].GetCheck()==BST_CHECKED)
		{
			user->setUObj(DataBase[i].nazwa,n);
			n++;
		}
	}
	myIllness(user); //wyszukiwarka
	if (user->diagnoza!=-1)

		state=choroby[user->diagnoza].leczenie;
	else 
		state="Zadna choroba nie pasuje do zadanego kryterium";

	Cstate.Format(_T("%S"), state.c_str());
	pLeczenie->SetWindowTextW(Cstate);
	
	
}


void PROJECT_FORM::OnBnClickedAdmin()
{

	admin_pw->EnableWindow(true);
	admin_id->EnableWindow(true);

	Login->EnableWindow(true);
	
	

}




void PROJECT_FORM::OnBnClickedLogin()
{
	admin_id->GetWindowTextW(C_Id);
	admin_pw->GetWindowTextW(C_Pw);

	if (C_Id == "admin" && C_Pw == "admin" )
	{
		pError->SetWindowText(L"zalogowno admina");
		admin = true;
	}
	else 
		pError->SetWindowText(L"Niepoprawna nazwa uzytkownika i/lub haslo");
	if (admin == true)
	{
		D_choroby->EnableWindow(true);
		D_metody->EnableWindow(true);
		D_objawy->EnableWindow(true);
		pom ->EnableWindow(true);
	}
}


void PROJECT_FORM::OnBnClickedDobj()
{
	adm.write("objawy.txt");	
}


void PROJECT_FORM::OnBnClickedDch()
{
	adm.write("choroby.txt");	
}


void PROJECT_FORM::OnBnClickedDmet()
{
	adm.write("leczenie.txt");	
}
