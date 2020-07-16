#include "helperHeader.h"
#include "SVGParser.h"
#include "LinkedListAPI.h"
#include <math.h>
#include <string.h>
#define M_PI 3.14159265358979323846

//function will realloc a and concatenate b on to a
char* addString(char* a, char* b)
{
    if(!b || !a)
    {
        return a;
    }
    a = realloc(a, strlen(a) + strlen(b) + 50);
    strcat(a,b);
    return a;
}

//function will create and allocate Attribute
Attribute* writeA(char* name, char* value)
{
    if(name == NULL || value == NULL)
    {
        return NULL;
    }

    Attribute *shape = malloc(sizeof(Attribute));
    shape->name = malloc(1);
    shape->value = malloc(1);
    shape->name[0] = '\0';
    shape->value[0] = '\0';
    shape->name = addString(shape->name, name);
    shape->value = addString(shape->value, value);

    return shape;
}

//function will get the attribute name and value then create a attribute
Attribute* createA(xmlAttr* attr)
{
    if (attr != NULL)
    { 
        xmlNode *value = attr->children;
        return writeA((char *)attr->name, (char *)(value->content));
    }
    return NULL;
}

//function will get info from group node and make a group
Group *createG(xmlNode* node)
{
    if(node == NULL)
    {
        return NULL;
    }
    
    xmlNode* cur_node = node;
    Group *shape = malloc(sizeof(Group));
    shape->rectangles = initializeList(&rectangleToString,&deleteRectangle,&compareRectangles);
    shape->circles = initializeList(&circleToString,&deleteCircle,&compareCircles);
    shape->paths = initializeList(&pathToString,&deletePath,&comparePaths);
    shape->groups = initializeList(&groupToString,&deleteGroup,&compareGroups);
    shape->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    
    xmlAttr *attr;
    char* value = malloc(255);
    value[0] = '\0';

    //for loop to get attributes of group
    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        Attribute* temp1;
        temp1 = createA(attr);
        if(temp1 == NULL){deleteGroup(shape);return NULL;}
        insertBack(shape->otherAttributes,temp1);
    }

    node = node->children;
    
    //add stuff within the group into their own lists
    for (cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"rect")==0) 
        {
            Rectangle* temp2;
            temp2 = createR(cur_node);
            if(temp2 == NULL){free(value);deleteGroup(shape);return NULL;}
            insertBack(shape->rectangles,temp2);
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"circle")==0) 
        {
            Circle* temp3;
            temp3 = createC(cur_node);
            if(temp3 == NULL){free(value);deleteGroup(shape);return NULL;}
            insertBack(shape->circles,temp3);
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"path")==0) 
        {
            Path* temp4;
            temp4 = createP(cur_node);
            if(temp4 == NULL){free(value);deleteGroup(shape);return NULL;}
            insertBack(shape->paths,temp4);
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"g")==0) 
        {
            Group* temp5;
            temp5 = createG(cur_node);
            if(temp5 == NULL){free(value);deleteGroup(shape);return NULL;}
            insertBack(shape->groups,temp5);
        }
        else if (strcmp((char*)cur_node->name,"text")==0) 
        {
            sscanf((char*)cur_node->content, "%s[^\n]", value);
            if(value[0] != '\0')
            {
                insertBack(shape->otherAttributes, writeA((char*)cur_node->name,value));
            }
            value[0] = '\0';
        }
    }
    free(value);
    return shape;
}

//function will make a rectangle from the info givin by the node
Rectangle *createR(xmlNode* node)
{
    if(node == NULL)
    {
        return NULL;
    }
    xmlNode* cur_node = node;
    Rectangle* shape = malloc(sizeof(Rectangle));
    shape->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    shape->units[0] = '\0';
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        if(strcmp(attrName,"x") == 0)
        {
            shape->x = atof((char *)(value->content));
        }
        else if(strcmp(attrName,"y") == 0)
        {
            shape->y = atof((char *)(value->content));
        }
        else if(strcmp(attrName,"width") == 0)
        {
            sscanf((char *)(value->content), "%f%s",&(shape->width),shape->units);
        }
        else if(strcmp(attrName,"height") == 0)
        {
            shape->height = atof((char *)(value->content));
        }
        else
        {
            insertBack(shape->otherAttributes,createA(attr));
        }
        
    }

    return shape;
}

//function will create a circle from the info from the node givin
Circle *createC(xmlNode* node)
{
    if(node == NULL)
    {
        return NULL;
    }
    xmlNode* cur_node = node;
    Circle* shape = malloc(sizeof(Circle));
    shape->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    shape->units[0] = '\0';
    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        if(strcmp(attrName,"cx") == 0)
        {
            shape->cx = atof((char *)(value->content));
        }
        else if(strcmp(attrName,"cy") == 0)
        {
            shape->cy = atof((char *)(value->content));
        }
        else if(strcmp(attrName,"r") == 0)
        {
            sscanf((char *)(value->content), "%f%s",&(shape->r),shape->units);
        }
        else
        {
            insertBack(shape->otherAttributes,createA(attr));
        }
        
    }
    return shape;
}

//function will return Path * which will be created from the info of the node givin
Path *createP(xmlNode* node)
{
    if(node == NULL)
    {
        return NULL;
    }
    xmlNode *cur_node = node;
    Path* shape = malloc(sizeof(Path));
    shape->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    shape->data = malloc(1);
    shape->data[0] = '\0';


    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        if(strcmp(attrName,"d") == 0)
        {
            shape->data = addString(shape->data,(char *)(value->content));
        }
        else
        {
            insertBack(shape->otherAttributes,createA(attr));
        }
        
    }
    return shape;
}

