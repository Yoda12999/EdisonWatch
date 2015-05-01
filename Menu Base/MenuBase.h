#ifndef menubase_h
#define menubase_h

class MenuNode {
protected:
    MenuNode* _parent;
    string _name;
    MenuNode*[16] _children;
    
public:
    MenuNode(string name, MenuNode parent);

    MenuNode* getParent();
    string getName();
    
    void addChild(MenuNode* child);
    bool removeChild(int index);
    MenuNode* getChild(int index);
    
    virtual void display();
    virtual void input();
}

#endif
