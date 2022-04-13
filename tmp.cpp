

class SyntaxAnalysisClient {

public:
  void HandleSyntaxFile(SyntaxFile &sf) = 0;
};

class CompileInstance : public SyntaxAnalysisClient,
                        public SemanticAnalysisClient {

public:
  void CompileWithSyntaxAnalysis(Sources &source,
                                 SyntaxAnalysisClient &client) {
    client.HandleSyntaxFile(sf);
  }

public:
  void HandleSyntaxFile() override {}
};

int main() { return 0; }
