class Admitt {
private:
  double r;
  double x;
  double sh;
  double tap;
  double angle_phi;
  double type;

public:
  Admitt(double r, double x, double sh): r(r), x(x), sh(sh), tap(1), angle_phi(0), type(TRANSMISSION_LINE) {};
  Admitt(double r, double x, double sh, double tap, double angle_phi) : r(r), x(x), sh(sh), tap(tap), angle_phi(angle_phi), type(TRANSMISSION_LINE) {};
  Admitt(double r, double x, double sh, double tap, double angle_phi, int type) : r(r), x(x), sh(sh), tap(tap), angle_phi(angle_phi) {};
  Admitt(double r, double x) : r(r), x(x), sh(0), tap(1), angle_phi(0), type(TRANSMISSION_LINE) {};
  Admitt() : r(0), x(0), sh(0), tap(1), angle_phi(0), type(0) {};
  void SetR(double r);
  void SetX(double x);
  void SetSh(double sh);
  void SetTap(double tap);
  void SetType(int type);

  double GetR();
  double GetX();
  double GetSh();
  double GetTap();
  void GetType();
};
