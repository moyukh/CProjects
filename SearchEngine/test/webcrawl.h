
struct URLRecord {
  char * _url;          // URL of the document.
  char * _description;  // First 200 chars of the document without tags
};

struct URLRecordList {
  URLRecord * _urlRecord;  // Pointer to the URL Record
  URLRecordList * _next;   // Pointer to the next URL Record in the list
};

struct URLNumList{
	int _num;
	URLNumList * _next;
};

