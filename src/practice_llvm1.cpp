#include <sstream>
#include <string>
#include <iostream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Sample");

struct VarInfo
{
    string name;
    string type;
    
    string fileName;
    unsigned int startLineNum;
};

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> 
{
    public:
        MyASTVisitor(SourceManager &SM, vector<VarInfo> vList) : SM(SM), vList(vList){}

        bool VisitVarDecl(VarDecl *VD)
        {

        }

    private:
        SourceManager &SM;
        vector<VarInfo> &vList;
};

class MyASTConsumer : public ASTConsumer 
{
    public:
        MyASTConsumer(SourceManager &SM, vector<VarInfo> &vList) : Visitor(SM, vList) {}

        bool HandleTopLevelDecl(DeclGroupRef DR) override 
        {
            for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) 
            {
                Visitor.TraverseDecl(*b);
                (*b)->dump();
            }
            return true;
        }

    private:
        MyASTVisitor Visitor;
};

class MyFrontendAction : public ASTFrontendAction 
{
    public:
        MyFrontendAction() {}

        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override 
        {
            return llvm::make_unique<MyASTConsumer>(CI.getSourceManager(), vList);
        }

        void EndSourceFileAction ()
        {
            cout << "Variable List from input:" << endl;
        }

    private:
        vector<VarInfo> vList;
};

int main(int argc, const char **argv) 
{
    CommonOptionsParser op(argc, argv, ToolingSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