//function will create an SVGimage and return it
SVGimage* createSVGimage(char* fileName)
{
    SVGimage* img = malloc(sizeof(SVGimage));
    img->rectangles = initializeList(&rectangleToString,&deleteRectangle,&compareRectangles);
    img->circles = initializeList(&circleToString,&deleteCircle,&compareCircles);
    img->paths = initializeList(&pathToString,&deletePath,&comparePaths);
    img->groups = initializeList(&groupToString,&deleteGroup,&compareGroups);
    img->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    img->namespace[0] = '\0';
    img->title[0] = '\0';
    img->description[0] = '\0';

	xmlDoc *doc;
	xmlNode *root = NULL;
	xmlNode *node = NULL;
    xmlNode *cur_node = NULL;
    doc = xmlReadFile(fileName, NULL, 0);
    //check if doc is null or not
    if(!doc)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        deleteSVGimage(img);
        return NULL;
    }
    root = xmlDocGetRootElement(doc);
    node = root;
    node = node->children;
    xmlAttr *attr;
    char* value = malloc(255);
    value[0] = '\0';
    //get the name space
    snprintf(img->namespace,256,"%s",(char*)root->ns->href);
    
    //get the attributes of the svg
    for (attr = root->properties; attr != NULL; attr = attr->next)
    {
        Attribute* temp1;
        temp1 = createA(attr);
        if(temp1 == NULL){deleteSVGimage(img);return NULL;}
        insertBack(img->otherAttributes,temp1);
    }
    
    for (cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
        
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"rect")==0) 
        {
            insertBack(img->rectangles,createR(cur_node));
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"circle")==0) 
        {
            insertBack(img->circles,createC(cur_node));
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"title")==0) 
        {
            snprintf(img->title, 256,"%s",(char*)cur_node->children->content);
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"desc")==0) 
        {
            snprintf(img->description, 256,"%s",(char*)cur_node->children->content);
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"path")==0) 
        {
            insertBack(img->paths,createP(cur_node));
        }
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"g")==0) 
        {
            insertBack(img->groups,createG(cur_node));
        }
        else if (strcmp((char*)cur_node->name,"text")==0) 
        {
            sscanf((char*)cur_node->content, "%s[^\n]", value);
            if(value[0] != '\0')
            {
                insertBack(img->otherAttributes,writeA((char*)cur_node->name,value));
            }
            value[0] = '\0';
        }
        else
        {
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                insertBack(img->otherAttributes,createA(attr));
            }
        }
    }

    //free any not used memory
    xmlFreeDoc(doc);
    xmlCleanupParser();
    free(value);
    return img;
}

//function will get info of SVGimge struct and turn it in to a read able string
char* SVGimageToString(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    char *string = malloc(1);
    char *buffer;
    string[0] = '\0';
    string = addString(string, "Img: \n");
    string = addString(string, "Name: ");
    string = addString(string, img->namespace);
    
    string = addString(string, "\nTitle: ");
    string = addString(string, img->title);
    string = addString(string, "\nDescription: ");
    string = addString(string, img->description);

    buffer = toString(img->otherAttributes);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(img->rectangles);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(img->circles);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(img->paths);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(img->groups);
    string = addString(string, buffer);
    free(buffer);

    return string;
}

//function free SVGimage struct
void deleteSVGimage(SVGimage* img)
{
    if(img){
        freeList(img->rectangles);
        freeList(img->circles);
        freeList(img->paths);
        freeList(img->groups);
        freeList(img->otherAttributes);
        
        free(img);
    }
}

void deleteAttribute( void* data)
{
    Attribute* node = (Attribute*)data;
    if(node){
        free(node->name);
        free(node->value);
        free(node);
    }
}

char* attributeToString( void* data)
{
    Attribute* node = (Attribute*)data;
    char *string = malloc(1);
    string[0] = '\0';
    string = addString(string, "Attribute: \n");
    string = addString(string,"Attribute name: ");
    string = addString(string,node->name);
    string = addString(string,"\n");
    string = addString(string,"Attribute value: ");
    string = addString(string,node->value);
    string = addString(string,"\n");

    return string;
}
int compareAttributes(const void *first, const void *second)
{
    int compare = 0;
    compare += strcmp(((Attribute*)first)->name, ((Attribute*)second)->name);
    compare += strcmp(((Attribute*)first)->value, ((Attribute*)second)->value);
    return compare;
}

void deleteGroup(void* data)
{
    Group* node = (Group*)data;
    if(node){
        freeList(node->rectangles);
        freeList(node->circles);
        freeList(node->paths);
        freeList(node->groups);
        freeList(node->otherAttributes);

        free(node);
    }
}
char* groupToString( void* data)
{
    Group* node = (Group*)data;
    char* string = malloc(1);
    char *buffer;
    string[0] = '\0';

    string = addString(string, "Group: \n");

    buffer = toString(node->rectangles);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(node->circles);
    string = addString(string, buffer);
    free(buffer);
    
    buffer = toString(node->paths);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(node->otherAttributes);
    string = addString(string, buffer);
    free(buffer);

    buffer = toString(node->groups);
    string = addString(string, buffer);
    free(buffer);

    return string;
}
int compareGroups(const void *first, const void *second)
{
    return 0;
}

void deleteRectangle(void* data)
{
    Rectangle* node = (Rectangle*)data;
    if(node){
        freeList(node->otherAttributes);
        free(node);
    }
}
char* rectangleToString(void* data)
{
    
    Rectangle* node = (Rectangle*)data;
    char *string = malloc(1);
    
    char *buffer = malloc(1000);
    char *extraA = toString(node->otherAttributes);
    
    string[0] = '\0';
    string = addString(string, "Rectangle: \n");
    string = addString(string, "Coordinate: x = ");
    
    snprintf(buffer, 99, "%.2f", node->x);
    string = addString(string, buffer);

    string = addString(string, ", y = ");

    snprintf(buffer, 99, "%.2f", node->y);
    string = addString(string, buffer);

    string = addString(string, "\nThe width: ");
    snprintf(buffer, 99, "%.2f", node->width);
    string = addString(string, buffer);
    
    string = addString(string, "\nThe height: ");
    snprintf(buffer, 99, "%.2f", node->height);
    
    string = addString(string, buffer);
    string = addString(string, "\nUnits for the rectangle coordinates and size: ");
    string = addString(string, node->units);
    string = addString(string, "\n");
    string = addString(string, extraA);
    string = addString(string, "\n");
    
    free(extraA);
    free(buffer);
    return string;
}
int compareRectangles(const void *first, const void *second)
{
    return 0;
}


