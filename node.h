class Node{
private:
  double c;
  double s;

public:
  Node(double c, double s): c(c), s(s)
  {};
  Node(): c(0), s(0) {};

  double GetS();
  double GetC();
  int GetId();
  void SetS(double s);
  void SetC(double c);
  bool Exist();
};
