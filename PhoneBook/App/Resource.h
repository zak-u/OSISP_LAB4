//{{NO_DEPENDENCIES}}
// Включаемый файл, созданный в Microsoft Visual C++.
// Используется App.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_APP_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_APP			107
#define IDI_SMALL				108
#define IDC_APP			109
#define IDC_MYICON				2
#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif
// Следующие стандартные значения для новых объектов
//
#ifdef _WIN64 
#define ADD_FUNCTION "?addNewRecord@@YAHPEAVRecord@@@Z"
#define FORM_FUNCTION "?formDatabase@@YA?AV?$vector@PEAVRecord@@V?$allocator@PEAVRecord@@@std@@@std@@XZ"
#define DELETE_FUNCTION "?deleteRecord@@YAXPEAVRecord@@@Z"
#define CHANGE_FUNCTION "?changeRecord@@YAXPEAVRecord@@0H@Z"
#define SEARCH_FUNCTION "?search@@YA?AV?$vector@PEAVRecord@@V?$allocator@PEAVRecord@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@H@Z"
#define GET_ALL_FUNCTION "?getAllRecords@@YA?AV?$vector@PEAVRecord@@V?$allocator@PEAVRecord@@@std@@@std@@XZ"
#else
#define ADD_FUNCTION "?addNewRecord@@YAHPAVRecord@@@Z"
#define FORM_FUNCTION "?formDatabase@@YA?AV?$vector@PAVRecord@@V?$allocator@PAVRecord@@@std@@@std@@XZ"
#define DELETE_FUNCTION "?deleteRecord@@YAXPAVRecord@@@Z"
#define CHANGE_FUNCTION "?changeRecord@@YAXPAVRecord@@0H@Z"
#define SEARCH_FUNCTION "?search@@YA?AV?$vector@PAVRecord@@V?$allocator@PAVRecord@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@H@Z"
#define GET_ALL_FUNCTION "?getAllRecords@@YA?AV?$vector@PAVRecord@@V?$allocator@PAVRecord@@@std@@@std@@XZ"
#endif
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif
