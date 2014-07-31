class Admitt {
private:
  double r;
  double x;
  double sh;
  double tap;

public:
  Admitt(double r, double x, double sh, double tap) : r(r), x(x), sh(sh), tap(tap) {};
  Admitt(double r, double x) : r(r), x(x), sh(0), tap(0) {};
  Admitt() : r(0), x(0), sh(0), tap(0) {};
  void SetR(double r);
  void SetX(double x);
  void SetSh(double sh);
  void SetTap(double tap);

  double GetR();
  double GetX();
  double GetSh();
  double GetTap();
};
