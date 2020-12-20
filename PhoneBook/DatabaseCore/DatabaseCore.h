#pragma once
#include <windows.h>
#include <iostream>
#include "Record.h"
#include <fstream>
#include <vector>
using namespace std;

#ifdef SOME_EXPORTS
#define DATABASE_API  __declspec(dllimport)
#else
#define DATABASE_API __declspec(dllexport)
#endif

#ifdef _WIN64
#define PATH_TO_DB_FILE "..\\addresses.txt"
#else
#define PATH_TO_DB_FILE "..\\addresses.txt"
#endif

#define ADDRESS 1
#define SURNAME 2
#define PHONE 4


struct node {
    int index;
    Record* record;

    node* leftChild;
    node* rightChild;

    node(int k, Record* _record) {
        index = k;
        record = _record;
        leftChild = NULL;
        rightChild = NULL;
    }
};

//export funtions
DATABASE_API vector<Record*> __cdecl formDatabase();
DATABASE_API vector<Record*> __cdecl search(string searchingParameter, int criterium = 2);
DATABASE_API VOID __cdecl deleteRecord(Record* deletingRecord);
DATABASE_API VOID __cdecl changeRecord(Record* oldRecord, Record* newRecord, int changedIndex = 0);
DATABASE_API BOOL __cdecl addNewRecord(Record* newRecord);
DATABASE_API vector<Record*> __cdecl getAllRecords();

//function used by "database"
vector<Record*> loadData();
vector<string> split(string str, char separator = ' ');
int hashFunction(string str, int num = 199, int salt = 31);
node* insert(node* root, int index, Record* insertingValue);
vector<Record*> select(node* currnNode, string searchingParameter);
node* deleteNode(node** root, int index, int uniqueIndex);
void change(node** root, Record* newRecord, int index);
node* myMin(node* root);
bool neededRecord(Record* curr, int uniqueIndex);
void getRecords(node*,vector<Record*>*);
//void freeDatabase();

CRITICAL_SECTION crSection;

node* rootForSurnameIndexedTree = NULL;
node* rootForPhoneIndexedTree = NULL;
node* rootForStreetIndexedTree = NULL;