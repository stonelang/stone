#ifndef DIT_AST_H
#define DIT_AST_H

namespace dit {

class DitNode {

};

class DitDecl : public DitNode {
}
class DitProjectDecl : public DitDecl {

};


class DitPackageDecl : public DitDecl {

};

class DitModuleDecl : public DitDecl {

};
class DitFileDecl : public DitDecl {

};

}

#endif 