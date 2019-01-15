#define interface struct
#include <iostream>
#include <string>

class INode_identifier {

    protected:
        virtual ~INode_identifier() = 0;
        int node_id;
        string node_name;
};
