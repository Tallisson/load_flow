class Node{
private:
  double c;
  double s;
  double sh;

public:
  Node(double c, double s): c(c), s(s), sh(0)
  {};
  Node(double c, double s, double sh): c(c), s(s), sh(sh)
  {};
  Node(): c(0), s(0), sh(0) {};

  double GetS();
  double GetC();
  double GetSh();
  int GetId();
  void SetS(double s);
  void SetC(double c);
  bool Exist();
};
