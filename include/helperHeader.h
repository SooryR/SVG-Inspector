#include "SVGParser.h"
#ifndef HELPERHEADER_H 
#define HELPERHEADER_H 
void print_element_names(xmlNode * a_node);
void xmltreeParce(xmlNode * p_node);
char* addString(char* a, char* b);
Attribute *createA(xmlAttr* node);
Group *createG(xmlNode* node);
Rectangle *createR(xmlNode* node);
Circle *createC(xmlNode* node);
Path *createP(xmlNode* node);
Attribute* writeA(char* name, char* value);
List* getG(List* g, List* out);
void addToList(List* a, List* b);
void dummyFree(void* data);
xmlDoc* svgImgToDoc(SVGimage* img);
void addAttr(xmlNode* node, List* attr);
void addRect(xmlNode* parent, List* rect);
void addCircle(xmlNode* parent, List* circle);
void addPath(xmlNode* parent, List* path);
int checkAttr(List* a);
int checkPath(List* p);
int checkCircle(List* c);
int checkRect(List* rect);
int checkG(List* groups);
bool customCompareAttr(const void *first, const void *second);
char* fileNametoJSON(char* fileName);
char* fileNameTitletoJSON(char* fileName);
char* fileNameDestoJSON(char* fileName);
char* fileSvgsToJSON(char* fileName);
char* svgOtherAttr(char* fileName);
#endif


