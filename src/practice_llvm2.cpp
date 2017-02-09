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

struct ParmInfo
{
    string name;
    string type;
}

struct FuncInfo
{
    string name;
    string type;
    unsigned int parmNum;

    string fileName;
    int startLineNumber;
    vector<ParmInfo> pList;
};

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> 
{
    public:
        MyASTVisitor(SourceManager &SM, vector<FuncInfo> &fList) : SM(SM), fList(fList){}

        bool VisitFunctionDecl(FunctionDecl *FD)
        {
            
            if(FD->hasBody())
            {
                FuncInfo funcInfo;
                
                //Get Function's Name Info
                DeclarationNameInfo DNI = FD->getNameInfo();
                DeclarationName DN = DNI.getName();

                funcInfo.name  = DN.getAsString();

                //Get Function's Type Info
                QualType QT = FD->getType();
                
                funcInfo.type = QT.getAsString();

                //Get Function's the number of parameters
                funcInfo.parmNum = FD->getNumParams();
                
                //Get Source Location Information
                SourceLocation SL = FD->getLocStart();

                funcInfo.fileName = SM.getFilename(SL);
                funcInfo.startLineNumber = SM.getExpansionLineNumber(SL);

                //add code to get ParameterInfomation
                
                fList.push_back(funcInfo);
            }
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
            int i=0;

            cout << "Function List from input:" << endl;
            
            for(i=0; i<fList.size(); i++)
            {
                cout << fList[i].name << "(" << fList[i].type << ") | ParmNum : "
                    << fList[i].parmNum << " | "
                    << fList[i].fileName << "(Line : " << fList[i].startLineNumber << ")" << endl;
            }
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
