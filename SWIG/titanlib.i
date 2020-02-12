%module titanlib
%include "typemaps.i"
%include "std_vector.i"
namespace std {
  %template(IntVector) vector<int>;
  %template(DoubleVector) vector<double>;
  %template(FloatVector) vector<float>;
}
%apply std::vector<float>& OUTPUT { std::vector<int>& output };
%apply std::vector<float>& OUTPUT { std::vector<int>& flags };
%apply std::vector<float>& OUTPUT { std::vector<float>& sct };
%{
/*  Put header files here or function declarations like below */
#include "titanlib.h"
%}
%include "titanlib.h"
