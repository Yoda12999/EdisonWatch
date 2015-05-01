MenuInternalNode::MenuInternalNode(string name, MenuNode parent) {
    _name = name;
    _parent = parent;
    _type = INTERNAL
}

MenuLeafNode::MenuLeafNode(string name, MenuNode parent, void (*click)()) {
    _name = name;
    _parent = parent;
    _type = LEAF;
    
    selectFunc = click;
}

MenuNode* MenuNode::getParent() {
    return _parent;
}

string MenuNode::getName() {
    return _name;
}

NodeType MenuNode::getType() {
    return _type;
}