void deleteCircle(void* data)
{
    Circle *node = (Circle*)data;
    if(node)
    {
        freeList(node->otherAttributes);
        free(node);
    }
        
}
char* circleToString(void* data)
{
    Circle *node = (Circle*)data;
    char *string = malloc(1);
    char *buffer = malloc(100);
    char *extraA = toString(node->otherAttributes);
    string[0] = '\0';
    string = addString(string, "Circle: \n");
    string = addString(string, "Coordinate: x = ");

    snprintf(buffer, 99, "%.2f", node->cx);
    string = addString(string, buffer);

    string = addString(string, ", y = ");

    snprintf(buffer, 99, "%.2f", node->cy);
    string = addString(string, buffer);

    string = addString(string, "\nThe radius: ");
    snprintf(buffer, 99, "%.2f", node->r);
    string = addString(string, buffer);

    string = addString(string, "\nUnits for the circle coordinates and size: ");
    string = addString(string, node->units);

    string = addString(string, extraA);
    free(extraA);
    free(buffer);

    return string;
}
int compareCircles(const void *first, const void *second)
{
    return 0;
}

void deletePath(void* data)
{
    Path* node = (Path*)data;

    if(node){
        free(node->data);
        freeList(node->otherAttributes);
        free(node);
    }
}
char* pathToString(void* data)
{
    Path* node = (Path*)data;
    char *string = malloc(1);
    string[0] = '\0';
    char *extraA = toString(node->otherAttributes);
    string = addString(string, "Path: \n");
    string = addString(string, "Path data: ");
    string = addString(string, node->data);
    string = addString(string, "\n");
    string = addString(string, extraA);
    free(extraA);
    return string;
}
int comparePaths(const void *first, const void *second)
{
    return 0;
}

//add elements of list b into list a
void addToList(List* a, List* b)
{
    if(!a || !b)
    {
        return;
    }
    ListIterator iter;
    iter = createIterator(b);
    void* node = nextElement(&iter);
    while(node)
    {
        insertBack(a,node);
        node = nextElement(&iter);
    }
}

//gets all the groups and add it to out
List* getG(List* g, List* out)
{
    List* tempG;
    //add all elements of g into out
    addToList(out,g);
    ListIterator iter;
    iter = createIterator(g);
    void* node;
    Group* gg;
    int i;
    //go through list g
    for(i = 0; i<getLength(g); i++)
    {
        node = nextElement(&iter);
        gg = (Group*)node;
        tempG = gg->groups;
        if(getLength(tempG) > 0)
        {
            //recursivly add the list groups from the elements of list g
            getG(tempG, out);
        }
    }
    return out;
}

//get all the rectangles from all the groups and svgimage and return it
List* getRects(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    List* rects = initializeList(&rectangleToString,&dummyFree,&compareRectangles);
    ListIterator iter;
    Group* nodeG;
    int i;
    
    List* g = getGroups(img);
    addToList(rects, img->rectangles);
    iter = createIterator(g);
    
    for(i = 0; i<getLength(g); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        if(nodeG->rectangles)
        {
            addToList(rects,nodeG->rectangles);
        }
    }
    freeList(g);
    return (rects);
}
//get all the circles from all the groups and svgimage and return it
List* getCircles(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    List* circles = initializeList(&circleToString,&dummyFree,&compareCircles);
    ListIterator iter;
    Group* nodeG;
    int i;
    
    List* g = getGroups(img);
    addToList(circles, img->circles);
    iter = createIterator(g);
    
    for(i = 0; i<getLength(g); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        if(nodeG->circles)
        {
            addToList(circles,nodeG->circles);
        }
    }
    freeList(g);
    return (circles);
}
//get all the groups from all the groups and svgimage and return it
List* getGroups(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    List* out = initializeList(&groupToString,&dummyFree,&compareGroups);
    return (getG(img->groups, out));
}
//get all the paths from all the groups and svgimage and return it
List* getPaths(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    List* paths = initializeList(&pathToString,&dummyFree,&comparePaths);
    ListIterator iter;
    Group* nodeG;
    int i;
    
    List* g = getGroups(img);
    addToList(paths, img->paths);
    iter = createIterator(g);
    
    for(i = 0; i<getLength(g); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        if(nodeG->paths)
        {
            addToList(paths,nodeG->paths);
        }
    }
    freeList(g);
    return (paths);
}
//check if the rectangles from the list rect is valid
int checkRect(List* rect)
{
    if(!rect)
    {
        return 0;
    }
    ListIterator iter;
    Rectangle* nodeR;
    iter = createIterator(rect);
    for(int i = 0; i<getLength(rect); i++)
    {
        nodeR = (Rectangle*)nextElement(&iter);
        
        if(!nodeR &&(nodeR->width < 0 || nodeR->height < 0 || !nodeR->otherAttributes))
        {
            return 0;
        }
        if(!checkAttr(nodeR->otherAttributes))
        {
            return 0;
        }
    }

    return 1;
}
//check if the groups from the list groups is valid
int checkG(List* groups)
{
    if(!groups)
    {
        return 0;
    }
    ListIterator iter;
    Group* nodeG;
    iter = createIterator(groups);
    for(int i = 0; i<getLength(groups); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        
        if(!nodeG || !nodeG->groups|| !nodeG->rectangles || !nodeG->otherAttributes || !nodeG->circles || !nodeG->paths)
        {
            return 0;
        }
        if(!checkAttr(nodeG->otherAttributes) || !checkCircle(nodeG->circles) || !checkG(nodeG->groups) || !checkPath(nodeG->paths) || !checkRect(nodeG->rectangles))
        {
            return 0;
        }
    }

    return 1;
}
//check if the circles from the list c is valid
int checkCircle(List* c)
{
    if(!c)
    {
        return 0;
    }
    ListIterator iter;
    Circle* nodeC;

    iter = createIterator(c);
    for(int i = 0; i<getLength(c); i++)
    {
        nodeC = (Circle*)nextElement(&iter);
        
        if(!nodeC || (nodeC->r < 0) || !nodeC->otherAttributes)
        {
            return 0;
        }
        if(!checkAttr(nodeC->otherAttributes))
        {
            return 0;
        }
    }

    return 1;
}
//check if the paths from the list p is valid
int checkPath(List* p)
{
    if(!p)
    {
        return 0;
    }
    ListIterator iter;
    Path* nodeP;

    iter = createIterator(p);
    for(int i = 0; i<getLength(p); i++)
    {
        nodeP = (Path*)nextElement(&iter);
        if(!nodeP || nodeP->data == NULL || !nodeP->otherAttributes)
        {
            return 0;
        }
        if(!checkAttr(nodeP->otherAttributes))
        {
            return 0;
        }
    }

    return 1;
}
//check if the Attributes from the list a is valid
int checkAttr(List* a)
{
    if(!a)
    {
        return 0;
    }

    ListIterator iter;
    Attribute* nodeA;
    iter = createIterator(a);

    for(int i = 0; i<getLength(a); i++)
    {
        nodeA = (Attribute*)nextElement(&iter);
        
        if(!nodeA || nodeA->name == NULL || nodeA->value == NULL)
        {
            return 0;
        }
    }

    return 1;
}

