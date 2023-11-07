#include<vector>

enum class ModeKind{
	None = 0,
	EmitObject,
};

class CompilingSession {

protected:
	std::vector<CompilingSession> deps; 

public:
    CompilingSession() {
    	for(auto dep : deps){
    		dep.Start();
    		dep.Execute();
    		dep.Stop();
    	}
    }

 public:
 	virtual void Start(){}
 	virtual void Execute() {}
 	virtual void Stop() {}
};


class ParsingSession : public CompilingSession {
public:
	ParsingSession() {

	}
public:
	virtual void Start(){ printf("test");}
 	virtual void Execute() {}
 	virtual void Stop() {}
};

class TypeCheckingSession : public CompilingSession {
public:
	TypeCheckingSession() {
		
	}	
public:
	void Start() override {	
		deps.push_back(ParsingSession());
	}
};

int main() {

	TypeCheckingSession session;
	session.Start();
	session.Execute();
	session.Stop();
	return 0;
}