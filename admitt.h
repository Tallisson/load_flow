class Admitt {
private:
  double r;
  double x;
  double sh;

public:
  Admitt(double r, double x, double sh) : r(r), x(x), sh(sh) {};
  Admitt(double r, double x) : r(r), x(x), sh(0) {};
  Admitt() : r(0), x(0), sh(0) {};
  void SetR(double r);
  void SetX(double x);
  void SetSh(double sh);

  double GetR();
  double GetX();
  double GetSh();
};
