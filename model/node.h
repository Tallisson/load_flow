#define LINE_TANSMISSION 100
#define PHASE_T 200
#define PHASE_SHIFTING_T 300

class Node{
private:
  double c;
  double s;
  double sh;
  double tap;
  int type;
  double phase_shifting;

public:
  Node(double c, double s): c(c), s(s), sh(0), tap(1), type(LINE_TANSMISSION), phase_shifting(0)
  {};
  Node(double c, double s, double sh, double tap, double phase_shifting): c(c), s(s), sh(sh), tap(tap), phase_shifting(phase_shifting)
  {
    if(tap == 1 && phase_shifting == 0) {
      type = LINE_TANSMISSION;
    } else {
      if(sh == 0 && phase_shifting == 0) {
        type = PHASE_T;
      } else {
        if(sh == 0 && tap == 1) {
          type = PHASE_SHIFTING_T;
        }
      }
    }
  };
  Node(): c(0), s(0), sh(0), tap(1), type(LINE_TANSMISSION) {};

  double GetS();
  double GetC();
  double GetSh();
  double GetTap();
  int GetId();
  void SetS(double s);
  void SetC(double c);
  void SetTap(double t);
  bool Exist();
};