//get the number of rectangles within img that have the area of the input area
int numRectsWithArea(SVGimage* img, float area)
{
    if(!img || area < 0)
    {
        return 0;
    }

    int numOut = 0;
    ListIterator iter;
    List* rect = getRects(img);
    Rectangle* nodeR;
    iter = createIterator(rect);
    for(int i = 0; i<getLength(rect); i++)
    {
        nodeR = (Rectangle*)nextElement(&iter);
        
        if(nodeR && ceil(nodeR->height * nodeR->width) == ceil(area))
        {
            numOut++;
        }
    }
    freeList(rect);
    return numOut;
}

//get the number of circles within img that have the area of the input area
int numCirclesWithArea(SVGimage* img, float area)
{
    if(!img || area < 0)
    {
        return 0;
    }

    
    int numOut = 0;
    ListIterator iter;
    List* c = getCircles(img);

    Circle* nodeC;

    iter = createIterator(c);
    for(int i = 0; i<getLength(c); i++)
    {
        nodeC = (Circle*)nextElement(&iter);
        if(nodeC && ceil(nodeC->r * nodeC->r * M_PI) == ceil(area))
        {
            numOut++;
        }
    }

    freeList(c);

    return numOut;
}

//get the number of paths within img that have the data of the input data
int numPathsWithdata(SVGimage* img, char* data)
{
    if(!img || !data)
    {
        return 0;
    }
    
    int numOut = 0;
    ListIterator iter;
    List* p = getPaths(img);

    Path* nodeP;

    iter = createIterator(p);
    for(int i = 0; i<getLength(p); i++)
    {
        nodeP = (Path*)nextElement(&iter);
        if(nodeP && strcmp((nodeP->data),data) == 0)
        {
            numOut++;
        }
    }

    freeList(p);

    return numOut;
}

//get the number of groups within img that have the number of sub-elements of the number len
int numGroupsWithLen(SVGimage* img, int len)
{
    if(!img || len < 0)
    {
        return 0;
    }
    
    int numOut = 0;
    ListIterator iter;
    List* g = getGroups(img);

    Group* nodeG;

    iter = createIterator(g);
    for(int i = 0; i<getLength(g); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        if(nodeG && (getLength(nodeG->groups) + getLength(nodeG->circles) + getLength(nodeG->paths) + getLength(nodeG->rectangles)) == len)
        {
            numOut++;
        }
    }

    freeList(g);

    return numOut;
}

//get the number of Attributes within img
int numAttr(SVGimage* img)
{
    if(!img)
    {
        return 0;
    }
    
    int numOut = 0;
    numOut += getLength(img->otherAttributes);

    ListIterator iter;
    List* temp = getRects(img);
    Rectangle* nodeR;
    iter = createIterator(temp);

    for(int i = 0; i<getLength(temp); i++)
    {
        nodeR = (Rectangle*)nextElement(&iter);
        
        if(nodeR)
        {
            numOut += getLength(nodeR->otherAttributes);
        }
    }
    freeList(temp);
    temp = getCircles(img);
    Circle* nodeC;
    iter = createIterator(temp);
    for(int i = 0; i<getLength(temp); i++)
    {
        nodeC = (Circle*)nextElement(&iter);
        if(nodeC)
        {
            numOut += getLength(nodeC->otherAttributes);
        }
    }
    freeList(temp);
    temp = getPaths(img);
    Path* nodeP;

    iter = createIterator(temp);
    for(int i = 0; i<getLength(temp); i++)
    {
        nodeP = (Path*)nextElement(&iter);
        if(nodeP)
        {
            numOut += getLength(nodeP->otherAttributes);
        }
    }
    freeList(temp);
    temp = getGroups(img);
    Group* nodeG;

    iter = createIterator(temp);
    for(int i = 0; i<getLength(temp); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        if(nodeG)
        {
            numOut += getLength(nodeG->otherAttributes);
        }
    }
    freeList(temp);
    return numOut;
}

//dummyFree to free the list and not the elements of the list
void dummyFree(void* data)
{
}

