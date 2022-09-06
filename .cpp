/// This is just meant for testing quick ideas -- 




class Type {

private:
// Direct comparison is disabled for types, because they may not be canonical.
  void operator==(Type T) const = delete;
  void operator!=(Type T) const = delete;
};



class BitterType : Type {


}
class SugarType  : Type {
};

int main() {

	return 0;

}



