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

struct FuncInfo
{
    string name;
    string type;
    unsigned int parmNum;
    
    string fileName;
    unsigned int startLineNum;
};

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> 
{
    public:
        MyASTVisitor(SourceManager &SM, vector<FuncInfo> fList) : SM(SM), fList(fList){}

        bool VisitFunctionDecl(FunctionDecl *FD)
        {

        }

    private:
        SourceManager &SM;
        vector<FuncInfo> &fList;
};

class MyASTConsumer : public ASTConsumer 
{
    public:
        MyASTConsumer(SourceManager &SM, vector<FuncInfo> &fList) : Visitor(SM, fList) {}

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
            return llvm::make_unique<MyASTConsumer>(CI.getSourceManager(), fList);
        }

        void EndSourceFileAction ()
        {
            cout << "Function List from input:" << endl;
        }

    private:
        vector<FuncInfo> fList;
};

int main(int argc, const char **argv) 
{
    CommonOptionsParser op(argc, argv, ToolingSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