//create and add all the attribute to the xmlNode
void addAttr(xmlNode* node, List* attr)
{
    if(!node || !attr)
    {
        return;
    }
    ListIterator iter;
    Attribute* nodeA;
    iter = createIterator(attr);
    for(int i = 0; i<getLength(attr); i++)
    {
        nodeA = (Attribute*)nextElement(&iter);
        xmlNewProp(node, BAD_CAST nodeA->name, BAD_CAST nodeA->value);
    }
}

//create and add all the rectangles to the parent node
void addRect(xmlNode* parent, List* rect)
{
    if(!parent || !rect)
    {
        return;
    }
    ListIterator iter;
    Rectangle* nodeR;
    xmlNode* node;
    char *temp = malloc(255);
    iter = createIterator(rect);
    for(int i = 0; i<getLength(rect); i++)
    {
        nodeR = (Rectangle*)nextElement(&iter);
        node = xmlNewChild(parent, NULL, BAD_CAST "rect",NULL);

        snprintf(temp, 200, "%.2f", nodeR->x);
        temp = addString(temp, nodeR->units);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST temp);
        snprintf(temp, 200, "%.2f", nodeR->y);
        temp = addString(temp, nodeR->units);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST temp);

        snprintf(temp, 200, "%.2f", nodeR->width);
        temp = addString(temp, nodeR->units);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST temp);

        snprintf(temp, 200, "%.2f", nodeR->height);
        temp = addString(temp, nodeR->units);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST temp);

        addAttr(node, nodeR->otherAttributes);
    }
    free(temp);
}

//create and add all the circles to the parent node
void addCircle(xmlNode* parent, List* circle)
{
    if(!parent || !circle)
    {
        return;
    }

    ListIterator iter;
    Circle* nodeC;
    xmlNode* node;
    char *temp = malloc(255);
    temp[0] = '\0';
    iter = createIterator(circle);

    for(int i = 0; i<getLength(circle); i++)
    {
        nodeC = (Circle*)nextElement(&iter);
        if(nodeC == NULL)
        {
            continue;
        }
        node = xmlNewChild(parent, NULL, BAD_CAST "circle",NULL);
        
        if(nodeC->cx)
        {
            snprintf(temp, 200, "%.2f", nodeC->cx);
            temp = addString(temp, nodeC->units);
            xmlNewProp(node, BAD_CAST "cx", BAD_CAST temp);
        }
        
        if(nodeC->cy)
        {
            snprintf(temp, 200, "%.2f", nodeC->cy);
            temp = addString(temp, nodeC->units);
            xmlNewProp(node, BAD_CAST "cy", BAD_CAST temp);
        }

        if(nodeC->r)
        {
            snprintf(temp, 200, "%.2f", nodeC->r);
            temp = addString(temp, nodeC->units);
            xmlNewProp(node, BAD_CAST "r", BAD_CAST temp);
        }
        addAttr(node, nodeC->otherAttributes);
    }
    free(temp);
}

//create and add all the paths to the parent node
void addPath(xmlNode* parent, List* path)
{
    if(!parent || !path)
    {
        return;
    }

    ListIterator iter;
    Path* nodeP;
    xmlNode* node;
    iter = createIterator(path);

    for(int i = 0; i<getLength(path); i++)
    {
        nodeP = (Path*)nextElement(&iter);
        node = xmlNewChild(parent, NULL, BAD_CAST "path", NULL);

        xmlNewProp(node, BAD_CAST "d", BAD_CAST nodeP->data);
        addAttr(node, nodeP->otherAttributes);
    }
}

//create and add all the groups to the parent node
void addGroup(xmlNode* parent, List* g)
{
    if(!parent || !g)
    {
        return;
    }

    ListIterator iter;
    Group* nodeG;
    xmlNode* node;
    iter = createIterator(g);

    for(int i = 0; i<getLength(g); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        node = xmlNewChild(parent, NULL, BAD_CAST "g",NULL);
        addRect(node,nodeG->rectangles);
        addCircle(node,nodeG->circles);
        addPath(node,nodeG->paths);
        addAttr(node, nodeG->otherAttributes);
        addGroup(node,nodeG->groups);
    }
}

//make a xmlDoc from a svgimage struct
xmlDoc* svgImgToDoc(SVGimage* img)
{
    if(!img)
    {
        return NULL;
    }
    xmlDoc* doc;
    xmlNode* root;
    xmlNsPtr ns;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "svg");
    ns = xmlNewNs(root,BAD_CAST img->namespace,NULL);
    xmlSetNs(root,ns);
    xmlDocSetRootElement(doc, root);

    if(img->title[0] != '\0')
    {
        xmlNewTextChild(root,NULL,BAD_CAST "title",BAD_CAST img->title);
    }
    if(img->description[0] != '\0')
    {
        xmlNewTextChild(root,NULL,BAD_CAST "desc",BAD_CAST img->description);
    }
    
    addAttr(root,img->otherAttributes);
    
    addRect(root,img->rectangles);
    addCircle(root,img->circles);
    addPath(root,img->paths);
    addGroup(root,img->groups);
    return doc;
}

//create a valid svgimage and return it
SVGimage* createValidSVGimage(char* fileName, char* schemaFile)
{
    if(!fileName || !schemaFile)
    {
        return NULL;
    }
    SVGimage* img;
    img = createSVGimage(fileName);
    if(!img || validateSVGimage(img, schemaFile) == false)
    {
        return NULL;
    }

    return img;
}

