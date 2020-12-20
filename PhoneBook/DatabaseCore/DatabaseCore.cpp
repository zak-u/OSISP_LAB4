#include "pch.h"
#include "DatabaseCore.h"
#include <filesystem>

DATABASE_API vector<Record*> __cdecl formDatabase() {
    InitializeCriticalSection(&crSection);
    vector<Record*> records = loadData();
    int hashtableSize = 100;
    for (Record* record : records) {
        rootForSurnameIndexedTree = insert(rootForSurnameIndexedTree, hashFunction(record->surname), record);
        rootForPhoneIndexedTree = insert(rootForPhoneIndexedTree, hashFunction(record->phoneNumber), record);
        rootForStreetIndexedTree = insert(rootForStreetIndexedTree, hashFunction(record->street), record);
    }
    return records;
}
//
//void freeDatabase() {
//    DeleteCriticalSection(&crSection);
//}

DATABASE_API vector<Record*> __cdecl search(string searchingParameter, int criterium) {
    switch (criterium) {
    case ADDRESS:
        return select(rootForStreetIndexedTree, searchingParameter);
    case PHONE:
        return select(rootForPhoneIndexedTree, searchingParameter);
    case SURNAME:
        return select(rootForSurnameIndexedTree, searchingParameter);
    }
}

int hashFunction(string str, int num, int salt)
{
    int index = 1;
    for (char ch : str) {
        index *= (ch * ch + salt * ch + salt);
        index %= num;
    }
    return index;
}

vector<Record*> select(node* root, string searchingParameter) {
    int index = hashFunction(searchingParameter);
    vector<Record*> foundData;
    node* currNode = root;
    while (currNode != NULL) {
        if (currNode->index == index)
            foundData.push_back(currNode->record);
        if (index <= currNode->index)
            currNode = currNode->leftChild;
        else
            currNode = currNode->rightChild;
    }
    return foundData;
}

vector<Record*> loadData() {
    string filePath = PATH_TO_DB_FILE;
    ifstream dataFile;
    vector<Record*> records;
    dataFile.open(filePath);
    if (dataFile.is_open()) {
        string line;
        while (getline(dataFile, line)) {
            vector<string> data = split(line);
            Record* r = new Record(data[0],
                data[1],
                data[2],
                data[3],
                data[4],
                stoi(data[5]),
                stoi(data[6]),
                stoi(data[7])
            );
            records.push_back(r);
        }
        dataFile.close();
    }
    return records;
}

