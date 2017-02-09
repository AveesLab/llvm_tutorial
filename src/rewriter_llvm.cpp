#include <fstream>
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

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> 
{
    public:
        MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

        bool VisitFunctionDecl(FunctionDecl *FD)
        {
            SourceLocation SL = FD->getSourceRange().getBegin();
            string insertText = "// this is a Function Declaration\n";

            TheRewriter.InsertText(SL, insertText);
            
            return true;
        }

    private:
        Rewriter &TheRewriter;
};

class MyASTConsumer : public ASTConsumer 
{
    public:
        MyASTConsumer(Rewriter &R) : Visitor(R) {}

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
            TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
            return llvm::make_unique<MyASTConsumer>(TheRewriter);
        }

        void EndSourceFileAction()
        {
            SourceManager &SM = TheRewriter.getSourceMgr();
            const RewriteBuffer *RewriteBuf = TheRewriter.getRewriteBufferFor(SM.getMainFileID());

            string file = SM.getFileEntryForID(SM.getMainFileID())->getName();

            if(RewriteBuf != NULL)
            {
                file += ".new";
                ofstream dst(file, ios::binary);
                dst<<string(RewriteBuf->begin(), RewriteBuf->end());
                cout<<"File "<<file<<" is changed."<<endl;
            }
            else
            {
                cout<<"File "<<file<<" is not changed."<<endl;
            }
        }

    private:
        Rewriter TheRewriter;
};

int main(int argc, const char **argv) 
{
    CommonOptionsParser op(argc, argv, ToolingSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