//check if the svgimage is valid with the schemaFile and our own struct restrictions
bool validateSVGimage(SVGimage* doc, char* schemaFile)
{
    if(!doc || !schemaFile)
    {
        return false;
    }

    xmlDoc* xDoc;
    xDoc = svgImgToDoc(doc);
    if(!xDoc)
    {
        return false;
    }
    if(!checkAttr(doc->otherAttributes) || !checkCircle(doc->circles) || !checkG(doc->groups) || !checkPath(doc->paths) || !checkRect(doc->rectangles))
    {
        xmlFreeDoc(xDoc);
        xmlCleanupParser();
        return false;
    }
    
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    ctxt = xmlSchemaNewParserCtxt(schemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    
    int ret;
    xmlSchemaValidCtxtPtr ctxt2;
    ctxt2 = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt2, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt2, xDoc);
    if (ret > 0)
    {
        xmlFreeDoc(xDoc);
        xmlCleanupParser();
        return false;
    }
    else if(ret == 0){}
    else
    {
        printf("%s validation generated an internal error\n", schemaFile);
    }
    

    xmlSchemaFreeValidCtxt(ctxt2);
    if(schema != NULL)
    {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlFreeDoc(xDoc);
    xmlCleanupParser();
    return true;
}

//changes the svgimage to a xml doc then writes it to a file
bool writeSVGimage(SVGimage* doc, char* fileName)
{
    if(!doc || !fileName)
    {
        return false;
    }

    xmlDoc* xDoc;
    xDoc = svgImgToDoc(doc);
    if(xDoc)
    {
        xmlSaveFormatFileEnc(fileName, xDoc, "UTF-8", 1);
        xmlFreeDoc(xDoc);
    }
    xmlCleanupParser();
    return true;
}

//fuction compares the name of two Attributes
bool customCompareAttr(const void *first, const void *second)
{
    if(strcmp(((Attribute*)first)->name, ((Attribute*)second)->name) == 0)
    {
        return true;
    } 
    else
    {
        return false;
    }
}

//function that sets or adds the attribute of an SVGimage, Circle, Rectangle, Path, or Group.
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute*newAttribute)
{
    if(!image || !newAttribute)
    {
        return;
    }
    Attribute* nodeA;
    ListIterator iter;
    
    
    if(elemType == SVG_IMAGE)
    {
        nodeA = (Attribute*)findElement(image->otherAttributes, &customCompareAttr, newAttribute);
        if(nodeA)
        {
            nodeA->value[0] = '\0';
            addString(nodeA->value, newAttribute->value);
        }
        else
        {
            insertBack(image->otherAttributes, newAttribute);
            return;
        }
    }
    else if(elemType == CIRC && !(elemIndex < 0))
    {
        if(getLength(image->circles) <= elemIndex)
        {
            return;
        }
        Circle* nodeC;
        iter = createIterator(image->circles);
        
        for(int i = 0; i <= elemIndex; i++)
        {
            nodeC = (Circle*)nextElement(&iter);
        }
        if(strcmp(newAttribute->name,"cx") == 0)
        {
            nodeC->cx = atof((char*)newAttribute->value);
        }
        else if(strcmp(newAttribute->name,"cy") == 0)
        {
            nodeC->cy = atof((char*)newAttribute->value);
        }
        else if(strcmp(newAttribute->name,"r") == 0)
        {
            nodeC->r = atof((char*)newAttribute->value);
        }
        else
        {
            nodeA = (Attribute*)findElement(nodeC->otherAttributes, &customCompareAttr, newAttribute);
            if(nodeA)
            {
                nodeA->value[0] = '\0';
                nodeA->value = addString(nodeA->value, newAttribute->value);
            }
            else
            {
                insertBack(nodeC->otherAttributes, newAttribute);
                return;
            }
        }
        
        
    }
    else if(elemType == RECT && !(elemIndex < 0))
    {
        if(getLength(image->rectangles) <= elemIndex)
        {
            return;
        }

        Rectangle* nodeR;
        iter = createIterator(image->rectangles);
        for(int i = 0; i <= elemIndex; i++)
        {
            nodeR = (Rectangle*)nextElement(&iter);
        }
        if(strcmp(newAttribute->name,"x") == 0)
        {
            nodeR->x = atof((char*)newAttribute->value);
        }
        else if(strcmp(newAttribute->name,"y") == 0)
        {
            nodeR->y = atof((char*)newAttribute->value);
        }
        else if(strcmp(newAttribute->name,"width") == 0)
        {
            nodeR->width = atof((char*)newAttribute->value);
        }
        else if(strcmp(newAttribute->name,"height") == 0)
        {
            nodeR->height = atof((char*)newAttribute->value);
        }
        else
        {
            nodeA = (Attribute*)findElement(nodeR->otherAttributes, &customCompareAttr, newAttribute);
            if(nodeA)
            {
                nodeA->value[0] = '\0';
                nodeA->value = addString(nodeA->value, newAttribute->value);
            }
            else
            {
                insertBack(nodeR->otherAttributes, newAttribute);
                return;
            }
        }
    }
    else if(elemType == PATH && !(elemIndex < 0))
    {
        if(getLength(image->paths) <= elemIndex)
        {
            return;
        }

        Path* nodeP;
        iter = createIterator(image->paths);
        for(int i = 0; i <= elemIndex; i++)
        {
            nodeP = (Path*)nextElement(&iter);
        }

        if(strcmp(newAttribute->name,"d") == 0)
        {
            free(nodeP->data);
            nodeP->data = malloc(999);
            snprintf(nodeP->data,255,"%s",newAttribute->value);
        }
        else
        {
            nodeA = (Attribute*)findElement(nodeP->otherAttributes, &customCompareAttr, newAttribute);
            if(nodeA)
            {
                nodeA->value[0] = '\0';
                nodeA->value = addString(nodeA->value, newAttribute->value);
            }
            else
            {
                insertBack(nodeP->otherAttributes, newAttribute);
                return;
            }
        }
    }
    else if(elemType == GROUP && !(elemIndex < 0))
    {
        if(getLength(image->groups) <= elemIndex)
        {
            return;
        }
        Group* nodeG;
        iter = createIterator(image->groups);
        for(int i = 0; i <= elemIndex; i++)
        {
            nodeG = (Group*)nextElement(&iter);
        }

        nodeA = (Attribute*)findElement(nodeG->otherAttributes, &customCompareAttr, newAttribute);
        if(nodeA)
        {
            nodeA->value[0] = '\0';
            nodeA->value = addString(nodeA->value, newAttribute->value);
        }
        else
        {
            insertBack(nodeG->otherAttributes, newAttribute);
            return;
        }
    }

    deleteAttribute(newAttribute);
}

