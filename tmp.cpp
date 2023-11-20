


class CompilerActionConsumer {

};
class CompilerAction {

	CompilerActionConsumer* CreateConsumer();
};

class ASTFileAction : public CompilerAction {

public:
	void HandleSourceFile(SourceFile* sourceFile);

};

class ParseOnlyAction : public ASTAction {
};

class TypeCheckAction : public ParseOnlyAction {

};



