#include <cassert>
#include "location.h"


Location::Location (int r, int c, int rot) : row(r), column(c), rotation(rot) { 
  assert (rotation == 0 || 
          rotation == 90 || 
          rotation == 180 || 
          rotation == 270); 
}

bool operator==(const Location &loc1, const Location &loc2) {
  return (loc1.row      == loc2.row      && 
          loc1.column   == loc2.column   && 
          loc1.rotation == loc2.rotation );
}

std::ostream& operator<<(std::ostream &ostr, const Location &loc) {
  ostr << "(" << loc.row << "," << loc.column << "," << loc.rotation << ")";
  return ostr;
}