//add a component to image that is type elemType
void addComponent(SVGimage* image, elementType elemType, void* newComponent)
{
    if(!elemType || !image || !newComponent)
    {
        return;
    }
    
    if(elemType == CIRC)
    {
        Circle* nodeC = (Circle*)newComponent;
        
        if(!nodeC || (nodeC->r < 0) || !nodeC->otherAttributes)
        {
            return;
        }
        insertBack(image->circles, nodeC);
    }
    else if(elemType == RECT)
    {
        Rectangle* nodeR = (Rectangle*)newComponent;
        if(!nodeR &&(nodeR->width < 0 || nodeR->height < 0 || !nodeR->otherAttributes))
        {
            return;
        }
        insertBack(image->rectangles, nodeR);
    }
    else if(elemType == PATH)
    {
        Path* nodeP = (Path*)newComponent;
        if(!nodeP || !nodeP->data || !nodeP->otherAttributes)
        {
            return;
        }
        insertBack(image->paths, nodeP);
    }
    else if(elemType == GROUP)
    {
        Group* nodeG = (Group*)newComponent;
        if(!nodeG || !nodeG->groups|| !nodeG->rectangles || !nodeG->otherAttributes || !nodeG->circles || !nodeG->paths)
        {
            return;
        }
        if(!checkAttr(nodeG->otherAttributes) || !checkCircle(nodeG->circles) || !checkG(nodeG->groups) || !checkPath(nodeG->paths) || !checkRect(nodeG->rectangles))
        {
            return;
        }
        insertBack(image->groups, nodeG);
    }
    else if(elemType == SVG_IMAGE)
    {
        Attribute* nodeA = (Attribute*)newComponent;
        if(!nodeA || !nodeA->value || !nodeA->name)
        {
            return;
        }
        insertBack(image->otherAttributes, nodeA);
    }
}

//attribute to JASON string
char* attrToJSON(const Attribute *a)
{
    char* out = malloc(1);
    out[0] = '\0';

    if(!a)
    {
        out = addString(out,"{}");
        return out;
    }

    out = realloc(out, strlen(a->name) + strlen(a->value) + 30);
    sprintf(out,"{\"name\":\"%s\",\"value\":\"%s\"}",a->name,a->value);

    return out;
}

