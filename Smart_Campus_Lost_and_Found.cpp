// Lab Sheet 10: Smart Campus Lost & Found Management System
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

const int MAX_RECORDS=1000;
struct ItemRecord {
    int id;
    string studentName, rollNumber, itemName, category, location, date, description, type;
};
ItemRecord records[MAX_RECORDS];
int recordCount=0, nextId=1;
string loggedInName, loggedInRoll;

string lowerText(string s) {
    for (char &c:s) c=(char)tolower((unsigned char)c);
    return s;
}

void loadRecords() {
    ifstream f("lost_found.txt"); string line;
    while(getline(f,line) && recordCount<MAX_RECORDS) {
        stringstream ss(line); string id;
        ItemRecord r;
        getline(ss,id,'|'); getline(ss,r.studentName,'|'); getline(ss,r.rollNumber,'|');
        getline(ss,r.itemName,'|'); getline(ss,r.category,'|'); getline(ss,r.location,'|');
        getline(ss,r.date,'|'); getline(ss,r.description,'|'); getline(ss,r.type,'|');
        if(!id.empty()) { r.id=stoi(id); records[recordCount++]=r; nextId=max(nextId,r.id+1); }
    }
}
void saveRecords() {
    ofstream f("lost_found.txt");
    for(int i=0;i<recordCount;i++) {
        ItemRecord r=records[i];
        f<<r.id<<'|'<<r.studentName<<'|'<<r.rollNumber<<'|'<<r.itemName<<'|'
         <<r.category<<'|'<<r.location<<'|'<<r.date<<'|'<<r.description<<'|'<<r.type<<'\n';
    }
}
bool studentLogin() {
    cout<<"\n--- STUDENT LOGIN ---\nStudent name: ";
    getline(cin>>ws,loggedInName);
    cout<<"Roll number: "; getline(cin>>ws,loggedInRoll);
    if(loggedInName.empty()||loggedInRoll.empty()) { cout<<"Details cannot be empty.\n"; return false; }
    cout<<"Login successful. Welcome, "<<loggedInName<<"!\n"; return true;
}
void reportItem(string type) {
    if(recordCount>=MAX_RECORDS) { cout<<"Record storage is full.\n"; return; }
    ItemRecord r; r.id=nextId++; r.studentName=loggedInName; r.rollNumber=loggedInRoll; r.type=type;
    cout<<"\n--- REPORT "<<type<<" ITEM ---\n";
    cout<<"Item name: "; getline(cin>>ws,r.itemName);
    cout<<"Category (ID Card/Wallet/Keys/Books/Electronics/Other): "; getline(cin>>ws,r.category);
    cout<<"Location: "; getline(cin>>ws,r.location);
    cout<<"Date (YYYY-MM-DD): "; getline(cin>>ws,r.date);
    cout<<"Description: "; getline(cin>>ws,r.description);
    if(r.itemName.empty()||r.category.empty()||r.location.empty()||r.date.empty()) {
        cout<<"Required details cannot be empty.\n"; nextId--; return;
    }
    records[recordCount++]=r; saveRecords();
    cout<<"Report saved. Record ID: "<<r.id<<"\n";
    bool match=false;
    for(int i=0;i<recordCount-1;i++) {
        if(records[i].type!=type && lowerText(records[i].itemName)==lowerText(r.itemName)
           && lowerText(records[i].category)==lowerText(r.category)) {
            match=true;
            cout<<"\n*** POSSIBLE MATCH FOUND ***\nRecord ID: "<<records[i].id
                <<"\nType: "<<records[i].type<<"\nLocation: "<<records[i].location
                <<"\nReported by: "<<records[i].studentName
                <<"\nPlease verify ownership with the campus office.\n";
        }
    }
    if(!match) cout<<"No matching opposite-type record found yet.\n";
}
void searchItem() {
    string key; cout<<"Search item name, category or location: "; getline(cin>>ws,key);
    key=lowerText(key); bool found=false;
    for(int i=0;i<recordCount;i++) {
        ItemRecord r=records[i];
        if(lowerText(r.itemName).find(key)!=string::npos ||
           lowerText(r.category).find(key)!=string::npos ||
           lowerText(r.location).find(key)!=string::npos) {
            found=true; cout<<"ID "<<r.id<<" | "<<r.type<<" | "<<r.itemName
              <<" | "<<r.category<<" | "<<r.location<<" | "<<r.date<<"\n";
        }
    }
    if(!found) cout<<"No matching record found.\n";
}
void viewAllRecords() {
    ItemRecord sorted[MAX_RECORDS];
    for(int i=0;i<recordCount;i++) sorted[i]=records[i];
    // Bubble sort records alphabetically by item name.
    for(int i=0;i<recordCount-1;i++)
        for(int j=0;j<recordCount-i-1;j++)
            if(lowerText(sorted[j].itemName)>lowerText(sorted[j+1].itemName))
                swap(sorted[j],sorted[j+1]);
    if(recordCount==0) cout<<"No records available.\n";
    for(int i=0;i<recordCount;i++) {
        ItemRecord r=sorted[i];
        cout<<"\nID: "<<r.id<<"\nType: "<<r.type<<"\nItem: "<<r.itemName
            <<"\nCategory: "<<r.category<<"\nLocation: "<<r.location
            <<"\nDate: "<<r.date<<"\nDescription: "<<r.description
            <<"\nReported by: "<<r.studentName<<" ("<<r.rollNumber<<")\n";
    }
}
void autoMatch() {
    bool found=false; cout<<"\n--- POSSIBLE LOST & FOUND MATCHES ---\n";
    for(int i=0;i<recordCount;i++) for(int j=i+1;j<recordCount;j++) {
        if(records[i].type!=records[j].type &&
           lowerText(records[i].itemName)==lowerText(records[j].itemName) &&
           lowerText(records[i].category)==lowerText(records[j].category)) {
            found=true; cout<<records[i].itemName<<" | IDs "<<records[i].id<<" and "
                <<records[j].id<<" | Locations: "<<records[i].location<<" / "
                <<records[j].location<<"\n";
        }
    }
    if(!found) cout<<"No possible matches found.\n";
    cout<<"Matches are suggestions only; verify ownership manually.\n";
}
void deleteRecord() {
    int id; cout<<"Enter record ID to delete: "; cin>>id;
    for(int i=0;i<recordCount;i++) if(records[i].id==id && records[i].rollNumber==loggedInRoll) {
        for(int j=i;j<recordCount-1;j++) records[j]=records[j+1];
        recordCount--; saveRecords(); cout<<"Your record was deleted.\n"; return;
    }
    cout<<"Record not found, or you are not authorized to delete it.\n";
}
void statistics() {
    int lost=0,found=0;
    for(int i=0;i<recordCount;i++) { if(records[i].type=="LOST") lost++; else if(records[i].type=="FOUND") found++; }
    cout<<"\nTotal records: "<<recordCount<<"\nLost items: "<<lost<<"\nFound items: "<<found<<"\n";
}
void exportReport() {
    ofstream f("lost_found_report.txt");
    if(!f) { cout<<"Could not create report file.\n"; return; }
    f<<"SMART CAMPUS LOST & FOUND MANAGEMENT SYSTEM\n\n";
    for(int i=0;i<recordCount;i++) {
        ItemRecord r=records[i];
        f<<"ID: "<<r.id<<"\nType: "<<r.type<<"\nItem: "<<r.itemName
         <<"\nCategory: "<<r.category<<"\nLocation: "<<r.location<<"\nDate: "<<r.date
         <<"\nDescription: "<<r.description<<"\nReported by: "<<r.studentName
         <<" ("<<r.rollNumber<<")\n------------------------------\n";
    }
    f.close(); cout<<"Report exported to lost_found_report.txt\n";
}
void mainMenu() {
    int choice;
    do {
        cout<<"\n===== SMART CAMPUS LOST & FOUND =====\n"
            <<"1. Report Lost Item\n2. Report Found Item\n3. Search Item\n"
            <<"4. Auto Match Lost & Found\n5. View All Records\n6. Delete Record\n"
            <<"7. Item Statistics\n8. Export Report\n9. Exit\nChoice: ";
        cin>>choice;
        switch(choice) {
            case 1: reportItem("LOST"); break;
            case 2: reportItem("FOUND"); break;
            case 3: searchItem(); break;
            case 4: autoMatch(); break;
            case 5: viewAllRecords(); break;
            case 6: deleteRecord(); break;
            case 7: statistics(); break;
            case 8: exportReport(); break;
            case 9: cout<<"Thank you for using the system.\n"; break;
            default: cout<<"Invalid choice.\n";
        }
    } while(choice!=9);
}
int main() {
    loadRecords();
    cout<<"SMART CAMPUS LOST & FOUND MANAGEMENT SYSTEM\n";
    if(studentLogin()) mainMenu();
    return 0;
}