vector<string> split(string str, char separator) {
    vector<string> substrings;
    int startPos = 0;
    int endPos = str.find(separator);
    while (endPos != -1) {
        substrings.push_back(str.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos = str.find(separator, startPos);
    }
    substrings.push_back(str.substr(startPos, str.length() - startPos));
    return substrings;
}

node* insert(node* currentNode, int index, Record* insertingValue) {
    if (currentNode != NULL) {
        if (index <= currentNode->index)
            currentNode->leftChild = insert(currentNode->leftChild, index, insertingValue);
        else
            currentNode->rightChild = insert(currentNode->rightChild, index, insertingValue);
    }
    else {
        node* n = new node(index, insertingValue);
        currentNode = n;
    }
    return currentNode;
}

DATABASE_API VOID __cdecl deleteRecord(Record* deletingRecord) {
    int uniqueIndex = hashFunction(deletingRecord->phoneNumber);
    EnterCriticalSection(&crSection);
    deleteNode(&rootForPhoneIndexedTree, hashFunction(deletingRecord->phoneNumber), uniqueIndex);
    deleteNode(&rootForSurnameIndexedTree, hashFunction(deletingRecord->surname), uniqueIndex);
    deleteNode(&rootForStreetIndexedTree, hashFunction(deletingRecord->street), uniqueIndex);
    LeaveCriticalSection(&crSection);
}

node* deleteNode(node** root, int index, int uniqueIndex) {
    if (*root == NULL)
        return *root;
    if (index <= (*root)->index && !neededRecord((*root)->record, uniqueIndex))
        (*root)->leftChild = deleteNode(&(*root)->leftChild, index, uniqueIndex);
    else if (index > (*root)->index)
        (*root)->rightChild = deleteNode(&(*root)->rightChild, index, uniqueIndex);
    else if ((*root)->leftChild != NULL && (*root)->rightChild != NULL) {
        node* tmp = myMin((*root)->rightChild);
       (*root)->index = tmp->index;
       (*root)->record = tmp->record;
       (*root)->rightChild = deleteNode(&(*root)->rightChild, (*root)->index, hashFunction((*root)->record->phoneNumber));
    }
    else {
        if ((*root)->leftChild != NULL)
            *root = (*root)->leftChild;
        else if ((*root)->rightChild != NULL)
            *root = (*root)->rightChild;
        else
            *root = NULL;
    }
    return *root;
}

bool neededRecord(Record* curr, int uniqueIndex) {
    if (uniqueIndex == hashFunction(curr->phoneNumber))
        return true;
    return false;
}

void getRecords(node* currentNode,vector<Record*>* v)
{
    if (currentNode != NULL) {
        v->push_back(currentNode->record);
        getRecords(currentNode->leftChild,v);
        getRecords(currentNode->rightChild,v);
    }
}

DATABASE_API BOOL __cdecl addNewRecord(Record* newRecord)
{
    if (search(newRecord->phoneNumber, PHONE).size() != 0)
        return false;
    EnterCriticalSection(&crSection);
    insert(rootForPhoneIndexedTree, hashFunction(newRecord->phoneNumber), newRecord);
    insert(rootForStreetIndexedTree, hashFunction(newRecord->street), newRecord);
    insert(rootForSurnameIndexedTree, hashFunction(newRecord->surname), newRecord);
    LeaveCriticalSection(&crSection);
    return true;
}

DATABASE_API vector<Record*> __cdecl getAllRecords()
{
    vector<Record*> records;
    getRecords(rootForPhoneIndexedTree,&records);
    return records;
}

DATABASE_API VOID __cdecl changeRecord(Record* oldRecord, Record* newRecord, int changedIndex)
{
    if (!changedIndex) {
        EnterCriticalSection(&crSection);
        change(&rootForPhoneIndexedTree, newRecord, hashFunction(newRecord->phoneNumber));
        change(&rootForSurnameIndexedTree, newRecord, hashFunction(newRecord->surname));
        change(&rootForStreetIndexedTree, newRecord, hashFunction(newRecord->street)); 
        LeaveCriticalSection(&crSection);
        return;
    }
    int uniqueIndex = hashFunction(oldRecord->phoneNumber);
    if ((changedIndex & PHONE) == PHONE) {
        changedIndex ^= PHONE;
        EnterCriticalSection(&crSection);
        deleteNode(&rootForPhoneIndexedTree, hashFunction(oldRecord->phoneNumber), uniqueIndex);
        insert(rootForPhoneIndexedTree, hashFunction(newRecord->phoneNumber), newRecord);
        change(&rootForSurnameIndexedTree, newRecord, hashFunction(newRecord->surname));
        change(&rootForStreetIndexedTree, newRecord, hashFunction(newRecord->street));
        LeaveCriticalSection(&crSection);
    }
    if ((changedIndex & SURNAME) == SURNAME) {
        changedIndex ^= SURNAME;
        EnterCriticalSection(&crSection);
        deleteNode(&rootForSurnameIndexedTree, hashFunction(oldRecord->surname), uniqueIndex);
        insert(rootForSurnameIndexedTree, hashFunction(newRecord->surname), newRecord);
        change(&rootForPhoneIndexedTree, newRecord, hashFunction(newRecord->surname));
        change(&rootForStreetIndexedTree, newRecord, hashFunction(newRecord->street));
        LeaveCriticalSection(&crSection);
    }
    if ((changedIndex & ADDRESS) == ADDRESS) {
        changedIndex ^= ADDRESS;
        EnterCriticalSection(&crSection);
        deleteNode(&rootForStreetIndexedTree, hashFunction(oldRecord->street), uniqueIndex);
        insert(rootForStreetIndexedTree, hashFunction(newRecord->street), newRecord);
        change(&rootForSurnameIndexedTree, newRecord, hashFunction(newRecord->surname));
        change(&rootForPhoneIndexedTree, newRecord, hashFunction(newRecord->street));
        LeaveCriticalSection(&crSection);
    }
}

void change(node** root, Record* newRecord, int index) {
    int uniqueIndex = hashFunction(newRecord->phoneNumber);
    node* tmp = *root;

    while (!neededRecord(tmp->record, uniqueIndex)) {
        if (index <= tmp->index)
            if (tmp->leftChild == NULL)
                break;
            else
                tmp = tmp->leftChild;
        else
            if (tmp->rightChild == NULL)
                break;
            else
                tmp = tmp->rightChild;
    }
    LeaveCriticalSection(&crSection);
    tmp->record = newRecord;
}

node* myMin(node* currNode) {
    if (currNode->leftChild == NULL)
        return currNode;
    return myMin(currNode->leftChild);
}