//Circle to JASON string
char* circleToJSON(const Circle *c)
{
    char* out = malloc(2000);
    out[0] = '\0';

    if(!c)
    {
        out = addString(out,"{}");
        return out;
    }
    sprintf(out,"{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units);
    return out;
}

//Rectangle to JASON string
char* rectToJSON(const Rectangle *r)
{
    char* out = malloc(2000);
    out[0] = '\0';

    if(!r)
    {
        out = addString(out,"{}");
        return out;
    }
    sprintf(out,"{\"x\":%.2f,\"y\":%.2f,\"width\":%.2f,\"height\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);
    return out;
}

//Path to JASON string
char* pathToJSON(const Path *p)
{
    char* out = malloc(2000);
    char* temp = malloc(256);
    out[0] = '\0';

    if(!p)
    {
        out = addString(out,"{}");
        return out;
    }
    snprintf(temp,64,"%s",p->data);
    sprintf(out,"{\"d\":\"");
    out = addString(out,temp);
    sprintf(temp,"\",\"numAttr\":%d}", getLength(p->otherAttributes));
    out = addString(out,temp);
    free(temp);
    return out;
}

//Group to JASON string
char* groupToJSON(const Group *g)
{
    char* out = malloc(2000);
    out[0] = '\0';

    if(!g)
    {
        out = addString(out,"{}");
        return out;
    }
    sprintf(out,"{\"children\":%d,\"numAttr\":%d}", getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups), getLength(g->otherAttributes));
    return out;
}

//Attribute list to JASON string
char* attrListToJSON(const List *list)
{
    if(!list)
    {
        return "[]";
    }

    List* list1 = (List*)list;
    ListIterator iter;
    Attribute* nodeA;
    
    iter = createIterator(list1);
    char* out = malloc(100);
    char* temp;
    out[0] = '\0';
    out = addString(out,"[");

    for(int i = 0; i<getLength(list1); i++)
    {
        nodeA = (Attribute*)nextElement(&iter);
        temp = attrToJSON(nodeA);
        out = addString(out, attrToJSON(nodeA));
        free(temp);
        if(getLength(list1)-1 > i)
        {
            out = addString(out,",");
        }
    }
    out = addString(out,"]");
    return out;
}

//circle list to JASON string
char* circListToJSON(const List *list)
{
    if(!list)
    {
        return "[]";
    }

    List* list1 = (List*)list;
    ListIterator iter;
    Circle* nodeC;
    
    iter = createIterator(list1);
    char* out = malloc(100);
    char* temp;
    out[0] = '\0';
    out = addString(out,"[");

    for(int i = 0; i<getLength(list1); i++)
    {
        nodeC = (Circle*)nextElement(&iter);
        temp = circleToJSON(nodeC);
        out = addString(out, temp);
        free(temp);
        if(getLength(list1)-1 > i)
        {
            out = addString(out,",");
        }
    }
    out = addString(out,"]");
    return out;
}

//rectangle list to JASON string
char* rectListToJSON(const List *list)
{
    if(!list)
    {
        return "[]";
    }

    List* list1 = (List*)list;
    ListIterator iter;
    Rectangle* nodeR;
    
    iter = createIterator(list1);
    char* out = malloc(100);
    char* temp;
    out[0] = '\0';
    out = addString(out,"[");

    for(int i = 0; i<getLength(list1); i++)
    {
        nodeR = (Rectangle*)nextElement(&iter);
        temp = rectToJSON(nodeR);
        out = addString(out, temp);
        free(temp);
        if(getLength(list1)-1 > i)
        {
            out = addString(out,",");
        }
    }
    out = addString(out,"]");
    return out;
}

//path list to JASON string
char* pathListToJSON(const List *list)
{
    if(!list)
    {
        return "[]";
    }

    List* list1 = (List*)list;
    ListIterator iter;
    Path* nodeP;
    
    iter = createIterator(list1);
    char* out = malloc(100);
    char* temp;
    out[0] = '\0';
    out = addString(out,"[");

    for(int i = 0; i<getLength(list1); i++)
    {
        nodeP = (Path*)nextElement(&iter);
        temp = pathToJSON(nodeP);
        out = addString(out, temp);
        free(temp);
        if(getLength(list1)-1 > i)
        {
            out = addString(out,",");
        }
    }
    out = addString(out,"]");
    return out;
}

//group list to JASON string
char* groupListToJSON(const List *list)
{
    if(!list)
    {
        return "[]";
    }

    List* list1 = (List*)list;
    ListIterator iter;
    Group* nodeG;
    
    iter = createIterator(list1);
    char* out = malloc(100);
    char* temp;
    out[0] = '\0';
    out = addString(out,"[");

    for(int i = 0; i<getLength(list1); i++)
    {
        nodeG = (Group*)nextElement(&iter);
        temp = groupToJSON(nodeG);
        out = addString(out, temp);
        free(temp);
        if(getLength(list1)-1 > i)
        {
            out = addString(out,",");
        }
    }
    out = addString(out,"]");
    return out;
}

//SVGimage struct to JASON string
char* SVGtoJSON(const SVGimage* imge)
{
    char* out = malloc(2000);
    out[0] = '\0';
    SVGimage* image = (SVGimage*)imge;

    if(!imge)
    {
        out = addString(out,"{}");
        return out;
    }
    
    List* r = getRects(image);
    List* c = getCircles(image);
    List* p = getPaths(image);
    List* g = getGroups(image);


    sprintf(out,"{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", getLength(r), getLength(c), getLength(p), getLength(g));

    freeList(r);
    freeList(c);
    freeList(p);
    freeList(g);
    
    return out;
}

char* fileNameTitletoJSON(char* fileName)
{
    SVGimage* img;
    img = createSVGimage(fileName);
    if(img == NULL)
    {
        return "ERROR";
    }
    else
    {
        return img->title;
    }
}

char* fileNameDestoJSON(char* fileName)
{
    SVGimage* img;
    img = createSVGimage(fileName);
    if(img == NULL)
    {
        return "ERROR";
    }
    else
    {
        return img->description;
    }
}

char* fileNametoJSON(char* fileName)
{
    SVGimage* img;

    char* output = malloc(4);
    output[0] = '[';
    output[1] = '\0';
    char* temp;
    img = createSVGimage(fileName);
    if(img == NULL)
    {
        return "ERROR";
    }
    else
    {

        temp = rectListToJSON(img->rectangles);
        output = addString(output, temp);
        free(temp);
        output = addString(output, ",");
        
        temp = circListToJSON(img->circles);
        output = addString(output, temp);
        free(temp);
        output = addString(output, ",");

        temp = pathListToJSON(img->paths);
        output = addString(output, temp);
        free(temp);
        output = addString(output, ",");

        temp = groupListToJSON(img->groups);
        output = addString(output, temp);
        free(temp);


        output = addString(output, "]");
        deleteSVGimage(img);
        return output;
    }
}

char* fileSvgsToJSON(char* fileName)
{
    SVGimage* img;

    char* output;
    img = createSVGimage(fileName);
    if(img == NULL)
    {
        return "ERROR";
    }
    else
    {
        output = SVGtoJSON(img);
        return output;
    }
}

char* svgOtherAttr(char* fileName)
{
    SVGimage* img;

    char* output = malloc(4);
    output[0] = '[';
    output[1] = '[';
    output[2] = '\0';
    char* temp;
    img = createSVGimage(fileName);
    if(img == NULL)
    {
        return "ERROR";
    }
    else
    {
        ListIterator iter;
        
        Rectangle* nodeR;
        iter = createIterator(img->rectangles);
        for(int i = 0; i<getLength(img->rectangles); i++)
        {
            nodeR = (Rectangle*)nextElement(&iter);
            if(nodeR)
            {
                temp = attrListToJSON(nodeR->otherAttributes);
                output = addString(output, temp);
                if(i < getLength(img->rectangles)-1)
                {
                    output = addString(output, ",");
                }
                
                free(temp);
            }
        }
        output = addString(output,"],[");
    
        Circle* nodeC;
        iter = createIterator(img->circles);
        for(int i = 0; i<getLength(img->circles); i++)
        {
            nodeC = (Circle*)nextElement(&iter);
            if(nodeC)
            {
                temp = attrListToJSON(nodeC->otherAttributes);
                output = addString(output, temp);
                if(i < getLength(img->circles)-1)
                {
                    output = addString(output, ",");
                }
                free(temp);
            }
        }
        output = addString(output,"],[");


        Path* nodeP;
        iter = createIterator(img->paths);
        for(int i = 0; i<getLength(img->paths); i++)
        {
            nodeP = (Path*)nextElement(&iter);
            if(nodeP)
            {
                temp = attrListToJSON(nodeP->otherAttributes);
                output = addString(output, temp);
                if(i < getLength(img->paths)-1)
                {
                    output = addString(output, ",");
                }
                free(temp);
            }
        }
        output = addString(output,"],[");


        Group* nodeG;
        iter = createIterator(img->groups);
        for(int i = 0; i<getLength(img->groups); i++)
        {
            nodeG = (Group*)nextElement(&iter);
            if(nodeG)
            {
                temp = attrListToJSON(nodeG->otherAttributes);
                output = addString(output, temp);
                if(i < getLength(img->groups)-1)
                {
                    output = addString(output, ",");
                }
                free(temp);
            }
        }
        output = addString(output,"]]");
        deleteSVGimage(img);
        return output;
    }
}

